### ASE - 8086 Emulator
ASE is designed to act as a minimal replacement for existing aging emulators.

ASE is currently in active development (as a hobby project, you are welcome to contribute!) and as of now supports approx 30-40 common
instructions - Move, jump, set/clear flag values etc.

### Building ASE:

```bash
./build.sh
```

### Tested on:
Ubuntu 18.04 - `gcc & clang`

Windows 10 x64 1903 (WSL)

### Running ASE:

`./ase file.asm -a`

### Supported command line args
```
-a : Enable all (below) emulator specified flags
-d : Enable debug mode
-f : Show flag contents
-h : Show help (this) screen
-m : Show memory contents
-r : Show register contents
-s : Show stack contents
-v : Show version info
```

### Sample program

```asm
ORG 100h

MOV AL, 12         ; This is a comment line!
MOV BX, 9876

STC
JC L1

PUSH BX
L1: PUSH AX

STD
```

### Console output
```
Flags:
[CF]:[1]
[DF]:[1]
[IF]:[0]
[OF]:[0]
[PF]:[1]
[SF]:[0]
[ZF]:[0]

Register:
[AX]:[c]
[BX]:[2694]
[CX]:[]
[DX]:[]

[0x7fffee47a4c8]:[c]
```
