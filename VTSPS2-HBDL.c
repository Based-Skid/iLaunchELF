/*
                 VTSPS2-HBDL by VTSTech
	Based on iLaunchELF by krHACKen & Based_Skid

   ELF loading Portions of this Code are From Function LoadElf()
   from main.c MPLUS-LOADER3.ELF

 */
// All Includes and Declarations are in Main.h, additional C Files should include main.h

#include "VTSPS2-HBDL.h"
#include "strings.h"

int http_mirror = 0;
int dbsize = 56; //lines in VTSPS2-HBDL.TXT
char CRC32DB[56][128] = {""};
char remotecrc[9];
char localcrc[9];
	
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

void substring(char s[], char sub[], int p, int l) {
   int c = 0;
   
   while (c < l) {
      sub[c] = s[p+c-1];
      c++;
   }
   sub[c] = '\0';
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
	char remotefn[15];
	char hbsize[7];
	int x = (dbsize / 2); //starting line # of CRC's from VTSPS2-HBDL.TXT
	int z = 0;
	if (http_mirror == 0) {
		sprintf(mirror0,"http://hbdl.vts-tech.org/");
		scr_printf("IP Address: %s Mirror: %s\n",vtsip,mirror0);
	} else if (http_mirror == 1) {
		sprintf(mirror1,"http://www.hwc.nat.cu/ps2-vault/ps2hbdl/");
		scr_printf("IP Address: %s Mirror: %s\n",vtsip,mirror1);
	}
	scr_printf(" \n");
	if ((strcmp(action,"CHECK") != 0) && (strcmp(action,"DOWNLOAD") != 0) && (strcmp(action,"LAUNCH") != 0)) {
		strcpy(action,"CHECK");
	}
	if ((strcmp(device,"mc0:/") != 0) && (strcmp(device,"mc1:/") != 0) && (strcmp(device,"mass:/") != 0)) {
		strcpy(device,"mc0:/");
	}
	if (strlen(fn) >= 13) {
		strcpy(fn,"AURA.ELF");
		strcpy(path,"APPS/");
	}
	char spc_pad[] = "";
	int spc_cnt = 0;

	if (strlen(fn) > strlen(action)) {
		spc_cnt = (strlen(fn) - strlen(action));
	} else {
		spc_cnt = (strlen(action) - strlen(fn));	
	}
	
	for (spc_cnt = spc_cnt;spc_cnt!=0;spc_cnt=spc_cnt-1 ) {
		strcat(spc_pad," ");
	}
	
	if (strlen(fn) > strlen(action)) {
		scr_printf("[M]: %s%s [D]: %s [P]: %s \n[T]: %s ",action,spc_pad,device,path,fn);
	} else {	
		scr_printf("[M]: %s [D]: %s [P]: %s \n[T]: %s%s ",action,device,path,fn,spc_pad);
	}
	
	while(z<=x) {
		int fnsize = (strlen(fn));
		char hbfn[] = "";
		char hbver[] = "";
		substring(CRC32DB[z],hbfn,14,fnsize);
		sprintf(hbsize,"");
		//scr_printf("DEBUG: %s %d \n",hbfn, fnsize);
		if (strstr(hbfn,fn)) {
			substring(CRC32DB[z],hbsize,(14+fnsize),6);
			substring(CRC32DB[z],hbver,(21+fnsize),strlen(CRC32DB[z]));
			scr_printf("[S]:%s [V]:%s \n", hbsize, hbver);
			//scr_printf("DEBUG: %d %d",strlen(hbsize),hbsize);
		}
		//scr_printf("D2: %s",CRC32DB[x][-8]);
		z++;
	}
	while(x<=dbsize) {//total lines in VTSPS2-HBDL.TXT
		int fnsize = (strlen(CRC32DB[x]) - 9);
		sprintf(remotefn,"");
		substring(CRC32DB[x],remotefn,1,fnsize);
		//scr_printf("DEBUG: %d %d",strlen(fn),strlen(remotefn));
		if (strstr(remotefn,fn) && strlen(remotefn) == (strlen(fn)+2)) {
			//strcpy(remotefn,fn);
			substring(CRC32DB[x],remotecrc,(strlen(fn)+2),9);
			scr_printf("Local CRC32: ");
			if (strcmp(localcrc,"00000001") == 0) {
				scr_printf("unchecked");
			} else if (strcmp(localcrc,"00000000") == 0) {
				scr_printf("00000000");
			} else if (strlen(localcrc) == 7) {
				scr_printf("0%s",localcrc);
			} else if (strlen(localcrc) == 6) {
				scr_printf("00%s",localcrc);
			} else if (strlen(localcrc) == 5) {
				scr_printf("000%s",localcrc);
			} else {
			//sprintf(localcrc,file_crc32(device,path,fn));
			scr_printf("%s",localcrc);
			}
			scr_printf(" Remote CRC32:%s\n", remotecrc);
		}
		//scr_printf("D2: %s",CRC32DB[x][-8]);
		x=x+1;
	}	
	//scr_printf("Test CRC: %s\n",CRC32DB[18]);
	//sleep(10);
	scr_printf(" \n");
	scr_printf("-Press UP to Set [D]evice.\n");
	scr_printf("-Press DOWN to Set [M]ode.\n");
	scr_printf("-Press LEFT to Set [P]ath.\n");
	scr_printf("-Press RIGHT to Set [T]arget.\n");
	scr_printf("-Press SELECT to Set Mirror.\n");
	scr_printf("-Press START to Exit.\n");
	scr_printf("-Press any other key to perform selected action\n");
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
    scr_printf(" Could not load ps2http.IRX! %d\n", ret);
		gotoOSDSYS(5);
	}

	ret = SifExecModuleBuffer(&usbd, size_usbd, 0, NULL, NULL);
	if (ret < 0)
	{
		scr_printf(" Could not load usbd.irx! %d\n", ret);
		SleepThread();
	}

	ret = SifExecModuleBuffer(&usbhdfsd, size_usbhdfsd, 0, NULL, NULL);
	if (ret < 0)
	{
		scr_printf(" Could not load usbhdfsd.irx! %d\n", ret);
		SleepThread();
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

char* file_crc32(char device[], char path[], char fn[])
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
  char buf[5600000], *file = full_path;
  //Close the file
  //fclose(fp);
  //scr_printf("File Closed: %d\n", fp);
  if (NULL == (fp = fopen(file, "rb")))
  {
        printf("Error! Unable to open %s for reading\n", file);
        sprintf(localcrc,"00000000");
        return NULL;
  }
  //read file, store length in len, file contents in buf
  len = fread(buf, sizeof(char), sizeof(buf), fp);
  //scr_printf("%d bytes read\n", len);
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
  	scr_printf("%s",f_crc32);
  	return f_crc32;
  } else {
  	scr_printf("%s",tmp);
  	return tmp;
  }
}

