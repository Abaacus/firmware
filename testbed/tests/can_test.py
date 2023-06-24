import slash
from slash import logger
import time
from testbeds.hil_testbed import teststand
from drivers.boards.vcu import VCU
from drivers.hil_boards.vcu_hil import VCUHil


def test_can(teststand):
    print(teststand.hil_boards)
    vcu_hil: VCUHil = teststand.hil_boards["vcu_hil"]
    
    mV = 0
    while 1:
        mV = (mV + 200) % 3300
        vcu_hil.set_signal("Throttle_position_A", {"Throttle_position_A": mV})
        time.sleep(0.2)
        assert vcu_hil.get_signal("Throttle_A_status") == 1
        print(f"Sent {mV} to Throttle A")
        
        vcu_hil.set_signal("Throttle_position_B", {"Throttle_position_B": mV})
        time.sleep(0.2)
        assert vcu_hil.get_signal("Throttle_B_status") == 1
        print(f"Sent {mV} to Throttle B")
