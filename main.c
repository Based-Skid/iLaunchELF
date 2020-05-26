/*
                 VTSLaunchELF
	Based on iLaunchELF by krHACKen & Based_Skid

   ELF loading Portions of this Code are From loader.c
   https://github.com/AKuHAK/uLaunchELF/blob/master/loader/loader.c

 */
// All Includes and Declarations are in Main.h, additional C Files should include main.h
#include "main.h"
// App Strings
#include "strings.h"

extern void loader_elf; // wLaunchELF's loader.elf. Embed this sukka in your ELF.


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
	int size = fioLseek(fd, 0, SEEK_END);
	fioLseek(fd, 0, SEEK_SET);
	return size;
}

void menu_header(void)
{
	scr_printf(appName);
	scr_printf(appVer);
	scr_printf(appAuthor);
	//scr_printf(appNotice);
}

void menu_Text(void)
{
	scr_clear();
	menu_header();
	scr_printf(txtcrossBtn);
	scr_printf(txtsqrBtn);
	scr_printf(txtcirBtn);
	scr_printf(txttriBtn);
	scr_printf(txtR1Btn);
	scr_printf(txtL1Btn);
	scr_printf(txtselBtn);
	scr_printf(txtstrtBtn);
	scr_printf(txtR2Btn);
	scr_printf(txtL3Btn);
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
		printf("Failed to Load freesio2 sw module");
		gotoOSDSYS(1);
	}

	ret = SifExecModuleBuffer(&iomanX, size_iomanX, 0, NULL, NULL);
	if (ret < 0)
	{
		printf("Failed to Load iomanx sw module");
	}

	ret = SifExecModuleBuffer(&fileXio, size_fileXio, 0, NULL, NULL);
	if (ret < 0)
	{
		printf("Failed to Load freesio2 sw module");
	}


	ret = SifExecModuleBuffer(&freepad, size_freepad, 0, NULL, NULL);
	if (ret < 0)
	{
		printf("Failed to Load freepad sw module");
		gotoOSDSYS(3);
	}

	ret = SifExecModuleBuffer(&mcman, size_mcman, 0, NULL, NULL);
	if (ret < 0)
	{
		printf("Failed to Load mcman sw module");
		gotoOSDSYS(4);
	}

	ret = SifExecModuleBuffer(&mcserv, size_mcserv, 0, NULL, NULL);
	if (ret < 0)
	{
		printf("Failed to Load mcserv sw module");
		gotoOSDSYS(5);

	}

	ret = SifExecModuleBuffer(&ps2dev9, size_ps2dev9, 0, NULL, NULL);
	if (ret < 0)
	{
		printf(" Could not load ps2dev9.IRX! %d\n", ret);
		SleepThread();
	}


	ret = SifExecModuleBuffer(&netman, size_netman, 0, NULL, NULL);
	if (ret < 0)
	{
		printf(" Could not load netman.IRX! %d\n", ret);
		SleepThread();
	}

	ret = SifExecModuleBuffer(&smap, size_smap, 0, NULL, NULL);
	if (ret < 0)
	{
		printf(" Could not load smap.IRX! %d\n", ret);
		SleepThread();
	}

	ret = SifExecModuleBuffer(&ps2ipnm, size_ps2ipnm, 0, NULL, NULL);
	if (ret < 0)
	{
		printf(" Could not load ps2ip.IRX! %d\n", ret);
		SleepThread();
	}

	ret = SifExecModuleBuffer(&ps2ips, size_ps2ips, 0, NULL, NULL);
	if (ret < 0)
	{
		printf(" Could not load ps2ips.IRX! %d\n", ret);
		SleepThread();
	}
	ps2ip_init();

	ret = SifExecModuleBuffer(&ps2http, size_ps2http, 0, NULL, NULL);
	if (ret < 0)
	{
        	printf("	Could not load ps2http.IRX! %d\n", ret);
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

	fd = open(arg, O_RDONLY);

	if(fd >= 0) {
		size = lseek(fd, 0, SEEK_END);
		close(fd);
	} else return fd;

	return size;
}

