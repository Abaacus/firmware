import time
from typing import List
import cantools
import slash
import can.interfaces.slcan as slcan
from can.interfaces.socketcan.socketcan import SocketcanBus
import can
from drivers.common_drivers.can_driver import CANDriver, CANListener


class HWManifestItem:
    '''
    Class to store information about a hardware board in the testbed manifest
    @name: Name of the board, must NOT have spaces
    @classObj: Class object of the board driver, ex: VCU
    '''

    def __init__(self, name: str, classObj: CANDriver):
        self.name = name
        self.classObj = classObj

    def __repr__(self) -> str:
        return f"HWManifestItem(name={self.name}, classObj={self.classObj})"


class Testbed:
    vehicle_manifest: List[HWManifestItem] = []
    hil_manifest: List[HWManifestItem] = []

    def __init__(self):
        # Choose which to enable, useful for debugging individual buses
        setup_vehicle = False
        setup_hil = True

        # Vehicle Bus
        if setup_vehicle:
            # TODO: Maybe move this to .slashrc as an arg
            vehicle_db = cantools.db.load_file("../common/Data/2018CAR.dbc")

            vehicle_bus = SocketcanBus(channel='slcan1', bitrate=500000)
            assert vehicle_bus is not None, "Vehicle bus not initialized"

            vehicle_listener = CANListener()
            can.Notifier(hil_bus, [vehicle_listener])

            self.vehicle_boards = {}
            for board in self.vehicle_manifest:
                self.vehicle_boards[board.name] = board.classObj(
                    board.name, vehicle_bus, vehicle_db, vehicle_listener)

        # HIL Bus
        if setup_hil:
            hil_db = cantools.db.load_file("HIL_Firmware/HIL.dbc")

            hil_bus = SocketcanBus(channel='slcan0', bitrate=500000)
            assert hil_bus is not None, "HIL bus not initialized"

            hil_listener = CANListener()
            can.Notifier(hil_bus, [hil_listener])

            self.hil_boards = {}
            for board in self.hil_manifest:
                self.hil_boards[board.name] = board.classObj(
                    board.name, hil_bus, hil_db, hil_listener)
