import serial
import time
import numpy as np
from enum import Enum
import serial.tools.list_ports

class State(Enum):
    GET_COMMAND = 1
    START_TRAJECTORY = 2
    RUN_TRAJECTORY = 3

current_state = State.GET_COMMAND
ports = list(serial.tools.list_ports.comports())
for p in ports:
    if p.pid == 1155: #Teensy 4.1 PID
        teensy_port = p
    
teensy = serial.Serial(port=teensy_port.device, baudrate=115200, timeout=0)


def write_to_arm(x):
    teensy.write(bytes(x, 'utf-8'))

def angles_to_str(angles):
    cmd = "<0,"
    for angle in angles:
        cmd+=str(int(angle))
        cmd+=","
    cmd=cmd[:-1]
    cmd+=">"
    return cmd


def cubic_trajectory(q0, qf, t0, tf, v0, vf):
    a_vals = np.zeros(4)
    m = np.matrix([[1, t0, pow(t0, 2), pow(t0, 3)],
                   [0, 1, np.logical_and(2, t0), 3 * pow(t0, 2)],
                   [1, tf, pow(tf, 2), pow(tf, 3)],
                   [0, 1, 2 * tf, 3 * pow(tf, 2)]])
    m_inverse = np.linalg.inv(m)
    q = np.matrix([[q0], [v0], [qf], [vf]])

    a_vals = np.matmul(m_inverse, q)
    return a_vals.tolist()


def get_pos():
    write_to_arm("<1>")
    #time.sleep(0.05)
    while True:
        data = teensy.readline()
        data = (data.decode("utf-8"))
        if "<1" in data:
            break

    pos = data.replace("<1,", "").replace(">", "")
    a = pos.split(",")
    angles = [float(i) for i in a]
    return angles

def string_to_angles(cmd):
    cmd = cmd.split(",")
    angles = [float(i)*100 for i in cmd]
    return angles

targets = [[-2760.00,3816.00,24.00,-2688.00,-5592.00,24.00],[-4536.00,-5256.00,7224.00,-6264.00,-3552.00,48.00],[-8592.00,-3000.00,72.00,11112.00,-2040.00,72.00],[0,0,0,0,0,0,0]]
idx = 0
while True:
    match current_state:
        case State.GET_COMMAND:
            # command = input("Enter command: ")
            # current_target = string_to_angles(command)
            current_target = targets[idx]
            current_pos = get_pos()
            current_state = State.START_TRAJECTORY
        case State.START_TRAJECTORY:
            time_start = time.time()
            dur = 2
            end_time = time_start+dur
            j0 = cubic_trajectory(current_pos[0], current_target[0], 0, dur, 0, 0)
            j1 = cubic_trajectory(current_pos[1], current_target[1], 0, dur, 0, 0)
            j2 = cubic_trajectory(current_pos[2], current_target[2], 0, dur, 0, 0)
            j3 = cubic_trajectory(current_pos[3], current_target[3], 0, dur, 0, 0)
            j4 = cubic_trajectory(current_pos[4], current_target[4], 0, dur, 0, 0)
            j5 = cubic_trajectory(current_pos[5], current_target[5], 0, dur, 0, 0)
            current_state = State.RUN_TRAJECTORY
        case State.RUN_TRAJECTORY:
            time_now = time.time()
            if time_now>end_time:
                current_state = State.GET_COMMAND
                idx+=1
                if idx>=len(targets):
                    idx = 0
                write_to_arm(angles_to_str(current_target))
                continue
            elapsed = time_now-time_start
            p0 = j0[0][0] + j0[1][0] * elapsed + j0[2][0] * pow(elapsed, 2) + j0[3][0] * pow(elapsed, 3)
            p1 = j1[0][0] + j1[1][0] * elapsed + j1[2][0] * pow(elapsed, 2) + j1[3][0] * pow(elapsed, 3)
            p2 = j2[0][0] + j2[1][0] * elapsed + j2[2][0] * pow(elapsed, 2) + j2[3][0] * pow(elapsed, 3)
            p3 = j3[0][0] + j3[1][0] * elapsed + j3[2][0] * pow(elapsed, 2) + j3[3][0] * pow(elapsed, 3)
            p4 = j4[0][0] + j4[1][0] * elapsed + j4[2][0] * pow(elapsed, 2) + j4[3][0] * pow(elapsed, 3)
            p5 = j5[0][0] + j5[1][0] * elapsed + j5[2][0] * pow(elapsed, 2) + j5[3][0] * pow(elapsed, 3)
            write_to_arm(angles_to_str([p0, p1, p2, p3, p4, p5]))
            time.sleep(0.05)