int Download(char *urll, char *full_path)
{
	int size, urld, target, ret = 0;
	char buf[5600000];
	//FILE *urld;
	//FILE *target;
	fioClose(urld);
	fileXioClose(target);
	urld = fioOpen(urll, O_RDONLY);
	scr_printf("* URL Opened... %d\n", urld);
	target = fileXioOpen(full_path, O_RDWR | O_CREAT);
	//scr_printf("* Local File Opened... %d\n", target);
	//fioClose(target);
	if(urld >= 0) {
		size = fioLseek(urld, 0, SEEK_END);
		//fileXioLseek(urll, 0, SEEK_SET);
		fioRead(urld, buf, size);
		sleep(2);
		scr_printf("* Downloaded Size... %d\n", size);
		ret = fileXioWrite(target,buf,size);
		scr_printf("* Local File Written! %d\n", ret);
		sleep(2);
		fioClose(urld);
		//scr_printf("* URL Closed... %d\n", urld);
		fileXioClose(target);
		//scr_printf("* Local File Closed... %d\n", target);
		//sprintf(localcrc,file_crc32(device,path,fn));
		//if (strcmp(localcrc,remotecrc) != 0) {
			//Warns even when they do match, need to try another way.
			//scr_printf("\nWarning Local and Remote CRC32 do not match!\n");
			//sleep(4);
		//}
	} else {
		scr_printf("Download Error! Debug: %d %d %d", urld, target, size);
	}
	return size;
}

