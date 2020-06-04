/*
                 VTSPS2-HBDL by VTSTech
	Based on iLaunchELF by krHACKen & Based_Skid

   ELF loading Portions of this Code are From loader.c
   https://github.com/AKuHAK/uLaunchELF/blob/master/loader/loader.c

 */
// All Includes and Declarations are in Main.h, additional C Files should include main.h

#include "main.h"
#include "strings.h"

extern void loader_elf; // wLaunchELF's loader.elf. Embed this sukka in your ELF.

int http_mirror = 0;

typedef struct {
	u8	ident[16];
	u16	type;
	u16	machine;
	u32	version;
	u32	entry;
	u32	phoff;
	u32	shoff;
	u32	flags;
	u16	ehsize;
	u16	phentsize;
	u16	phnum;
	u16	shentsize;
	u16	shnum;
	u16	shstrndx;
} elf_header_t;

typedef struct {
	u32	type;
	u32	offset;
	void	*vaddr;
	u32	paddr;
	u32	filesz;
	u32	memsz;
	u32	flags;
	u32	align;
} elf_pheader_t;

int getFileSize(int fd) {
	int size = fileXioLseek(fd, 0, SEEK_END);
	fileXioLseek(fd, 0, SEEK_SET);
	return size;
}

void menu_header(void)
{
	scr_printf(" \n");
	scr_printf(appName);
	scr_printf(appVer);
	scr_printf(appAuthor);
	//scr_printf(appNotice);
}

void menu_Text(void)
{
	scr_clear();
	menu_header();
	extern char vtsip[15];
	extern char mirror0[];
	extern char mirror1[];
	//char *devices[] = {"mc0:/", "mc1:/"};
	//char *paths[] = {"APPS/", "APP_$ELF/", "$ELF/"};
	//char *actions[] = {"CHECK", "DOWNLOAD", "LAUNCH"};
	//char *targets[]= {"2048.ELF", "ESR.ELF", "FCEU.ELF", "GBA.ELF", "GSM.ELF", "HDL.ELF", "NES.ELF", "OPL.ELF", "PICO.ELF", "SMS.ELF", "SNES.ELF", "WLE.ELF"};
	if (http_mirror == 0) {
		sprintf(mirror0,"http://hbdl.vts-tech.org/");
		scr_printf("IP Address: %s Mirror: %s\n",vtsip,mirror0);
	} else if (http_mirror == 1) {
		sprintf(mirror1,"http://www.hwc.nat.cu/ps2-vault/ps2hbdl/");
		scr_printf("IP Address: %s Mirror: %s\n",vtsip,mirror1);
	}
	scr_printf(" \n");
	//scr_printf("DEBUG: %s %s %s %s %d %d %d %d\n", action, device, path, fn, strlen(action), strlen(device), strlen(path), strlen(fn));
	//if ((strlen(action) == 0) || (strlen(device) == 0) || (strlen(path) == 0) || (strlen(fn) == 0))  {
		//strcpy(action,actions[0]);
		//strcpy(device,devices[0]);
		//strcpy(path,paths[0]);	
		//strcpy(fn,targets[0]);
	//}	
	if ((strcmp(action,"CHECK") != 0) && (strcmp(action,"DOWNLOAD") != 0) && (strcmp(action,"LAUNCH") != 0)) {
		strcpy(action,"CHECK");
	}
	if ((strcmp(device,"mc0:/") != 0) && (strcmp(device,"mc1:/") != 0)) {
		strcpy(device,"mc0:/");
	}
	if (strlen(fn) >= 13) {
		strcpy(fn,"ESDL.ELF");
		strcpy(path,"APPS/");
	}
	scr_printf("Mode: %s Device: %s Path: %s Target: %s\n",action,device,path,fn);
	scr_printf(" \n");
	scr_printf("-Press UP to Set Device.\n");
	scr_printf("-Press DOWN to Set Mode.\n");
	scr_printf("-Press LEFT to Set Path.\n");
	scr_printf("-Press RIGHT to Set Target.\n");
	scr_printf("-Press SELECT to Set Mirror.\n");
	scr_printf("-Press START to Exit.\n");
	scr_printf("-Press any other key to preform selected action\n");
	scr_printf(" \n");
}

