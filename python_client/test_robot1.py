#!/usr/bin/env python

from robot import *

sleep(2)

hits = []
for name in AXIS_NAMES:
    axis = AXES[name]
    if not axis.connected:
        hits.append(name)
if len(hits) > 0:
    print str(hits), 'PVs not connected'
    exit()

led_on()
move(base, 0.2)
sleep(1)
led_off()
move(base, -0.2)
led_on()
sleep(0.25)
led_off()
sleep(.5)

