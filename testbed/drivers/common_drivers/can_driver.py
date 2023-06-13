import cantools
from drivers.common_drivers.driver import TestbedDriver
import can
from slash import logger
import slash
from can.interfaces.socketcan.socketcan import SocketcanBus


class CANListener(can.Listener):
    def __init__(self):
        super().__init__()
        self.callbacks = {}

    def register_callback(self, board_id, func_callback):
        assert board_id not in self.callbacks
        self.callbacks[board_id] = func_callback

    def on_message_received(self, msg):
        board_id = (msg.arbitration_id >> 8) & 0x07
        assert board_id in self.callbacks, f"{board_id} not in {self.callbacks}"
        self.callbacks[board_id](msg)


class CANDriver(TestbedDriver):
    def __init__(self, name, bus: SocketcanBus, db: cantools.db.Database, listener: CANListener, can_id):
        super().__init__(name)
        self.can_id = can_id
        self.name = name
        self._bus = bus
        self.db = db

        listener.register_callback(self.can_id, self.can_msg_rx)

        self.store = {}

    def can_msg_rx(self, msg):
        decoded_data = self.db.decode_message(
            msg.arbitration_id,
            msg.data,
            allow_truncated=True,
            decode_choices=False,
            decode_containers=False
        )
        for signal_name, signal_value in decoded_data.items():
            self.store[signal_name] = signal_value

    def get_signal(self, signal_name):
        # Should be faster to fail fast than to check if signal_name is in self.store
        try:
            return self.store[signal_name]
        except KeyError:
            logger.warning(f"Signal {signal_name} not found in {self.name}")
            return None


class VehicleBoard(CANDriver):
    def __init__(self, name, bus, db, listener, can_id):
        super().__init__(name, bus, db, listener, can_id)


class HILBoard(CANDriver):
    def __init__(self, name, bus, db, listener, can_id):
        super().__init__(name, bus, db, listener, can_id)

    def set_signal(self, message_name, signal_name, signal_value):
        self._bus.flush_tx_buffer()
        try:
            data = self.db.encode_message(
                message_name, {signal_name: signal_value})
        except KeyError:
            logger.warning(f"Message {message_name} not found in {self.db}")
            return
        msg = self.db.get_message_by_name(message_name)

        msg = can.Message(arbitration_id=msg.frame_id, data=data)
        self._bus.send(msg)