void ResetIOP()
{

	SifInitRpc(0);           //Initialize SIFRPC and SIFCMD. Although seemingly unimportant, this will update the addresses on the EE, which can prevent a crash from happening around the IOP reboot.
	SifIopReset("", 0);      //Reboot IOP with default modules (empty command line)
	while(!SifIopSync()){}   //Wait for IOP to finish rebooting.
	SifInitRpc(0);           //Initialize SIFRPC and SIFCMD.
	SifLoadFileInit();       //Initialize LOADFILE RPC.
	fioInit();               //Initialize FILEIO RPC.

	// SBV Patches Are Not part of a Normal IOP Reset.
	sbv_patch_enable_lmb(); //SBV Patches
	sbv_patch_disable_prefix_check(); //SBV Patch Load Executable IRX And ELF Files From User-Writable Storage
	//sbv_patch_user_mem_clear(0x00100000); // You Can Specify a Starting Address for the Wipe
	//sbv_patch_user_mem_clear(0x02000000); // Disable Clear Memory With LoadExecPS2() when 0x02000000 is passed as an arg
}


void gotoOSDSYS(int sc)
{
	if (sc != 0)
	{
		scr_printf(appFail);
		if(sc ==1 || sc ==2 || sc ==3 || sc ==4 || sc ==5)
		{
			scr_printf(modloadfail);
		}
		if (sc == 1)
		{
			scr_printf("SIO2MAN\n");
		}
		if (sc == 2)
		{
			scr_printf("CDVDMAN\n");
		}
		if (sc == 3)
		{
			scr_printf("PADMAN\n");
		}
		if (sc == 4)
		{
			scr_printf("MCMAN\n");
		}
		if (sc == 5)
		{
			scr_printf("MCSERV\n");
		}
		if (sc == 6)
		{
			scr_printf("ERROR: Unknown\n");
		}
		//sleep(5);
	}
	ResetIOP();
	scr_printf(osdmsg);
	LoadExecPS2("rom0:OSDSYS", 0, NULL);
}

void LoadModules(void)
{
	int ret;

	ret = SifExecModuleBuffer(&freesio2, size_freesio2, 0, NULL, NULL);
	if (ret < 0)
	{
		scr_printf("Failed to Load freesio2 sw module");
		gotoOSDSYS(1);
	}

	ret = SifExecModuleBuffer(&iomanX, size_iomanX, 0, NULL, NULL);
	if (ret < 0)
	{
		scr_printf("Failed to Load iomanx sw module");
	}

	ret = SifExecModuleBuffer(&fileXio, size_fileXio, 0, NULL, NULL);
	if (ret < 0)
	{
		scr_printf("Failed to Load freesio2 sw module");
	}


	ret = SifExecModuleBuffer(&freepad, size_freepad, 0, NULL, NULL);
	if (ret < 0)
	{
		scr_printf("Failed to Load freepad sw module");
		gotoOSDSYS(3);
	}

	ret = SifExecModuleBuffer(&mcman, size_mcman, 0, NULL, NULL);
	if (ret < 0)
	{
		scr_printf("Failed to Load mcman sw module");
		gotoOSDSYS(4);
	}

	ret = SifExecModuleBuffer(&mcserv, size_mcserv, 0, NULL, NULL);
	if (ret < 0)
	{
		scr_printf("Failed to Load mcserv sw module");
		gotoOSDSYS(5);

	}

	ret = SifExecModuleBuffer(&ps2dev9, size_ps2dev9, 0, NULL, NULL);
	if (ret < 0)
	{
		scr_printf(" Could not load ps2dev9.IRX! %d\n", ret);
		SleepThread();
	}


	ret = SifExecModuleBuffer(&netman, size_netman, 0, NULL, NULL);
	if (ret < 0)
	{
		scr_printf(" Could not load netman.IRX! %d\n", ret);
		SleepThread();
	}

	ret = SifExecModuleBuffer(&smap, size_smap, 0, NULL, NULL);
	if (ret < 0)
	{
		scr_printf(" Could not load smap.IRX! %d\n", ret);
		SleepThread();
	}

	ret = SifExecModuleBuffer(&ps2ipnm, size_ps2ipnm, 0, NULL, NULL);
	if (ret < 0)
	{
		scr_printf(" Could not load ps2ip.IRX! %d\n", ret);
		SleepThread();
	}

	ret = SifExecModuleBuffer(&ps2ips, size_ps2ips, 0, NULL, NULL);
	if (ret < 0)
	{
		scr_printf(" Could not load ps2ips.IRX! %d\n", ret);
		SleepThread();
	}
	ps2ip_init();

	ret = SifExecModuleBuffer(&ps2http, size_ps2http, 0, NULL, NULL);
	if (ret < 0)
	{
        	scr_printf("	Could not load ps2http.IRX! %d\n", ret);
		gotoOSDSYS(5);
	}

}

