import serial
import time

import cv2
import mediapipe as mp
import os
import urllib.request
import warnings

import asyncio
import threading
import tkinter as tk
import ttkbootstrap as tb
from scipy.stats import alpha
from ttkbootstrap.widgets import Meter
from bleak import BleakClient

import math



#_________________________________________________________________
# CONFIGURATION
##################################################################

PORT = 'COM4'
bluetooth_PORT = 'COM7'
BAUD = 115200
TIMEOUT = 1

periodic_mode = False  # default: manual mode
periodic_speed = 250 # in milliseconds
hand_tracking = False
com_mode = "none"  # tracks which is active

getters = []
setters = []
hand_tracking_values = []

serial_com = None  # <-- add this here
bluetooth_com = None

# Suppress MediaPipe warnings
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'
warnings.filterwarnings('ignore')

MODEL_PATH = "hand_landmarker.task"

def download_model_if_needed():
    if not os.path.exists(MODEL_PATH):
        print("Downloading hand landmarker model...")
        urllib.request.urlretrieve(
            "https://storage.googleapis.com/mediapipe-models/hand_landmarker/hand_landmarker/float16/1/hand_landmarker.task",
            MODEL_PATH
        )
        print("Done!")

#_________________________________________________________________
# INIT
##################################################################
def init_serial_com():
    serial_com = serial.Serial()
    serial_com.port = PORT
    serial_com.baudrate = BAUD
    serial_com.timeout = TIMEOUT
    serial_com.dtr = False
    serial_com.rts = False
    serial_com.open()  # open after setting DTR/RTS low

    print("Serial connection opened")

    return serial_com

def init_bluetooth_com():
    bluetooth_com = serial.Serial(bluetooth_PORT, BAUD, timeout=TIMEOUT)
    bluetooth_com.dtr = False
    bluetooth_com.rts = False

    print("Bluetooth serial connected on", bluetooth_PORT)

    return bluetooth_com

#_________________________________________________________________
# Functions
##################################################################
def distance(p1, p2):
    return math.sqrt((p1[0] - p2[0])**2 + (p1[1] - p2[1])**2)

def send_serial_command(serial,msg):
    print("Sending via serial com")
    #cmd = "POS001, 2000, 60, 40, 20, 0, 0, 0, 0, 0\nf"
    serial.write((msg + '\n').encode('utf-8'))

def send_bluetooth_command(bluetooth,msg):
    if bluetooth and bluetooth.is_open:
        print("Sending via Bluetooth com")
        #cmd = "POS001, 2000, 20, 0, 0, 30, 0, 0, 0, 0\nf"
        bluetooth.write((msg + '\n').encode('utf-8'))

def build_msg(marker="POS001",values = [],input_msg="---"):
    msg = ""

    if marker == "POS001":
        msg = msg + marker + ",0"
        for v in values:
            msg += ", " + str(v)

    if marker == "GRIP01":
        if not input_msg == "---":
            msg = msg + marker + "," + input_msg

    if marker == "SPEED1":
        msg = ""

    if marker == "OFF001":
        msg = ""

    print(f"Build Msg: {msg} ")

    return msg

def is_behind_perp(A, B, P):
    AB = (B[0] - A[0], B[1] - A[1])
    BP = (P[0] - B[0], P[1] - B[1])
    dot = AB[0]*BP[0] + AB[1]*BP[1]
    return dot < 0  # True if P is behind the perpendicular line at B

