
========
Overview
========

.. compound::

    .. _fig.robot-arm-stock:

    .. figure:: graphics/robot-arm-stock.jpg
        :alt: OWI-535 Edge Robotic Arm

        OWI-535 Edge Robotic Arm

The OWI-535 Edge Robotic Arm is a child's toy that is built from a kit. [#]_
The arm has some interesting specifications:

* four motorized rotary axes: base, shoulder, elbow, wrist
* one motorized grip
* LED
* hand-held switch box
* maximum 100g load

It's a fun learning device.
The robot arm has its limitations that make it useless for any 
practical robotics implementation:

* no twist axis for the wrist
* no limit switches
* no encoders
* DC motor speed depends on power available from batteries

An optional USB interface [#]_ is available, providing a Windows application
to operate the robot.  The USB command protocol was deciphered and posted online
by a third party, enabling communication from a Linux computer.


USB protocol
------------

Device appears on Linux as::

  Bus 005 Device 007: ID 1267:0000 Logic3 / SpectraVideo plc

A simple USB vendor control transfer of three bytes appears to be the
entire control method. The bits in these bytes appear to directly
control the physical lines of the microcontroller. Effectively the
microcontroller is behaving as nothing more than a USB attached I/O
expander.

* Bits 0-7 control the LED (0 for off 0xff for on) 
* Bits 8-15 turn a motor output on (direction is just done by having two switches per motor)

The bits in the motor bytes are used in pairs as inputs to ST1152
motor controllers. The truth table for these controllers is::

    Input | Motor
    A | B | Output 
   ---+---+-------
    0 | 0 | Idle
    0 | 1 | Forwards
    1 | 0 | Backwards
    1 | 1 | Brake

The windows software only ever uses 00, 01 and 10 i.e. it never
applies a brake signal.  To summarize, bits 0 and 1 control the 
first motor, bits 2 and 3 the
second and so on for all five motors. This leaves bits 10-15 unused.

2012 ANL Energy Showcase - First Demo of EPICS IOC
--------------------------------------------------

.. compound::

    .. _fig.2012-aps-demo:

    .. figure:: graphics/2012-09-16-7996170862_f09f62a379_o.jpg

        Robotic Arm demo at 2012 ANL Energy Showcase


In preparation for the 2012 Argonne National Laboratory Energy Showcase 
(an open house for the community [#]_), 
the BCDA group [#] created linux-based
EPICS controls [#]_ for the robot arm 
to simulate how robots install samples into X-ray detectors at 
several of the APS experiment area beamlines. The robots allow for faster sample loading 
and enable scientists to use the APS while at their home institutions. 

Using a Raspberry Pi as the Linux IOC host and EPICS, 
this hands-on IOC demonstrates how modestly a "complete" 
control system might be constructed.  A GUI can be added on the network
for alternative control of the robot.


.. _overview.demo.joystick:

Demo System with joystick
-------------------------

Recently, USB joystick control was added to the IOC [#]_
which enables truly headless (no GUI needed) operations.  
In the photo here, a wooden marble tree instrument [#]_
is used to provide an interesting target for the robot arm actions.

.. sidebar:: photo

   .. figure:: graphics/20150513_164853.jpg
      :width: 12cm
      
      photograph of demo system, with marble tree instrument


.. compound::

    .. _fig.headless-ioc_joystick:

    .. figure:: graphics/arm-IOC-joystick.png
      :width: 8cm
      
      Basic connection schematic for headless 
      IOC and operation using a joystick

-----------

.. [#] official site: 
       http://www.owirobot.com/robotic-arm-edge-1/
.. [#] USB kit: 
       https://www.owirobot.com/products/USB-Interface-for-Robotic-Arm-Edge.html
.. [#] libusb program for Linux : http://www.kyllikki.org/rbtarm.c

.. [#] 2012 ANL Energy Showcase: 
       https://www.flickr.com/photos/argonne/7996170862/in/album-72157631558448229
.. [#] BCDA: http://www.aps.anl.gov/bcda
.. [#] First IOC was created by Jeff Gebhardt, APS BCDA group

.. [#] EPICS IOC joystick control added to the IOC by Keenan Lang, APS, BCDA group
.. [#] marble tree: http://www.berea.com/appalachian-fireside-gallery/

.. - - - - -
