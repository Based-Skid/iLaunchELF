<img src="https://i.gyazo.com/6e8264c6e2de352aeb649bdba04a75e4.png">

VTSPS2-HBDL v0.24
HomeBrew DownLoader

Forked from iLaunchELF by VTSTech

<b>Somewhat unstable right now. Under heavy development. Works in testing on real hardware. Use with caution. Writes to either MC based on user selection.
Do not use on SCPH-9XXXX with 2.30 BIOS. Tested on SCPH-70001

* Fully functional in PCSX2
* Downloading works on Real PS2.
* Checking works on Real PS2
* Overwriting works on Real PS2
* Creating folders works on Real PS2</b>

Credits:<br>
Original iLaunchELF Created by krHACKen & Based_Skid<br>
crc32.c written by Lammert Bies, (c) 1999-2016 Lammert Bies. File is licensed under the MIT License<br>
Thanks to krHACKen, Based_Skid, HWC & TnA on Discord for helping me debug issues<br>

<pre>
Changelog:
v0.24 2020-06-05 6:29AM
Now displays Remote CRC32
Local CRC32 defaults to 'unchecked'
Checking for files that don't exist tentatively working.
00000000 is File Not Exist

v0.23 06-03-2020 8:29PM
Added INFOGB, PS2SX
Now Supports 15 Total Homebrews
Now using 8.3 Filename standard.
Current CRC32 List now distributed with binary releases.

v0.22 06-02-2020 9:45PM
Added support for more homebrew
Added HTTP Mirror support.
Added Cuban mirror. Thanks HWC!
Added missing 'Push Start to Exit' to menu
Added ESDL.ELF, 2048.ELF, FCEU.ELF, PICO.ELF
Launching Homebrew now partially functions.
Launches WLE/OPL in real hardware testing .. but not Pico/FCEU

v0.21 06-02-2020 4:45PM
New Menu
Now displays IP on main screen
Supports many more homebrews.
Fully functional in PCSX2
Tentatively fully working on Real PS2
-Checking
-Downloading+Overwrite
-Downloading+Creating Folder
All work on my SCPH-70001 with this build.
Code cleanup.
Removed delays/debug output.

v0.2 05-30-2020 10:15PM
First functional release.

Supports OPL, WLE, HDL, ESR
CRC32 is displayed but not compared currently.
Tested on real hardware (SCPH-70001).
Overwrites existing files correctly.
Only writing to MC0 for now.

v0.1 05-26-2020 01:44AM
Initial Fork
</pre>

## Notice
This Program Was Built with The PS2DEV sdk and has utilized sample code from it https://github.com/ps2dev/ps2sdk

This Program Utilizes the HTTP Client File System from the PS2DEV sdk

The ELF Loading Portion of the Code is From the w/uLE project https://github.com/AKuHAK/uLaunchELF/tree/master/loader


## Warranty
This Application Has No Warranty. The Creators of this app cannot be held Responsible for any damage
