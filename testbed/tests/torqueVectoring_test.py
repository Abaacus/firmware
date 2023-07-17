import time
import slash

# import HIL_Firmware.globals as globals
from hil_constants import HIL_wip_workaround
# from hil_common import Car
from testbeds.hil_testbed import teststand
from drivers.common_drivers.can_driver import HILBoard, VehicleBoard, Vehicle


# variables for testing
sleepDurationMs = 0.2
tolerance = 5

class Car:
    def __init__(self):
        self.vehicle = Vehicle()

    def set_EM_enable(self):
        try:
            vcu_hil.set_signal("Brake_position", {"Brake_position": 3300})
            self.vehicle.send_CAN_message("DCU_buttonEvents", {"ButtonEMEnabled": 1} )
            self.vehicle.send_CAN_message("DCU_buttonEvents", {"ButtonHVEnabled": 1} )
            time.sleep(0.5)
            vcu_hil.set_signal("Brake_position", {"Brake_position": 0})
        except:
            print("set_EM_enable asd message NOT sent")
            return False
        
    def set_TC_enable(self):
        try:
            self.vehicle.send_CAN_message("ButtonTCEnabled", {"ButtonTCEnabled": 1})
        except:
            print("set_TC_enable message NOT sent")
            return False

    def set_endurance_enable(self):
        try:
            self.vehicle.send_CAN_message("ButtonEnduranceToggleEnabled", {"ButtonEnduranceToggleEnabled": 1})
        except:
            print("set_endurance_enable message NOT sent")
            return False
            

def test_TV_setup(teststand):
    print(teststand.hil_boards)
    vcu_hil: HILBoard = teststand.hil_boards["vcu_hil"]
    vcu: VehicleBoard = teststand.vehicle_boards["vcu"]
    # globals.init()
    #assert globals.HIL_wip_workaround, "Error: workaround is set to 0"
    car = Car()
    car.set_EM_enable()

    # set 50% throttle
    throttleA_mV = 2090
    throttleB_mV = 1130
    vcu_hil.set_signal("Throttle_position_A", {"Throttle_position_A": throttleA_mV})
    time.sleep(sleepDurationMs)
    assert vcu_hil.get_signal("Throttle_A_status")

    vcu_hil.set_signal("Throttle_position_B", {"Throttle_position_B": throttleB_mV})
    time.sleep(sleepDurationMs)
    assert vcu_hil.get_signal("Throttle_B_status")

    vcu_hil.set_signal("Brake_position", {"Brake_position": 3300})
    time.sleep(sleepDurationMs)
    assert vcu_hil.get_signal("Brake_pos_status")

'''
# test 1: steering angle in dead zone (0)
def test_TV_deadzone():
    #print(teststand.hil_boards)
    vcu_hil: HILBoard = teststand.hil_boards["vcu_hil"]
    vcu: VehicleBoard = teststand.vehicle_boards["vcu"]
    vcu_hil.set_signal("Steering_raw", {"Steering_raw": 1650})
    time.sleep(sleepDurationMs)
    assert vcu_hil.get_signal("Steering_status")

    torqueDemandR = vcu.get_signal("TorqueDemandRight")
    torqueDemandL = vcu.get_signal("TorqueDemandLeft")
    assert torqueDemandR == torqueDemandL,\
    f"torqueDemandR <{torqueDemandR}>, torqueDemandL <{torqueDemandL}>"

# test 2 & 3: steering angle fully right and left
@slash.parametrize(('testValue', 'expR', 'expL'), [(3300, 2.50, 27.50), (0, 27.50, 2.50)])
def test_TV_100R(testValue, expR, expL):
    #print(teststand.hil_boards)
    vcu_hil: HILBoard = teststand.hil_boards["vcu_hil"]
    vcu: VehicleBoard = teststand.vehicle_boards["vcu"]
    vcu_hil.set_signal("Steering_raw", {"Steering_raw": testValue})
    time.sleep(sleepDurationMs)
    assert vcu_hil.get_signal("Steering_status")

    torqueDemandR = vcu.get_signal("TorqueDemandRight")
    torqueDemandL = vcu.get_signal("TorqueDemandLeft")
    assert abs(torqueDemandR - expR) <= tolerance,\
    f"torqueDemandR <{torqueDemandR}> exceeds tolerance <{tolerance}>"
    assert abs(torqueDemandL - expL) <= tolerance,\
    f"torqueDemandL <{torqueDemandL}> exceeds tolerance <{tolerance}>"
'''