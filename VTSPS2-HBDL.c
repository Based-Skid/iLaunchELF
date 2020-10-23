/*
                 VTSPS2-HBDL by VTSTech
	Based on iLaunchELF by krHACKen & Based_Skid

   ELF loading Portions of this Code are From Function LoadElf()
   from main.c MPLUS-LOADER3.ELF

 */
// All Includes and Declarations are in Main.h, additional C Files should include main.h

#include "VTSPS2-HBDL.h"
#include "strings.h"

//0.32
int http_mirror = 0;
int dbsize = 56; //lines in VTSPS2-HBDL.TXT
char CRC32DB[56][128] = {""};
char remotecrc[9];
char localcrc[9];
char font_path[256];

GSGLOBAL *gsGlobal;
//GSFONT *gsFont;
GSFONTM *gsFontM;

u64 White, Black, WhiteFont, RedFont, GreenFont, TealFont, YellowFont;

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

void drawScreen(void){
	gsKit_set_finish(gsGlobal);
	gsKit_queue_exec(gsGlobal);
	gsKit_finish();
	gsKit_sync_flip(gsGlobal);
	gsKit_TexManager_nextFrame(gsGlobal);
	//gsKit_setactive(gsGlobal);
}

void menu_header(void)
{
	gsKit_clear(gsGlobal, Black);
	gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 10, 1, 0.32f, YellowFont, appName);
	gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 25, 1, 0.32f, YellowFont, appAuthor);
}

