TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================
LIBRARY_IOC_Linux += usb

DBD += usbSupport.dbd

usb_SRCS_Linux += devUSB.cpp

usb_SRCS_Linux += hidDriverMain.cpp
usb_SRCS_Linux += hidDriverStatus.cpp
usb_SRCS_Linux += hidDriverConnect.cpp
usb_SRCS_Linux += hidDriverInput.cpp
usb_SRCS_Linux += hidDriverOutput.cpp

usb_SRCS_Linux += Allocation.cpp
usb_SRCS_Linux += StringUtils.cpp
usb_SRCS_Linux += DataLayout.cpp
usb_SRCS_Linux += StorageType.cpp
usb_SRCS_Linux += DataRead.cpp
usb_SRCS_Linux += DataWrite.cpp

usb_LIBS_Linux += asyn 
USR_SYS_LIBS_Linux += usb-1.0
usb_LIBS_Linux += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

