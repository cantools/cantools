#!/usr/bin/env python3
#
# Script testing the motor.
#

import os
import cantools
import can


SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
WASHING_MACHINE_KCD_PATH = os.path.join(SCRIPT_DIR, 'system.kcd')

can.rc['interface'] = 'socketcan'
can.rc['channel'] = 'vcan0'

can_bus = can.interface.Bus()
database = cantools.db.load_file(WASHING_MACHINE_KCD_PATH)
tester = cantools.tester.Tester('Motor',
                                database,
                                can_bus,
                                'MainBus')
tester.start()

status = tester.expect('MotorStatus')
print('Motor speed is {Speed} rpm and load is {Load}%.'.format(**status))

print('Setting motor speed to 50 rpm.')
tester.send('MotorControl', {'Speed': 50})

status = tester.expect('MotorStatus', {'Speed': 50, 'Load': 12})
print('Motor speed is {Speed} rpm and load is {Load}%.'.format(**status))

print('Setting motor speed to 0 rpm.')
tester.send('MotorControl', {'Speed': 0})

status = tester.expect('MotorStatus', {'Speed': 0})
print('Motor speed is {Speed} rpm and load is {Load}%.'.format(**status))

print('Setting motor speed to 65535 rpm to terminate it.')
tester.send('MotorControl', {'Speed': 65535})

print('Done!')
