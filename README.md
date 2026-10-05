# BEKANT Firmware (single-button gestures)

Fork of [ivanwick/bekantfirmware](https://github.com/ivanwick/bekantfirmware). The original firmware uses two-button gestures (<kbd>△</kbd> + <kbd>▽</kbd>); this fork drives position memory with one button: double click to move to a stored position, click then hold to save it. See the [User's Guide](#users-guide).

![Position memory diagram](https://github.com/ivanwick/bekantfirmware/wiki/images/diagram.png)

Control firmware for IKEA BEKANT adjustable-height desk with lower and upper memory positions. Can be flashed onto the OEM controller, without changing any hardware.

Pre-program a sitting and standing position for your desk and move between them with a double click.

## Build & Flash

The microcontroller (PIC16LF1938) can be programmed through ICSP using a PICKit or other PIC programmer.

[Latest Release](https://github.com/arnidan/bekantfirmware/releases/latest)

Notes:
- MPLAB X / IPE v6.25 and later dropped PICkit 3 support. Use MPLAB X IPE v6.20 from the [Microchip archive](https://www.microchip.com/en-us/tools-resources/archives/mplab-ecosystem); installing only the 8-bit device support is enough.
- The OEM firmware is code protected, so a read-back "backup" contains only zeroes. Flashing is a one-way trip from the stock firmware.
- Disconnect the controller from the legs and the desk from power while flashing: `-W` powers the board from the PICkit.

Command line build and flash on macOS, without the IDE (XC8 v4.00, MPLAB X IPE v6.20 for PICkit 3). Both run natively on Apple silicon:

```sh
brew install --cask mplab-xc8
DFP=/Applications/microchip/mplabx/v6.20/packs/Microchip/PIC12-16F1xxx_DFP/1.7.242/xc8
mkdir -p /tmp/bekant
cd bekantfirmware.X
xc8-cc -mcpu=16LF1938 -mdfp=$DFP -O0 -o /tmp/bekant/bekant.hex $(find . -name '*.c')

# -Z keeps the saved positions in EEPROM
cd /Applications/microchip/mplabx/v6.20/mplab_platform/bin
../../sys/java/zulu8*/bin/java -jar ../mplab_ipe/ipecmd.jar \
    -TPPK3 -P16LF1938 -W -M -F/tmp/bekant/bekant.hex -Y -Z
```

`ipecmd.sh` in v6.20 cannot find its bundled Java on macOS, so the jar is run directly. To check the connection without writing anything, replace `-M -F... -Y -Z` with `-I`.

## Documentation

[Project Wiki](https://github.com/ivanwick/bekantfirmware/wiki/)

[Installation Guide](https://github.com/ivanwick/bekantfirmware/wiki/Installation-Guide)

The wiki describes the original two-button gestures; use the [User's Guide](#users-guide) below for this fork.

## User's Guide

| Gesture | Action |
| ------- | ------ |
| <kbd>△</kbd> (hold) | Move up  |
| <kbd>▽</kbd> (hold) | Move down |
| <kbd>△</kbd><kbd>△</kbd> (double click) | Move up to upper memory position  |
| <kbd>▽</kbd><kbd>▽</kbd> (double click) | Move down to lower memory position  |
| <kbd>△</kbd>, then hold <kbd>△</kbd> 3 sec | Save current position as upper |
| <kbd>▽</kbd>, then hold <kbd>▽</kbd> 3 sec | Save current position as lower |

A click is a press shorter than 0.3 sec; the two clicks of a double click must be within 0.4 sec. Manual movement starts after holding a button for 0.3 sec.

Automatic movement to a stored position can be canceled by pressing either button or cutting power.

For the *Save* gesture, click a button, then press it again and hold for 3 seconds until the legs click. The table does not move during the hold.

## Tests

The button gesture logic has a host test:

```sh
clang -Wall -o /tmp/gesture_test test/gesture_test.c && /tmp/gesture_test
```

## Memory Positions

Memory positions are stored in the PIC EEPROM. These are 16-bit little-endian integers for the encoder values of the motorized table legs.

| Offset | Length | Default | Description |
| ------ | ------ | ------- | ----------- |
| 0x00   | 2 bytes | 0x0636 | Lower position encoder value, default about 70cm |
| 0x02   | 2 bytes | 0x1600 | Upper position encoder value, default about 110cm |

## Disclaimer

Use at your own risk.

The real reason that position memory is not built into the stock firmware is probably for safety and liability. Requiring a human to hold a button while the table is moving keeps a human in the control loop.

Ensure that the path of the table is clear during all movement.

## References
 1. <a name="1">https://web.archive.org/web/20190116092248/https://blog.rnix.de/hacking-ikea-bekant/</a>
 2. <a name="2">https://github.com/trainman419/bekant</a>
 3. <a name="3">https://github.com/robin7331/IKEA-Hackant</a>
 4. <a name="4">https://github.com/diodenschein/TableMem</a>
 5. <a name="5">https://en.wikipedia.org/wiki/Local_Interconnect_Network</a>
