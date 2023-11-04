from enum import Enum

# For board IDs, follow the CAN bus wiki page: http://208.68.36.87/projects/firmware/wiki/can-bus
class VehicleBoardId(Enum):
    BMU = 1
    VCU = 2
    PDU = 3
    CHARGE_CART = 4
    MOTOR_L = 5
    MOTOR_R = 6
    DCU = 7
    WSB_FL = 8
    WSB_FR = 9
    WSB_RL = 10
    WSB_RR = 11
    BEAGLEBONE = 12
    IMU = 13
    CHARGER = 14
    DEBUG = 15

class HilBoardId(Enum):
    VCU_HIL = 2
    PDU_HIL = 3