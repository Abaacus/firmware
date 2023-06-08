from drivers.common_drivers.can_driver import HilCANDriver
class VCUHil(HilCANDriver):
    CAN_ID = 2
    def __init__(self, name, bus, db):
        super().__init__(name, bus, db, self.CAN_ID)
        