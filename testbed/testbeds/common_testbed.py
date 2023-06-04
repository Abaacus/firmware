import time
from typing import Any, Callable, Dict, List, Optional, Tuple, Union
from dataclasses import dataclass

import cantools
from drivers.common_drivers.can_driver import CANDriver
from drivers.boards.vcu import VCU
from drivers.sim_boards.vcu_sim import VCUSim
from slash import logger
import slash
from copy import copy
import atexit
import can.interfaces.slcan as slcan
import can


class HWManifestItem:
        '''
        Class to store information about a hardware board in the testbed manifest
        @name: Name of the board, must NOT have spaces
        @classObj: Class object of the board driver, ex: VCU
        @params: Dictionary of parameters to pass to the board driver
        '''
        def __init__(self, name: str, classObj, params) -> None:
            self.name = name
            self.classObj: CANDriver = classObj
            self.params = params

        def __repr__(self) -> str:
            return f"HWManifestItem(name={self.name}, classObj={self.classObj}, params={self.params})"

class Testbed:
    vehicle_manifest: List[HWManifestItem] = []
    hil_manifest: List[HWManifestItem] = []
    def __init__(self):
        vehicle_port = "COM4"
        hil_port = "COM12"
        # TODO: Maybe move this to .slashrc as an arg
        self.vehicle_db = cantools.db.load_file("../common/Data/2018CAR.dbc")

        # self.hil_db = cantools.db.load_file("testbeds/dbc/hil.dbc")


        vehicle_bus = slcan.slcanBus(channel=vehicle_port, ttyBaudrate=230400, bitrate=500000)
        # self.hil_bus = slcan.slcanBus(channel=hil_port, ttyBaudrate=230400, bitrate=500000)

        
        self.vehicle_boards = {}
        self.hil_boards = {}

        for board in self.vehicle_manifest:
            self.vehicle_boards[board.name] = board.classObj(board.name, vehicle_bus, self.vehicle_db, **board.params) # type: ignore

        # for board in self.hil_manifest:
        #     hil_boards[board.name] = board.classObj(board.name, self.hil_bus, **board.params) # type: ignore

        
        assert vehicle_bus is not None, "Vehicle bus not initialized"
        # assert self.hil_bus is not None, "HIL bus not initialized"

        can.Notifier(vehicle_bus, [listener for name, listener in slash.g.vehicle_listeners.items()])
        # can.Notifier(self.hil_bus, slash.g.hil_listeners)
    
    def close_buses(self):
        self.vehicle_bus.shutdown()
        self.hil_bus.shutdown()