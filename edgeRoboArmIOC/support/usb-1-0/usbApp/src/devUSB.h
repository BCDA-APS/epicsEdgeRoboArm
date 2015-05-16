#ifndef INC_DEVUSB_H
#define INC_DEVUSB_H

#include <string>

#include <fstream>
#include <vector>

#include <iocsh.h>

#include "DataLayout.h"
#include "hidDriver.h"


bool checkConnectionArgs(const iocshArgBuf* args);
bool checkLoadArgs(const iocshArgBuf* args);
bool checkTimeoutArgs(const iocshArgBuf* args);
bool checkFrequencyArgs(const iocshArgBuf* args);
bool checkDelayArgs(const iocshArgBuf* args);
bool checkDebugArgs(const iocshArgBuf* args);
bool checkInterfaceArgs(const iocshArgBuf* args);

#endif
