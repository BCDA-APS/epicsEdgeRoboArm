#!/bin/bash

# restart_ioc_check.sh
# must run as root to use USB support

# run by  crontab -e
#     * * * * *  /root/restart_ioc_check.sh 2>&1 /dev/null
#              field          allowed values
#              -----          --------------
#              minute         0-59
#              hour           0-23
#              day of month   1-31
#              month          1-12 (or names, see below)
#	       day of week    0-7 (0 or 7 is Sun, or use names)
#
#  # auto-start the robotic arm IOC
#  * * * * *  /root/restart_ioc_check.sh 2>&1 /dev/null
#
#------------
#
# also, as root (do these steps BEFORE enabling the cron job):
#  cd /root
#  ln -s ${ST_CMD_DIR} ./ioc
#  ln -s ioc/restart_ioc_check.sh ./
#  ln -s ioc/is_ioc_up.py ./
 

export EPICS_HOST_ARCH=`/usr/local/epics/base/startup/EpicsHostArch`
export EPICS_BASE_BIN=/usr/local/epics/base/bin/${EPICS_HOST_ARCH}
export ROBOT_DIR=/usr/local/epics/epicsEdgeRoboArm
export IOC_TOP=${ROBOT_DIR}/edgeRoboArmIOC/support/xxx-5-6
export ST_CMD_DIR=${IOC_TOP}/iocBoot/iocLinux


# ID 1267:0000 Logic3 / SpectraVideo plc
export usb_connect=`lsusb | grep  "ID 1267:0000 Logic3 / SpectraVideo plc"`

if [ "${usb_connect}" != "" ]; then
  #echo "<${usb_connect}>"
  export ioc_off=`/root/is_ioc_up.py`
  if [ "${ioc_off}" != "" ]; then
    #echo "IOC is not running"
    ${EPICS_BASE_BIN}/caRepeater &

    cd  ${ST_CMD_DIR}
    ./in-screen.sh
  fi
fi