/////////////////////////////////////////////////////////////////////
//waitPadReady
/////////////////////////////////////////////////////////////////////
static int waitPadReady(int port, int slot)
{
	int state;
	int lastState;
	char stateString[16];

	state = padGetState(port, slot);
	lastState = -1;
	while ((state != PAD_STATE_STABLE) && (state != PAD_STATE_FINDCTP1)) {
		if (state != lastState) {
			padStateInt2String(state, stateString);
		}
		lastState = state;
		state = padGetState(port, slot);
	}
	// Were the pad ever 'out of sync'?
	if (lastState != -1) {

	}
	return 0;
}

/////////////////////////////////////////////////////////////////////
//initalizePad
/////////////////////////////////////////////////////////////////////
static int initializePad(int port, int slot)
{

	int ret;
	int modes;
	int i;

	waitPadReady(port, slot);
	modes = padInfoMode(port, slot, PAD_MODETABLE, -1);
	if (modes > 0) {
		for (i = 0; i < modes; i++) {
		}

	}
	if (modes == 0) {
		return 1;
	}

	i = 0;
	do {
		if (padInfoMode(port, slot, PAD_MODETABLE, i) == PAD_TYPE_DUALSHOCK)
			break;
		i++;
	} while (i < modes);
	if (i >= modes) {
		return 1;
	}

	ret = padInfoMode(port, slot, PAD_MODECUREXID, 0);
	if (ret == 0) {
		return 1;
	}
	padSetMainMode(port, slot, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK);

	waitPadReady(port, slot);
	padInfoPressMode(port, slot);

	waitPadReady(port, slot);
	padEnterPressMode(port, slot);

	waitPadReady(port, slot);
	actuators = padInfoAct(port, slot, -1, 0);

	if (actuators != 0) {
		actAlign[0] = 0;
		actAlign[1] = 1;
		actAlign[2] = 0xff;
		actAlign[3] = 0xff;
		actAlign[4] = 0xff;
		actAlign[5] = 0xff;

		waitPadReady(port, slot);

		padSetActAlign(port, slot, actAlign);
	}
	else {
	}
	return 1;
}

/////////////////////////////////////////////////////////////////////
//buttonStatts
/////////////////////////////////////////////////////////////////////
static void buttonStatts(int port, int slot)
{
	int ret;
	ret = padRead(port, slot, &buttons);

	if (ret != 0) {
		paddata = 0xffff ^ buttons.btns;

		new_pad = paddata & ~old_pad;
		old_pad = paddata;
	}
}

/////////////////////////////////////////////////////////////////////
//checkPadConnected
/////////////////////////////////////////////////////////////////////
void checkPadConnected(void)
{
	int ret, i;
	ret = padGetState(0, 0);
	while ((ret != PAD_STATE_STABLE) && (ret != PAD_STATE_FINDCTP1)) {
		if (ret == PAD_STATE_DISCONN) {
			#if defined DEBUG
				scr_printf("Controller(%d, %d) is disconnected\n", 0, 0);
			#endif
		}
		ret = padGetState(0, 0);
	}
	if (i == 1) {
	}
}

