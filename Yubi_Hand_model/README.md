\# Yubi Hand Model



Main folder of the Yubi Hand project. It contains the 3D-printable parts (`.stl`), the URDF model for simulation and robotics software, the material list, and the recommended print orientation.



\## Folder Structure



```

Yubi\_Hand\_model

├── Hand\_Assembly

│   ├── Finger\_Module\_withoutPads

│   ├── Finger\_Module\_withPads

│   ├── Mounting

│   │   └── UR3s\_50mm

│   ├── Palm

│   ├── Thumb\_withoutPad

│   └── Tumb\_withPad

├── Hand\_Assembly\_Model\_URDF

│   ├── meshes

│   └── Hand\_v3\_Assembly.urdf

├── Hand\_MaterialList.xlsx

└── Recommended orientation.pdf

```



\## Contents



\### Hand\_Assembly



Printable parts of the hand, sorted by module.



| Folder | Description |

|--------|-------------|

| `Finger\_Module\_withoutPads` | Finger module without pads. |

| `Finger\_Module\_withPads` | Finger module with pads. |

| `Mounting` | Mounting system for connecting the hand to different applications. |

| `Mounting/UR3s\_50mm` | Mounts for the UR3s with 50 mm pattern (see the README in that folder). |

| `Palm` | Palm of the hand. |

| `Thumb\_withoutPad` | Thumb without pad. |

| `Tumb\_withPad` | Thumb with pad. |



\### Hand\_Assembly\_Model\_URDF



URDF model of the complete hand assembly.



\- `Hand\_v3\_Assembly.urdf` – URDF description of the hand (version 3).

\- `meshes` – mesh files referenced by the URDF.



\### Other Files



\- `Hand\_MaterialList.xlsx` – list of the materials and parts needed to build the hand.

\- `Recommended orientation.pdf` – recommended orientation of the parts for 3D printing.

