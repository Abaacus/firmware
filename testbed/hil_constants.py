from enum import Enum
HIL_wip_workaround = 1

class BOARD_ID():
    pass

class VehicleBoardId(BOARD_ID, Enum):
    BMU = 1
    VCU = 2
    PDU = 3

class HilBoardId(BOARD_ID, Enum):
    VCU_HIL = 7
