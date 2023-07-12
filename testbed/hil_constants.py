from enum import Enum

class BOARD_ID():
    pass

class VEHICLE_BOARD_ID(BOARD_ID, Enum):
    BMU = 1
    VCU = 2
    PDU = 3

class HIL_BOARD_ID(BOARD_ID, Enum):
    VCU = 7