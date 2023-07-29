from drivers.common_drivers.can_driver import HILBoard, VehicleBoard

def board_init(hil_board: HILBoard):
    '''Initialize signals to non-erroring values for a given board'''

    if hil_board.name == 'vcu_hil':
        hil_board.set_signal("Steering_raw", {"Steering_raw": 1.65})
        hil_board.set_signal("Brake_pres_raw", {"Brake_pres_raw": 0})
        hil_board.set_signal("Brake_position", {"Brake_position": 0})
        hil_board.set_signal("Throttle_position_A", {"Throttle_position_A": 0})
        hil_board.set_signal("Throttle_position_B", {"Throttle_position_B": 0})

# def vcuInit() -> bool:

#     if setVoltage('Throttle_position_A',volts_to_mV(2.19)) == False:
#         # 2.19v === ~15% throttle (note- throttle A is inverted)
#         print("Throttle A not init")
#         return False
#     if setVoltage('Throttle_position_B',volts_to_mV(1.3)) == False:
#         # 1.3v === ~15% throttle
#         print("Throttle B not init")
#         return False
#     if setVoltage('Brake_position',volts_to_mV(1)) == False:
#         # brake pos > 0.956V === break is hard pressed
#         print("Brake pos not init")
#         return False
#     if setVoltage('Steering_raw',volts_to_mV(1.65)) == False:
#         print("Steering not init")
#         return False
#     if setVoltage('Brake_pres_raw',volts_to_mV(1)) == False:
#         print("Brake pressure not init")
#         return False

#     print('VCU sensors initialized successfully')
#     return True