def hand_calculations(hand_points):
    global hand_tracking_values
    '''
    | ** 0 **  | WRIST              |   —       | Root reference point
    | ** 1 **  | THUMB CMC          | Thumb     | Base
    | ** 2 **  | THUMB MCP          | Thumb     | Knuckle
    | ** 3 **  | THUMB IP           | Thumb     | Middle joint
    | ** 4 **  | THUMB TIP          | Thumb     | --- Tip ---
    | ** 5 **  | INDEX FINGER MCP   | Index     | Knuckle
    | ** 6 **  | INDEX FINGER PIP   | Index     | Middle joint
    | ** 7 **  | INDEX FINGER DIP   | Index     | Lower joint
    | ** 8 **  | INDEX FINGER TIP   | Index     | --- Tip ---
    | ** 9 **  | MIDDLE FINGER MCP  | Middle    | Knuckle
    | ** 10 ** | MIDDLE FINGER PIP  | Middle    | Middle joint
    | ** 11 ** | MIDDLE FINGER DIP  | Middle    | Lower joint
    | ** 12 ** | MIDDLE FINGER TIP  | Middle    | --- Tip ---
    | ** 13 ** | RING FINGER MCP    | Ring      | Knuckle
    | ** 14 ** | RING FINGER PIP    | Ring      | Middle joint
    | ** 15 ** | RING FINGER DIP    | Ring      | Lower joint
    | ** 16 ** | RING FINGER TIP    | Ring      | --- Tip ---
    | ** 17 ** | PINKY MCP          | Pinky     | Knuckle
    | ** 18 ** | PINKY PIP          | Pinky     | Middle joint
    | ** 19 ** | PINKY DIP          | Pinky     | Lower joint
    | ** 20 ** | PINKY TIP          | Pinky     | --- Tip ---
    '''

    # Index - Middle - Ring - Pinky
    ##################################################################
    d_wrist_1 = distance([hand_points[0].x,hand_points[0].y], [hand_points[5].x,hand_points[5].y])
    d_wrist_2 = distance([hand_points[0].x,hand_points[0].y], [hand_points[9].x,hand_points[9].y])
    d_wrist_3 = distance([hand_points[0].x,hand_points[0].y], [hand_points[13].x,hand_points[13].y])
    d_wrist_4 = distance([hand_points[0].x,hand_points[0].y], [hand_points[17].x,hand_points[17].y])

    d_index_finger = distance([hand_points[5].x,hand_points[5].y], [hand_points[8].x,hand_points[8].y])
    d_middle_finger = distance([hand_points[9].x,hand_points[9].y], [hand_points[12].x,hand_points[12].y])
    d_ring_finger = distance([hand_points[13].x,hand_points[13].y], [hand_points[16].x,hand_points[16].y])
    d_pinky_finger = distance([hand_points[17].x,hand_points[17].y], [hand_points[20].x,hand_points[20].y])

    d_index = d_index_finger*1.15/d_wrist_1
    d_middle = d_middle_finger*1.1/d_wrist_2
    d_ring = d_ring_finger*1.2/d_wrist_3
    d_pinky =  d_pinky_finger*1.3/d_wrist_4

    d_over_index = is_behind_perp([hand_points[0].x,hand_points[0].y], [hand_points[5].x,hand_points[5].y],[hand_points[8].x,hand_points[8].y])
    d_over_middle = is_behind_perp([hand_points[0].x,hand_points[0].y], [hand_points[9].x,hand_points[9].y],[hand_points[12].x,hand_points[12].y])
    d_over_ring = is_behind_perp([hand_points[0].x,hand_points[0].y], [hand_points[13].x,hand_points[13].y],[hand_points[16].x,hand_points[16].y])
    d_over_pinky = is_behind_perp([hand_points[0].x,hand_points[0].y], [hand_points[17].x,hand_points[17].y],[hand_points[20].x,hand_points[20].y])

    #Switching
    if not d_over_index:
        angle_index = 80 - (d_index * 80)
    else:
        angle_index = 80 +  (d_index * 10)*5

    if not d_over_middle:
        angle_middle = 90 - (d_middle * 90)
    else:
        angle_middle = 80 + (d_middle * 10)*5

    if not d_over_ring:
        angle_ring = 80 - (d_ring * 80)
    else:
        angle_ring = 80 + (d_ring * 10)*5

    if not d_over_pinky:
        angle_pinky = 80 - (d_pinky * 80)
    else:
        angle_pinky = 80 + (d_pinky * 10)*5

    # Thumb - Swing
    ##################################################################


    # Angles
    hand_tracking_values = [int(angle_index), int(angle_middle), int(angle_ring), int(angle_pinky), -1, -1, -1, -1]
    #print(f"Values: | {hand_tracking_values} |")

