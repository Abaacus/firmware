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
    def __init__(self, vcu_hil, vcu):
        self.vehicle = Vehicle()
        self.vcu_hil = vcu_hil
        self.vcu = vcu

    def set_EM_enable(self):
        try:
            self.vcu_hil.set_signal("Throttle_position_A", {"Throttle_position_A": 2307}) #calculated 1958
            self.vcu_hil.set_signal("Throttle_position_B", {"Throttle_position_B": 936}) #calculated 991
            self.vcu_hil.set_signal("Brake_pres_raw", {"Brake_pres_raw": 1000})
            self.vcu_hil.set_signal("Brake_position", {"Brake_position": 3300})
            self.vehicle.send_CAN_message("BMU_HV_Power_State", {"HV_Power_State": 1} )
            self.vehicle.send_CAN_message("TempInverterLeft", {"PRO_CAN_CRC": 0, "PRO_CAN_COUNT": 0, "PRO_CAN_RES": 0, "InverterAUTHSEEDLeft": 0,\
                                                               "TempInverterLeft": 0, "TempInverterDeltaLeft": 0, "StateInverterLeft": 0x18})
            self.vehicle.send_CAN_message("TempInverterRight", {"PRO_CAN_CRC": 0, "PRO_CAN_COUNT": 0, "PRO_CAN_RES": 0, "InverterAUTHSEEDRight": 0,\
                                                               "TempInverterRight": 0, "TempInverterDeltaRight": 0, "StateInverterRight": 0x18})
            time.sleep(0.05)
            self.vehicle.send_CAN_message("DCU_buttonEvents", {"ButtonEMEnabled": 1, "ButtonHVEnabled": 1, "ButtonTCEnabled": 0, "ButtonEnduranceLapEnabled": 0, "ButtonEnduranceToggleEnabled":0 } )
            self.vehicle.send_CAN_message("PDU_ChannelStatus", {"StatusPowerVCU": 0, "StatusPowerMCRight": 2, "StatusPowerMCLeft": 2, \
                                                                 "StatusPowerIMD": 0, "StatusPowerDCU": 0, "StatusPowerDAU": 0, "StatusPowerCoolingPumpRight" : 0,\
                                                                 "StatusPowerCoolingPumpLeft": 0, "StatusPowerCoolingFanRight": 0, "StatusPowerCoolingFanLeft": 0,\
                                                                 "StatusPowerCoolingFanBattery": 0, "StatusPowerBMU": 0})
           
            self.vcu_hil.set_signal("Brake_position", {"Brake_position": 0})
        except Exception as e:
            print("set_EM_enable message NOT sent")
            print(e)
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
    car = Car(vcu_hil, vcu)
    car.set_EM_enable()

    # set 50% throttle
    # throttleA_mV = 2090
    # throttleB_mV = 1130
    # vcu_hil.set_signal("Throttle_position_A", {"Throttle_position_A": throttleA_mV})
    # time.sleep(sleepDurationMs)
    # assert vcu_hil.get_signal("Throttle_A_status")

    # vcu_hil.set_signal("Throttle_position_B", {"Throttle_position_B": throttleB_mV})
    # time.sleep(sleepDurationMs)
    # assert vcu_hil.get_signal("Throttle_B_status")

    # vcu_hil.set_signal("Brake_position", {"Brake_position": 3300})
    # time.sleep(sleepDurationMs)
    # assert vcu_hil.get_signal("Brake_pos_status")

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
