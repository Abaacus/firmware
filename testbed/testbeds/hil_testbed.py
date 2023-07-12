import slash
from drivers.hil_boards.vcu_hil import VCUHil
from testbeds.common_testbed import Testbed, HWManifestItem
from drivers.boards.vcu import VCU
from hil_constants import VEHICLE_BOARD_ID, HIL_BOARD_ID

class HILTestbed(Testbed):
    pass


class VehicleHIL(HILTestbed):
    vehicle_manifest = [
        HWManifestItem("vcu", VEHICLE_BOARD_ID.VCU.value),
    ]
    hil_manifest = [
        HWManifestItem("vcu_hil", HIL_BOARD_ID.VCU.value),
    ]
    


@slash.fixture
def teststand():
    return slash.g.testbed
