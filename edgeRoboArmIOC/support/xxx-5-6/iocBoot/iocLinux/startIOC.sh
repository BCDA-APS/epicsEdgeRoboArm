#!/bin/sh

#
# startIOC.sh v 1.2 08/04/2012 jrg
#

##    ------------ PATHS TO EPICS TOOLS -----------------
## 
## Path to epics base, extensions and synApps 

if [ -z "${MY_EPICS_BASE}" ] ; then
	MY_EPICS_BASE=/usr/local/epics/base
fi
if [ -z "${MY_EPICS_EXTENSIONS}" ] ; then
	MY_EPICS_EXTENSIONS=/usr/local/epics/extensions
fi
if [ -z "${MY_EPICS_SYNAPPS}" ] ; then
	MY_EPICS_SYNAPPS=/usr/local/epics/synApps
fi

##    --------------- EPICS HOST ARCH -------------------
##

EPICS_HOST_ARCH=

##    ------------- EPICS DISPLAY PATH ------------------
##

EPICS_DISPLAY_PATH=

PATH=${PATH}:${MY_EPICS_BASE}/bin/${EPICS_HOST_ARCH}
PATH=${PATH}:${MY_EPICS_EXTENSIONS}/bin/${EPICS_HOST_ARCH}
PATH=${PATH}:${MY_EPICS_SYNAPPS}/support/utils

export EPICS_HOST_ARCH
export PATH
if [ -z %1 ] ; then
    export EPICS_DISPLAY_PATH
    medm -x -noMsg -macro "P=xxx:, A=A1:" roboArm.adl &
fi
../../bin/$EPICS_HOST_ARCH/xxx st.cmd
