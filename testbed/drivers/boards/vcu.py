from drivers.common_drivers.can_driver import VehicleBoard
class VCU(VehicleBoard):
    CAN_ID = 2
    def __init__(self, name, bus, db, listener):
        super().__init__(name, bus, db, listener, self.CAN_ID)
