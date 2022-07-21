#convert from .csv to .blf

import sys
import csv
import can

with can.CSVReader(sys.argv[1]) as reader:
    with can.BLFWriter(sys.argv[2]) as writer:
        for msg in reader:
            print(msg)
            writer.on_message_received(msg)

# csv file should have and only have following columns:
# Timestamp,arbitration_id,isExtendedId,isRemote,isError,dlc,data

