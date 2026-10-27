# Yubi Hand Model

Main folder of the Yubi Hand project. It contains the 3D-printable parts (`.stl`), the URDF model for simulation and robotics software, the material list, and the recommended print orientation.

## Folder Structure

```
Yubi_Hand_model
├── Hand_Assembly
│   ├── Finger_Module_withoutPads
│   ├── Finger_Module_withPads
│   ├── Mounting
│   │   └── UR3s_50mm
│   ├── Palm
│   ├── Thumb_withoutPad
│   └── Tumb_withPad
├── Hand_Assembly_Model_URDF
│   ├── meshes
│   └── Hand_v3_Assembly.urdf
├── Hand_MaterialList.xlsx
└── Recommended orientation.pdf
```

## Contents

### Hand_Assembly

Printable parts of the hand, sorted by module.

| Folder | Description |
|--------|-------------|
| `Finger_Module_withoutPads` | Finger module without pads. |
| `Finger_Module_withPads` | Finger module with pads. |
| `Mounting` | Mounting system for connecting the hand to different applications. |
| `Mounting/UR3s_50mm` | Mounts for the UR3s with 50 mm pattern (see the README in that folder). |
| `Palm` | Palm of the hand. |
| `Thumb_withoutPad` | Thumb without pad. |
| `Tumb_withPad` | Thumb with pad. |

### Hand_Assembly_Model_URDF

URDF model of the complete hand assembly.

- `Hand_v3_Assembly.urdf` – URDF description of the hand (version 3).
- `meshes` – mesh files referenced by the URDF.

### Other Files

- `Hand_MaterialList.xlsx` – list of the materials and parts needed to build the hand.
- `Recommended orientation.pdf` – recommended orientation of the parts for 3D printing.