# UI Functions
##################
def create_slider(root, x, y, rotation="horizontal", rng=(0, 180), start=90, label="Slider"):
    orient = tk.HORIZONTAL if rotation == "horizontal" else tk.VERTICAL
    is_horiz = rotation == "horizontal"

    # Labels (unchanged)
    lbl = tk.Label(root, text=label, bg="black", fg="white")
    lbl.place(x=x, y=y - 25 if is_horiz else y + 200)

    val_label = tk.Label(root, text=str(start), bg="black", fg="cyan")
    val_label.place(x=x if not is_horiz else x + 220,
                    y=y if is_horiz else y - 25)

    def on_change(v):
        val_label.config(text=str(int(float(v))))

    slider = tk.Scale(root, from_=rng[0], to_=rng[1], orient=orient,
                      length=200 if is_horiz else 180,
                      command=on_change)
    slider.set(start)
    slider.place(x=x, y=y)

    # --- Jump-to-click behavior ---
    def jump_to_click(event):
        # Get the trough's actual geometry (excluding borders/handle)
        # The trough is the inner area; we approximate by excluding the 3D border
        trough = 9  # tk.Scale has ~3px borders on each side

        if is_horiz:
            # Horizontal: x-axis, left to right
            trough_width = slider.winfo_width() - 2 * trough
            if trough_width <= 0:
                return
            click_pos = event.x - trough
            ratio = max(0, min(1, click_pos / trough_width))
            # tk.Scale goes from_ to to_ (left to right regardless of values)
            value = rng[0] + (rng[1] - rng[0]) * ratio
        else:
            # Vertical: y-axis, but tk.Scale goes from_ at top, to_ at bottom
            trough_height = slider.winfo_height() - 2 * trough
            if trough_height <= 0:
                return
            click_pos = event.y - trough
            ratio = max(0, min(1, click_pos / trough_height))
            # Invert: top = from_, bottom = to_
            value = rng[0] + (rng[1] - rng[0]) * ratio

        # Clamp to range and snap to integer
        value = max(rng[0], min(rng[1], value))
        slider.set(int(value))

    # Bind to left-click on the slider widget
    slider.bind("<Button-1>", jump_to_click)
    # --- End jump-to-click ---

    return slider, lambda: slider.get(), lambda v: slider.set(v)

def create_knob(root, x, y, rng=(0, 180), start=90, label="Knob"):
    knob = Meter(
        root,
        metersize=200,
        padding=10,
        amountused=start,
        amounttotal=rng[1],
        interactive=True,
        textleft=label,
        textright="°",
        subtext="Wrist",
        stripethickness=10,
        bootstyle="info",
    )
    knob.place(x=x, y=y)
    return knob, lambda: int(knob.amountusedvar.get()), lambda v: knob.configure(amountused=v)

def create_button(root, x, y, text, action, style="primary", width=150, height=100):
    btn = tb.Button(root, text=text, command=action, bootstyle=style)
    btn.place(x=x, y=y, width=width, height=height)
    return btn

#_________________________________________________________________
# Secondary
##################################################################
def send_pose(pose):
    global serial_com, bluetooth_com  # access the global inside

    if com_mode == "serial" and serial_com:
        send_serial_command(serial_com, build_msg(marker="GRIP01",input_msg=pose))
    if com_mode == "bluetooth" and bluetooth_com:
        send_bluetooth_command(bluetooth_com, build_msg(marker="GRIP01",input_msg=pose))

def sending(root,getters):
    global serial_com, bluetooth_com  # access the global inside
    global hand_tracking_values

    if periodic_mode:
        print("Periodic send:")
        values = [g() for g in getters]
        if hand_tracking:
            values = hand_tracking_values

        if com_mode == "serial" and serial_com:
            send_serial_command(serial_com, build_msg(values=values))
        if com_mode == "bluetooth" and bluetooth_com:
            send_bluetooth_command(bluetooth_com, build_msg(values=values))

    root.after(periodic_speed, lambda: sending(root,getters))  # schedule every 0.5 s

def send_by_button():
    global serial_com, bluetooth_com

    print("Send via Button: ")

    values = [g() for g in getters]

    if com_mode == "serial" and serial_com:
        print("Serial send:")
        send_serial_command(serial_com, build_msg(values=values))

    if com_mode == "bluetooth" and bluetooth_com:
        print("Bluetooth send:")
        send_bluetooth_command(bluetooth_com, build_msg(values=values))

