import time
import slash

from testbeds.hil_testbed import teststand
from drivers.common_drivers.can_driver import HILBoard, VehicleBoard
from .utilities.hil_init import board_init
from .utilities.common_HIL import Car
from .utilities import setVcuHilOutputs

# variables for testing
sleepDurationMs = 0.2
tolerance = 5

def test_TV_setup(teststand):
    #assert board_init(hil_board=vcu_hil)
    vcu_hil: HILBoard = teststand.hil_boards["vcu_hil"]
    vcu: VehicleBoard = teststand.vehicle_boards["vcu"]
    #assert globals.HIL_wip_workaround, "Error: workaround is set to 0"\
    car = Car()
    car.set_EM_enable()
    
    # test 50% throttle
    setVcuHilOutputs.setThrottleA(2.154) # off by 47
    setVcuHilOutputs.setThrottleB(1.047) # ADC: 1452, 1.153 = 100%, 
    #assert vcu_hil.get_signal("Throttle_A_status")
    assert vcu_hil.get_signal("Throttle_B_status")

    # why is this needed
    # time.sleep(0.05) 
    # setVcuHilOutputs.setBrakePosition(3300)
    # assert vcu_hil.get_signal("Brake_pos_status")


# test 1: steering angle in dead zone (0)
def test_TV_deadzone(teststand):
    vcu_hil: HILBoard = teststand.hil_boards["vcu_hil"]
    vcu: VehicleBoard = teststand.vehicle_boards["vcu"]
    setVcuHilOutputs.setSteering(1.650)
    assert vcu_hil.get_signal("Steering_status")

    torqueDemandR = vcu.get_signal("TorqueDemandRight")
    torqueDemandL = vcu.get_signal("TorqueDemandLeft")
    print(f"Right: {torqueDemandR}")
    print(f"Left: {torqueDemandL}")
    assert torqueDemandR == torqueDemandL,\
    f"torqueDemandR <{torqueDemandR}>, torqueDemandL <{torqueDemandL}>"

# test 2 & 3: steering angle fully right and left
@slash.parametrize(('testValue', 'expR', 'expL'), [(3300, 2.50, 27.50), (0, 27.50, 2.50)])
def test_TV_100R(teststand, testValue, expR, expL):
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