void DownloadList(char device[], char path[], char fn[]){
	char arg0[256], arg1[256], arg2[256], arg3[256], arg4[256], arg5[256], arg6[256], arg7[256], arg8[256];
	char *exec_args[9] = { arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8 };
	int argc = 0;
	int y = 0;
	int z = 0;
	int ret = 0;
	int fd,file_size;
	char full_path[256];
	//patches.ppi
	
	if (http_mirror == 0) {
		sprintf(mirror0,"http://hbdl.vts-tech.org/");
		scr_printf("* Mirror 1 Selected... \n");
	} else if (http_mirror == 1) {
		sprintf(mirror1,"http://www.hwc.nat.cu/ps2-vault/ps2hbdl/");
		scr_printf("* Mirror 2 Selected... \n");
	}
	scr_printf("* Building Download List... \n");
	if (strstr("DOSBOX.ELF",fn)) {
		argc = 4;
		scr_printf("* DOSBox ... \n");
		if (http_mirror == 0) {
			for (y=0;y<=argc;y=y+1) {
				strcpy(exec_args[y],mirror0);
			}
		} else if (http_mirror == 1) {
			for (y=0;y<=argc;y=y+1) {
				strcpy(exec_args[y],mirror1);
			}
		}
		strcat(exec_args[0],fn);
		strcpy(exec_args[1],fn);
		strcat(exec_args[2],"dosbox.bin");
		strcpy(exec_args[3],"dosbox.conf");
	} else if (strstr("PS2DOOM.ELF",fn)) {
		argc = 6;
		scr_printf("* PS2Doom ... \n");
		if (http_mirror == 0) {
			for (y=0;y<=argc;y=y+1) {
				strcpy(exec_args[y],mirror0);
			}
		} else if (http_mirror == 1) {
			for (y=0;y<=argc;y=y+1) {
				strcpy(exec_args[y],mirror1);
			}
		}
		strcat(exec_args[0],fn);
		strcpy(exec_args[1],fn);
		strcat(exec_args[2],"ps2doom.bin");
		strcpy(exec_args[3],"ps2doom.config");
		strcat(exec_args[4],"doom1.wad");
		strcpy(exec_args[5],"doom1.wad");
	} else if (strstr("PS2ESDL.ELF",fn)) {
		argc = 4;
		scr_printf("* PS2ESDL ... \n");
		if (http_mirror == 0) {
			for (y=0;y<=argc;y=y+1) {
				strcpy(exec_args[y],mirror0);
			}
		} else if (http_mirror == 1) {
			for (y=0;y<=argc;y=y+1) {
				strcpy(exec_args[y],mirror1);
			}
		}
		strcat(exec_args[0],fn);
		strcpy(exec_args[1],fn);
		strcat(exec_args[2],"patches.ppi");
		strcpy(exec_args[3],"patches.ppi");
	}
	
	for (z=0;z<argc;z=z+2) {
		fileXioClose(fd);
		sprintf(full_path,"");
		strcpy(full_path,device);
		strcat(full_path,path);
		strcat(full_path,exec_args[z+1]);
		//strcpy(url,exec_args[0]);
		scr_printf("* Downloading...\n");
		scr_printf("* URL: %s\n", exec_args[z]);
		scr_printf("* Path: %s\n", full_path);
		ret = Download(exec_args[z],full_path);
		sleep(4);
		if(ret <= 0) {
			scr_printf("* Error! Could not open the file\n");
		} else {
			//scr_printf("* File Size: %d bytes\n", ret);
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
}

//** Function LoadElf() from main.c MPLUS-LOADER3.ELF
//** http://lukasz.dk/2008/04/22/datel-memory-plus-64-mb/
//slightly modified
void LoadElf(const char *elf, char* path)
{
	char* args[1];
	t_ExecData exec;	

	SifLoadElf(elf, &exec);

	#ifdef DEBUG
		//scr_printf("Trying to load ELF: %s\n", elf);
	#endif	

	if(exec.epc > 0)
	{	
		FlushCache(0);
		FlushCache(2);

		// Reset IOP, since not all ELF's do it and we've loaded additional IOP
		// modules which need to be unloaded
		ResetIOP();

		if(path != 0)
		{
			args[0] = path;
			ExecPS2((void*)exec.epc, (void*)exec.gp, 1, args);
		}
		else
		{
			ExecPS2((void*)exec.epc, (void*)exec.gp, 0, NULL);
		}
	}
}
void DoTask(int task)
{
	int ret,launching,downloading,checking;
	char arg0[256], arg1[256], arg2[256], arg3[256], arg4[256], arg5[256], arg6[256], arg7[256], arg8[256];
	char *exec_args[9] = { arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8 };
	int argc = 0;
	int fd,file_size;
	//extern char device[128], path[128], fn[128];
	char full_path[256];
	char half_path[256];
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
			sleep(1);
			strcat(full_path,path);
			//remove trailing / for MkDir
			sleep(1);
			substring(full_path,make_path,1,strlen(full_path)-1);
			fileXioMkdir(make_path,0777);
			sleep(1);
			strcat(full_path,fn);  				
			argc = 1;
			sleep(2);							
		}
		if (task == 3)
		{
			launching=1;
			strcpy(full_path,device);
			strcat(full_path,path);
			strcpy(half_path,full_path);
			strcat(full_path,fn);
			strcpy(exec_args[0],full_path);
			argc = 1;
		}		
	} else asm volatile("break\n"); // OUT OF BOUNDS, UNDEFINED ITEM!
	//Clear Screen To Make This Look tidy!
	scr_clear();
	menu_header();
	if (downloading==1){
	  if (strstr("PS2ESDL.ELF",fn)) {
	  	DownloadList(device,path,"PS2ESDL.ELF");
	  } else if (strstr("PS2DOOM.ELF",fn)) {
	  	DownloadList(device,path,"PS2DOOM.ELF");
	  } else if (strstr("DOSBOX.ELF",fn)) {
	  	DownloadList(device,path,"DOSBOX.ELF");
	  } else {
			  fileXioClose(fd);
			  //char buf[4000000], *file = full_path;
			  //strcpy(url,exec_args[0]);
				scr_printf("* Downloading...\n");
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
		}
	if (checking == 1) {
		fileXioClose(fd);
		strcpy(full_path,device);
		strcat(full_path,path);
		strcat(full_path,fn);
		//scr_printf("DEBUG: %s %s %s %s\n", full_path, device, path, fn);
		fd = fileXioOpen(full_path, O_RDONLY);
		//scr_printf("* Local File Opened... %d \n", fd);
		file_size = getFileSize(fd);
		//scr_printf("* File Size... %d \n", file_size);
		if (file_size >= 1) {
			//scr_printf("* %s Exists!\n", full_path);
		} else {
			//scr_printf("! %s Does Not Exist!\n", full_path);
			sprintf(localcrc,"00000000");
			return;
		}
		fileXioClose(fd);
		//scr_printf("CRC32: ");
		//scr_printf("DEBUG: %s %s %s %s\n", full_path, device, path, fn);
		sprintf(localcrc,file_crc32(device,path,fn));
		//scr_printf("CRC32:%s\n",localcrc);
		sleep(2);		
	}
	if (launching == 1) {
		// Display Path The ELF Is Being Loaded From
		scr_printf("* Launching Application from %s", arg0);
		sleep(2);
	//padPortClose(0, 0);
	//padEnd();
	//NetManDeinit();
	//SifExitRpc();
	//FlushCache(0);
	//FlushCache(2);
	LoadElf(full_path, half_path);
	}
	scr_printf("\n* Operations complete. Returning to Main Menu...\n");
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

void readcrc() {
  char line[dbsize][128]; //maxlines
	//uncomment for release builds
	char fname[15] = "VTSPS2-HBDL.TXT";
	//hardcoded path during dev. cwd is 'host' in PCSX2
	//char fname[25] = "mc0:/APPS/VTSPS2-HBDL.TXT";
  FILE *fptr = NULL;
  char hbdl_path[256];
	getcwd(hbdl_path,256);
	strcat(hbdl_path,fname);
  int i = 0;
  int tot = 0;
	scanf("%s",fname);	
    fptr = fopen(hbdl_path, "r");
    while(fgets(line[i], 128, fptr)) 
	{
        line[i][strlen(line[i]) - 1] = '\0';
        i++;
    }
    tot = i;
    for(i = 0; i < tot; ++i)
    {
        //printf(" %s\n", line[i]);
        sprintf(CRC32DB[i]," %s\n", line[i]);
    }
    //printf("\n\nDebug: %s %s %s\n", CRC32DB[1], CRC32DB[2], CRC32DB[20]);
    fclose(fptr);
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
	scr_printf("Modules Loaded. Obtaining an IP Address ...\n");
	dhcpmain(); // Setup Network Config With DHCP <dhcpmain.c>
	menu_header();
	strcpy(url,"http://hbdl.vts-tech.org/");
	scr_printf("IP Address obtained. Downloading homebrew list from hbdl.vts-tech.org ...\n");
	char hbdl_path[] = "";
	//uncomment below for release
	getcwd(hbdl_path,256);
	//sprintf(hbdl_path,"mc0:/APPS/");//hardcoded.
	strcat(hbdl_path,"VTSPS2-HBDL.TXT");
	//scr_printf("Debug: %s",hbdl_path);
	Download("http://hbdl.vts-tech.org/VTSPS2-HBDL.BIN",hbdl_path);
	//file_crc32("mc0:/","APPS/","VTSPS2-HBDL.TXT");
	strcpy(action,actions[0]);
	strcpy(device,devices[0]);
	strcpy(path,paths[0]);	
	strcpy(fn,targets[0]);
	sprintf(localcrc,"00000001");
	sleep(2);
	readcrc(); //populates CRC32DB[]
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
			} else if (strcmp(device,"mc1:/") == 0) {
				strcpy(device,"mass:/");
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
			//scr_printf("DEBUG: %s %s %s %s\n", action, device, path, fn);
			sleep(2);
			if (strcmp(path,"APPS/") == 0) {
				substring(fn,ELF_NO_EXT,1,(strlen(fn)-4));
				sleep(1);
				sprintf(path,"APP_%s/",ELF_NO_EXT);
				strcpy(PATH_APP,path);
			} else if (strcmp(path,PATH_APP) == 0) {
				substring(fn,ELF_NO_EXT,1,(strlen(fn)-4));
				sleep(1);
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
			if (strcmp(fn,"AURA.ELF") == 0) {
				strcpy(fn,targets[1]);
			} else if (strcmp(fn,"DOSBOX.ELF") == 0) {
				strcpy(fn,targets[2]);
			} else if (strcmp(fn,"EJECT.ELF") == 0) {
				strcpy(fn,targets[3]);
			}	else if (strcmp(fn,"ESR.ELF") == 0) {
				strcpy(fn,targets[4]);
			}	else if (strcmp(fn,"GSM.ELF") == 0) {
				strcpy(fn,targets[5]);
			}	else if (strcmp(fn,"HDL.ELF") == 0) {
				strcpy(fn,targets[6]);
			} else if (strcmp(fn,"INFOGB.ELF") == 0) {
				strcpy(fn,targets[7]);
			} else if (strcmp(fn,"LBFN.ELF") == 0) {
				strcpy(fn,targets[8]);
			} else if (strcmp(fn,"NEOCD.ELF") == 0) {
				strcpy(fn,targets[9]);
			} else if (strcmp(fn,"OPL.ELF") == 0) {
				strcpy(fn,targets[10]);
			} else if (strcmp(fn,"PGEN.ELF") == 0) {
				strcpy(fn,targets[11]);
			} else if (strcmp(fn,"PS2DOOM.ELF") == 0) {
				strcpy(fn,targets[12]);
			} else if (strcmp(fn,"PS2ESDL.ELF") == 0) {
				strcpy(fn,targets[13]);
			} else if (strcmp(fn,"PS2SX.ELF") == 0) {
				strcpy(fn,targets[14]);
			} else if (strcmp(fn,"PSMS.ELF") == 0) {
				strcpy(fn,targets[15]);
			} else if (strcmp(fn,"PVCS.ELF") == 0) {
				strcpy(fn,targets[16]);
			} else if (strcmp(fn,"RA_2048.ELF") == 0) {
				strcpy(fn,targets[17]);
			} else if (strcmp(fn,"RA_FCEU.ELF") == 0) {
				strcpy(fn,targets[18]);
			} else if (strcmp(fn,"RA_MGBA.ELF") == 0) {
				strcpy(fn,targets[19]);
			} else if (strcmp(fn,"RA_PICO.ELF") == 0) {
				strcpy(fn,targets[20]);
			} else if (strcmp(fn,"RA_QNES.ELF") == 0) {
				strcpy(fn,targets[21]);
			} else if (strcmp(fn,"SMS.ELF") == 0) {
				strcpy(fn,targets[22]);
			} else if (strcmp(fn,"SNES9X.ELF") == 0) {
				strcpy(fn,targets[23]);				
			} else if (strcmp(fn,"SNESSTN.ELF") == 0) {
				strcpy(fn,targets[24]);				
			} else if (strcmp(fn,"TESTMODE.ELF") == 0) {
				strcpy(fn,targets[25]);				
			} else if (strcmp(fn,"WLE.ELF") == 0) {
				strcpy(fn,targets[26]);	
			} else if (strcmp(fn,"ZONELDR.ELF") == 0) {
				strcpy(fn,targets[0]);	
			}
		//scr_printf("DEBUG: %s %s %s %s\n", action, device, path, fn);
		//sleep(2);
		sprintf(localcrc,"00000001");
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
