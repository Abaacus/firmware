import os
import can
import cantools
import math
import time
from testbeds.hil_testbed import teststand
from drivers.common_drivers.can_driver import HILBoard, VehicleBoard


MAX_VOLTAGE_V = 3.3

bus = can.Bus(bustype='vector', app_name='CANalyzer', channel=0, bitrate=500000)
hil_dbc = cantools.database.load_file(r'HIL_Firmware\common\data\HIL.dbc')
vcu_hil = cantools.tester.Tester('VCU_HIL', hil_dbc, bus)
vcu_hil.start()

#voltage in V
def setThrottleA(voltage):
    if(voltage>MAX_VOLTAGE_V):
        voltage = MAX_VOLTAGE_V
    elif(voltage < 0):
        voltage = 0
    voltage= math.trunc(voltage*1000)
    try:
        vcu_hil.send("ThrottlePositionA", {"ThrottlePositionA": voltage})
        print(f"message sent on {bus.channel_info}")
    except:
        print("message NOT sent")
        return False
    try:
        vcu_hil.expect('VCU_MessageStatus', None,0.01)
        print("message received")
        return True
    except can.CanError:
        print("status message not received!")
        return False

def setThrottleB(voltage):
    if(voltage>MAX_VOLTAGE_V):
        voltage = MAX_VOLTAGE_V
    elif(voltage < 0):
        voltage = 0
    voltage= math.trunc(voltage*1000)
    try:
        vcu_hil.send("ThrottlePositionB", {"ThrottlePositionB": voltage})
        print(f"message sent on {bus.channel_info}")
    except:
        print("message NOT sent")
        return False
    try:
        vcu_hil.expect('VCU_MessageStatus', None,0.01)
        print("message received")
        return True
    except can.CanError:
        print("status message not received!")
        return False

def setSteering(voltage):
    if(voltage>MAX_VOLTAGE_V):
        voltage = MAX_VOLTAGE_V
    elif(voltage < 0):
        voltage = 0
    voltage= math.trunc(voltage*1000)
    try:
        vcu_hil.send("SteeringRaw", {"SteeringRaw": voltage})
        print(f"message sent on {bus.channel_info}")
    except:
        print("message NOT sent")
        return False
    try:
        vcu_hil.expect('VCU_MessageStatus', None,0.01)
        print("message received")
        return True
    except can.CanError:
        print("status message not received!")
        return False

def setBrakePres(voltage):
    if(voltage>MAX_VOLTAGE_V):
        voltage = MAX_VOLTAGE_V
    elif(voltage < 0):
        voltage = 0
    voltage= math.trunc(voltage*1000)
    try:
        vcu_hil.send("BrakePresRaw", {"BrakePresRaw": voltage})
        print(f"message sent on {bus.channel_info}")
    except:
        print("message NOT sent")
        return False
    try:
        vcu_hil.expect('VCU_MessageStatus', None,0.01)
        print("message received")
        return True
    except can.CanError:
        print("status message not received!")
        return False

def setBrakePosition(voltage):
    if(voltage>MAX_VOLTAGE_V):
        voltage = MAX_VOLTAGE_V
    elif(voltage < 0):
        voltage = 0
    voltage= math.trunc(voltage*1000)
    try:
        vcu_hil.send("BrakePosition", {"BrakePosition": voltage})
        print(f"message sent on {bus.channel_info}")
    except:
        print("message NOT sent")
        return False
    try:
        vcu_hil.expect('VCU_MessageStatus', None,0.01)
        print("message received")
        return True
    except can.CanError:
        print("status message not received!")
        return False