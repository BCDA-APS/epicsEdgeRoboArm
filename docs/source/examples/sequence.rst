=====================
Programmable Sequence
=====================

.. provide a link to the automation movie online.
   https://vimeo.com/epicsEdgeRoboArm
   https://vimeo.com/128020522
   preview: https://vimeo.com/user40225047/epicsedgeroboarm

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

A movie of the automation sequence is available online:
https://vimeo.com/128020522

Database, sequence, and GUI support are provided in this IOC project
under the ``ip-2-13`` subdirectory.

-----------

.. [#] 2012 ANL Energy Showcase: 
       https://www.flickr.com/photos/argonne/7996170862/in/album-72157631558448229
.. [#] BCDA: http://www.aps.anl.gov/bcda
.. [#] First IOC was created by Jeff Gebhardt, APS BCDA group
