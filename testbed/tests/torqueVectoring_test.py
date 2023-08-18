import time
import slash

from testbeds.hil_testbed import teststand
from drivers.common_drivers.can_driver import HILBoard, VehicleBoard
from .utilities.hil_init import board_init
from .utilities.common_HIL import Car

# variables for testing
sleepDurationMs = 0.2
tolerance = 5

def test_TV_setup(teststand):
    print(teststand.hil_boards) # try moving these three lines outside?  it is repeated in each 
    vcu_hil: HILBoard = teststand.hil_boards["vcu_hil"]
    vcu: VehicleBoard = teststand.vehicle_boards["vcu"]

    #assert board_init(hil_board=vcu_hil)

    #assert globals.HIL_wip_workaround, "Error: workaround is set to 0"\
    car = Car(name='myCar')
    car.set_EM_enable()

    # # set 50% throttle
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