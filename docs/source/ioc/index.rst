
.. index:: EPICS; IOC


=========
EPICS IOC
=========

The IOC must be run as root.  EPICS base 3.1412.1 (or higher) is required.
The support is provided by modifying synApps v5.6 [#]_, 
removing modules that are not used, and adding support where appropriate.

.. [#] synApps: http://www.aps.anl.gov/bcda/synApps/

USB
---

The Linux host must provide a libusb support library.
USB communications must be performed by root, so the IOC must run as root. 

:uses: *drvAsynUSBPort.c*  : Asyn device support using local usb interface

.. compound::

   .. rubric:: asyn configuration of USB communications in IOC's ``st.cmd`` file
   
   .. code-block:: guess
      :linenos:

      drvAsynUSBPortConfigure("USB1", "Robotic Arm", 0x1267, 0, 0, 0, 0, 1)
      asynOctetConnect("USB1", "USB1")


Databases
---------

All actions of the robot are provided through a single EPICS database:
``edgeRoboArmIOC/support/ip-2-13/ipApp/Db/roboArm.db``

The USB communication is controlled by *asyn* through a single PV:

.. compound::

   .. rubric:: USB communication through *asyn*
   
   .. code-block:: guess
      :linenos:

      record(stringout, "$(P)$(A)send_cmd_str") {
        field(DESC, "send the motion command string")
        field(DTYP, "asyn robo stringParm")
        field(OUT, "@asyn($(PORT))")
      }

The bit position of each motion axis is encoded in the database, such as:

.. compound::

   .. rubric:: bit command of each axis is encoded: elbow UP=16, DOWN=32, STOP=0
   
   .. code-block:: guess
      :linenos:

      record(mbbo,  "$(P)$(A)elbow_move") {
        field(DESC, "elbow motion")
        field(DTYP, "Raw Soft Channel")
        field(ZRST, "STOP")
        field(ZRVL, "0")
        field(ONST, "UP")
        field(ONVL, "16")
        field(TWST, "DOWN")
        field(TWVL, "32")
        field(FLNK, "$(P)$(A)send_cmd.PROC  PP MS")
      }

Commands for all axes are aggregated in these two records:

.. compound::

   .. rubric:: USB command assembled in two records
   
   .. code-block:: guess
      :linenos:

      record(calc,  "$(P)$(A)send_cmd") {
        field(DESC, "send the motion command")
        field(INPA, "$(P)$(A)grip_move.RVAL      NPP NMS")
        field(INPB, "$(P)$(A)wrist_move.RVAL     NPP NMS")
        field(INPC, "$(P)$(A)elbow_move.RVAL     NPP NMS")
        field(INPD, "$(P)$(A)shoulder_move.RVAL  NPP NMS")
        field(CALC, "A+B+C+D")
        field(FLNK, "$(P)$(A)send_cmd_2.PROC      PP  MS") 
      }
      
      record(scalcout, "$(P)$(A)send_cmd_2") {
        field(DESC, "send the motion command")
        field(INPA, "$(P)$(A)send_cmd.VAL        NPP NMS")
        field(INPB, "$(P)$(A)base_move.RVAL      NPP NMS")
        field(INPC, "$(P)$(A)led_onoff.VAL       NPP NMS")
        field(CALC, "STR(A)+' '+STR(B)+' '+STR(C)")
        field(OUT,  "$(P)$(A)send_cmd_str.VAL     PP  MS") 
      }


IOC startup
-----------

A standard *xxx* IOC from synApps was used to create the IOC for the robot.
All configuration details are provided in the *st.cmd* and related scripts.
The IOC is started by running the bash script 
``edgeRoboArmIOC/support/xxx-5-6/iocBoot/iocLinux/run``.  
An additional script is provided to run the IOC in a detached *screen* session:
``in-screen.sh``.

cron task
---------

A bash script was created to be run as a periodic (once a minute) 
*cron* task, checking to see if the IOC is not running.  If not 
running, it checks if the robot's USB connection is detected and 
then tries to start the IOC.  With this task running, the EPICS 
IOC starts automatically after the Linux OS is booted and the robot arm 
is connected by USB.  The file is stored in the startup directory:
``edgeRoboArmIOC/support/xxx-5-6/iocBoot/iocLinux/restart_ioc_check.sh``

.. compound::

    .. rubric:: **restart_ioc_check.sh**

    .. literalinclude:: ../../../edgeRoboArmIOC/support/xxx-5-6/iocBoot/iocLinux/restart_ioc_check.sh
       :tab-width: 4
       :linenos:
       :language: guess


SNL state program (optional)
----------------------------

In an attempt to automate the actions of the robot arm in 
a programmed sequence, Jeff Gebhardt wrote a state notation 
language sequence program (and accompanying database).
The automation allows for move sequences up to five steps.
This support can be found in:

* ``edgeRoboArmIOC/support/ip-2-13/ipApp/Db/roboArmSeq.db``
* ``edgeRoboArmIOC/support/ip-2-13/ipApp/Db/roboArmSeq_settings.req``
* ``edgeRoboArmIOC/support/ip-2-13/ipApp/src/RoboArm.st``

A movie was created showing the robot locating, grasping, and 
lifting a toy block, then dropping it into a nearby coffee cup.

To accomplish this, the batteries were new and
the robot, block, and coffee cup were placed 
in a known starting position.

Moves were programmed based on elapsed time.
Due to lack of feedback encoding, backlash and windup of the motor gears, 
and unreliable positioning based on battery power available for a given time of movement,
it is not realistic to program any sequence of more than 5 waypoints.

In short, we were lucky to get a good video.  
Took some careful work to be that lucky.

GUI support
-----------

Initial user interfaces created were:

* CSS BOY
* MEDM

Screens are provided for each.

   *Interesting to note the first "user" at the 2012 ANL Energy Showcase
   was a six-year old child who wanted to press the CSS BOY screen button
   directly with her finger, completely ignoring the offered mouse interface to the GUI.*  

(Now, with touch-screen laptops, the CSS BOY
interface can be tested for multi-touch compatibility.)

Later, a Python GUI was created to work on the Raspberry Pi.
This interface allowed the use of keyboard bindings to each of the
GUI buttons.  
From this keyboard binding interface, a true multitouch capability was added.

Joystick support
----------------

Keenan Lang, APS BCDA group, had developed an HMI module to allow human-machine
interface devices such as mice, keyboards, and joysticks (and other) to
communicate directly into an EPICS IOC.  In a few hours, he added that
support to the robot IOC project so that a particular joystick can be used
to control the robot arm directly within the IOC.

With added joystick control in the IOC, it is not necessary to require a 
KVM GUI (video screen + keyboard + mouse) to operate the robot.

Now, the LED feature on the robot arm becomes useful!
Verify the IOC is running by pulsing the LED with the programmed button on the joystick.
Once that works, the joystick is now ready to be used.
