import time
import math
from testbeds.hil_testbed import teststand
from drivers.common_drivers.can_driver import HILBoard, VehicleBoard

# board setup
vcu_hil: HILBoard = teststand.hil_boards["vcu_hil"]
vcu: VehicleBoard = teststand.vehicle_boards["vcu"]

# variables for testing
sleepDurationMs = 0.2
tolerance = 5

def test_TV_setup(teststand):
    # setup the car configuration for the tests
    print(teststand.hil_boards)
    
    # set 50% throttle
    throttleA_mV = 2090
    throttleB_mV = 1130
    vcu_hil.set_signal("Throttle_position_A", {"Throttle_position_A": throttleA_mV})
    time.sleep(sleepDurationMs)
    assert vcu_hil.get_signal("Throttle_A_status")

    vcu_hil.set_signal("Throttle_position_B", {"Throttle_position_B": throttleB_mV})
    time.sleep(sleepDurationMs)
    assert vcu_hil.get_signal("Throttle_B_status")


def test_TV_deadzone(teststand):
    # test 1: 50% throttle and steering angle in dead zone (0 degree)
    vcu_hil.set_signal("Steering_raw", {"Steering_raw": 1650})
    time.sleep(sleepDurationMs)
    assert vcu_hil.get_signal("Steering_status")

    torqueDemandR = vcu.get_signal("TorqueDemandRight")
    torqueDemandL = vcu.get_signal("TorqueDemandLeft")
    assert torqueDemandR == torqueDemandL,\
    f"torqueDemandR <{torqueDemandR}>, torqueDemandL <{torqueDemandL}>"


def test_TV_100R(teststand):
    # test 2: 50% throttle and steering fully right (100 degrees)
    vcu_hil.set_signal("Steering_raw", {"Steering_raw": 3300})
    time.sleep(sleepDurationMs)
    assert vcu_hil.get_signal("Steering_status")

    torqueDemandR = vcu.get_signal("TorqueDemandRight")
    torqueDemandL = vcu.get_signal("TorqueDemandLeft")
    assert math.isclose(torqueDemandR, expectedTR = 2.50, abs_tol = tolerance),\
    f"torqueDemandR <{torqueDemandR}> exceeds tolerance <{tolerance}>"
    assert math.isclose(torqueDemandL, expectedTL = 27.50, abs_tol = tolerance),\
    f"torqueDemandR <{torqueDemandR}> exceeds tolerance <{tolerance}>"


def test_TV_100L(teststand):
    # test 3: 50% throttle and steering fully left (100 degrees)
    vcu_hil.set_signal("Steering_raw", {"Steering_raw": 0})
    time.sleep(sleepDurationMs)
    assert vcu_hil.get_signal("Steering_status")

    torqueDemandR = vcu.get_signal("TorqueDemandRight")
    torqueDemandL = vcu.get_signal("TorqueDemandLeft")
    assert math.isclose(torqueDemandR, expectedTR = 27.50, abs_tol = tolerance),\
    f"torqueDemandR <{torqueDemandR}> exceeds tolerance <{tolerance}>"
    assert math.isclose(torqueDemandL, expectedTL = 2.50, abs_tol = tolerance),\
    f"torqueDemandR <{torqueDemandR}> exceeds tolerance <{tolerance}>"


def test_TV_50R(teststand):
    # test 4: 50% throttle and steering 50 degrees right
    vcu_hil.set_signal("Steering_raw", {"Steering_raw": 2480})
    time.sleep(sleepDurationMs)
    assert vcu_hil.get_signal("Steering_status")

    torqueDemandR = vcu.get_signal("TorqueDemandRight")
    torqueDemandL = vcu.get_signal("TorqueDemandLeft")
    assert math.isclose(torqueDemandR, expectedTR = 10.50, abs_tol = tolerance),\
    f"torqueDemandR <{torqueDemandR}> exceeds tolerance <{tolerance}>"
    assert math.isclose(torqueDemandL, expectedTL = 25.50, abs_tol = tolerance),\
    f"torqueDemandL <{torqueDemandL}> exceeds tolerance <{tolerance}>"