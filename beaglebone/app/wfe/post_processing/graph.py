import plotly.express as px
import plotly.io as pio
import matplotlib.pyplot as plt
import json
import argparse
from pathlib import Path


def parse_args():
    parser = argparse.ArgumentParser(description='Graph parsed log')
    parser.add_argument('-s', '--src', dest='src', required=True,
                        type=Path, help='path to json file to graph')
    parser.add_argument('--msg', dest='msg', required=True,
                        type=str, help='message to graph')
    parser.add_argument('--sig', dest='sig', required=True,
                        type=str, help='message to graph')
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('-p', '--plotly', dest='px',
                       action='store_true', help='graph with plotly')
    group.add_argument('-m', '--matplotlib', dest='plt',
                       action='store_true', help='graph with matplotlib')
    return parser.parse_args()


def main():
    args = parse_args()

    json_filename = Path(args.src)
    data = json.load(open(json_filename, 'r'))
    msg = args.msg
    sig = args.sig
    if args.px:
        pio.renderers.default = 'browser'
        px.line(data[msg], x='timestamp', y=sig).show()
    if args.plt:
        plt.plot(data[msg]['timestamp'], data[msg][sig])
        plt.show()
    # print(max(data['WSBFR_Sensors']['FR_Speed']))
    # plt.plot(data['WSBFR_Sensors']['timestamp'],data['WSBFR_Sensors']['FR_WheelDistance'])
    # plt.show()
    # px.line(data['WheelSpeedKPH'], x='timestamp', y='RLSpeedKPH').show()


if __name__ == '__main__':
    main()
