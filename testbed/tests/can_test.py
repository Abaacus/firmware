import time
from testbeds.hil_testbed import teststand
from drivers.common_drivers.can_driver import HILBoard, VehicleBoard
import numpy as np
import tqdm


def test_steeringAngle(teststand):
    print(teststand.vehicle_boards)
    vcu_hil: HILBoard = teststand.hil_boards["vcu_hil"]
    vcu: VehicleBoard = teststand.vehicle_boards["vcu"]
    
    for angle in range(-100, 101, 5):
        vcu_hil.set_signal("Steering_raw", {"Steering_raw": np.interp(angle, [-100, 100], [0, 3300])})
        time.sleep(0.001)
        assert vcu_hil.get_signal("Steering_status")
        time.sleep(0.75)
        steerAngle = vcu.get_signal("SteeringAngle")
        assert abs(steerAngle-angle) <= 5

#  BMU Brake Pressure Test
def test_BMU_BrakePressure(teststand):
    print(teststand.vehicle_boards)
    bmu_hil: HILBoard = teststand.hil_boards["bmu_hil"]
    bmu: VehicleBoard = teststand.vehicle_boards["bmu"]

    for brake_pres in range (0, 4095, 1000):
        # Set signal via the HIL board (Signal yet to be confirmed)
        bmu_hil.set_signal("Brake_Sense_Analog",
         {"Brake_Sense_Analog": np.interp(brake_pres, [0, 4095], [0, 3.3])})
        test_val = np.interp(brake_pres, [0, 4095], [0, 3.3])
        time.sleep(0.001)
        
        # Check status of the set signal
        assert _hil.get_signal("Brake_Sense_Status")
        time.sleep(0.75)
        
        # Get signal value from the vehicle CANBus
        msg_brake_pres = bmu.get_signal("BrakePressureBMU")

        # Validate if sent value is same as received
        assert msg_brake_pres == brake_pres


### TEMPORARY
# How to check for DTC example (where 10 = a DTC code, vcu = teststand.vehicle_boards["vcu"])
# vcu.dtc_logger.has_dtc(10) 
# --> returns a bool for if it has a dtc
# vcu.dtc_logger.get_dtc_data(10) 
# --> returns an array of DTC data where the length is the number of times the dtc was received
#     (also marks the dtc as read, so the second time you call it you'll get an empty array
#       unless you received another dtc of the same code)
# vcu.dtc_logger.get_dtc_structs(10)
# --> same as before but returns the actual DTC class objects instead of just the data
# vcu.dtc_logger.list_dtcs() 
# --> returns a list of all the dtc codes that have been received
# vcu.dtc_logger.reset_logger()
# --> resetst the logger to {}
