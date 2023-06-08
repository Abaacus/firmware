import cantools
from drivers.common_drivers.driver import TestbedDriver
import can
from slash import logger
import slash
from can.interfaces.socketcan.socketcan import SocketcanBus


class CANListener(can.Listener):
    def __init__(self, interface_name):
        super().__init__()
        self.interface_name = interface_name
        self.callbacks = dict()

    def register_can_listener(self, can_id, func_callback):
        self.callbacks[0] = func_callback

    def on_message_received(self, msg):
        # board_id = (msg.arbitration_id >> 8) & 0x07
        # if board_id in self.callbacks:
        self.callbacks[0](msg)


class CANDriver(TestbedDriver):
    def __init__(self, name, bus: SocketcanBus , db: cantools.db.Database, can_id):
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
            allow_truncated=True,
            decode_choices=False,
            decode_containers=False
        )
        for signal_name, signal_value in decoded_data.items():  # type: ignore
            # print(f"{signal_name}: {signal_value}")
            self.store[signal_name] = signal_value
    
    def get_signal(self, signal_name):
        # Should be faster to fail fast than to check if signal_name is in self.store
        try:
            return self.store[signal_name]
        except KeyError:
            logger.warning(f"Signal {signal_name} not found in {self.name}")
            return None


class VehicleCANDriver(CANDriver):
    def __init__(self, name, bus, db, can_id):
        super().__init__(name, bus, db, can_id)
        assert self.name not in slash.g.vehicle_listeners

        self.listener = CANListener(self.name)
        slash.g.vehicle_listeners[self.name] = self.listener

        self.listener.register_can_listener(self.can_id, self.can_msg_rx)

    # def can_msg_rx(self, msg):
    #     try:
    #         self.store[msg.arbitration_id] = int.from_bytes(msg.data, 'little')
    #     except ValueError:
    #         logger.warning(f"Bus too fast?")

class HilCANDriver(CANDriver):
    def __init__(self, name, bus, db, can_id):
        super().__init__(name, bus, db, can_id)
        assert self.name not in slash.g.hil_listeners

        self.listener = CANListener(self.name)
        slash.g.hil_listeners[self.name] = self.listener

        self.listener.register_can_listener(self.can_id, self.can_msg_rx)

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
        
        
