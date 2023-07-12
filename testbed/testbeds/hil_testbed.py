import slash
from drivers.hil_boards.vcu_hil import VCUHil
from testbeds.common_testbed import Testbed, HWManifestItem
from drivers.boards.vcu import VCU
from hil_constants import VehicleBoardId, HilBoardId

class HILTestbed(Testbed):
    pass


class VehicleHIL(HILTestbed):
    vehicle_manifest = [
        HWManifestItem("vcu", VehicleBoardId.VCU),
    ]
    hil_manifest = [
        HWManifestItem("vcu_hil", HilBoardId.VCU_HIL),
    ]
    


@slash.fixture
def teststand():
    return slash.g.testbed
