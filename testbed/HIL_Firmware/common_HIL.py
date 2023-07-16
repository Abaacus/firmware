import time
from testbeds.hil_testbed import teststand
from drivers.common_drivers.can_driver import HILBoard, VehicleBoard, Vehicle

# board setup
vcu_hil: HILBoard = teststand.hil_boards["vcu_hil"]
vcu: VehicleBoard = teststand.vehicle_boards["vcu"]

class Car:
    def __init__(self, name):
        self.name = name

    def set_EM_enable():
        try:
            vcu_hil.set_signal("Brake_position", {"Brake_position": 3300})
            Vehicle.send_CAN_message("ButtonEMEnabled", {"ButtonEMEnabled": 1} )
            Vehicle.send_CAN_message("ButtonHVEnabled", {"ButtonHVEnabled": 1} )
            time.sleep(0.5)
            vcu_hil.set_signal("Brake_position", {"Brake_position": 0})
        except:
            print("set_EM_enable message NOT sent")
            return False
        
    def set_TC_enable():
        try:
            Vehicle.send_CAN_message("ButtonTCEnabled", {"ButtonTCEnabled": 1})
        except:
            print("set_TC_enable message NOT sent")
            return False

    def set_endurance_enable():
        try:
            Vehicle.send_CAN_message("ButtonEnduranceToggleEnabled", {"ButtonEnduranceToggleEnabled": 1})
        except:
            print("set_endurance_enable message NOT sent")
            return False

        

        