void menu_Text(void)
{
	//scr_clear();
	menu_header();
	extern char vtsip[15];
	extern char mirror0[];
	extern char mirror1[];
	char remotefn[15];
	char hbsize[7];
	char str[256] = "";
	int x = (dbsize / 2); //starting line # of CRC's from VTSPS2-HBDL.TXT
	int z = 0;
	//sleep(4);
	strcpy(str,"");
	if (http_mirror == 0) {
		sprintf(mirror0,"http://hbdl.vts-tech.org/");
		//scr_printf("IP Address: %s Mirror: %s\n",vtsip,mirror0);
		sprintf(str,"IP Address: %s Mirror: %s\n",vtsip,mirror0);
		gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 61, 1, 0.32f, TealFont, str);
	} else if (http_mirror == 1) {
		sprintf(mirror1,"http://www.hwc.nat.cu/ps2-vault/ps2hbdl/");
		//scr_printf("IP Address: %s Mirror: %s\n",vtsip,mirror1);
		sprintf(str,"IP Address: %s Mirror: %s\n",vtsip,mirror1);
		gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 61, 1, 0.32f, TealFont, str);
	}
	//scr_printf(" \n");
	if ((strcmp(action,"CHECK") != 0) && (strcmp(action,"DOWNLOAD") != 0) && (strcmp(action,"LAUNCH") != 0)) {
		strcpy(action,"CHECK");
	}
	if ((strcmp(device,"mc0:/") != 0) && (strcmp(device,"mc1:/") != 0) && (strcmp(device,"mass:/") != 0)) {
		strcpy(device,"mc0:/");
	}
	if (strlen(fn) >= 13) {
		//strncpy(fn,"",1);
		strcpy(fn,"AURA.ELF");
		strcpy(ELF_NO_EXT,"AURA");
		strcpy(path,"APPS/");
	}
	char spc_pad[8] = "";
	char spc_pad2[8] = "";
	int spc_cnt = 0;
	int spc_gs = 0;
	if (strlen(fn) > strlen(action)) {
		spc_cnt = (strlen(fn) - strlen(action));
	} else {
		spc_cnt = (strlen(action) - strlen(fn));
	}
	spc_gs = spc_cnt;
	strncpy(spc_pad,"",1);
	strncpy(spc_pad2,"",1);
	for (spc_cnt = spc_cnt;spc_cnt!=0;spc_cnt=spc_cnt-1 ) {
		strcat(spc_pad," ");
		if (spc_cnt <= (spc_gs/2)-1){
			strcat(spc_pad2," ");
		}
	}
	//if (spc_pad2 == ""){
	//	strcpy(spc_pad2, "  ");
	//} else if (spc_pad2 == " "){
	//	strcpy(spc_pad2,"");
	//}
	
	if (strlen(spc_pad) == 8){
		strncpy(spc_pad," ",2);
	}
	strncpy(str,"",2);
	if (strlen(fn) > strlen(action)) {
		//scr_printf("[M]: %s%s [D]: %s [P]: %s \n[T]: %s ",action,spc_pad,device,path,fn);
		sprintf(str,"[M]: %s%s[D]: %s [P]: %s ",action,spc_pad,device,path);
	} else {
		//scr_printf("[M]: %s [D]: %s [P]: %s \n[T]: %s%s ",action,device,path,fn,spc_pad);
		sprintf(str,"[M]: %s [D]: %s [P]: %s ",action,device,path);
	}
	gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 95, 1, 0.32f, TealFont, str);
	strncpy(str,"",1);
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
			//scr_printf("[S]:%s [V]:%s \n", hbsize, hbver);
			if (spc_gs>=1){
				sprintf(str,"[T]: %s%s[S]:%s [V]:%s",fn, spc_pad2, hbsize, hbver);
			} else {
				sprintf(str,"[T]: %s [S]:%s [V]:%s",fn, hbsize, hbver);
			}
			gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 107, 1, 0.32f, TealFont, str);
			//scr_printf("DEBUG: %d %d",strlen(hbsize),hbsize);
		}
		//scr_printf("D2: %s",CRC32DB[x][-8]);
		z++;
	}
	strcpy(str,"");
	while(x<=dbsize) { //total lines in VTSPS2-HBDL.TXT
		int fnsize = (strlen(CRC32DB[x]) - 12);
		sprintf(remotefn,"");
		substring(CRC32DB[x],remotefn,1,fnsize);
		if (strstr(remotefn,fn) && strlen(remotefn) == (strlen(fn)+1)) {
			//strcpy(remotefn,fn);
			substring(CRC32DB[x],remotecrc,(strlen(fn)+3),9);
			//scr_printf("Local CRC32: ");
			sprintf(str,"Local CRC32: ");		
			if (strcmp(localcrc,"00000001") == 0) {
				//scr_printf("unchecked");
				strcat(str,"unchecked");
			} else if (strcmp(localcrc,"00000000") == 0) {
				//scr_printf("00000000");
				strcat(str,"00000000");
			} else if (strlen(localcrc) == 7) {
				//scr_printf("0%s",localcrc);
				sprintf(localcrc,"0%s",localcrc);
				strcat(str,localcrc);
			} else if (strlen(localcrc) == 6) {
				//scr_printf("00%s",localcrc);
				sprintf(localcrc,"00%s",localcrc);
				strcat(str,localcrc);
			} else if (strlen(localcrc) == 5) {
				//scr_printf("000%s",localcrc);
				sprintf(localcrc,"000%s",localcrc);
				strcat(str,localcrc);
			} else {
			//scr_printf("%s",localcrc);
			strcat(str,localcrc);
			}
			//scr_printf(" Remote CRC32:%s \n", remotecrc);
			strcat(str," Remote CRC32:");
			strcat(str,remotecrc);
		}
		x=x+1;
	}
	gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 146, 1, 0.32f, GreenFont, str);	
	strcpy(str,"-Press UP to Set [D]evice. \n");
	strcat(str,"-Press DOWN to Set [M]ode. \n");
	strcat(str,"-Press LEFT to Set [P]ath. \n");
	strcat(str,"-Press RIGHT to Set [T]arget. \n");
	strcat(str,"-Press SELECT to Set Mirror. \n");
	strcat(str,"-Press START to Exit. \n");
	strcat(str,"-Press any other key to perform selected action \n");
	gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 180, 1, 0.32f, WhiteFont, str);
	drawScreen();
}