def UI():
    # Settings
    ##########################################################################################
    global periodic_mode, hand_tracking
    global getters, setters

    root = tb.Window(themename="cosmo")
    root.title("Prosthetic hand - Control Panel")
    #root.geometry("900x900")
    fullWindowDimensions = [1000, 900]
    fingerSettingWindowDimensions = [fullWindowDimensions[0]*0.6, fullWindowDimensions[1]]
    buttonsWindowDimensions = [fullWindowDimensions[0]*0.4, fullWindowDimensions[1]]
    windowDimensions = [600, 800]
    root.geometry(f"{fullWindowDimensions[0]}x{fullWindowDimensions[1]}")

    ''''
    # BACKGROUND IMAGE SETTINGS
    ##############################
    IMG_PATH = "../backgrounds/Snímka obrazovky 2025-05-18 024907.png"  # <--- path inside your project
    IMG_SCALE = 1.3  # <--- 1.0 = original size, 0.5 = half, 2.0 = double

    from PIL import Image, ImageTk
    bg_image_raw = Image.open(IMG_PATH)

    # Scale the image
    w, h = bg_image_raw.size
    bg_image_raw = bg_image_raw.resize((int(w * IMG_SCALE), int(h * IMG_SCALE)))
    bg_image = ImageTk.PhotoImage(bg_image_raw)

    # Canvas for background
    bg_canvas = tk.Canvas(root, width=w, height=h, highlightthickness=0)
    bg_canvas.place(x=0, y=0, relwidth=1, relheight=1)

    # Draw image
    bg_canvas.create_image(0, 0, anchor="nw", image=bg_image)

    # Keep reference (very important)
    root.bg_image = bg_image
    '''

    # Button boxes
    ##########################################################################################
    paddingXY = 20
    # LEFT SIDE - Sliders Box
    slider_box = tb.Labelframe(
        root,
        text="  Finger Controls  ",
        bootstyle="info",  # Color theme
        padding=10
    )
    slider_box.place(x=paddingXY, y=paddingXY, width=fingerSettingWindowDimensions[0]-paddingXY, height=fingerSettingWindowDimensions[1]-paddingXY*2)

    buttonBoxOffset = fingerSettingWindowDimensions[0]+paddingXY
    # RIGHT SIDE - Buttons Box
    button_box = tb.Labelframe(
        root,
        text="  Actions  ",
        bootstyle="success",
        padding=10
    )
    button_box.place(x=fingerSettingWindowDimensions[0]+paddingXY, y=20, width=buttonsWindowDimensions[0]-paddingXY*2, height=buttonsWindowDimensions[1]-paddingXY*2)

    # Sliders
    ##########################################################################################
    startPos = [0, 0, 0, 0, 0, 0, 90, 90]
    limits = [ (0, 90), (0, 90), (0, 90), (0, 90), (0, 90), (0, 90), (55,125), (55,125) ]
    positions = [
        (350, 90), (275, 75), (200, 90), (125, 110),
        (450, 300), (200, 480), (150, 700), (400,600)
    ]
    slider_names = ["Index", "Middle", "Ring", "Pinky", "Thumb", "Swing", "Wrist 1", "Wrist 2"]
    slider_orientation = ["vertical", "vertical", "vertical", "vertical", "vertical", "horizontal", "horizontal", "vertical"]

    # Create the sliders
    for i, (x, y) in enumerate(positions):
        s, g, setf = create_slider(root, x, y, slider_orientation[i], limits[i], startPos[i], slider_names[i])
        getters.append(g)
        setters.append(setf)

    # Buttons
    ##########################################################################################
    button_spacings = 12

    # Pos buttons
    ##########################################################################################
    pos_button_width = int(buttonsWindowDimensions[0] / 2) - paddingXY
    pos_button_height = 65
    open_pose_button = create_button(root=root,
                                     x=buttonBoxOffset + button_spacings, y=paddingXY*2,
                                     width=pos_button_width - int(button_spacings * 1.5), height=pos_button_height,
                                     text="Open",
                                     action=lambda: send_pose(pose="open"),
                                     style="primary")

    fist_pose_button = create_button(root=root,
                                     x=buttonBoxOffset + pos_button_width + button_spacings*0.5, y=paddingXY*2,
                                     width=pos_button_width - int(button_spacings * 1.5), height=pos_button_height,
                                     text="Fist",
                                     action=lambda: send_pose(pose="fist"),
                                     style="primary")

    pinch_pose_button = create_button(root=root,
                                     x=buttonBoxOffset + button_spacings, y=paddingXY * 2 + pos_button_height + button_spacings,
                                     width=pos_button_width - int(button_spacings * 1.5), height=pos_button_height,
                                     text="Pinch",
                                     action=lambda: send_pose(pose="pinch"),
                                     style="primary")

    pinch3_pose_button = create_button(root=root,
                                      x=buttonBoxOffset + pos_button_width + button_spacings * 0.5, y=paddingXY * 2 + pos_button_height + button_spacings,
                                      width=pos_button_width - int(button_spacings * 1.5), height=pos_button_height,
                                      text="Pinch 3",
                                      action=lambda: send_pose(pose="3pinch"),
                                      style="primary")

    cylinder_pose_button = create_button(root=root,
                                      x=buttonBoxOffset + button_spacings,
                                      y=paddingXY * 2 + pos_button_height*2 + button_spacings*2,
                                      width=pos_button_width - int(button_spacings * 1.5), height=pos_button_height,
                                      text="Cylinder",
                                      action=lambda: send_pose(pose="cilinder"),
                                      style="primary")

    smallCylinder_pose_button = create_button(root=root,
                                       x=buttonBoxOffset + pos_button_width + button_spacings * 0.5,
                                       y=paddingXY * 2 + pos_button_height*2 + button_spacings*2,
                                       width=pos_button_width - int(button_spacings * 1.5), height=pos_button_height,
                                       text="Small Cylinder",
                                       action=lambda: send_pose(pose="small_cilinder"),
                                       style="primary")

    relax_pose_button = create_button(root=root,
                                         x=buttonBoxOffset + button_spacings,
                                         y=paddingXY * 2 + pos_button_height *3 + button_spacings * 3,
                                         width=pos_button_width*2 - int(button_spacings * 2), height=pos_button_height,
                                         text="Relax",
                                         action=lambda: send_pose(pose="relax"),
                                         style="primary")


    # Hand tracking button
    ##########################################################################################
    def toggle_tracking():
        global hand_tracking
        hand_tracking = not hand_tracking

        if hand_tracking:
            tracking_thread = threading.Thread(target=hand_recognition, daemon=True)
            tracking_thread.start()
            hand_tracking_button.config(text="Tracking ON", bootstyle="success")
            print("👋 Hand tracking ON")
        else:
            hand_tracking_button.config(text="Tracking OFF", bootstyle="primary")
            print("👋 Hand tracking OFF")

    hand_tracking_button = create_button(root=root,
                              x=buttonBoxOffset + button_spacings, y=600 - 75 - button_spacings,
                              width=buttonsWindowDimensions[0] - paddingXY*2 - button_spacings*2, height=75,
                              text="Tracking OFF",
                              action=toggle_tracking,
                              style="primary")

    # Mode Buttons
    ##########################################################################################
    def toggle_mode():
        nonlocal mode_btn, send_btn
        global periodic_mode
        periodic_mode = not periodic_mode
        if periodic_mode:
            mode_btn.config(text="🟢 Periodic Mode", bootstyle="success")
            send_btn.config(state="disabled")
            print("🔁 Periodic sending ON")
        else:
            mode_btn.config(text="🟣 Manual Mode", bootstyle="primary")
            send_btn.config(state="normal")
            print("🎛 Manual sending ON")

    mode_send_button_width = int(buttonsWindowDimensions[0]*0.8)
    mode_btn = create_button(root=root,
                             x=buttonBoxOffset + buttonsWindowDimensions[0]/2 - mode_send_button_width/2 - paddingXY ,y= 700 - button_spacings,
                             width=mode_send_button_width, height=75,
                             text="🟣 Manual Mode",
                             action=toggle_mode,
                             style="primary")

    send_btn = create_button(root=root,
                             x=buttonBoxOffset + buttonsWindowDimensions[0]/2 - mode_send_button_width/2 - paddingXY ,y= 775,
                             width=mode_send_button_width, height=75,
                             text="📤 Send Now",
                             action=send_by_button,
                             style="info")

    # Switches for COM
    ##########################################################################################
    def switch_com_mode(selected):
        global com_mode, serial_com, bluetooth_com
        com_mode = selected

        if selected == "serial":
            # connect serial
            if serial_com is None or not serial_com.is_open:
                serial_com = init_serial_com()
            # disconnect bluetooth
            if bluetooth_com and bluetooth_com.is_open:
                bluetooth_com.close()
                bluetooth_com = None
            # update buttons
            com_serial_button.config(bootstyle="success")
            com_bluetooth_button.config(bootstyle="primary")
            print("🔌 Serial mode ON")

        elif selected == "bluetooth":
            # connect bluetooth
            if bluetooth_com is None or not bluetooth_com.is_open:
                bluetooth_com = init_bluetooth_com()
            # disconnect serial
            if serial_com and serial_com.is_open:
                serial_com.close()
                serial_com = None
            # update buttons
            com_bluetooth_button.config(bootstyle="success")
            com_serial_button.config(bootstyle="primary")
            print("📡 Bluetooth mode ON")

    serial_bluetooth_button_width = int(buttonsWindowDimensions[0]/2) - paddingXY
    com_serial_button = create_button(root=root,
                                      x=buttonBoxOffset + button_spacings, y=600,
                                      width=serial_bluetooth_button_width - int(button_spacings*1.5), height=75,
                                      text="Serial COM",
                                      action=lambda: switch_com_mode("serial"),
                                      style="primary")

    com_bluetooth_button = create_button(root=root,
                                         x=buttonBoxOffset +serial_bluetooth_button_width + button_spacings*0.5, y=600,
                                         width=serial_bluetooth_button_width - int(button_spacings*1.5), height=75,
                                         text="Bluetooth",
                                         action=lambda: switch_com_mode("bluetooth"),
                                         style="primary")




    sending(root=root,getters=getters)

    root.mainloop()

