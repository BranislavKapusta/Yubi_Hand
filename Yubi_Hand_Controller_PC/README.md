# Yūbi Hand Control with PC via Bluetooth
> [!WARNING]
> This is not currently describing the Controller but only the type of communication


This is control program for Yubi Hand project (prosthetic hand)

## Control messages sent via Serial and Serial Bluetooth.

> **Note:** dont forget to check if the /n is sent or not.

### `POS001` — Position Control

Sets individual positions for each finger/joint, with a wait time before the next command executes.

**Format:**
```
TYPE, time, n1, n2, n3, n4, n5, n6, n7, n8
```

| Field | Description | Type | Range |
|-------|-------------|------|-------|
| `TYPE` | Command type — `POS001` | string | — |
| `time` | Wait time until next command executes | int | ms |
| `n1` | Index finger | int | `[0, 90]` |
| `n2` | Middle finger | int | `[0, 90]` |
| `n3` | Ring finger | int | `[0, 90]` |
| `n4` | Pinky finger | int | `[0, 90]` |
| `n5` | Thumb finger | int | `[0, 90]` |
| `n6` | Seing | int | `[0, 90]` |
| `n7` | Wrist 1 | int | `[55, 125]` |
| `n8` | Wrist 2 | int | `[55, 125]` |

> **Note:** `-1` means keep the old position.

**Example:**
```
POS001, 1000, 45, 45, 45, 45, -1, -1, -1, -1
```

---

### `GRIP01` — Grip Preset

Sets a predefined grip pose.

**Format:**
```
TYPE, POSE
```

| Field | Description | Type |
|-------|-------------|------|
| `TYPE` | Command type — `GRIP01` | string |
| `POSE` | Grip pose | string |

**Available poses:**
- `open`
- `fist`
- `pinch`
- `3pinch`
- `point`
- `cilinder`
- `small_cilinder`
- `relax`

**Example:**
```
GRIP01, open
```

---

### `OFF001` — Motor On/Off

Turns a specific motor on or off.

**Format:**
```
TYPE, CMD, MotorIndex
```

| Field | Description | Type | Range |
|-------|-------------|------|-------|
| `TYPE` | Command type — `OFF001` | string | — |
| `CMD` | `ON` or `OFF` | string | — |
| `MotorIndex` | Motor index | int | `[0, 7]` |

**Example:**
```
OFF001, off, 6
```
