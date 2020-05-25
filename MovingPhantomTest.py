import serial
import math
import datetime as dt
import argparse
import sys
import time

def parse_args():
    parser = argparse.ArgumentParser()

    parser.add_argument('-p', '--port', type=str, default='COM3',\
        help="USB port name.")
    parser.add_argument('-b', '--baud-rate', type=int, default=115200,\
        help="Baudrate of USB.")

    if not len(sys.argv) > 1:
        parser.print_help()
        sys.exit(-1)

    return parser.parse_args()

def moving_vib(start_tactor, end_tactor, duration, amplitude, ser_port):
    gamma = 1
    update_time = 10000 # 10ms = 10000us
    start_time = dt.datetime.now()
    curr_time = start_time
    tmp_time = start_time

    moment = (curr_time - start_time).microseconds * 1000
    start_amp = amplitude * math.pow((1 - moment / duration), gamma) * 0.99
    end_amp = amplitude * math.pow((moment / duration), gamma) * 0.99

    start_cmd = "m" + str(start_tactor) + "%02d" % start_amp + "\n"
    end_cmd = "m" + str(end_tactor) + "%02d" % end_amp + "\n"
    ser_port.write(start_cmd.encode())
    ser_port.write(end_cmd.encode())

    while (curr_time - start_time).seconds * 1000000 + (curr_time - start_time).microseconds < duration * 1000:
        curr_time = dt.datetime.now()

        if (curr_time - tmp_time).microseconds > update_time:
            tmp_time = curr_time
            moment = (curr_time - start_time).microseconds / 1000
            start_amp = amplitude * math.pow((1 - moment / duration), gamma) * 0.99
            end_amp = amplitude * math.pow((moment / duration), gamma) * 0.99

            #print("m" + str(start_tactor - 1) + ": %02d" % start_amp)
            #print("m" + str(end_tactor - 1) + ": %02d" % end_amp)
            start_cmd = "m" + str(start_tactor) + "%02d" % start_amp + "\n"
            end_cmd = "m" + str(end_tactor) + "%02d" % end_amp + "\n"
            ser_port.write(start_cmd.encode())
            ser_port.write(end_cmd.encode())
            ser_port.flush()
    ser_port.write(("m" + str(start_tactor) + "%02d" % 0 + "\n").encode())
    ser_port.write(("m" + str(end_tactor) + "%02d" % 0 + "\n").encode())
    ser_port.flush()

def main(args):
    ser = serial.Serial(args.port, args.baud_rate, timeout = 1)

    moving_vib(1, 2, 1000, 100, ser)
    moving_vib(2, 3, 1000, 100, ser)
    moving_vib(3, 4, 1000, 100, ser)

    ser.close()

if __name__ =='__main__':
    args = parse_args()
    main(args)
