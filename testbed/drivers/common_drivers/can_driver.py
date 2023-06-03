from drivers.common_drivers.driver import TestbedDriver
import can
from slash import logger
import slash
import can.interfaces.slcan as slcan
import time
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
    def __init__(self, name, bus, can_id):
        super().__init__(name)
        self.can_id = can_id
        self.name = name
        self._bus = bus
        self.listener = None
        
            # slash.g.can_listeners[self.can_interface] = CANListener(self.can_interface)
            # notifier = can.Notifier(slash.g.vehicle_bus, slash.g.can_listeners[self.can_interface])
            # can.Notifier(slash.g.hil_bus, slash.g.can_listeners[self.can_interface])
        # slash.g.can_listeners[self.can_interface].register_can_listener(self.can_id, self.can_msg_rx)


    def can_msg_rx(self, msg):
        logger.debug(f"RXed id: {msg.arbitration_id} {msg.data}")
        # TODO: Store the CAN data within the CANDriver object

    def get_can_signal(self, signal_name):
        # TODO: Add DBC connection for signal name parsing
        # TODO: Add signal formatting and parsing through DBC
        pass

class VehicleCANDriver(CANDriver):
    def __init__(self, name, bus, can_id):
        super().__init__(name, bus, can_id)
        assert self.name not in slash.g.vehicle_listeners
        self.listener = CANListener(self.name)
        slash.g.vehicle_listeners[self.name] = self.listener

class HilCANDriver(CANDriver):
    def __init__(self, name, bus, can_id):
        super().__init__(name, bus, can_id)
        assert self.name not in slash.g.hil_listeners
        self.listener = CANListener(self.name)
        slash.g.hil_listeners[self.name] = self.listener

    def send_can_msg(self, msg):
        self._bus.send(msg)


print('__bus' in vars(CANDriver))