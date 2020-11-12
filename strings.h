//Strings
char *appName = "VTSPS2-HBDL HomeBrew DownLoader";
char *appVer = " v0.33 2020-11-12 (Commit: d864608)\n";
char *appAuthor = "Forked from iLaunchElf by VTSTech github.com/Veritas83/VTSPS2-HBDL\nOriginal iLaunchElf By: krHACKen, Based_Skid, Copyright \xa9 2018 \n \n";
char *appNotice = "Notice: This App Contains Code from uLaunchELF \n";
//char *osdmsg = "Exiting to OSDSYS\n";
//char *appFail = "Application Failure!\n";
//char *modloadfail = "Failed to load module: ";

// Menu Option Text
//char *txtcrossBtn = "-Press X to Check WLE.ELF\n";
//char *txtsqrBtn = "-Press Square to Check HDL.ELF\n";
//char *txtcirBtn = "-Press Circle to Check OPL.ELF\n";
//char *txttriBtn = "-Press Triangle to Check ESR.ELF\n";
//char *txtR1Btn = "-Press R1 to Update WLE.ELF\n";
//char *txtL1Btn = "-Press L1 to Update HDL.ELF\n";
//char *txtR2Btn = "-Press R2 to Update OPL.ELF\n";
//char *txtL2Btn = "-Press L2 to Update ESR.ELF\n";
//char *txtselBtn = "-Press Select to unused...\n";
//char *txtstrtBtn = "-Press Start to Reboot PS2.\n";
//char *txtL3Btn = "-Press L3 to Start WLE.ELF.\n";

char *devices[] = {"mc0:/", "mc1:/", "mass:/"};
char *paths[] = {"APPS/", "APP_$ELF/", "$ELF/"};
char *actions[] = {"CHECK", "DOWNLOAD", "LAUNCH"};
char *targets[]= {"AURA.ELF", "DOSBOX.ELF", "EJECT.ELF", "ESR.ELF", "GSM.ELF", "HDL.ELF", "INFOGB.ELF", "LBFN.ELF", "NEOCD.ELF", "OPL.ELF", "PGEN.ELF", "PS2DOOM.ELF", "PS2ESDL.ELF", "PS2SX.ELF", "PSMS.ELF", "PVCS.ELF", "RA_2048.ELF", "RA_FCEU.ELF", "RA_MGBA.ELF", "RA_PICO.ELF", "RA_QNES.ELF", "SMS.ELF", "SNES9X.ELF", "SNESSTN.ELF", "TESTMODE.ELF", "WLE.ELF", "ZONELDR.ELF"};
char ELF_NO_EXT[] = "";
char PATH_ELF[] = "";
char PATH_APP[] = "";
char make_path[] = "";
char action[32], device[32], path[256], fn[16], ofn[16];
char url[] = "";
char mirror0[] = "http://hbdl.vts-tech.org/";
char mirror1[] = "http://www.hwc.nat.cu/ps2-vault/ps2hbdl/";
