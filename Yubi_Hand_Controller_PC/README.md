# Yūbi Hand Control with PC via Bluetooth

A desktop control panel for the Yūbi Hand, written in Python. It sends finger positions and predefined grasps to the hand over USB serial or Bluetooth serial. An optional, experimental webcam hand tracking mode is also included.

## UI

<img width="700" alt="Control panel UI" src="https://github.com/user-attachments/assets/16e0a6da-671f-40e4-aa27-7937891165fd" />

## Features

### Finger controls
Sliders for each joint of the hand (Index, Middle, Ring, Pinky, Thumb, Swing, Wrist 1, Wrist 2).

### Grasps
One-click buttons for predefined grasps:

- Open
- Fist
- Pinch
- Pinch 3
- Cylinder
- Small Cylinder
- Relax

### Sending modes
- **Manual Mode:** the current slider values are sent with the **Send Now** button.
- **Periodic Mode:** the current slider values are sent automatically every 250 ms.

### Communication
- **Serial COM:** USB serial connection to the hand
- **Bluetooth:** Bluetooth serial connection to the hand

The two connections are switched with the **Serial COM** and **Bluetooth** buttons, and only one is active at a time.

### Hand tracking (optional, experimental)
Controls the fingers from a webcam image using MediaPipe hand detection.

- Tracks the Index, Middle, Ring, and Pinky fingers
- Shows a live camera window with the detected hand landmarks
- Sent to the hand in Periodic Mode
- Toggled with the **Tracking** button
- Press `q` in the camera window to close it

## Requirements

- Python 3
- `pyserial`
- `ttkbootstrap`
- `opencv-python`
- `mediapipe`

The hand landmarker model (`hand_landmarker.task`) is downloaded automatically on first use of hand tracking.

## Configuration

Set at the top of the script:

| Setting | Default | Description |
|---|---|---|
| `PORT` | `COM4` | USB serial port |
| `bluetooth_PORT` | `COM7` | Bluetooth serial port |
| `BAUD` | `115200` | Baud rate |
| `periodic_speed` | `250` | Periodic send interval in ms |

The camera used for hand tracking is selected in `cv2.VideoCapture(1)`.

## Usage

1. Set the ports in the configuration.
2. Run the script.
3. Select **Serial COM** or **Bluetooth**.
4. Move the sliders and press **Send Now**, use a grasp button, or switch to Periodic Mode.
5. Optionally, press **Tracking** to control the fingers with a webcam.
