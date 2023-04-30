import csv
from pathlib import Path
import re
import os
import argparse

def parse_args():
    parser = argparse.ArgumentParser(description='Process CAN log')
    parser.add_argument('-s', '--src', dest='src', type=Path, required=True,
                        help='CAN log file (.log) to be read in')
    parser.add_argument('-d', '--dest', dest='dest', required=True,
                        type=Path, help='Output directory for parsed files')
    args = parser.parse_args()
    return args

def main():

    args = parse_args()
    temp_out = Path('canLog.out')
    can_log = Path(args.src)
    dbc = Path('../../../../common/Data/2018CAR.dbc')

    command = f'cat {can_log} | python -m cantools decode --single-line "{dbc}" > {temp_out}'
    os.system(command=command)

    out = []
    with open(temp_out, 'r') as file:
        for line in file.readlines():
            try:
                line = line.strip()
                candump, decoded = line.split('::')

                ts = float(candump.split(' ')[0].strip('()'))
                decoded = re.split(r'(\(|\))', decoded)[2].split(',')

                for sig in decoded:
                    sig_name, val = sig.split(':')
                    sig_name = sig_name.strip()
                    val = val.strip()
                    out.append([ts, sig_name, val])
            except ValueError:
                pass

    out.sort(key=lambda x: x[0])
    out.insert(0, ['Timestamp', 'Signal', 'Data'])
    out_file = Path(f'{args.dest}/parsed_{can_log.stem}.csv')
    if not os.path.exists(args.dest):
        os.makedirs(args.dest)
    with open(out_file, 'w', newline='') as file:
        writer = csv.writer(file)
        writer.writerows(out)

    os.remove(temp_out)

    


if __name__ == "__main__":
    main()
