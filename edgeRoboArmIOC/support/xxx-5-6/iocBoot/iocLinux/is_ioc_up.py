#!/usr/bin/env python

import epics
pv = 'xxx:A1:led_onoff'
epics.caget(pv, timeout=1)
