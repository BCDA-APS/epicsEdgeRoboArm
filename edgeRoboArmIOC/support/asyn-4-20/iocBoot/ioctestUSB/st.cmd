< envPaths

dbLoadDatabase("../../dbd/testUSB.dbd")
testUSB_registerRecordDeviceDriver(pdbbase)

dbLoadRecords("../../db/asynRecord.db","P=asyn,R=usb,PORT=ROBO,ADDR=0,OFMT=Binary,OMAX=3,IMAX=0,NOWT=3")

drvAsynUSBPortConfigure("ROBO","Robotic Arm",0x1267,0,0,3,0,1)

iocInit()
