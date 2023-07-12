from typing import Dict, List

class DTC:
    def __init__(self, code: int, severity: int, data: int):
        self.code = code
        self.data = data
        self.severity = severity
        self.was_read = False

    def read(self):
        self.was_read = True
        return self.data

class DTCLogger:
    def __init__(self):
        # Future todo? Store depending on which board received it?
        self.dtc_log: Dict[int, DTC] = {} # {code : DTC structure}

    def log_dtc(self, decoded_dtc: Dict[str, int]):
        code = decoded_dtc['DTC_CODE']
        severity = decoded_dtc['DTC_Severity']
        data = decoded_dtc['DTC_Data']

        received_dtc = DTC(code=code, severity=severity, data=data)

        # Todo: maybe add more details to the structure like time received
        if code not in self.dtc_log:
            self.dtcs[code] = []

        self.dtcs[code].append(received_dtc)
    
    def has_unread_data(self, code: int) -> bool:
        assert self.has_dtc(code), f"DTC Code {code} not found in DTC Log"

        # Given a DTC Code, checks if you have requested the data already
        for dtc in self.dtc_log[code]:
            if not dtc.was_read:
                return True
        return False
    
    def get_dtc_data(self, code: int) -> List[int]:
        assert self.has_dtc(code), f"DTC Code {code} not found in DTC Log"

        # Given a DTC Code, return a list of DTC data received with that code
        dtcs_data = []
        for dtc in self.dtc_log[code]:
            if not dtc.was_read:
                dtcs_data.append(dtc.read())
    
    def has_dtc(self, code: int) -> bool:
        # Check if a DTC code was logged
        return code in self.dtc_log
    
    def list_dtcs(self) -> List[int]:
        # Return a list of all DTC codes logged
        return list(self.dtc_log.keys())
                
        