/////////////////////////////////////////////////////////////////////
//pad_wat_button
/////////////////////////////////////////////////////////////////////
void pad_wait_button(u32 button)
{
	while (1)
	{
		buttonStatts(0, 0);
		if (new_pad & button) return;
	}
}

int Access_Test(char *arg)
{
	int fd, size;
	fileXioClose(fd);
	fd = fileXioOpen(arg, O_RDONLY);

	if(fd >= 0) {
		size = fileXioLseek(fd, 0, SEEK_END);
		fileXioClose(fd);
	} else return fd;

	return size;
}

int Download(char *urll, char *full_path)
{
	int size, urld, target, ret = 0;
	char buf[4000000];
	//FILE *urld;
	//FILE *target;
	fioClose(urld);
	fileXioClose(target);
	urld = fioOpen(urll, O_RDONLY);
	scr_printf("* URL Opened... %d\n", urld);
	target = fileXioOpen(full_path, O_RDWR | O_CREAT);
	scr_printf("* Local File Opened... %d\n", target);
	//fioClose(target);
	if(urld >= 0) {
		size = fioLseek(urld, 0, SEEK_END);
		//fileXioLseek(urll, 0, SEEK_SET);
		fioRead(urld, buf, size);
		sleep(1);
		scr_printf("* Downloaded Size... %d\n", size);
		ret = fileXioWrite(target,buf,size);
		scr_printf("* Local File Written! %d\n", ret);
		sleep(1);
		fioClose(urld);
		scr_printf("* URL Closed... %d\n", urld);
		fileXioClose(target);
		scr_printf("* Local File Closed... %d\n", target);
	} else {
		scr_printf("Download Error! Debug: %d %d %d", urld, target, size);
	}
	return size;
}

void substring(char s[], char sub[], int p, int l) {
   int c = 0;
   
   while (c < l) {
      sub[c] = s[p+c-1];
      c++;
   }
   sub[c] = '\0';
}

void file_crc32(char device[], char path[], char fn[])
{
	//scr_printf("DEBUG: file_crc32() called...\n");
	FILE *fp;
	size_t len;
	char tmp[32] = "";
	char f_crc32[16] = "";
	char full_path[256] = "";
  //Build full_path string
  strcpy(full_path,device);
  strcat(full_path,path);
  strcat(full_path,fn);
  //4MB file buffer.
  char buf[4000000], *file = full_path;
  //Close the file
  //fclose(fp);
  //scr_printf("File Closed: %d\n", fp);
  if (NULL == (fp = fopen(file, "rb")))
  {
        printf("Error! Unable to open %s for reading\n", file);
        //return -1;
  }
  //read file, store length in len, file contents in buf
  len = fread(buf, sizeof(char), sizeof(buf), fp);
  scr_printf("%d bytes read\n", len);
  //Close the file
  //fclose(fp);
  sleep(1);
  //Use sprintf to store crc_32() return value in tmp
  //  
  //If file is larger than buffer, update_crc_32() will
  //need to be looped to get large file CRC32
  sprintf(tmp,"%lX",crc_32(buf, len));
  //We only need the last 8 bytes of crc_32 return value
  //Except .. not anymore apparently?
  substring(tmp,f_crc32,9,8);
  //Display CRC32
  if (strlen(tmp) > 8) {
  	scr_printf("CRC32: %s\n",f_crc32);
  } else {
  	scr_printf("CRC32: %s\n",tmp);
  }
}

void str_crc32(char str[])
{
  size_t len;
  char tmp[32] = "";
  char f_crc32[16] = "";
  char full_str[4000000] = "";
  char buf[4000000] = "";
  strncpy(full_str,str,strlen(str));
  strcpy(buf,full_str);
  len = strlen(full_str);
  scr_printf("%d bytes read\n", len);
  sleep(1);
  sprintf(tmp,"%lX",crc_32(buf, strlen(full_str)));
  substring(tmp,f_crc32,9,8);
  scr_printf("CRC32: %s\n",f_crc32);
}