int Download(char *urll, char *full_path)
{
	int size = 0;
	int urld = 0;
	int target = 0;
	//int ret = 0;
	char buf[5600000];
	char str[256] = "";
	//FILE *urld;
	//FILE *target;
	close(urld);
	close(target);
	if ((urld = open(urll,O_RDONLY)) !=-1) {
		//scr_printf("* URL Opened... %d\n", urld);
	} else {
		sprintf(str,"! URL Open Failed... %d\n",urld);
		
	}
	//scr_printf("DEBUG: %s\n", full_path);
	target = open(full_path, O_RDWR | O_CREAT);
	//scr_printf("* Local File Opened... %d\n", target);
	//fclose(target);
	if(urld != -1) {
		size = getFileSize(urld);
		read(urld, buf, size);
		sleep(1);
		//scr_printf("* Downloaded Size... %d\n", size);
		write(target,buf,size);
		//scr_printf("* Local File Written... (%d bytes)\n", size);
		sleep(1);
		close(urld);
		//scr_printf("* URL Closed... %d\n", urld);
		close(target);
		//scr_printf("* Local File Closed... %d\n", target);
		//sprintf(localcrc,file_crc32(device,path,fn));
		//if (strcmp(localcrc,remotecrc) != 0) {
			//Warns even when they do match, need to try another way.
			//scr_printf("\nWarning Local and Remote CRC32 do not match!\n");
			//sleep(4);
		//}
	} else {
		printf("Download Error! Debug: %d %d %d", urld, target, size);
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
	int fd = 0;
	int file_size= 0;
	char full_path[256];
	char str[256] = "";
	//patches.ppi

	if (http_mirror == 0) {
		strcpy(mirror0,"http://hbdl.vts-tech.org/");
		strcpy(str,"* Mirror 1 Selected... \n");
	} else if (http_mirror == 1) {
		strcpy(mirror1,"http://www.hwc.nat.cu/ps2-vault/ps2hbdl/");
		strcpy(str,"* Mirror 2 Selected... \n");
	}
	gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 146, 1, 0.32f, WhiteFont, str);
	strcpy(str,"* Building Download List... \n");
	gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 163, 1, 0.32f, WhiteFont, str);
	drawScreen();
	if (strstr("DOSBOX.ELF",fn)) {
		argc = 4;
		strcpy(str,"* DOSBox ... \n");
		gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 180, 1, 0.32f, WhiteFont, str);
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
		strcpy(str,"* PS2Doom ... \n");
		gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 180, 1, 0.32f, WhiteFont, str);
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
		strcpy(str,"* PS2ESDL ... \n");
		gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 180, 1, 0.32f, WhiteFont, str);
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
	drawScreen();
	for (z=0;z<argc;z=z+2) {
		close(fd);
		sprintf(full_path,"");
		strcpy(full_path,device);
		strcat(full_path,path);
		strcat(full_path,exec_args[z+1]);
		//strcpy(url,exec_args[0]);
		strcpy(str,"* Downloading...\n");
		gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 197, 1, 0.32f, WhiteFont, str);
		sprintf(str,"* URL: %s\n", exec_args[z]);
		gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 214, 1, 0.32f, WhiteFont, str);
		sprintf(str,"* Path: %s\n", full_path);
		gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 231, 1, 0.32f, WhiteFont, str);
		drawScreen();
		ret = Download(exec_args[z],full_path);
		sleep(4);
		if(ret <= 0) {
			printf("* Error! Could not open the file\n");
		} else {
			//scr_printf("* File Size: %d bytes\n", ret);
			fd = open(full_path, O_RDONLY);
			file_size = getFileSize(fd);
			if (file_size >= 1) {
				sprintf(str,"* %s Exists!\n", full_path);
				gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 248, 1, 0.32f, WhiteFont, str);
			} else {
				scr_printf("* %s Does Not Exist!\n", full_path);
			}
			close(fd);
			//scr_printf("DEBUG: %s %s %s %s\n", action, device, path, fn);
			//file_crc32(device,path,fn);
		}
		drawScreen();
	}
}


