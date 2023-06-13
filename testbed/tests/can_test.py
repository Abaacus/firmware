import slash
from slash import logger
import time
from testbeds.hil_testbed import teststand
from drivers.boards.vcu import VCU
from drivers.hil_boards.vcu_hil import VCUHil
# from testbeds.common_testbed import Testbed
# def test_setup(hil_testbed):
#     pass


def test_can(teststand):
    print(teststand.hil_boards)
    vcu_hil: VCUHil = teststand.hil_boards["vcu_hil"]
    errors = 0
    v = 0
    while 1:
        v = (v + 200) % 3300
        vcu_hil.set_signal(message_name="Throttle_position_A", signal_name="Throttle_position_A", signal_value=v)
        vcu_hil.set_signal(message_name="Throttle_position_B", signal_name="Throttle_position_B", signal_value=v)
        time.sleep(1)
        try:
            store = vcu_hil.store
            print(store, f'{errors=}')
        except ValueError:
            errors += 1
        except KeyError:
            pass
            # input()
        # if time.time() - a > 1:
       

        # a = time.time()
