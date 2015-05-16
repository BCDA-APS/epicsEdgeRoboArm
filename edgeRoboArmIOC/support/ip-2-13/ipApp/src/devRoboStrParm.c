/* devRoboStrParm.c */
/*
 *      Author: Mark Rivers
 *      Date:   06-May-2004
 *      26-July-2004  MLR Convert to pasynEpicsUtils->parseLink
***********************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include <dbScan.h>
#include <dbDefs.h>
#include <dbAccess.h>
#include <dbCommon.h>
#include <epicsString.h>
#include <errlog.h>
#include <alarm.h>
#include <link.h>
#include <recGbl.h>
#include <recSup.h>
#include <devSup.h>
#include <asynDriver.h>
#include <asynEpicsUtils.h>
#include <asynOctet.h>
#include <aiRecord.h>
#include <aoRecord.h>
#include <biRecord.h>
#include <boRecord.h>
#include <longinRecord.h>
#include <longoutRecord.h>
#include <stringinRecord.h>
#include <stringoutRecord.h>
#include <epicsExport.h>

#define HEXCHAR2INT(c) ((c)>'9' ? 10+tolower(c)-'a' : tolower(c)-'0')

typedef enum {opTypeOutput} opType;
typedef enum {recTypeRobo} recType;

typedef struct devStrParmPvt {
    asynUser     *pasynUser;
    asynOctet    *pasynOctet;
    void         *octetPvt;
    opType       opType;
    recType      recType;
    asynStatus   status;
    size_t       nread;
    size_t       nwrite;
    char         buffer[100];
    int          bufferStartIndex;
    char         term[10];
    char         format[32];
    int          termlen;
    double       timeout;
    int          nchar;
    char         zerostring[32];
    char         onestring[32];
} devStrParmPvt;

typedef struct dsetStrParm{
        long      number;
        DEVSUPFUN report;
        DEVSUPFUN init;
        DEVSUPFUN init_record;
        DEVSUPFUN get_ioint_info;
        DEVSUPFUN io;
        DEVSUPFUN convert;
} dsetStrParm;

static long initCommon(dbCommon *pr, DBLINK *plink, 
                       opType ot, recType rt, char *format);
static long startIOCommon(dbCommon *pr);
static long completeIOCommon(dbCommon *pr);
static void devRoboStrParmCallback(asynUser *pasynUser);

static long initRobo(stringoutRecord *pr);
static long writeRobo(stringoutRecord *pr);
dsetStrParm devRoboStrParm = {6,0,0,initRobo,0,writeRobo,0};
epicsExportAddress(dset,devRoboStrParm);

static long initCommon(dbCommon *pr, DBLINK *plink, 
                       opType ot, recType rt, char *format)
{
   char *port, *userParam;
   int signal;
   asynUser *pasynUser=NULL;
   asynStatus status;
   asynInterface *pasynInterface;
   devStrParmPvt *pPvt=NULL;
      
   /* Allocate private structure */
   pPvt = calloc(1, sizeof(devStrParmPvt));
   pPvt->opType = ot;
   pPvt->recType = rt;

   /* Create an asynUser */
   pasynUser = pasynManager->createAsynUser(devRoboStrParmCallback, 0);
   pasynUser->userPvt = pr;

   /* Parse link */
   status = pasynEpicsUtils->parseLink(pasynUser, plink,
                                       &port, &signal, &userParam);
   if (status != asynSuccess) {
      errlogPrintf("devRoboStrParm::initCommon %s bad link %s\n",
                   pr->name, pasynUser->errorMessage);
      goto bad;
   }

   status = pasynManager->connectDevice(pasynUser,port,signal);
   if(status!=asynSuccess) goto bad;
   pasynInterface = pasynManager->findInterface(pasynUser,asynOctetType,1);
   if(!pasynInterface) goto bad;
   pPvt->pasynOctet = (asynOctet *)pasynInterface->pinterface;
   pPvt->octetPvt = pasynInterface->drvPvt;
   pPvt->pasynUser = pasynUser;
   pr->dpvt = pPvt;

   /* Initialize parameters */
   strcpy(pPvt->term, "\r\n");
   pPvt->termlen=2;
   /* NOTE: in the past the "signal" was used for the buffer start index.  This cannot be
    * used any more, since GPIB will use this for actual address.  Must find another way to
    * pass buffer start index
    * pPvt->bufferStartIndex = signal; */
   pPvt->bufferStartIndex = 0;
   pPvt->timeout=1.0;
   pPvt->nchar=100;
   if (pr->desc[0]) {
      strncpy(pPvt->format, pr->desc, 31);
   } else {
      strcpy(pPvt->format, format);
   }
   return 0;

bad:
   if(status!=asynSuccess) {
       asynPrint(pasynUser,ASYN_TRACE_ERROR,
           "%s asynManager error %s\n",
           pr->name,pasynUser->errorMessage);
   }
   if(pasynUser) pasynManager->freeAsynUser(pasynUser);
   if(pPvt) free(pPvt);
   pr->pact = 1;
   return 0;
}

static long initRobo(stringoutRecord *pr)
{
   return(initCommon((dbCommon *)pr, &pr->out, opTypeOutput, recTypeRobo, "%s"));
}

static long writeRobo(stringoutRecord *pr)
{
   devStrParmPvt *pPvt = (devStrParmPvt *)pr->dpvt;
    
   if (!pr->pact) {
      strncpy(pPvt->buffer, &(pr->val[pPvt->bufferStartIndex]), 
              sizeof(pr->val));
      return(startIOCommon((dbCommon *)pr));
   } else { 
      return(completeIOCommon((dbCommon *)pr));
   }
}

static long startIOCommon(dbCommon* pr)
{
   devStrParmPvt *pPvt = (devStrParmPvt *)pr->dpvt;
   asynUser *pasynUser = pPvt->pasynUser;
   int status;

   pr->pact = 1;
   status = pasynManager->queueRequest(pasynUser, 0, 0);
   if (status != asynSuccess) status = -1;
   return(status);
}

static void devRoboStrParmCallback(asynUser *pasynUser)
{
   dbCommon *pr = (dbCommon *)pasynUser->userPvt;
   devStrParmPvt *pPvt = (devStrParmPvt *)pr->dpvt;
   struct rset *prset = (struct rset *)(pr->rset);

   pPvt->pasynUser->timeout = pPvt->timeout;
   switch(pPvt->opType) {
      case opTypeOutput:
         pPvt->status = pPvt->pasynOctet->write(pPvt->octetPvt, pasynUser, 
                                                pPvt->buffer, strlen(pPvt->buffer), 
                                                &pPvt->nwrite);
         break;
   }
   /* Process the record. This will result in the readXi or writeXi routine
      being called again, but with pact=1 */
   dbScanLock(pr);
   (*prset->process)(pr);
   dbScanUnlock(pr);
}

static long completeIOCommon(dbCommon *pr)
{
   devStrParmPvt *pPvt = (devStrParmPvt *)pr->dpvt;
   int rc=0;

   switch(pPvt->opType) {
      case opTypeOutput:
         if (pPvt->status != 0) rc = -1;
         break;
   }
   return(rc);
}