void DoTask(int task)
{
	int ret,launching,downloading,checking;
	char arg0[256], arg1[256], arg2[256], arg3[256], arg4[256], arg5[256], arg6[256], arg7[256], arg8[256];
	char *exec_args[9] = { arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8 };
	int argc = 0;
	int fd = 0;
	int file_size = 0;
	//extern char device[128], path[128], fn[128];
	char full_path[256];
	char half_path[256];
	char str[256] = "";
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
			mkdir(make_path,0777);
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
	//scr_clear();
	menu_header();
	if (downloading==1){
	  if (strstr("PS2ESDL.ELF",fn)) {
	  	DownloadList(device,path,"PS2ESDL.ELF");
	  } else if (strstr("PS2DOOM.ELF",fn)) {
	  	DownloadList(device,path,"PS2DOOM.ELF");
	  } else if (strstr("DOSBOX.ELF",fn)) {
	  	DownloadList(device,path,"DOSBOX.ELF");
	  } else {
			  close(fd);
			  //char buf[4000000], *file = full_path;
			  //strcpy(url,exec_args[0]);
				//scr_printf("* Downloading... \n");
				gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 146, 1, 0.32f, WhiteFont, "* Downloading ...");
				drawScreen();
				sprintf(str,"* URL: %s \n", exec_args[0]);
				gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 163, 1, 0.32f, WhiteFont, str);
				drawScreen();
				sprintf(str,"* Path: %s \n", full_path);
				gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 180, 1, 0.32f, WhiteFont, str);
				drawScreen();
				ret = Download(exec_args[0],full_path);
				sleep(2);
				if(ret <= 0) {
					printf("* Error! Could not open the file \n");
				} else {
					sprintf(str,"* File Size: %d bytes \n", ret);
					gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 197, 1, 0.32f, WhiteFont, str);
					drawScreen();
					sleep(2);
					fd = open(full_path, O_RDONLY);
					file_size = getFileSize(fd);
					if (file_size >= 1) {
						sprintf(str,"* %s Exists! \n", full_path);
						gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 214, 1, 0.32f, WhiteFont, str);
					} else {
						sprintf(str,"* %s Does Not Exist! \n", full_path);
						gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 214, 1, 0.32f, RedFont, str);
					}
					close(fd);
					drawScreen();
					//scr_printf("DEBUG: %s %s %s %s\n", action, device, path, fn);
					//file_crc32(device,path,fn);
				}
			}
		}
	if (checking == 1) {
		close(fd);
		strcpy(full_path,device);
		strcat(full_path,path);
		strcat(full_path,fn);
		//scr_printf("DEBUG: %s %s %s %s\n", full_path, device, path, fn);
		fd = open(full_path, O_RDONLY);
		sprintf(str,"* Local File Opened... %d \n", fd);
		gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 146, 1, 0.32f, GreenFont, str);
		file_size = getFileSize(fd);
		sprintf(str,"* File Size... %d \n", file_size);
		gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 163, 1, 0.32f, GreenFont, str);
		if (file_size >= 1) {
			sprintf(str,"* Calculating CRC32 %s \n", full_path);
			gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 180, 1, 0.32f, GreenFont, str);
		} else {
			sprintf(str,"! %s Does Not Exist!\n", full_path);
			gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 180, 1, 0.32f, RedFont, str);
			sprintf(localcrc,"00000000");
			return;
		}
		close(fd);
		drawScreen();
		sleep(2);
		//scr_printf("CRC32: ");
		//scr_printf("DEBUG: %s %s %s %s\n", full_path, device, path, fn);
		strcpy(localcrc,file_crc32(device,path,fn));
		sprintf(str,"Local CRC32: %s\n",localcrc);
		gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 197, 1, 0.32f, GreenFont, str);
		drawScreen();
		
	}
	if (launching == 1) {
		// Display Path The ELF Is Being Loaded From
		scr_printf("* Launching Application from %s \n", arg0);
		sleep(2);
		LoadElf(full_path, half_path);
	}
	//scr_printf(" \n* Operations complete. Returning to Main Menu... \n");
	sleep(1);
	menu_Text();
}

char *set_hbdl_path(){
	static char hbdl_path[256];
	static char cwd[256];
	//uncomment for release
	getcwd(hbdl_path,256);
	//sprintf(hbdl_path,"mc0:/APPS/"); //hardcoded.
	strcat(hbdl_path,"VTSPS2-HBDL.TXT");
	//scr_printf("Debug: %s\n",hbdl_path);
	return hbdl_path;
}

void readcrc() {
	char line[dbsize][128]; //maxlines
	char fname[15] = "VTSPS2-HBDL.TXT";//uncomment for release builds
	//hardcoded path during dev. cwd is 'host' in PCSX2
	//char fname[25] = "mc0:/APPS/VTSPS2-HBDL.TXT";
	char hbdl_path[256];
	getcwd(hbdl_path,256); 		   //uncomment for release builds
	strcat(hbdl_path,fname);	   //uncomment for release builds
	set_hbdl_path();
	FILE *fptr = fopen(hbdl_path,"r"); //fname dev, hbdl_path rls
	int i = 0;
	int tot = 0;
	char *tmp = NULL;
	size_t len = 0;
	ssize_t read;
	if (fptr>=0){
		while((read = getline(&tmp,&len,fptr)) != -1)
		{
			strcpy(line[i],tmp);
			//scr_printf("DEBUG: %s\n", line[i]);
		        i++;
		}
		tot = dbsize;
		for(i = 0; i < tot; ++i)
		{
	        //printf(" %s\n", line[i]);
	        //substring(line[i],line[i],1,(strlen(line[i])-1));
	        sprintf(CRC32DB[i]," %s\n", line[i]);
		}
		fclose(fptr);
		} else {
			printf("readcrc() error");
		}
}


