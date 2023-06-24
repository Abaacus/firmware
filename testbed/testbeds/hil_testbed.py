import slash
from drivers.hil_boards.vcu_hil import VCUHil
from testbeds.common_testbed import Testbed, HWManifestItem
from drivers.boards.vcu import VCU

class HILTestbed(Testbed):
    pass


class VehicleHIL(HILTestbed):
    vehicle_manifest = [
        HWManifestItem("vcu", 2),
    ]
    hil_manifest = [
        HWManifestItem("vcu_hil", 7),
    ]
    


@slash.fixture
def teststand():
    return slash.g.testbed
