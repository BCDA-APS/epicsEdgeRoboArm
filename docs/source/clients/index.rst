
.. index:: EPICS; clients


===============
User Interfaces
===============

Since the robot arm does not have encoders, position limit switches,
or other position sensors, there is no ability to determine position.
The controls for the robot are provided through these 6 PVs:

========================  ========  =======================
EPICS PV name             PV RTYP   values
========================  ========  =======================
``xxx:A1:led_onoff``      bo        OFF=0, ON=1
``xxx:A1:base_move``      mbbo      STOP=0, CW=1, CCW=2
``xxx:A1:shoulder_move``  mbbo      STOP=0, UP=1, DOWN=2
``xxx:A1:elbow_move``     mbbo      STOP=0, UP=1, DOWN=2
``xxx:A1:wrist_move``     mbbo      STOP=0, UP=1, DOWN=2
``xxx:A1:grip_move``      mbbo      STOP=0, CLOSE=1, OPEN=2
========================  ========  =======================

These client interfaces have been demonstrated:

.. toctree::
   :maxdepth: 1
   
   cssboy
   python
   joystick