int Download(char *url, char *full_path)
{
	int fd, size;
	FILE *target;
	fd = open(url, O_RDONLY);
	target = fopen(full_path,"w");
	if(fd >= 0) {
		size = lseek(fd, 0, SEEK_END);
		fwrite(lseek(fd, 0, SEEK_END),1,size,target);
		close(fd);
		sleep(2);
		fclose(target);
	} else return fd;
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
char file_crc32(char device[], char path[], char fn[])
{
  FILE *fp;
  size_t len;
  char tmp[32];
  char f_crc32[16];
  char full_path[128];
	strcpy(full_path,device);
	strcat(full_path,path);
	strcat(full_path,fn);  
  char buf[8000000], *file = full_path;
  if (NULL == (fp = fopen(file, "rb")))
  {
        printf("Error! Unable to open %s for reading\n", file);
        //return -1;
  }
  len = fread(buf, sizeof(char), sizeof(buf), fp);
  //scr_printf("%d bytes read\n", len);
  //scr_printf("The checksum of %s is:\n\n", file);
  fclose(fp);
  sleep(1);
  sprintf(tmp,"%lX",crc32(buf, len));
  substring(tmp,f_crc32,9,8);
  scr_printf("%s",f_crc32);
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
	char device[128], path[128], fn[128], full_path[256];
	sleep(1);
	
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
			//Check OPL
			strcpy(device,"mc0:/");
			strcpy(path,"APPS/");
			strcpy(fn,"OPL.ELF");			
		}
		else if (task == 2)
		{
			checking=1;
			//Check WLE
			strcpy(device,"mc0:/");
			strcpy(path,"APPS/");
			strcpy(fn,"WLE.ELF");
		}
		else if (task == 3)
		{
			//Download OPL
			downloading=1;
			strcpy(exec_args[0], "http://hbdl.vts-tech.org/OPL.ELF");
			strcpy(device,"mc0:/");
			strcpy(path,"APPS/");
			strcpy(fn,"OPL.ELF");			
			strcpy(full_path,device);
			strcat(full_path,path);
			strcat(full_path,fn);  				
			argc = 1;
			sleep(10);
		}
		else if (task == 4)
		{
			//Download WLE
			downloading=1;
			strcpy(exec_args[0], "http://hbdl.vts-tech.org/WLE.ELF");
			strcpy(device,"mc0:/");
			strcpy(path,"APPS/");
			strcpy(fn,"WLE.ELF");
			strcpy(full_path,device);
			strcat(full_path,path);
			strcat(full_path,fn);  				
			argc = 1;
			sleep(10);
		}
		else if (task == 5)
		{
			//strcpy(exec_args[0], "http://hbdl.vts-tech.org/1MB.test");
			//argc = 1;
			sleep(2);
		}
		else if (task == 6)
		{
			//strcpy(exec_args[0], "http://hbdl.vts-tech.org/1MB.test");
			//argc = 1;
			sleep(2);
		}
		else if (task == 7)
		{
			//strcpy(exec_args[0], "http://hbdl.vts-tech.org/1MB.test");
			//argc = 1;
			sleep(2);
		}
		else if (task == 8)
		{
			//strcpy(exec_args[0], "http://hbdl.vts-tech.org/1MB.test");
			//argc = 1;
			sleep(2);
		}
		else if (task == 9)
		{
			//strcpy(exec_args[0], "http://hbdl.vts-tech.org/1MB.test");
			//argc = 1;
			sleep(2);
		}
	} else asm volatile("break\n"); // OUT OF BOUNDS, UNDEFINED ITEM!
	//Clear Screen To Make This Look tidy!
	scr_clear();
	menu_header();
	if (downloading==1){
	  char buf[8000000], *file = full_path;
	  char *url;
	  strcpy(url,exec_args[0]);
		scr_printf("Downloading...\n");
		//Access Test (Make sure The Elf can Actually be Loaded)
		ret = Download(url,full_path);
		if(ret < 0) {
			scr_printf("Error! Could not open the file\n");
		} else {
			scr_printf("File Size: %d bytes\n", ret);
			fclose(fd);
			sleep(1);
			fd = fopen(full_path, "w");
			file_size = getFileSize(fd);
			if (file_size >= 1) {
				scr_printf("%s Exists!\n", full_path);
			} else {
				scr_printf("%s Does Not Exist!\n", full_path);
			}
		}
	}
	if (checking == 1) {
		fioClose(fd);
		strcpy(full_path,device);
		strcat(full_path,path);
		strcat(full_path,fn);
		fd = fioOpen(full_path, O_RDONLY);
		file_size = getFileSize(fd);
		if (file_size > 1) {
			scr_printf("%s Exists!\n", full_path);
		} else {
			scr_printf("%s Does Not Exist!\n", full_path);
		}
		fioClose(fd);
		scr_printf("CRC32: ");
		file_crc32(device,path,fn);
		scr_printf("\n");
		sleep(5);		
	}
	if (launching == 1) {
		// Display URL The ELF Is Being Loaded From
		scr_printf("Launching Application from \n %s", arg0);
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
	scr_printf("Operations complete. Returning to Main Menu...\n");
	sleep(12);
	menu_Text();
		while (1)
	{
		//check to see if the pad is still connected
		checkPadConnected();
		//read pad 0
		buttonStatts(0, 0);
		
		if (new_pad & PAD_START)
		{
		 gotoOSDSYS(0);
		}
		
		if (new_pad & PAD_SELECT)
		{
		 DoTask(6);
		}
		
		if (new_pad & PAD_L1)
		{
		 DoTask(7);
		}
		
		if (new_pad & PAD_R1)
		{
		 scr_printf("\n-WARNING:This Tool Can Cause Data Loss! Please Be CareFul!\n");
		 sleep(5);
		 DoTask(5);
		}
		
		if (new_pad & PAD_R2)
		{
		 DoTask(8);
		}
		
		if (new_pad & PAD_TRIANGLE)
		{
		 //Download WLE
		 DoTask(4);
		}

		if (new_pad & PAD_CIRCLE)
		{
		 //Download OPL
		 DoTask(3);
		}

		if (new_pad & PAD_SQUARE)
		{
		 //Check WLE
		 DoTask(2);
		}
		
		if(new_pad & PAD_CROSS)
		{
		//Check OPL
		DoTask(1);
		}
	}
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

int main(int argc, char *argv[])
{
	// Initialize
	SifInitRpc(0);
	ResetIOP();
	// initialize
	initialize();
	scr_clear();
	menu_header();
	sleep(1);
	scr_printf("Modules Loaded Up. Starting up DHCP \n");
	dhcpmain(); // Setup Network Config With DHCP <dhcpmain.c>
	menu_Text();
		while (1)
	{
		//check to see if the pad is still connected
		checkPadConnected();
		//read pad 0
		buttonStatts(0, 0);
		
		if (new_pad & PAD_START)
		{
		 gotoOSDSYS(0);
		}
		
		if (new_pad & PAD_SELECT)
		{
		 DoTask(6);
		}
		
		if (new_pad & PAD_L1)
		{
		 DoTask(7);
		}
		
		if (new_pad & PAD_R1)
		{
		 scr_printf("\n-WARNING:This Tool Can Cause Data Loss! Please Be CareFul!\n");
		 sleep(5);
		 DoTask(5);
		}
		
		if (new_pad & PAD_R2)
		{
		 DoTask(8);
		}
		
		if (new_pad & PAD_TRIANGLE)
		{
		 //Download WLE
		 DoTask(4);
		}

		if (new_pad & PAD_CIRCLE)
		{
		 //Download OPL
		 DoTask(3);
		}

		if (new_pad & PAD_SQUARE)
		{
		 //Check WLE
		 DoTask(2);
		}
		
		if(new_pad & PAD_CROSS)
		{
		//Check OPL
		DoTask(1);
		}
	}

}

