/**********************************************************************
* Asyn device support using local usb interface                       *
**********************************************************************/
/***********************************************************************
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory, and the Regents of the University of
* California, as Operator of Los Alamos National Laboratory, and
* Berliner Elektronenspeicherring-Gesellschaft m.b.H. (BESSY).
* asynDriver is distributed subject to a Software License Agreement
* found in file LICENSE that is included with this distribution.
***********************************************************************/

/*
 * $Id: drvAsynUSBPort.c,v 0.1 2012-07-29 jrg $
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <cantProceed.h>
#include <errlog.h>
#include <iocsh.h>
#include <epicsAssert.h>
#include <epicsStdio.h>
#include <epicsString.h>
#include <epicsThread.h>
#include <epicsExport.h>
#ifdef _WIN32
#include "libusb.h"
#else
#include <libusb-1.0/libusb.h>
#endif

#include "asynDriver.h"
#include "asynOctet.h"
#include "asynOption.h"
#include "drvAsynUSBPort.h"

/*
 * This structure holds the hardware-specific information for a single
 * asyn link.  There is one for each usb port
 */
typedef struct {
    asynUser                           *pasynUser;
    char                               *portName;
    char                               *usbDeviceName;
    int                                vendor;
    int                                product;
    int                                debug;
    libusb_device                      *dev;
    libusb_context                     *context;
    struct libusb_config_descriptor    *config;
    struct libusb_device_descriptor    device;
    struct libusb_endpoint_descriptor  endpt;
    struct libusb_interface_descriptor intrfc;
    libusb_device_handle               *handle;
    asynInterface                      common;
    asynInterface                      option;
    asynInterface                      octet;
} usbController_t;

int i;
int found;

/*
 * asynOctet methods
 */
static asynStatus
writeIt(void *drvPvt, asynUser *pasynUser, const char *data,
        size_t numchars,size_t *nbytesTransfered)
{
    usbController_t *usb = (usbController_t *)drvPvt;
    double          cmdstr[3];
    unsigned char   cmd[3];

    assert(usb);
    asynPrint(pasynUser, ASYN_TRACE_FLOW,
              "%s write.\n", usb->usbDeviceName);
    asynPrintIO(pasynUser, ASYN_TRACEIO_DRIVER, data, numchars,
                "%s write %d\n", usb->usbDeviceName, numchars);
    if (usb->dev == NULL) {
        epicsSnprintf(pasynUser->errorMessage,pasynUser->errorMessageSize,
                      "%s disconnected:", usb->usbDeviceName);
        return asynError;
    }
    if (sscanf(data, "%lf %lf %lf", &cmdstr[0], &cmdstr[1], &cmdstr[2])!=3) return asynError;
    cmd[0] = cmdstr[0];
    cmd[1] = cmdstr[1];
    cmd[2] = cmdstr[2];
    if (numchars!=3) numchars = 3;
    *nbytesTransfered = 0;
    *nbytesTransfered = libusb_control_transfer(usb->handle, 0x40, 6, 0x100, 0,
                                                cmd, (uint16_t)numchars, 0);
    if (*nbytesTransfered>=numchars)
        return asynSuccess;
    else
        return asynError;
    return asynSuccess;
}

static asynStatus 
readIt(void *drvPvt, asynUser *pasynUser, char *data, size_t maxchars,
       size_t *nbytesTransfered, int *gotEom)
{
    usbController_t *usb = (usbController_t *)drvPvt;

    return asynSuccess;
}

static asynStatus
flushIt(void *drvPvt, asynUser *pasynUser)
{
    usbController_t *usb = (usbController_t *)drvPvt;

    return asynSuccess;
}

static asynOctet asynOctetMethods = {writeIt, readIt, flushIt};

/*
 * asynOption methods
 */
static asynStatus
getOption(void *drvPvt, asynUser *pasynUser, const char *key, char *val, int valSize)
{
    usbController_t *usb = (usbController_t *)drvPvt;

    return asynSuccess;
}

