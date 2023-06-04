import cantools
from drivers.common_drivers.driver import TestbedDriver
import can
from slash import logger
import slash

class CANListener(can.Listener):
    def __init__(self, interface_name):
        super().__init__()
        self.interface_name = interface_name
        self.callbacks = dict()
    def register_can_listener(self, can_id, func_callback):
        self.callbacks[can_id] = func_callback
    def on_message_received(self, msg):
        board_id = (msg.arbitration_id >> 8) & 0x07
        if board_id in self.callbacks:
            self.callbacks[board_id](msg)

class CANDriver(TestbedDriver):
    def __init__(self, name, bus, db: cantools.db.Database, can_id):
        super().__init__(name)
        self.can_id = can_id
        self.name = name
        self._bus = bus
        self.db = db

        self.store = {}

    def can_msg_rx(self, msg):
        decoded_data = self.db.decode_message(
            msg.arbitration_id,
            msg.data,
            allow_truncated=True
        )
        for signal_name, signal_value in decoded_data.items():
            self.store[signal_name] = signal_value

    def get_signal(self, signal_name):
        if signal_name not in self.store:
            logger.warning(f"Signal {signal_name} not found in {self.name}")
            return None
        return self.store[signal_name]

class VehicleCANDriver(CANDriver):
    def __init__(self, name, bus, db, can_id):
        super().__init__(name, bus, db, can_id)
        assert self.name not in slash.g.vehicle_listeners

        self.listener = CANListener(self.name)
        slash.g.vehicle_listeners[self.name] = self.listener

        self.listener.register_can_listener(self.can_id, self.can_msg_rx)
        

class HilCANDriver(CANDriver):
    def __init__(self, name, bus, db, can_id):
        super().__init__(name, bus, db, can_id)
        assert self.name not in slash.g.fil_listeners

        self.listener = CANListener(self.name)
        slash.g.hil_listeners[self.name] = self.listener

        self.listener.register_can_listener(self.can_id, self.can_msg_rx)

    def __can_msg_tx(self, msg):
        self._bus.send(msg)