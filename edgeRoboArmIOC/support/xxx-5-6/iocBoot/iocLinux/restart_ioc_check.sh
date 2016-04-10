#!/bin/bash

# restart_ioc_check.sh

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
 

export EPICS_HOST_ARCH=linux-arm

# ID 1267:0000 Logic3 / SpectraVideo plc
export usb_connect=`lsusb | grep  "ID 1267:0000 Logic3 / SpectraVideo plc"`

if [ "${usb_connect}" != "" ]; then
  #echo "<${usb_connect}>"
  export ioc_off=`/root/is_ioc_up.py`
  if [ "${ioc_off}" != "" ]; then
    #echo "IOC is not running"
    #/usr/local/epics/base/bin/linux-arm/caRepeater &
    /usr/local/epics/base/bin/${EPICS_HOST_ARCH}/caRepeater &

    cd /usr/local/epics/synAppsRobo/support/xxx-5-6/iocBoot/iocLinux/
    ./in-screen.sh
  fi
fi