static asynStatus
setOption(void *drvPvt, asynUser *pasynUser, const char *key, const char *val)
{
    usbController_t *usb = (usbController_t *)drvPvt;

    return asynSuccess;
}

static const struct asynOption asynOptionMethods = {setOption, getOption};

/*
 * asynCommon methods
 */
static void report(void *drvPvt, FILE *fp, int details)
{
    usbController_t *usb = (usbController_t *)drvPvt;

    printf("%s is online\n", usb->usbDeviceName);
}

static asynStatus connectIt(void *drvPvt, asynUser *pasynUser)
{
    usbController_t *usb = (usbController_t *)drvPvt;
    libusb_device   **list;
    ssize_t         cnt;
    struct libusb_device_descriptor desc;
    int err;

    if ((err=libusb_init(&(usb->context)))!=LIBUSB_SUCCESS) {
        printf("libusb_init failed with error: %d\n", err);
        return asynError;
    } else {
        libusb_set_debug(usb->context, usb->debug);
        if ((cnt=libusb_get_device_list(usb->context, &list))<=0) {
            printf("no devices found\n");
            libusb_free_device_list(list, 1);
            return asynError;
        }
        found = 0;
        for(i=0; i<cnt; i++) {
            if (!libusb_get_device_descriptor(list[i], &desc)) {
                if ((desc.idVendor==usb->vendor)&&
                    (desc.idProduct==usb->product)) {
                    usb->dev = list[i];
                    if ((err=libusb_open(usb->dev, &(usb->handle)))!=LIBUSB_SUCCESS) {
                        printf("libusb_open failed with error: %d\n", err);
                        return asynError;
                    } else {
                        i = cnt;
                        found = 1;
                        printf("libusb_open succeeded\n");
                    }
                }
            }
        }
        if (!found)
            printf("device with vendor: %x and product: %x not found\n", usb->vendor, usb->product);
    }
    if ((usb->dev==NULL)||((libusb_get_device_descriptor(usb->dev, &(usb->device)))&&
        (libusb_get_active_config_descriptor(usb->dev, &(usb->config))))) {
        printf("no descriptors found\n");
        libusb_free_device_list(list, 1);
        return asynError;
    } else {
        printf("%s is connected\n", usb->usbDeviceName);
        libusb_free_device_list(list, 1);
        pasynManager->exceptionConnect(pasynUser);
        return asynSuccess;
    }
}

static asynStatus disconnect(void *drvPvt, asynUser *pasynUser)
{
    usbController_t *usb = (usbController_t *)drvPvt;

    libusb_close(usb->handle);
    libusb_exit(usb->context);
    pasynManager->exceptionDisconnect(pasynUser);
    return asynSuccess;
}

static const struct asynCommon asynCommonMethods = {report, connectIt, disconnect};

/*
 * Configure and register a generic usb device
 */