def hand_recognition():
    global hand_tracking

    from mediapipe.tasks.python.vision.hand_landmarker import HandLandmarker, HandLandmarkerOptions
    from mediapipe.tasks.python.core.base_options import BaseOptions

    download_model_if_needed()

    base_options = BaseOptions(model_asset_path=MODEL_PATH)
    options = HandLandmarkerOptions(
        base_options=base_options,
        num_hands=1,
        min_hand_detection_confidence=0.5,
        min_hand_presence_confidence=0.5,
        min_tracking_confidence=0.5
    )
    landmarker = HandLandmarker.create_from_options(options)

    cap = cv2.VideoCapture(1)
    if not cap.isOpened():
        print("❌ Camera not found")
        hand_tracking = False
        landmarker.close()
        return

    while hand_tracking:
        ret, frame = cap.read()
        if not ret:
            break

        frame = cv2.flip(frame, 1)
        rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        mp_image = mp.Image(image_format=mp.ImageFormat.SRGB, data=rgb)
        results = landmarker.detect(mp_image)

        h, w = frame.shape[:2]

        if results.hand_landmarks:

            hand_calculations(results.hand_landmarks[0])

            for hand_idx, hand in enumerate(results.hand_landmarks):
                # Print index finger tip position
                tip = hand[8]
                status = f"Hand {hand_idx + 1}: Index x={tip.x:.2f} y={tip.y:.2f}"
                #print(f"\r{status:50}", end="")

                # Draw all landmarks
                for idx, lm in enumerate(hand):
                    cx, cy = int(lm.x * w), int(lm.y * h)
                    # Tips are green, others are blue
                    color = (0, 255, 0) if idx in [4, 8, 12, 16, 20] else (255, 0, 0)
                    cv2.circle(frame, (cx, cy), 5, color, -1)

                # Draw skeleton connections
                connections = [
                    (0, 1), (1, 2), (2, 3), (3, 4),  # Thumb
                    (0, 5), (5, 6), (6, 7), (7, 8),  # Index
                    (0, 9), (9, 10), (10, 11), (11, 12),  # Middle
                    (0, 13), (13, 14), (14, 15), (15, 16),  # Ring
                    (0, 17), (17, 18), (18, 19), (19, 20)  # Pinky
                ]
                for start, end in connections:
                    x1, y1 = int(hand[start].x * w), int(hand[start].y * h)
                    x2, y2 = int(hand[end].x * w), int(hand[end].y * h)
                    cv2.line(frame, (x1, y1), (x2, y2), (200, 200, 200), 2)

        # Show hand count
        cv2.putText(frame, f"Hands: {len(results.hand_landmarks)}", (10, 30),
                    cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)

        cv2.imshow("Hand Tracking", frame)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()
    landmarker.close()
    hand_tracking = False

#_________________________________________________________________
# MAIN
##################################################################
if __name__ == "__main__":
    #serial_com = init_serial_com()
    UI()

