# GC Pocket Adapter+ Firmware
This is the official Github page for the GC Pocket Adapter+ from Hand Held Legend.

## Software and PCB License
<a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/3.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-nc-sa/3.0/88x31.png" /></a><br />This firmware and PCB model are licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/3.0/">Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License</a>.

## 3D Models License

<a rel="license" href="http://creativecommons.org/licenses/by/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by/4.0/88x31.png" /></a><br />The 3D shell and buttons are licensed under a <a rel="license" href="http://creativecommons.org/licenses/by/4.0/">Creative Commons Attribution 4.0 International License</a>.

## Shop Link
- [GC Pocket Adapter+ Kit (Includes 3D Printed Parts)](https://handheldlegend.com/products/4-port-gc-pocket-adapter-gamecube-to-usb-c-controller-adapter-diy-kit)

## Detailed Information
All of our guides
are listed on [our Wiki linked here](https://wiki.handheldlegend.com).

## 3D Parts
All 3D parts are in the [/3d directory](https://github.com/HandHeldLegend/GC-Adapter-RP2040/tree/master/Production/3D) on this Github repo in STEP format. A 3D STEP file is provided for accurate reference on the PCB dimensions.

## Features
- 4 USB Input modes.
- 4 controller ports with rumble supported.
- Adapts GameCube controllers for use on Windows, Mac, Android, and Nintendo Switch.
- Nintendo Switch only supports 8ms polling rate (Locked by Nintendo Switch).
- RGB status lights for all 4 controllers.
- Two buttons to change input mode.

## Basic Operation
- When the device is plugged in, the LED light shows a color to indicate which mode it is in. 
	- Yellow - Nintendo Switch Pro Controller
	- Purple - GameCube OEM Mode 
	- Green - XInput (Xbox 360 Controllers)
    - Pink - Slippi (1khz polling)