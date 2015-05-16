TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

# For Kate Feng's implementation of devScaler.c, which uses Till
# Straumann's basicIoOps.h to bridge any mismatch between endianness
# of Joerger scaler hardware and endianness of processor.
#USR_CFLAGS += -DHAS_IOOPS_H

#==================================================
# build a support library that some other application can use

#LIBRARY_IOC += xxxSupport

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxxRecord
# install support-library .dbd into <top>/dbd
#DBD += xxxSupport.dbd

# The following are compiled and added to the Support library
#xxxSupport_SRCS += xxxRecord.c

#xxxSupport_LIBS += $(EPICS_BASE_IOC_LIBS)
#xxx_Common_LIBS += xxxSupport

#=============================
# build an ioc application

PROD_IOC = xxx

# Note that the order in which .dbd files are added to a list variable
# doesn't matter, but the order of libraries does.

# <name>.dbd will be created from <name>Include.dbd
ifeq (win32-x86,$(findstring win32-x86, $(T_A)))
	DBD             += iocxxxWin32.dbd
	iocxxxWin32_DBD += xxxCommonInclude.dbd
        xxx_LIBS_WIN32  += usb-1.0
endif

ifeq (windows-x64,$(findstring windows-x64, $(T_A)))
	DBD             += iocxxxWin64.dbd
	iocxxxWin64_DBD += xxxCommonInclude.dbd
        xxx_LIBS_WIN32  += usb-1.0
endif

ifneq (,$(findstring linux, $(T_A)))
	DBD                += iocxxxLinux.dbd
	iocxxxLinux_DBD    += xxxCommonInclude.dbd
        xxx_SYS_LIBS_Linux += usb-1.0
endif

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
xxx_SRCS_Linux    += iocxxxLinux_registerRecordDeviceDriver.cpp     xxxMain.cpp
ifeq ($(T_A),win32-x86)
xxx_SRCS_WIN32    += iocxxxWin32_registerRecordDeviceDriver.cpp     xxxMain.cpp
endif
ifeq ($(T_A),windows-x64)
xxx_SRCS_WIN32    += iocxxxWin64_registerRecordDeviceDriver.cpp     xxxMain.cpp
endif

xxx_Common_LIBS += calc
xxx_Common_LIBS += sscan
xxx_Common_LIBS += autosave
xxx_Common_LIBS += ip
xxx_Common_LIBS += usb
xxx_Common_LIBS += asyn
xxx_Common_LIBS += seqDev seq pv
xxx_Common_LIBS += $(EPICS_BASE_IOC_LIBS)

xxx_LIBS_Linux    += $(xxx_Common_LIBS)
xxx_LIBS_WIN32    += $(xxx_Common_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
