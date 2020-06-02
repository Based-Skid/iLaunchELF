//Strings
char *appName = "VTSPS2-HBDL HomeBrew DownLoader";
char *appVer = " v0.21 2020-06-02 (Commit: DEV)\n";
char *appAuthor = "http://hbdl.vts-tech.org\nForked from iLaunchElf by VTSTech\nOriginal iLaunchElf By: krHACKen, Based_Skid, Copyright \xa9 2018\n\n";
char *appNotice = "Notice: This App Contains Code from uLaunchELF \n";
char *osdmsg = "Exiting to OSDSYS\n";
char *appFail = "Application Failure!\n";
char *modloadfail = "Failed to load module: ";

// Menu Option Text
char *txtcrossBtn = "-Press X to Check WLE.ELF\n";
char *txtsqrBtn = "-Press Square to Check HDL.ELF\n";
char *txtcirBtn = "-Press Circle to Check OPL.ELF\n";
char *txttriBtn = "-Press Triangle to Check ESR.ELF\n";
char *txtR1Btn = "-Press R1 to Update WLE.ELF\n";
char *txtL1Btn = "-Press L1 to Update HDL.ELF\n";
char *txtR2Btn = "-Press R2 to Update OPL.ELF\n";
char *txtL2Btn = "-Press L2 to Update ESR.ELF\n";
char *txtselBtn = "-Press Select to unused...\n";
char *txtstrtBtn = "-Press Start to Reboot PS2.\n";
char *txtL3Btn = "-Press L3 to Start WLE.ELF.\n";

char *devices[] = {"mc0:/", "mc1:/"};
char *paths[] = {"APPS/", "APP_$ELF/", "$ELF/"};
char *actions[] = {"CHECK", "DOWNLOAD", "LAUNCH"};
char *targets[]= {"ESR.ELF", "GBA.ELF", "GSM.ELF", "HDL.ELF", "NES.ELF", "OPL.ELF", "SMS.ELF", "SNES.ELF", "WLE.ELF"};
char ELF_NO_EXT[] = "";
char PATH_ELF[] = "";
char PATH_APP[] = "";
char make_path[] = "";
char action[32], device[32], path[256], fn[128];