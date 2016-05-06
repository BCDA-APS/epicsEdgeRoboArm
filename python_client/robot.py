#!/usr/bin/env python

import time
import epics


STOP_MOVE = 0
POSITIVE_MOVE = 1
NEGATIVE_MOVE = 2
LED_ON = 1
LED_OFF = 0
MOVE_DIRECTION = {False: NEGATIVE_MOVE, True: POSITIVE_MOVE}


led = epics.PV('xxx:A1:led_onoff')
base = epics.PV('xxx:A1:base_move')
elbow = epics.PV('xxx:A1:elbow_move')
grip = epics.PV('xxx:A1:grip_move')
shoulder = epics.PV('xxx:A1:shoulder_move')
wrist = epics.PV('xxx:A1:wrist_move')
allstop = epics.PV('xxx:A1:all_stop')

BASE_NAME_INDEX = 0
SHOULDER_NAME_INDEX = 1
ELBOW_NAME_INDEX = 2
WRIST_NAME_INDEX = 3
GRIP_NAME_INDEX = 4
AXIS_NAMES = ('M1 base', 'M2 shoulder', 'M3 elbow', 'M4 wrist', 'M5 grip')
AXES = {
    'M1 base': base,
    'M2 shoulder': shoulder,
    'M3 elbow': elbow,
    'M4 wrist': wrist,
    'M5 grip': grip,
}


sleep = time.sleep


def wait_PV_connection(retries = 25, short_pause_s = 0.1):
    '''wait for all the PVs to connect with EPICS'''
    for _ in range(retries):
        if base.connected:
            return
        sleep(short_pause_s)
    if not base.connected:
        raise RuntimeError('PVs not connected')


def led_on():
    '''turn the LED on'''
    led.put(LED_ON)


def led_off():
    '''turn the LED off'''
    led.put(LED_OFF)


def move(axis, seconds):
    '''
    move the named axis for *seconds*
    
    :param str axis: motor name, requires name in AXES.keys()
    :param str axis: move duration, positive when :math:`seconds>0`
    '''
    way = MOVE_DIRECTION[seconds>=0]
    if abs(seconds) > 0:
        axis.put(way)
        sleep(abs(seconds))
        axis.put(STOP_MOVE)


def exercise():
    '''operate each axis of the robot arm'''
    print "Basic robot test"
    test_time_s = 0.5
    pause = 1.0
    for name in AXIS_NAMES:
        axis = AXES[name]
        print "move +:", name
        move(axis, test_time_s)
        sleep(pause)
        print "move -:", name
        move(axis, -test_time_s)
        sleep(pause)

    print "LED <<<on>>>"
    led_on()
    sleep(pause)
    print "LED ...off..."
    led_off()


def main():
    '''command-line interface'''
    wait_PV_connection()
    exercise()


if __name__ == '__main__':
    main()
