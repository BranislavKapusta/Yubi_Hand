# Yūbi Hand

A prosthetic hand prototype with human-hand proportions and all finger motors housed inside the palm.

<img width="270" alt="Yūbi Hand photo 1" src="https://github.com/user-attachments/assets/de925a20-921b-482e-b672-26fe52523cd5" />
<img width="270" alt="Yūbi Hand photo 2" src="https://github.com/user-attachments/assets/268b1985-2687-4c50-8729-d57e2625454f" />
<img width="270" alt="Yūbi Hand photo 3" src="https://github.com/user-attachments/assets/975b1d03-ae8d-48b5-932a-2a1e7fe04915" />
<img width="270" alt="Yūbi Hand photo 4" src="https://github.com/user-attachments/assets/bc900f51-fd58-4aa9-bc8d-70ee7ad8c9aa" />
<img width="270" alt="Yūbi Hand photo 5" src="https://github.com/user-attachments/assets/c7245660-f6e0-45c2-9074-9be354b2dc9e" />
<img width="270" alt="Yūbi Hand photo 6" src="https://github.com/user-attachments/assets/e339b598-6e86-4d81-baf8-d50514b23467" />

## Build

### Finger module

A modular finger that can be dismounted individually. The index, middle, ring, and pinky fingers all use the same finger module.

- **Actuator:** Actuonix PQ12-R linear servo
- **Joint 1 (base to part one):** driven directly by the motor
- **Joint 2 (part one to part two):** torsion spring for extension, string coupled to joint 1 for flexion (90:110 ratio)
- **Fingertip:** hollow TPE grip pad

<img width="800" alt="Finger module CAD" src="https://github.com/user-attachments/assets/baea6373-3a3d-4da0-9412-7215d76c21e6" />
<img width="800" alt="Finger module detail" src="https://github.com/user-attachments/assets/1b5600a8-846c-49b8-abf8-a0e65ebaa8b2" />
<img width="800" alt="Finger module motion" src="https://github.com/user-attachments/assets/e1744236-8432-49f3-97f1-1fea32d240a0" />

### Hand palm

The palm core holds the four finger modules and the thumb swing motor. Each finger has its own mounting position and angle, and a palm cover gives the hand its shape.

<img width="800" alt="Palm photo and CAD" src="https://github.com/user-attachments/assets/60ba8baa-9203-4d08-9ef9-23a70496c0dd" />
<img width="800" alt="Thumb connection photo and CAD" src="https://github.com/user-attachments/assets/7572c566-4a06-40ef-8ddf-f4990ef81b77" />
<img width="800" alt="Thumb swing motion" src="https://github.com/user-attachments/assets/69af7b28-88c4-4b49-87ce-a9ba0f31725f" />

#### Thumb

A single-joint thumb module connected to the palm core by a motor-driven plate hinge (swing). It is mounted at a fixed open angle and rotated 45° to the plane of the palm.

<img width="800" alt="Thumb module" src="https://github.com/user-attachments/assets/d33e4529-0eff-424d-8329-cf6788d3e97a" />

## Controls

The hand is controlled by an ESP32 with a servo board. It can be controlled over:

- **WiFi:** web page with manual finger control and predefined grasps
- **Bluetooth:** messages for finger positions and grasps, with a Python control script
- **Serial:** same messages as Bluetooth

Grasps are time-based sequences.

Control code: [ Hand_Control ](https://github.com/BranislavKapusta/Yubi_Hand/tree/main/Yubi_Hand_Controls)

---
Free for personal and non-commercial use, see LICENSE
