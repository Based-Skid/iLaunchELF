<img src="https://i.gyazo.com/9787d618dd7191cdfd8160f9d0c0d619.png">
<img src="https://i.gyazo.com/a84c48fa84df9e41d6d8e5a46c2d54a1.png">
<img src="https://i.gyazo.com/12ec4eceef16fc46ce1417a6d3bb028b.png">

VTSPS2-HBDL v0.31
HomeBrew DownLoader

Forked from iLaunchELF by VTSTech

<b>Rather stable right now. Almost feature complete. Works in testing on real hardware. Writes to either MC or USB based on user selection.
Do not use on SCPH-9XXXX with 2.30 BIOS. Extensively tested on SCPH-70001. Working reports from SCPH-5XXXX as well.

* Fully functional in PCSX2
* Downloading works on Real PS2.
* Checking works on Real PS2
* Overwriting works on Real PS2
* Creating folders works on Real PS2
* Launching mostly works on Real PS2 (WLE, OPL, GSM launch, a few others don't)
* USB Support, Sequential Downloads</b>

Credits:<br>
Original iLaunchELF Created by krHACKen & Based_Skid<br>
crc32.c written by Lammert Bies, (c) 1999-2016 Lammert Bies. File is licensed under the MIT License<br>
Packed with PS2-Packer v1.1.0 by Nicolas "Pixel" Noble<br>
Thanks to krHACKen, Based_Skid, HWC & TnA on Discord for helping me debug issues<br>
Big Thanks to fjtrujy for their PS2 Port of RetroArch<br>

<pre>
Changelog:
v0.31 2020-10-17 11:59PM
file_crc32() updates
Code cleanup

v0.30 2020-10-13 12:49AM
Now POSIX compliant
No longer using fio/fileXio calls
Updated wLaunchELF to v4.43A Commit 970ca992
Removed some verbosity from Download()

v0.29 2020-10-09 11:02PM
Added LBFN v0.07.19 WIP by Nika Mod by HWC
Updated ESR to R10F DVD Direct
Updated OPL to 1562-Beta CI-compile #85: Commit 41f7cf0
Updated RetroArch to v1.9.0
Now supports 27 HomeBrews

v0.28 2020-06-09 1:26AM
Now downloads HomeBrew List upon start.
Added DOSBox, PS2DOOM
Removed some verbosity from Download()
Added some verbosity to DownloadList()
Now supports 26 HomeBrews

v0.27 2020-06-08 5:10AM
No longer using loader.elf
Using LoadElf() from MPLUS-LOADER3
Now displays remote filesize
Now displays remote version
Reorganized menu to accommodate new items
Menu now dynamically aligns itself
Added 'Aura for Laura' Demo
Added ZoneLoader
Updated RetroArch, OPL, SNES9X, PSMS
Now supports 24 Homebrews

v0.26 2020-06-07 10:15AM
Renamed ESDL to PS2ESDL
Now supports downloading multiple files per homebrew!
Added NEOCD
Added PGEN
Added PSMS
Added PVCS
Added SNES9X
Fixed bug reading CRC's when entire filename was present in two homebrews.
Now supports 22 Homebrews

v0.25 2020-06-06 1:05AM
Now supports USB!
2 more Homebrews added
(VTSPS2-EJECT & VTSPS2-TESTMODE)
Now supports 17 total Homebrews
No longer in ALPHA, First BETA release.

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
This Program Was Built with The PS2DEV SDK (Compiled May 28th 2020, Commit: ec0cbfd) and has utilized sample code from it https://github.com/ps2dev/ps2sdk

This Program Utilizes the HTTP Client File System from the PS2DEV sdk

## Warranty
This Application Has No Warranty. The Creators of this app cannot be held Responsible for any damage
