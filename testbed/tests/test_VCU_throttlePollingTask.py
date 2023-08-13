import time
import math
from testbeds.hil_testbed import teststand
from drivers.common_drivers.can_driver import HILBoard, VehicleBoard
# variables for testing
sleepDurationMs = 0.2
tolerance = 5

class Car:
    def __init__(self, vcu_hil, vcu):
        self.vehicle = Vehicle()
        self.vcu_hil = vcu_hilc
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




def test_throttle_polling_task(teststand):
    print(teststand.hil_boards)
    vcu_hil: HILBoard = teststand.hil_boards["vcu_hil"]
    vcu: VehicleBoard = teststand.vehicle_boards["vcu"]
    #set up -- EM enable
    car = Car(vcu_hil, vcu)
    car.set_EM_enable()
    

    #tests
    #1
    throttle_at_idle_position()
    #2
    throttle_between_min_and_max()
    #3
    throttle_at_max()
    #4
    throttle_below_5_percent_brake_pressed()
    #5
    throttle_below_25_percent_brake_pressed_hard()
    #6
    throttle_A_out_of_range()
    #7
    throttle_B_out_of_range()
    #8
    throttle_A_out_of_tolerance()
    #9
    throttle_B_out_of_tolerance()


def throttle_at_idle_position():
    throttleA_mV = 1796
    throttleB_mV = 829
    steeringAngle_mV = 1650
    
    vcu_hil.set_signal("Throttle_position_A", {"Throttle_position_A": throttleA_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Throttle_A_status") 

    vcu_hil.set_signal("Throttle_position_B", {"Throttle_position_B": throttleB_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Throttle_B_status")

    vcu_hil.set_signal("Steering_raw", {"Steering_raw": steeringAngle_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Steering_status")

    torqueDemandR = vcu.get_signal("TorqueDemandRight")
    torqueDemandL = vcu.get_signal("TorqueDemandLeft")
    tolerance = 5
    assert math.isclose(torqueDemandR, expectedTR = 0, abs_tol = tolerance)
    assert math.isclose(torqueDemandL, expectedTL = 0, abs_tol = tolerance)
 
def throttle_between_min_and_max():
    throttleA_mV = 2100
    throttleB_mV = 1130
    steeringAngle_mV = 1650
    
    vcu_hil.set_signal("Throttle_position_A", {"Throttle_position_A": throttleA_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Throttle_A_status") 

    vcu_hil.set_signal("Throttle_position_B", {"Throttle_position_B": throttleB_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Throttle_B_status")

    vcu_hil.set_signal("Steering_raw", {"Steering_raw": steeringAngle_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Steering_status")

    torqueDemandR = vcu.get_signal("TorqueDemandRight")
    torqueDemandL = vcu.get_signal("TorqueDemandLeft")
    tolerance = 5
    assert math.isclose(torqueDemandR, expectedTR = 478, abs_tol = tolerance)
    assert math.isclose(torqueDemandL, expectedTL = 491, abs_tol = tolerance)

def throttle_at_max():
    throttleA_mV = 2390
    throttleB_mV = 1420
    steeringAngle_mV = 1650
    
    vcu_hil.set_signal("Throttle_position_A", {"Throttle_position_A": throttleA_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Throttle_A_status") 

    vcu_hil.set_signal("Throttle_position_B", {"Throttle_position_B": throttleB_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Throttle_B_status")

    vcu_hil.set_signal("Steering_raw", {"Steering_raw": steeringAngle_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Steering_status")

    torqueDemandR = vcu.get_signal("TorqueDemandRight")
    torqueDemandL = vcu.get_signal("TorqueDemandLeft")
    tolerance = 5
    assert math.isclose(torqueDemandR, expectedTR = 563, abs_tol = tolerance)
    assert math.isclose(torqueDemandL, expectedTL = 579, abs_tol = tolerance)

def throttle_below_5_percent_brake_pressed():
    #Throttle and brake should have been pressed at the same time as set up for the test. Is brake set through this function?
    throttleA_mV = 1960
    throttleB_mV = 990
    steeringAngle_mV = 1650
    brake_mV = 1150
    
    vcu_hil.set_signal("Throttle_position_A", {"Throttle_position_A": throttleA_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Throttle_A_status") 

    vcu_hil.set_signal("Throttle_position_B", {"Throttle_position_B": throttleB_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Throttle_B_status")

    vcu_hil.set_signal("Steering_raw", {"Steering_raw": steeringAngle_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Steering_status")

    vcu_hil.set_signal("Brake_position", {"Brake_position": brake_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Brake_position")

    torqueDemandR = vcu.get_signal("TorqueDemandRight")
    torqueDemandL = vcu.get_signal("TorqueDemandLeft")
    tolerance = 5
    assert math.isclose(torqueDemandR, expectedTR = 0, abs_tol = tolerance)
    assert math.isclose(torqueDemandL, expectedTL = 0, abs_tol = tolerance)

    #TorqueDemandRight == 0
    #TorqueDemandRight == 0

def throttle_above_5_percent_brake_pressed():
    throttleA_mV = 2100
    throttleB_mV = 1130
    steeringAngle_mV = 1650
    brake_mV = 1150
    
    vcu_hil.set_signal("Throttle_position_A", {"Throttle_position_A": throttleA_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Throttle_A_status") 

    vcu_hil.set_signal("Throttle_position_B", {"Throttle_position_B": throttleB_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Throttle_B_status")

    vcu_hil.set_signal("Steering_raw", {"Steering_raw": steeringAngle_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Steering_status")

    vcu_hil.set_signal("Brake_position", {"Brake_position": brake_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Brake_position")

    torqueDemandR = vcu.get_signal("TorqueDemandRight")
    torqueDemandL = vcu.get_signal("TorqueDemandLeft")
    tolerance = 5
    assert math.isclose(torqueDemandR, expectedTR = 0, abs_tol = tolerance)
    assert math.isclose(torqueDemandL, expectedTL = 0, abs_tol = tolerance)

def throttle_below_25_percent_brake_pressed_hard():
    throttleA_mV = 2100
    throttleB_mV = 1130
    steeringAngle_mV = 1650
    brake_mV = 3300
    
    vcu_hil.set_signal("Throttle_position_A", {"Throttle_position_A": throttleA_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Throttle_A_status") 

    vcu_hil.set_signal("Throttle_position_B", {"Throttle_position_B": throttleB_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Throttle_B_status")

    vcu_hil.set_signal("Steering_raw", {"Steering_raw": steeringAngle_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Steering_status")

    vcu_hil.set_signal("Brake_position", {"Brake_position": brake_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Brake_position")

    torqueDemandR = vcu.get_signal("TorqueDemandRight")
    torqueDemandL = vcu.get_signal("TorqueDemandLeft")
    tolerance = 5
    assert math.isclose(torqueDemandR, expectedTR = 0, abs_tol = tolerance)
    assert math.isclose(torqueDemandL, expectedTL = 0, abs_tol = tolerance)

def throttle_A_out_of_range():
    throttleA_mV = 2410
    throttleB_mV = 1130
    steeringAngle_mV = 1650
    
    vcu_hil.set_signal("Throttle_position_A", {"Throttle_position_A": throttleA_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Throttle_A_status") 

    vcu_hil.set_signal("Throttle_position_B", {"Throttle_position_B": throttleB_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Throttle_B_status")

    vcu_hil.set_signal("Steering_raw", {"Steering_raw": steeringAngle_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Steering_status")

    torqueDemandR = vcu.get_signal("TorqueDemandRight")
    torqueDemandL = vcu.get_signal("TorqueDemandLeft")
    tolerance = 5
    assert math.isclose(torqueDemandR, expectedTR = 0, abs_tol = tolerance)
    assert math.isclose(torqueDemandL, expectedTL = 0, abs_tol = tolerance)

def throttle_B_out_of_range():
    throttleA_mV = 2100
    throttleB_mV = 1450
    steeringAngle_mV = 1650
    
    vcu_hil.set_signal("Throttle_position_A", {"Throttle_position_A": throttleA_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Throttle_A_status") 

    vcu_hil.set_signal("Throttle_position_B", {"Throttle_position_B": throttleB_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Throttle_B_status")

    vcu_hil.set_signal("Steering_raw", {"Steering_raw": steeringAngle_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Steering_status")

    torqueDemandR = vcu.get_signal("TorqueDemandRight")
    torqueDemandL = vcu.get_signal("TorqueDemandLeft")
    tolerance = 5
    assert math.isclose(torqueDemandR, expectedTR = 0, abs_tol = tolerance)
    assert math.isclose(torqueDemandL, expectedTL = 0, abs_tol = tolerance)

def throttle_A_out_of_tolerance(): 
    #Throttle A is at a greater % than B %
    #Throttle A is set at 50% capacity and B is set at 3% capacity
    setThrottleA(2.41)
    setThrottleB(0.99)
    setSteering(1.65)

    throttleA_mV = 2410
    throttleB_mV = 990
    steeringAngle_mV = 1650
    
    vcu_hil.set_signal("Throttle_position_A", {"Throttle_position_A": throttleA_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Throttle_A_status") 

    vcu_hil.set_signal("Throttle_position_B", {"Throttle_position_B": throttleB_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Throttle_B_status")

    vcu_hil.set_signal("Steering_raw", {"Steering_raw": steeringAngle_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Steering_status")

    torqueDemandR = vcu.get_signal("TorqueDemandRight")
    torqueDemandL = vcu.get_signal("TorqueDemandLeft")
    tolerance = 5
    assert math.isclose(torqueDemandR, expectedTR = 0, abs_tol = tolerance)
    assert math.isclose(torqueDemandL, expectedTL = 0, abs_tol = tolerance)

def throttle_B_out_of_tolerance(): 
    #Throttle B is at a greater % than A %
    #Throttle B is set at 50% capacity and A is set at 3% capacity
    setThrottleA(1.96)
    setThrottleB(1.13)
    setSteering(1.65)

    throttleA_mV = 1960
    throttleB_mV = 1130
    steeringAngle_mV = 1650
    
    vcu_hil.set_signal("Throttle_position_A", {"Throttle_position_A": throttleA_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Throttle_A_status") 

    vcu_hil.set_signal("Throttle_position_B", {"Throttle_position_B": throttleB_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Throttle_B_status")

    vcu_hil.set_signal("Steering_raw", {"Steering_raw": steeringAngle_mV})
    time.sleep(0.2)
    assert vcu_hil.get_signal("Steering_status")

    torqueDemandR = vcu.get_signal("TorqueDemandRight")
    torqueDemandL = vcu.get_signal("TorqueDemandLeft")
    tolerance = 5
    assert math.isclose(torqueDemandR, expectedTR = 0, abs_tol = tolerance)
    assert math.isclose(torqueDemandL, expectedTL = 0, abs_tol = tolerance)
