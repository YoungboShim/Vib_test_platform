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

    return parser.parse_args()

def moving_vib(start_tactor, end_tactor, duration, amplitude, ser_port):
    gamma = 1.5
    update_time = 5000 # 10ms = 10000us
    start_time = dt.datetime.now()
    curr_time = start_time
    tmp_time = start_time

    moment = (curr_time - start_time).microseconds * 1000
    start_amp = amplitude * math.pow((1 - moment / duration), gamma) * 0.99
    end_amp = amplitude * math.pow((moment / duration), gamma) * 0.99

    while (curr_time - start_time).seconds * 1000000 + (curr_time - start_time).microseconds < duration * 1000:
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
        curr_time = dt.datetime.now()
    ser_port.write(("m" + str(start_tactor) + "%02d" % 0 + "\n").encode())
    ser_port.write(("m" + str(end_tactor) + "%02d" % 0 + "\n").encode())
    ser_port.flush()

def virtual_tactor(pos, amplitude):
    gamma = 1
    tactor_pos = [-3, -1, 1, 3]
    tactor_amp = []

    for i in range(4):
        single_amp = amplitude * math.pow((1 / (1 + math.pow(pos - tactor_pos[i], 2))), gamma) * 0.99
        tactor_amp.append(single_amp)

    return tactor_amp

# start_pos: 0 is middle of tactors. each tactors position (-3, -1, 1, 3)
# duration: ms
# amplitude: 0~100
def virtual_move(start_pos, end_pos, duration, amplitude, ser_port):
    update_time = 5 # 5ms
    start_time = dt.datetime.now()
    curr_time = start_time
    tmp_time = start_time
    elpased_time = 0

    while elpased_time < duration:
        if (curr_time - tmp_time).microseconds / 1e3 > update_time:
            tmp_time = curr_time
            curr_pos = start_pos + (end_pos - start_pos) * elpased_time / duration
            amp_list = virtual_tactor(curr_pos, amplitude)
            for i in range(4):
                tactor_cmd = "m" + str(i + 1) + "%02d" % amp_list[i] + "\n"
                ser_port.write(tactor_cmd.encode())
            ser_port.flush()

        curr_time = dt.datetime.now()
        elpased_time = (curr_time - start_time).seconds * 1e3 + (curr_time - start_time).microseconds / 1e3

    for i in range(4):
        tactor_cmd = "m" + str(i + 1) + "00\n"
        ser_port.write(tactor_cmd.encode())
    ser_port.flush()

def main(args):
    time.sleep(2)

    ser = serial.Serial(args.port, args.baud_rate, timeout = 1)

    virtual_move(-3, 7, 1000, 100, ser)

    time.sleep(1)
    ser.close()

if __name__ =='__main__':
    args = parse_args()
    main(args)