int main(int argc, char *argv[])
{
	extern char fn[16];
	// initialize
	initialize();

	Black = GS_SETREG_RGBAQ(0x00,0x00,0x00,0x00,0x00);
	White = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x00,0x00);

	WhiteFont = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x80,0x00);
	RedFont = GS_SETREG_RGBAQ(0xFF,0x00,0x00,0x80,0x00);
	GreenFont = GS_SETREG_RGBAQ(0x00,0xFF,0x00,0x80,0x00);
	TealFont = GS_SETREG_RGBAQ(0x00,0xFF,0xFF,0x80,0x00);
	YellowFont = GS_SETREG_RGBAQ(0xFF,0xFF,0x00,0x80,0x00);

	gsGlobal = gsKit_init_global();
	//getcwd(font_path,256);
	//strcat(font_path,"/dejavu.bmp");
	//if (strstr(font_path,"host:")){
	//	strcpy(font_path,"mc0:/APPS/dejavu.bmp");
	//}
	//gsFont = gsKit_init_font(GSKIT_FTYPE_BMP_DAT, font_path);
	gsFontM = gsKit_init_fontm();
	
	dmaKit_init(D_CTRL_RELE_OFF,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);

	gsGlobal->PrimAlpha = GS_BLEND_FRONT2BACK;
	gsGlobal->PSM = GS_PSM_CT16;
	gsGlobal->PSMZ = GS_PSMZ_16;
	gsGlobal->Width = 640;
	// Buffer Init
	gsGlobal->PSMZ = GS_PSMZ_16S;
	gsGlobal->ZBuffering = GS_SETTING_OFF;
	gsGlobal->PrimAlphaEnable = GS_SETTING_ON;
	gsGlobal->DoubleBuffering = GS_SETTING_ON;
	gsGlobal->Dithering = GS_SETTING_ON;

	// Do not draw pixels if they are fully transparent
	//gsGlobal->Test->ATE  = GS_SETTING_ON;
	gsGlobal->Test->ATST = 7; // NOTEQUAL to AREF passes
	gsGlobal->Test->AREF = 0x00;
	gsGlobal->Test->AFAIL = 0; // KEEP
	gsFontM->Spacing = 0.95f;
	//Enable transparency
	//gsGlobal->PrimAlphaEnable = GS_SETTING_ON;
	int text_height,edge_size,rownumber,rowoffset;
	text_height = (20.0f * gsFontM->Spacing * 0.15f);
	edge_size = text_height;
	gsKit_init_screen(gsGlobal);
	gsKit_mode_switch(gsGlobal, GS_ONESHOT);	
	//gsKit_set_test(gsGlobal, GS_ZTEST_OFF);
	//gsKit_font_upload(gsGlobal, gsFont);
	gsKit_fontm_upload(gsGlobal, gsFontM);
	//scr_clear();
	menu_header();
	//sleep(1);
	//scr_printf("Modules Loaded. Obtaining an IP Address ... \n");
	gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 61, 1, 0.32f, TealFont, "Modules Loaded. Obtaining an IP Address ... \n");
	//gsKit_fontm_print_scaled(gsGlobal, gsFontM, col, row, layer, scale, RedFont, str);
	drawScreen();
	dhcpmain(); // Setup Network Config With DHCP <dhcpmain.c>
	gsKit_clear(gsGlobal, Black);
	menu_header();
	strcpy(url,"http://hbdl.vts-tech.org/");
	gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 61, 1, 0.32f, TealFont, "IP Address obtained. Downloading homebrew list from hbdl.vts-tech.org ... \n");
	drawScreen();
	//scr_printf("IP Address obtained. Downloading homebrew list from hbdl.vts-tech.org ... \n");
	char *hbdl_path = set_hbdl_path();
	Download("http://hbdl.vts-tech.org/VTSPS2-HBDL.BIN",hbdl_path);
	//file_crc32("mc0:/","APPS/","VTSPS2-HBDL.TXT");
	strcpy(action,actions[0]);
	strcpy(device,devices[0]);
	strcpy(path,paths[0]);
	strcpy(fn,targets[0]);
	sprintf(localcrc,"00000001");
	//sleep(1);
	readcrc(); //populates CRC32DB[]
	gsKit_clear(gsGlobal, Black);
	menu_header();
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
			//sleep(2);
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
			if (strcmp(fn,"AURA.ELF") == 0) {
				strcpy(fn,targets[1]);
			} else if (strcmp(fn,"DOSBOX.ELF") == 0) {
				strcpy(fn,targets[2]);
			} else if (strcmp(fn,"EJECT.ELF") == 0) {
				strcpy(fn,targets[3]);
			} else if (strcmp(fn,"ESR.ELF") == 0) {
				strcpy(fn,targets[4]);
			} else if (strcmp(fn,"GSM.ELF") == 0) {
				strcpy(fn,targets[5]);
			} else if (strcmp(fn,"HDL.ELF") == 0) {
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
		printf("DEBUG: %s %s %s %s\n", action, device, path, fn);
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
	//gsKit_vram_clear(gsGlobal);
}
}
