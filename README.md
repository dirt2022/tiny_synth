<!--
SPDX-FileCopyrightText: 2026 dirt2022

SPDX-License-Identifier: CC0-1.0
-->

# FM Synthesizer

A streaming FM synthesizer written in C. Parses `.rdi` script files and renders audio in real-time.

## Build

```bash
mkdir build && cd build
cmake .. -DMULTI-THREAD=1 -DLTO=1
make
```

## Usage

```bash
# Play directly
./main pa song.rdi

# Export to PCM file
export PLAYER_OUTPUT=output.pcm
./main file song.rdi
```

## Script Syntax

### Global Settings

| Command | Description |
|---------|-------------|
| `SBPM bpm` | Set beats per minute |
| `STRN n` | Set number of tracks |
| `STGL track gain` | Track global loudness (sum should be 1.0) |
| `SLLP start end gain ...` | Global loudness pattern (loops) |

### Synthesis Settings

| Command | Description |
|---------|-------------|
| `SWAA` | Define FM operators and their envelopes |
| `SNAT track grad dur ...` | Track-level amplitude envelope |

### Data Mode

| Command | Description |
|---------|-------------|
| `DATA` | Enter note input mode |
| `!` | **Required** - Exit DATA mode |

### Note Format

```
@ track C4T0.5 G4T1L0.7 0T0.25
```

- `C4`: note name (supports `#`, e.g. `C#4`)
- `T0.5`: duration in beats
- `L0.7`: loudness factor (optional, default 1.0)
- `0`: rest

## Example

```
SBPM 120
STRN 2
STGL 0 0.7
STGL 1 0.3
SWAA
1 0 2 0 3 0 4 0 5 0 6 0
20 0.05 0 0.95
6.8 0.05 0 0.95
2.04 0.05 0 0.95
1.7 0.05 0 0.95
1.4 0.05 0 0.85 -0.3 0.1
1.3 0.05 0 0.80 -0.6 0.15
SNAT 0 20.000 0.050 -2.859 0.178 -1.402 0.178 -0.640 0.220 -0.269 0.212 -0.126 0.162
DATA
@ 0 C4T0.5 G4T0.5 A4T1 A4T1 B4T0.5 C5T0.5
@ 1 E4T1 E4T0.5 F4T0.5 G4T1 G4T1
!
```

## Threading

Set OpenMP thread count via `OMP_NUM_THREADS` environment variable.

## License

GPL-3.0-or-later
All commits prior to commit f41e16d868d95d31f77ecad4b03a253cdc95fb01 are also licensed under GPL-3.0-or-later by the copyright holder.