void DoTask(int task)
{
	u8 *pdata, *dest;
	elf_header_t *eh;
	elf_pheader_t *eph;
	int i, j, ret,launching,downloading,checking;
	char arg0[256], arg1[256], arg2[256], arg3[256], arg4[256], arg5[256], arg6[256], arg7[256], arg8[256];
	char *exec_args[9] = { arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8 };
	int argc = 0;
	int fd,file_size;
	//extern char device[128], path[128], fn[128];
	char full_path[256];
	extern char url[];
	/*
	exec_args[0] == the target ELF's URI. loader.elf will load that ELF.
	exec_args[1] to exec_args[8] == arguments to be passed to the target ELF.
	*/
	launching=0;
	downloading=0;
	checking=0;
	if (task != 0)
	{
		if (task == 1)
		{
			checking=1;
			strcpy(full_path,device);
			strcat(full_path,path);
			strcat(full_path,fn);  					
		}
		else if (task == 2)
		{
			downloading=1;
			if (http_mirror == 0) {
				strcpy(url,mirror0);
			} else if (http_mirror == 1) {
				strcpy(url,mirror1);
			}
			strcat(url,fn);
			strcpy(exec_args[0], url);
			strcpy(full_path,device);
			strcat(full_path,path);
			//remove trailing / for MkDir
			substring(full_path,make_path,1,strlen(full_path)-1);
			fileXioMkdir(make_path,0777);
			strcat(full_path,fn);  				
			argc = 1;
			sleep(2);							
		}
		if (task == 3)
		{
			launching=1;
			strcpy(full_path,device);
			strcat(full_path,path);
			strcat(full_path,fn);
			strcpy(exec_args[0],full_path);
			argc = 1;
		}		
	} else asm volatile("break\n"); // OUT OF BOUNDS, UNDEFINED ITEM!
	//Clear Screen To Make This Look tidy!
	scr_clear();
	menu_header();
	if (downloading==1){
	  fileXioClose(fd);
	  char buf[4000000], *file = full_path;
	  //strcpy(url,exec_args[0]);
		scr_printf("* Downloading...\n");
		//Access Test (Make sure The Elf can Actually be Loaded)
		scr_printf("* URL: %s\n", exec_args[0]);
		scr_printf("* Path: %s\n", full_path);
		ret = Download(exec_args[0],full_path);
		sleep(4);
		if(ret <= 0) {
			scr_printf("* Error! Could not open the file\n");
		} else {
			scr_printf("* File Size: %d bytes\n", ret);
			sleep(2);
			fd = fileXioOpen(full_path, O_RDONLY);
			file_size = getFileSize(fd);			
			if (file_size >= 1) {
				scr_printf("* %s Exists!\n", full_path);
			} else {
				scr_printf("* %s Does Not Exist!\n", full_path);
			}
			fileXioClose(fd);
			//scr_printf("DEBUG: %s %s %s %s\n", action, device, path, fn);
			//file_crc32(device,path,fn);
		}
	}
	if (checking == 1) {
		fileXioClose(fd);
		strcpy(full_path,device);
		strcat(full_path,path);
		strcat(full_path,fn);
		//scr_printf("DEBUG: %s %s %s %s\n", full_path, device, path, fn);
		fd = fileXioOpen(full_path, O_RDONLY);
		scr_printf("* Local File Opened... %d \n", fd);
		file_size = getFileSize(fd);
		scr_printf("* File Size... %d \n", file_size);
		if (file_size >= 1) {
			scr_printf("* %s Exists!\n", full_path);
		} else {
			scr_printf("* %s Does Not Exist!\n", full_path);
		}
		fileXioClose(fd);
		//scr_printf("CRC32: ");
		//scr_printf("DEBUG: %s %s %s %s\n", full_path, device, path, fn);
		file_crc32(device,path,fn);
		scr_printf(" \n");
		sleep(2);		
	}
	if (launching == 1) {
		// Display Path The ELF Is Being Loaded From
		scr_printf("* Launching Application from %s", arg0);
		sleep(2);
		/* Load the embedded wLaunchELF's loader.elf to its load address, by parsing its ELF header */
		eh = (elf_header_t *)&loader_elf;
		eph = (elf_pheader_t *)(&loader_elf + eh->phoff);

		for(i = 0; i < eh->phnum; i++) {
			dest = (u8*)(eph[i].vaddr);
			pdata = (u8*)(&loader_elf + eph[i].offset);
			for(j = 0; j < eph[i].filesz; j++) dest[j] = pdata[j];
			if(eph[i].memsz > eph[i].filesz) {
				dest = (u8 *)(eph[i].vaddr + eph[i].filesz);
				for(j = 0; j < eph[i].memsz - eph[i].filesz; j++) dest[j] = '\0';
			}
		}
	padPortClose(0, 0);
	padEnd();
	NetManDeinit();
	SifExitRpc();
	FlushCache(0);
	FlushCache(2);
	ExecPS2((void *)eh->entry, 0, argc, exec_args);
	}
	scr_printf("* Operations complete. Returning to Main Menu...\n");
	sleep(4);	
	menu_Text();
}


