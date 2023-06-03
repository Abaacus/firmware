import slash
from slash import logger
import time
# from testbeds.common_testbed import Testbed
# def test_setup(hil_testbed):
#     pass

def test_can():
    print(slash.g.testbed.vehicle_bus.state)
    time.sleep(2)
    slash.g.vehicle_bus.shutdown()