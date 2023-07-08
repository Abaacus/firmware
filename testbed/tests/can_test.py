import time
from testbeds.hil_testbed import teststand
from drivers.common_drivers.can_driver import HILBoard, VehicleBoard
import numpy as np
from tqdm import tqdm


def test_steeringAngle(teststand):
    print(teststand.hil_boards)
    vcu_hil: HILBoard = teststand.hil_boards["vcu_hil"]
    vcu: VehicleBoard = teststand.vehicle_boards["vcu"]
    
    for angle in tqdm(range(-100, 101, 5)):
        vcu_hil.set_signal("Steering_raw", {"Steering_raw": np.interp(angle, [-100, 100], [0, 3300])})
        time.sleep(0.2)
        assert vcu_hil.get_signal("Steering_status")
        steerAngle = vcu.get_signal("SteeringAngle")
        assert abs(steerAngle-angle) <= 3
        