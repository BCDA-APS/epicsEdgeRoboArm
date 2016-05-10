#!/usr/bin/env python

import sys
from PyQt4 import QtCore, QtGui
pyqtSignal = QtCore.pyqtSignal
import robot


class Axis(QtGui.QWidget):
    '''GUI controls for motorized axes'''

    def __init__(self, parent = None):
        QtGui.QWidget.__init__(self, None)

        self.axis = None
        self.text = 'label'
        
        self.layout = QtGui.QHBoxLayout()
        
        self.label = QtGui.QLabel(self.text)
        self.down = QtGui.QPushButton('down', self)
        self.up = QtGui.QPushButton('up', self)
        
        self.layout.addWidget(self.label, stretch=1)
        self.layout.addWidget(self.down, stretch=1)
        self.layout.addWidget(self.up, stretch=1)
        
        self.down.pressed.connect(self.onDown)
        self.up.pressed.connect(self.onUp)
        self.down.released.connect(self.onRelease)
        self.up.released.connect(self.onRelease)
        
        self.setLayout(self.layout)

    def setAxis(self, axis):
        self.axis = axis

    def setLabel(self, text):
        self.text = text
        self.label.setText(self.text)

    def onDown(self, event=None):
        if self.axis is not None:
            self.axis.put(robot.NEGATIVE_MOVE)

    def onUp(self, event=None):
        if self.axis is not None:
            self.axis.put(robot.POSITIVE_MOVE)

    def onRelease(self, event=None):
        if self.axis is not None:
            self.axis.put(robot.STOP_MOVE)


class Led(Axis):
    '''GUI controls for the LED'''

    def __init__(self, parent = None):
        Axis.__init__(self, None)

        self.pulse = QtGui.QPushButton('pulse (&p)')
        self.layout.addWidget(self.pulse, stretch=1)
        
        self.pulse.pressed.connect(self.onUp)
        self.pulse.released.connect(self.onRelease)
        self.up.pressed.disconnect()
        self.up.released.disconnect()
        self.up.pressed.connect(self.onUp)
        
        self.down.setText('off (&-)')
        self.up.setText('on (&+)')
        self.setLabel('LED')

    def onPulse(self, event=None):
        self.onUp()

    def onDown(self, event=None):
        self.onRelease()

    def onUp(self, event=None):
        if self.axis is not None:
            robot.led_on()

    def onRelease(self, event=None):
        if self.axis is not None:
            robot.led_off()


