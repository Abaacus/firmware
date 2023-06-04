import slash
from drivers.sim_boards.vcu_sim import VCUSim
from testbeds.common_testbed import Testbed, HWManifestItem
from drivers.boards.vcu import VCU

class HILTestbed(Testbed):
    pass


class VehicleHIL(HILTestbed):
    vehicle_manifest = [
        HWManifestItem("vcu", VCU, {}),
    ]
    hil_manifest = [
        HWManifestItem("vcu_hil", VCUSim, {}),
    ]
    


@slash.fixture
def teststand():
    return slash.g.testbed