epicsShareFunc int
drvAsynUSBPortConfigure(char         *portName,
                        char         *usbName,
                        int          vendor,
                        int          product,
                        unsigned int debug,
                        unsigned int priority,
                        int          noAutoConnect,
                        int          noProcessEos)
{
    usbController_t *usb;
    asynStatus status;

    /*
     * Check arguments
     */
    if (portName == NULL) {
        printf("Port name missing.\n");
        return -1;
    }
	if (usbName == NULL) {
        printf("USB name missing.\n");
        return -1;
    }

    /*
     * Create a driver
     */
    usb = (usbController_t *)callocMustSucceed(1, sizeof(*usb), "drvAsynUSBPortConfigure()");

    /*
     * Set params
     */
    usb->dev           = NULL;
    usb->context       = NULL;
    usb->config        = NULL;
    usb->handle        = NULL;
    usb->portName      = epicsStrDup(portName);
    usb->usbDeviceName = epicsStrDup(usbName);
    usb->vendor        = vendor;
    usb->product       = product;
    usb->debug         = debug;

    /*
     *  Link with higher level routines
     */
    usb->common.interfaceType = asynCommonType;
    usb->common.pinterface    = (void *)&asynCommonMethods;
    usb->common.drvPvt        = usb;
    usb->option.interfaceType = asynOptionType;
    usb->option.pinterface    = (void *)&asynOptionMethods;
    usb->option.drvPvt        = usb;
    if (pasynManager->registerPort(usb->portName,
                                   ASYN_CANBLOCK,
                                   !noAutoConnect,
                                   priority,
                                   0) != asynSuccess) {
        printf("drvAsynUSBPortConfigure: Can't register myself.\n");
        return -1;
    }
    status = pasynManager->registerInterface(usb->portName, &usb->common);
    if(status != asynSuccess) {
        printf("drvAsynUSBPortConfigure: Can't register common.\n");
        return -1;
    }
     status = pasynManager->registerInterface(usb->portName, &usb->option);
    if(status != asynSuccess) {
        printf("drvAsynUSBPortConfigure: Can't register option.\n");
        return -1;
    }
    usb->octet.interfaceType = asynOctetType;
    usb->octet.pinterface    = &asynOctetMethods;
    usb->octet.drvPvt = usb;
    status = pasynOctetBase->initialize(usb->portName, &usb->octet,
                             (noProcessEos ? 0 : 1), (noProcessEos ? 0 : 1), 1);
    if(status != asynSuccess) {
        printf("drvAsynUSBPortConfigure: Can't register octet.\n");
        return -1;
    }
    usb->pasynUser = pasynManager->createAsynUser(0,0);
    status = pasynManager->connectDevice(usb->pasynUser, usb->portName, -1);
    if(status != asynSuccess) {
        printf("connectDevice failed %s\n", usb->pasynUser->errorMessage);
        return -1;
    } else
        printf("connectDevice succeeded\n");
    return 0;
}

/*
 * IOC shell command registration
 */
static const iocshArg drvAsynUSBPortConfigureArg0    = {"port name",            iocshArgString};
static const iocshArg drvAsynUSBPortConfigureArg1    = {"usb name",             iocshArgString};
static const iocshArg drvAsynUSBPortConfigureArg2    = {"vendor",                  iocshArgInt};
static const iocshArg drvAsynUSBPortConfigureArg3    = {"product",                 iocshArgInt};
static const iocshArg drvAsynUSBPortConfigureArg4    = {"debug",                   iocshArgInt};
static const iocshArg drvAsynUSBPortConfigureArg5    = {"priority",                iocshArgInt};
static const iocshArg drvAsynUSBPortConfigureArg6    = {"disable auto-connect",    iocshArgInt};
static const iocshArg drvAsynUSBPortConfigureArg7    = {"noProcessEos",            iocshArgInt};
static const iocshArg *drvAsynUSBPortConfigureArgs[] = {&drvAsynUSBPortConfigureArg0,
                                                        &drvAsynUSBPortConfigureArg1,
                                                        &drvAsynUSBPortConfigureArg2,
                                                        &drvAsynUSBPortConfigureArg3,
                                                        &drvAsynUSBPortConfigureArg4,
                                                        &drvAsynUSBPortConfigureArg5,
                                                        &drvAsynUSBPortConfigureArg6,
                                                        &drvAsynUSBPortConfigureArg7};

static const iocshFuncDef drvAsynUSBPortConfigureFuncDef = {"drvAsynUSBPortConfigure", 8, drvAsynUSBPortConfigureArgs};

static void drvAsynUSBPortConfigureCallFunc(const iocshArgBuf *args)
{
    drvAsynUSBPortConfigure(args[0].sval, args[1].sval, args[2].ival, args[3].ival,
		            args[4].ival, args[5].ival, args[6].ival, args[7].ival);
}

/*
 * This routine is called before multitasking has started, so there's
 * no race condition in the test/set of firstTime.
 */
static void
drvAsynUSBPortRegisterCommands(void)
{
    static int firstTime = 1;

    if (firstTime) {
        iocshRegister(&drvAsynUSBPortConfigureFuncDef,drvAsynUSBPortConfigureCallFunc);
        firstTime = 0;
    }
}
epicsExportRegistrar(drvAsynUSBPortRegisterCommands);