class MainWindow(QtGui.QWidget):
    '''main GUI'''

    # allow to move motors by keypresses (could move more than one motor at once!)
    movekeyPressed = pyqtSignal(QtCore.QEvent)
    movekeyReleased = pyqtSignal(QtCore.QEvent)

    # allow to blink LED by keypresses
    ledkeyPressed = pyqtSignal(QtCore.QEvent)
    ledkeyReleased = pyqtSignal(QtCore.QEvent)
    
    # these are the keys that trigger the events
    keydict = {
        QtCore.Qt.Key_L: robot.AXIS_NAMES[0],   # move base +
        QtCore.Qt.Key_R: robot.AXIS_NAMES[0],   # move base -
        QtCore.Qt.Key_2: robot.AXIS_NAMES[1],   # move shoulder + 
        QtCore.Qt.Key_S: robot.AXIS_NAMES[1],   # move shoulder -
        QtCore.Qt.Key_3: robot.AXIS_NAMES[2],   # move elbow +
        QtCore.Qt.Key_E: robot.AXIS_NAMES[2],   # move elbow -
        QtCore.Qt.Key_4: robot.AXIS_NAMES[3],   # move wrist +
        QtCore.Qt.Key_W: robot.AXIS_NAMES[3],   # move wrist -
        QtCore.Qt.Key_5: robot.AXIS_NAMES[4],   # move grip +
        QtCore.Qt.Key_G: robot.AXIS_NAMES[4],   # move grip -
    }
    positive_keys = (QtCore.Qt.Key_L, QtCore.Qt.Key_2, QtCore.Qt.Key_3, 
                     QtCore.Qt.Key_4, QtCore.Qt.Key_5)

    def __init__(self):
        QtGui.QWidget.__init__(self, None)
        
        layout = QtGui.QVBoxLayout()
        
        title = QtGui.QLabel('Robot Arm Controls')
        layout.addWidget(title, stretch=1)

        robot.wait_PV_connection()

        widgets = {}
        for axis in robot.AXIS_NAMES:
            w = Axis()
            w.setAxis(robot.AXES[axis])
            w.setLabel(axis)
            layout.addWidget(w, stretch=1)
            widgets[axis] = w
        led = Led()
        led.setAxis(robot.led)
        layout.addWidget(led, stretch=1)

        # relabel the buttons
        axis_button_labels = { # values = (down_button_text, up_button_text)
            robot.BASE_NAME_INDEX: ('right (&r)', 'left (&l)'),
            robot.SHOULDER_NAME_INDEX: ('down (&s)', 'up (&2)'),
            robot.ELBOW_NAME_INDEX: ('down (&e)', 'up (&3)'),
            robot.WRIST_NAME_INDEX: ('down (&w)', 'up (&4)'),
            robot.GRIP_NAME_INDEX: ('open (&g)', 'closed (&5)'),   
        }
        for axis, button_text in axis_button_labels.items():
            axis_name = robot.AXIS_NAMES[axis]
            widget = widgets[axis_name]
            widget.down.setText(button_text[0])
            widget.up.setText(button_text[1])

        self.installEventFilter(self)
        self.movekeyPressed.connect(self.onMoveKeyPressed)
        self.movekeyReleased.connect(self.onMoveKeyReleased)
        self.ledkeyPressed.connect(self.onLedKeyPressed)
        self.ledkeyReleased.connect(self.onLedKeyReleased)
        self.axis_states = {}
        for item in robot.AXIS_NAMES:
            self.axis_states[item] = robot.AXES[item].get()
        
        self.setLayout(layout)
        self.setWindowState(QtCore.Qt.WindowMaximized)

    def onLedKeyPressed(self, event):
        '''turn on the LED'''
        robot.led_on()

    def onLedKeyReleased(self, event):
        '''turn off the LED'''
        robot.led_off()

    def onMoveKeyPressed(self, event):
        '''send a move command when key is pressed'''
        movement = robot.MOVE_DIRECTION[event.key() in self.positive_keys]
        axis = self.keydict[event.key()]
        if self.axis_states[axis] == robot.STOP_MOVE:
            robot.AXES[axis].put(movement)
            self.axis_states[axis] = movement

    def onMoveKeyReleased(self, event):
        '''send a STOP when key is released'''
        movement = robot.MOVE_DIRECTION[event.key() in self.positive_keys]
        axis = self.keydict[event.key()]
        if self.axis_states[axis] == movement:
            robot.AXES[axis].put(robot.STOP_MOVE)
            self.axis_states[axis] = robot.STOP_MOVE

    def eventFilter(self, watched, event):
        '''filter all events, watching for key presses on *movement* keys'''
        if event.type() == QtCore.QEvent.KeyPress:
            if event.key() in self.keydict.keys():
                self.movekeyPressed.emit(event)
                return True
            elif event.key() in (QtCore.Qt.Key_P,):
                self.onLedKeyPressed(event)
                return True
            elif event.key() in (QtCore.Qt.Key_Plus,):
                robot.led_on()
                return True
            elif event.key() in (QtCore.Qt.Key_Minus,):
                robot.led_off()
                return True
        if event.type() == QtCore.QEvent.KeyRelease:
            if event.key() in self.keydict.keys():
                self.movekeyReleased.emit(event)
                return True
            elif event.key() in (QtCore.Qt.Key_P, ):
                self.ledkeyReleased.emit(event)
                return True
        return False


def main():
    '''GUI starts here'''
    app = QtGui.QApplication(sys.argv)
    w = MainWindow()
    w.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()

# run by  crontab -e
#              field          allowed values
#              -----          --------------
#              minute         0-59
#              hour           0-23
#              day of month   1-31
#              month          1-12 (or names, see below)
#	       day of week    0-7 (0 or 7 is Sun, or use names)
#
#  # auto-start the robotic arm GUI
#  * * * * *  /home/pi/restart_gui_check.sh 2>&1 /dev/null
