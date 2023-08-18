import time
from . import setVcuHilOutputs
from drivers.common_drivers.can_driver import Vehicle

class Car:
    def __init__(self, name):
        self.name = name
        self.vehicle = Vehicle()

    def set_EM_enable(self):
        try:
            setVcuHilOutputs.setThrottleA(2.307) #calculated 1958
            setVcuHilOutputs.setThrottleA(0.936) #calculated 991
            setVcuHilOutputs.setBrakePres(1000)
            setVcuHilOutputs.setBrakePosition(3300)
            self.vehicle.send_CAN_message("BMU_HV_Power_State", {"HV_Power_State": 1} )
            self.vehicle.send_CAN_message("TempInverterLeft", {"PRO_CAN_CRC": 0, "PRO_CAN_COUNT": 0, "PRO_CAN_RES": 0, "InverterAUTHSEEDLeft": 0,\
                                                               "TempInverterLeft": 0, "TempInverterDeltaLeft": 0, "StateInverterLeft": 0x18})
            self.vehicle.send_CAN_message("TempInverterRight", {"PRO_CAN_CRC": 0, "PRO_CAN_COUNT": 0, "PRO_CAN_RES": 0, "InverterAUTHSEEDRight": 0,\
                                                               "TempInverterRight": 0, "TempInverterDeltaRight": 0, "StateInverterRight": 0x18})
            time.sleep(0.05) # todo: check if this is necessary
            self.vehicle.send_CAN_message("DCU_buttonEvents", {"ButtonEMEnabled": 1, "ButtonHVEnabled": 1, "ButtonTCEnabled": 0, "ButtonEnduranceLapEnabled": 0, "ButtonEnduranceToggleEnabled":0 } )
            self.vehicle.send_CAN_message("PDU_ChannelStatus", {"StatusPowerVCU": 0, "StatusPowerMCRight": 2, "StatusPowerMCLeft": 2, \
                                                                 "StatusPowerIMD": 0, "StatusPowerDCU": 0, "StatusPowerDAU": 0, "StatusPowerCoolingPumpRight" : 0,\
                                                                 "StatusPowerCoolingPumpLeft": 0, "StatusPowerCoolingFanRight": 0, "StatusPowerCoolingFanLeft": 0,\
                                                                 "StatusPowerCoolingFanBattery": 0, "StatusPowerBMU": 0})
           
            setVcuHilOutputs.setBrakePosition(0)
        except Exception as e:
            print("set_EM_enable message NOT sent")
            print(e)
            return False
        
    def set_TC_enable():
        try:
            Vehicle.send_CAN_message("ButtonTCEnabled", {"ButtonTCEnabled": 1})
        except:
            print("set_TC_enable message NOT sent")
            return False

    def set_endurance_enable():
        try:
            Vehicle.send_CAN_message("ButtonEnduranceToggleEnabled", {"ButtonEnduranceToggleEnabled": 1})
        except:
            print("set_endurance_enable message NOT sent")
            return False