from drivers.common_drivers.can_driver import HILBoard
class VCUHil(HILBoard):
    CAN_ID = 7
    def __init__(self, name, bus, db, listener):
        super().__init__(name, bus, db, listener, self.CAN_ID)
        