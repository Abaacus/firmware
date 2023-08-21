import time
import slash

from testbeds.hil_testbed import teststand
from drivers.common_drivers.can_driver import HILBoard, VehicleBoard
from .utilities.hil_init import board_init
from .utilities.common_HIL import Car
from .utilities import setVcuHilOutputs

# Variables for testing
sleepDurationMs = 0.05 
tolerance = 2  # Nm
setup_complete = False
vcu = None
vcu_hil = None

def test_TV_setup(teststand):
    global setup_complete
    global vcu
    global vcu_hil
    try:
        vcu = teststand.vehicle_boards["vcu"]
        vcu_hil = teststand.hil_boards["vcu_hil"]
        assert board_init(hil_board=vcu_hil)
        car = Car()
        assert car.set_EM_enable()
        
        # All tests conducted at 50% throttle
        setVcuHilOutputs.setThrottleA(2.154)
        time.sleep(sleepDurationMs)
        assert vcu_hil.get_signal("Throttle_A_status")

        setVcuHilOutputs.setThrottleB(1.047)
        time.sleep(sleepDurationMs)
        assert vcu_hil.get_signal("Throttle_B_status")
    except Exception as e:
        print("Setup failed!")
        print(e)
    else:
        setup_complete = True
        time.sleep(0.8) # Delay required after going to EM. 0.7 fails 100%. 

# Test 1: steering angle in dead zone (0)
def test_TV_deadzone():
    assert setup_complete
 
    setVcuHilOutputs.setSteering(1.650)
    time.sleep(sleepDurationMs)
    assert vcu_hil.get_signal("Steering_status")
    time.sleep(sleepDurationMs)

    torqueDemandR = vcu.get_signal("TorqueDemandRight")
    torqueDemandL = vcu.get_signal("TorqueDemandLeft")

    assert (torqueDemandR - torqueDemandL) <= tolerance

# Test 2 & 3: steering angle fully right and left
@slash.parametrize(('testValue', 'expR', 'expL'), [(3.30, 1.56, 19.24), (0.00, 19.24, 1.56)])
def test_TV_100R(testValue, expR, expL):
    assert setup_complete

    setVcuHilOutputs.setSteering(testValue)
    time.sleep(sleepDurationMs)
    assert vcu_hil.get_signal("Steering_status")
    time.sleep(sleepDurationMs)

    torqueDemandR = vcu.get_signal("TorqueDemandRight")
    torqueDemandL = vcu.get_signal("TorqueDemandLeft")

    assert abs(torqueDemandR - expR) <= tolerance
    assert abs(torqueDemandL - expL) <= tolerance