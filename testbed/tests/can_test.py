import slash
from slash import logger
import time
from testbeds.hil_testbed import teststand
# from testbeds.common_testbed import Testbed
# def test_setup(hil_testbed):
#     pass

def test_can(teststand):
    print(teststand.vehicle_boards)
    while 1:
        print(teststand.vehicle_boards["vcu"].get_signal('FR_Speed_RAD_S'))