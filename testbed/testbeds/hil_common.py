import time
from testbeds.hil_testbed import teststand
from drivers.common_drivers.can_driver import HILBoard, VehicleBoard, Vehicle

# board setup
vcu_hil: HILBoard = teststand.hil_boards["vcu_hil"]
vcu: VehicleBoard = teststand.vehicle_boards["vcu"]

