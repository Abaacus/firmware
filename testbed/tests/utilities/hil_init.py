from drivers.common_drivers.can_driver import HILBoard, VehicleBoard
import time

def volts_to_mV(volts: float) -> float:
    return volts * 1000

def board_init(hil_board: HILBoard):
    '''Initialize signals to non-erroring values for a given board'''

    if hil_board.name == 'vcu_hil':
        hil_board.set_signal("Steering_raw", {"Steering_raw": volts_to_mV(1.65)})
        time.sleep(0.001)
        if not hil_board.get_signal("Steering_status"):
            return False
        
        hil_board.set_signal("Brake_pres_raw", {"Brake_pres_raw": volts_to_mV(1)})
        time.sleep(0.001)
        if not hil_board.get_signal("Brake_pres_status"):
            return False
        
        hil_board.set_signal("Brake_position", {"Brake_position": volts_to_mV(1)})
        time.sleep(0.001)
        if not hil_board.get_signal("Brake_pos_status"):
            return False
        
        hil_board.set_signal("Throttle_position_A", {"Throttle_position_A": volts_to_mV(2.19)})
        time.sleep(0.001)
        if not hil_board.get_signal("Throttle_A_status"):
            return False
        
        hil_board.set_signal("Throttle_position_B", {"Throttle_position_B": volts_to_mV(1.3)})
        time.sleep(0.001)
        if not hil_board.get_signal("Throttle_B_status"):
            return False
        
        print('VCU sensor values initialized successfully')
        return True
    
    # Init function not created for board
    return False
