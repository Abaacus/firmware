import pprint
import cantools
import csv
import json
import argparse
from pathlib import Path
import os
import traceback


def add_to_dict(dictionary, signal, value):
    if signal in dictionary:
        dictionary[signal].append(value)
    else:
        dictionary[signal] = [value]


def decode_can_id(can_id):
    return can_id - 0x80000000  # account for extended ids


def parse_args():
    parser = argparse.ArgumentParser(description='Process CAN log')
    parser.add_argument('-s', '--src', dest='src', type=Path, required=True,
                        help='CAN log file (csv) to be read in')
    parser.add_argument('-d', '--dest', dest='dest', required=True,
                        type=Path, help='Output directory for parsed files')
    parser.add_argument('-j', '--json', dest='json',
                        action='store_true', help='Enable json output')
    parser.add_argument('-c', '--csv', dest='csv',
                        action='store_true', help='Enable csv output')
    args = parser.parse_args()
    return args


def main():
    args = parse_args()
    if not args.json and not args.csv:
        print('No output type specified')
        exit(1)

    csv_in = Path(f'{args.src}')

    dbc = Path('../../../../common/Data/2018CAR.dbc')
    db = cantools.db.load_file(dbc)

    csv_out = [['Timestamp', 'Signal', 'Value']]
    json_out = {}
    failed_ids = [['csv_id', 'corrected_id', 'csv_id', 'corrected_id']]
    with open(csv_in, 'r') as csv_file:
        csv_reader = csv.reader(csv_file)
        next(csv_reader)  # skip header

        for csv_data in csv_reader:
            # csv_data = csv_data[:-1]  # remove empty last element
            ms = int(csv_data[0])
            can_id = decode_can_id(int(csv_data[1]))
            data = bytes.fromhex(''.join(csv_data[2].strip().split(' ')))
            try:
                name = db.get_message_by_frame_id(can_id).name
                decoded_data = db.decode_message(can_id, data)

                if args.json:
                    if name not in json_out:
                        json_out[name] = {}
                    add_to_dict(json_out[name], 'timestamp', ms)

                for signal in decoded_data:
                    if args.json:
                        add_to_dict(json_out[name],
                                    signal, decoded_data[signal])
                    if args.csv:
                        csv_out.append([ms, signal, decoded_data[signal]])
            except KeyError:
                a = [hex(int(csv_data[1])), hex(can_id), int(csv_data[1]), int(can_id)]
                if a not in failed_ids:
                    failed_ids.append(a)

    if len(failed_ids) > 1:
        print("Failed to parse the following ids:")
        pprint.pprint(failed_ids)

    if args.csv:
        if not os.path.exists(args.dest):
            os.makedirs(args.dest)
        csv_out_filename = Path(f'{args.dest}/parsed_{csv_in.stem}.csv')
        with open(csv_out_filename, 'w') as csv_file:
            csv_writer = csv.writer(csv_file)
            csv_writer.writerows(csv_out)

    if args.json:
        if not os.path.exists(args.dest):
            os.makedirs(args.dest)
        json_filename = Path(f'{args.dest}/parsed_{csv_in.stem}.json')
        with open(json_filename, 'w') as out:
            out.write(json.dumps(json_out, indent=4))


if __name__ == '__main__':
    main()