void initialize(void)
{

	int ret;
	SifInitRpc(0);
	scr_clear();
	// init debug screen
	init_scr();
	scr_clear();
	scr_printf("Loading... Please Wait. \n");
	// load all modules
	LoadModules();
	// init pad
	padInit(0);
	if ((ret = padPortOpen(0, 0, padBuf)) == 0)
	{
		#if defined DEBUG
			scr_printf("padOpenPort failed: %d\n", ret);
		#endif
		SleepThread();
	}
	if (!initializePad(0, 0))
	{
		#if defined DEBUG
			scr_printf("pad initalization failed!\n");
		#endif
		SleepThread();

	}
}

void readlines(){
   FILE *fp;
   char str[80];
   fp = fopen("VTSPS2-HBDL.TXT", "r");
   if(fp == NULL) {
      perror("Error opening file");
      return(-1);
   }
   while(1) {
   if( fgets (str, 80, fp)!=NULL ) {
      /* writing content to stdout */
      puts(str);
   } else {
   		break;
   }
   }
   fclose(fp);
}

int main(int argc, char *argv[])
{
	// Initialize
	SifInitRpc(0);
	ResetIOP();
	// initialize
	initialize();
	scr_clear();
	menu_header();
	//sleep(1);
	scr_printf("Modules Loaded. Obtaining an IP Address...\n");
	dhcpmain(); // Setup Network Config With DHCP <dhcpmain.c>
	//strcpy(url,"http://hbdl.vts-tech.org/");
	strcpy(action,actions[0]);
	strcpy(device,devices[0]);
	strcpy(path,paths[0]);	
	strcpy(fn,targets[0]);
	readlines();
	menu_Text();
		while (1)
	{
		//check to see if the pad is still connected
		checkPadConnected();
		//read pad 0
		buttonStatts(0, 0);
		
		if(new_pad & PAD_UP) {
			if (strcmp(device,"mc0:/") == 0) {
				strcpy(device,"mc1:/");
			} else {
				strcpy(device,"mc0:/");			
			}
		menu_Text();
		} else if(new_pad & PAD_DOWN)	{
			//scr_printf("D1BUG: %s %s %s %s\n", action, device, path, fn);
			if (strcmp(action,"CHECK") == 0) {
				strcpy(action,actions[1]);
			} else if (strcmp(action,"DOWNLOAD") == 0) {
				strcpy(action,actions[2]);
			} else if (strcmp(action,"LAUNCH") == 0) {
				strcpy(action,actions[0]);
			}
		//scr_printf("D2BUG: %s %s %s %s\n", action, device, path, fn);
		//sleep(2);
		menu_Text();
		} else if(new_pad & PAD_LEFT)	{
			if (strcmp(path,"APPS/") == 0) {
				substring(fn,ELF_NO_EXT,1,(strlen(fn)-4));
				sprintf(path,"APP_%s/",ELF_NO_EXT);
				strcpy(PATH_APP,path);
			} else if (strcmp(path,PATH_APP) == 0) {
				substring(fn,ELF_NO_EXT,1,(strlen(fn)-4));
				sprintf(path,"%s/",ELF_NO_EXT);
				strcpy(PATH_ELF,path);
			} else if (strcmp(path,PATH_ELF) == 0) {
				substring(fn,ELF_NO_EXT,1,(strlen(fn)-4));
				strcpy(path,"APPS/");
			}
		//scr_printf("DEBUG: %s %s %s %s\n", action, device, path, fn);
		//sleep(2);
		menu_Text();
		}	else if(new_pad & PAD_RIGHT) {
			//"ESDL.ELF", "ESR.ELF", "GSM.ELF", "HDL.ELF", "INFOGB.ELF", "OPL.ELF", "PS2SX.ELF"
			//"RA_2048.ELF", "RA_FCEU.ELF", "RA_MGBA.ELF", "RA_PICO.ELF", "RA_QNES.ELF", "SMS.ELF", "SNESSTN.ELF", "WLE.ELF"
			if (strcmp(fn,"ESDL.ELF") == 0) {
				strcpy(fn,targets[1]);
			}	else if (strcmp(fn,"ESR.ELF") == 0) {
				strcpy(fn,targets[2]);
			}	else if (strcmp(fn,"GSM.ELF") == 0) {
				strcpy(fn,targets[3]);
			}	else if (strcmp(fn,"HDL.ELF") == 0) {
				strcpy(fn,targets[4]);
			} else if (strcmp(fn,"INFOGB.ELF") == 0) {
				strcpy(fn,targets[5]);
			} else if (strcmp(fn,"OPL.ELF") == 0) {
				strcpy(fn,targets[6]);
			} else if (strcmp(fn,"PS2SX.ELF") == 0) {
				strcpy(fn,targets[7]);
			} else if (strcmp(fn,"RA_2048.ELF") == 0) {
				strcpy(fn,targets[8]);
			} else if (strcmp(fn,"RA_FCEU.ELF") == 0) {
				strcpy(fn,targets[9]);
			} else if (strcmp(fn,"RA_MGBA.ELF") == 0) {
				strcpy(fn,targets[10]);
			} else if (strcmp(fn,"RA_PICO.ELF") == 0) {
				strcpy(fn,targets[11]);
			} else if (strcmp(fn,"RA_QNES.ELF") == 0) {
				strcpy(fn,targets[12]);
			} else if (strcmp(fn,"SMS.ELF") == 0) {
				strcpy(fn,targets[13]);
			} else if (strcmp(fn,"SNESSTN.ELF") == 0) {
				strcpy(fn,targets[14]);				
			} else if (strcmp(fn,"WLE.ELF") == 0) {
				strcpy(fn,targets[0]);	
			}
		//scr_printf("DEBUG: %s %s %s %s\n", action, device, path, fn);
		//sleep(2);
		menu_Text();
		}	else if (new_pad & PAD_START)	{
		 	return 0;
		}	else if (new_pad & PAD_SELECT) {
				if (http_mirror == 0) {
					http_mirror = 1;
					//strcpy(url,mirror1);
					
				} else if (http_mirror == 1) {
				 	http_mirror = 0;
				 	//strcpy(url,mirror0);
				}		
			menu_Text();
		} else if ((new_pad & PAD_CROSS) || (new_pad & PAD_CIRCLE) || (new_pad & PAD_TRIANGLE) || (new_pad & PAD_SQUARE) || (new_pad & PAD_R1) || (new_pad & PAD_L1) || (new_pad & PAD_R2) || (new_pad & PAD_L2)) {
		 	if (strcmp(action,"CHECK") == 0) {
		 		DoTask(1);
		 		menu_Text();
		} else if (strcmp(action,"DOWNLOAD") == 0) {
				DoTask(2);
				menu_Text();
		} else if (strcmp(action,"LAUNCH") == 0) {
				DoTask(3);
				menu_Text();
		}		
	}
}
}
