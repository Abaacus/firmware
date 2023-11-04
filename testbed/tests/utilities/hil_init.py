from drivers.common_drivers.can_driver import HILBoard, VehicleBoard
from . import setVcuHilOutputs
import time

sleepDuration = 0.001 # any lower, it may fail

def board_init(hil_board: HILBoard):
    '''Initialize signals to non-erroring values for a given board'''

    if hil_board.name == 'vcu_hil':
        setVcuHilOutputs.setSteering(1.650)
        time.sleep(sleepDuration)
        if not hil_board.get_signal("SteeringStatus"):
            return False

        setVcuHilOutputs.setBrakePres(1000)
        time.sleep(sleepDuration)
        if not hil_board.get_signal("BrakePresStatus"):
            return False

        setVcuHilOutputs.setBrakePosition(1000)
        time.sleep(sleepDuration)
        if not hil_board.get_signal("BrakePosStatus"):
            return False

        setVcuHilOutputs.setThrottleA(2190)
        time.sleep(sleepDuration)
        if not hil_board.get_signal("ThrottleAStatus"):
            return False

        setVcuHilOutputs.setThrottleB(1300)
        time.sleep(sleepDuration)
        if not hil_board.get_signal("ThrottleBStatus"):
            return False

        print('VCU sensor values initialized successfully')
        return True

    # Init function not created for board
    return False