#!/bin/bash

# restart_gui_check.sh

# run by  crontab -e
#              field          allowed values
#              -----          --------------
#              minute         0-59
#              hour           0-23
#              day of month   1-31
#              month          1-12 (or names, see below)
#	       day of week    0-7 (0 or 7 is Sun, or use names)
#
#  # auto-start the robotic arm python GUI
#  * * * * *  bash /home/pi/restart_gui_check.sh 2>&1 /dev/null
#
#-------
#
# also, before setting up the cron job:
#  ln -s ${ROBOT_DIR}/python_client/restart_gui_check.sh ~/


export EPICS_BASE_BIN=/usr/local/epics/base/bin/linux-arm
export CAGET=${EPICS_BASE_BIN}/caget
export result=`${CAGET} -tn xxx:A1:all_stop`

export ROBOT_DIR=/usr/local/epics/epicsEdgeRoboArm
export IOC_TOP=${ROBOT_DIR}/edgeRoboArmIOC/support/xxx-5-6
export ST_CMD_DIR=${IOC_TOP}/iocBoot/iocLinux


if [ "#${result}" == "#0" -o "#${result}" == "#1" ]; then
  #echo "<${usb_connect}>"
  export gui_process=`ps -ef | grep gui_robot.py | grep -v grep`
  if [ "${gui_process}" == "" ]; then
    cd ${ROBOT_DIR}/python_client
    /usr/bin/python ./gui_robot.py &
  fi
fi
