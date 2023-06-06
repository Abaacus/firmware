import slash
from slash import logger
import time
from testbeds.hil_testbed import teststand
# from testbeds.common_testbed import Testbed
# def test_setup(hil_testbed):
#     pass

def test_can(teststand):
    print(teststand.vehicle_boards)
    a = time.time()
    while 1:
        # if time.time() - a > 1:
        print(teststand.vehicle_boards["vcu"].store)
            # a = time.time()