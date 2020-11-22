/*
				VTSPS2-HBDL by VTSTech
	Based on iLaunchELF by krHACKen & Based_Skid
*/

#include "VTSPS2-HBDL.h"
#include <stdio.h>
#include <string.h>
#include <loadfile.h>
#include <elf-loader.h>

#include <fcntl.h>
#include <unistd.h>

#include "include/gui.h"
#include "include/menu.h"
#include "include/textures.h"
#include "include/pad.h"

#define DBSIZE 61 //lines in VTSPS2-HBDL.TXT

//0.32-GUI
static char remotecrc[9];
static char localcrc[9];
static char CRC32DB[DBSIZE][128];
char mirror[2][128];

char action[32], device[32], path[256], fn[16]; //there are better ways to do this

static int Download(char *urll, char *full_path)
{
	int fd, target;
	int size = 0;
	char buf[5600000];

	fd = open(urll, O_RDONLY);
	if (fd >= 0) {
		target = open(full_path, O_RDWR | O_CREAT);
		if(target >= 0) {
			size = lseek(fd, 0, SEEK_END);
			lseek(fd, 0, SEEK_SET);

			read(fd, buf, size);
			write(target, buf, size);

			sprintf(localcrc, file_crc32(device, path, fn));
			close(fd);
			close(target);
		} else
			printf("Download Error! Debug: %d %d %d", fd, target, size);
	}

	return size;
}

static void DownloadList(char *device, char *path, char *fn)
{
	char arg0[256], arg1[256], arg2[256], arg3[256], arg4[256], arg5[256], arg6[256], arg7[256], arg8[256];
	char *exec_args[9] = { arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8 };
	int argc = 0;
	int y = 0;
	int z = 0;
	int ret = 0;
	int fd = 0;
	int size = 0;
	char full_path[256];
	char str[256];
	int terminate = 0;
	int opsDone = 0;
	//patches.ppi

	while (!terminate) {
		guiDrawTerminal();

		if (http_mirror == 0)
			strcpy(str,"* Mirror 1 Selected... \n");
		else if (http_mirror == 1)
			strcpy(str,"* Mirror 2 Selected... \n");

		drawFont(35, 117, 0.32f, WhiteFont, str);
		drawFont(35, 134, 0.32f, WhiteFont, "* Building Download List... \n");

		if (strstr("DOSBOX.ELF",fn)) {
			drawFont(35, 151, 0.32f, WhiteFont, "* DOSBox ... \n");

			if (!opsDone) {
				argc = 4;
				for (y = 0; y <= argc; y++)
					strcpy(exec_args[y], mirror[http_mirror]);

				strcat(exec_args[0],fn);
				strcpy(exec_args[1],fn);
				strcat(exec_args[2],"dosbox.bin");
				strcpy(exec_args[3],"dosbox.conf");
			}
		} else if (strstr("PS2DOOM.ELF",fn)) {
			drawFont(35, 151, 0.32f, WhiteFont, "* PS2Doom ... \n");

			if (!opsDone) {
				argc = 6;
				for (y = 0; y <= argc; y++)
					strcpy(exec_args[y], mirror[http_mirror]);

				strcat(exec_args[0],fn);
				strcpy(exec_args[1],fn);
				strcat(exec_args[2],"ps2doom.bin");
				strcpy(exec_args[3],"ps2doom.config");
				strcat(exec_args[4],"doom1.wad");
				strcpy(exec_args[5],"doom1.wad");
			}
		} else if (strstr("PS2ESDL.ELF",fn)) {
			drawFont(35, 151, 0.32f, WhiteFont, "* PS2ESDL ... \n");

			if (!opsDone) {
				argc = 4;
				for (y = 0; y <= argc; y++)
					strcpy(exec_args[y], mirror[http_mirror]);

				strcat(exec_args[0],fn);
				strcpy(exec_args[1],fn);
				strcat(exec_args[2],"patches.ppi");
				strcpy(exec_args[3],"patches.ppi");
			}
		}

		int y = 151;
		int spacing = 17;
		for (z = 0; z < argc; z = z+2) {
			memset(full_path, 0, sizeof(full_path));
			snprintf(full_path, sizeof(full_path), "%s%s%s", device, path, exec_args[z+1]);
			drawFont(35, y += spacing, 0.32f, WhiteFont, "* Downloading...\n");
			sprintf(str,"* URL: %s\n", exec_args[z]);
			drawFont(35, y += spacing, 0.32f, WhiteFont, str);
			sprintf(str,"* Path: %s\n", full_path);
			drawFont(35, y += spacing, 0.32f, WhiteFont, str);

			if (!opsDone) {
				ret = Download(exec_args[z], full_path);
				if(ret <= 0) {
					printf("* Error! Could not open the file\n");
				} else {
					fd = open(full_path, O_RDONLY);
					size = lseek(fd, 0, SEEK_END);
					lseek(fd, 0, SEEK_SET);
					close(fd);
				}
			}

			if (size >= 1) {
				sprintf(str,"* %s Download Complete!\n", full_path);
				drawFont(35, y += spacing, 0.32f, GreenFont, str);
			} else {
				sprintf(str,"* %s Download Failed!\n", full_path);
				drawFont(35, y += spacing, 0.32f, RedFont, str);
			}
		}

		drawFont(35, 380, 0.32f, WhiteFont, "Press X or O to continue.\n");

		guiRender();
		opsDone = 1;

		buttonStatts(0, 0);
		if ((new_pad & PAD_CROSS) || (new_pad & PAD_CIRCLE))
			terminate = 1;
	}
}

void compareCRC(void)
{
	char remotefn[15];
	char hbsize[7];
	int x = (DBSIZE / 2); //starting line # of CRC's from VTSPS2-HBDL.TXT
	int z = 0;

	while(z <= x) {
		int fnsize = (strlen(fn));
		char hbfn[] = "";
		char hbver[] = "";
		substring(CRC32DB[z],hbfn,14,fnsize);

		hbsize[0] = 0;
		if (strstr(hbfn,fn)) {
			substring(CRC32DB[z],hbsize,(14+fnsize),6);
			substring(CRC32DB[z],hbver,(21+fnsize),strlen(CRC32DB[z]));
		}
		z++;
	}
	while(x<=DBSIZE) {//total lines in VTSPS2-HBDL.TXT
		int fnsize = (strlen(CRC32DB[x]) - 12);
		remotefn[0] = 0;
		substring(CRC32DB[x],remotefn,1,fnsize);
		if (strstr(remotefn,fn) && strlen(remotefn) == (strlen(fn)+1)) {
			//strcpy(remotefn,fn);
			substring(CRC32DB[x],remotecrc,(strlen(fn)+3),9);
			/*scr_printf("Local CRC32: ");
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
			scr_printf("%s",localcrc);
			}
			scr_printf(" Remote CRC32:%s \n", remotecrc);*/
		}
		x++;
	}
}

void DoTask(int task)
{
	int ret = 0, launching, downloading, checking;
	char arg0[256], arg1[256], arg2[256], arg3[256], arg4[256], arg5[256], arg6[256], arg7[256], arg8[256];
	char *exec_args[9] = { arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8 };
	int argc = 0;
	int fd = 0;
	int size = 0;
	char full_path[256];
	char str[256];
	/*
	exec_args[0] == the target ELF's URI. loader.elf will load that ELF.
	exec_args[1] to exec_args[8] == arguments to be passed to the target ELF.
	*/

	launching = 0;
	downloading = 0;
	checking = 0;

	//read pad 0
	buttonStatts(0, 0);

	if (task != 0)
	{
		if (task != 3) {
			guiClear();
			guiDrawTerminal();
		}

		if (task == 1)
		{
			checking = 1;
			snprintf(full_path, sizeof(full_path), "%s%s%s", device, path, fn);
			drawFont(35, 100, 0.35f, WhiteFont, "Checking...\n");
			guiRender();
		}
		else if (task == 2)
		{
			downloading = 1;
			drawFont(35, 100, 0.35f, WhiteFont, "Downloading...\n");
			guiRender();

			sprintf(exec_args[0], "%s%s", mirror[http_mirror], fn);
			snprintf(full_path, sizeof(full_path), "%s%s", device, path);

			//remove trailing / for MkDir
			char make_path[256];
			substring(full_path, make_path, 1, strlen(full_path) - 1);
			mkdir(make_path, 0777);

			strcat(full_path, fn);
			argc = 1;
		}
		if (task == 3)
		{
			launching = 1;
			snprintf(full_path, sizeof(full_path), "%s%s%s", device, path, fn);
			strcpy(exec_args[0], full_path);
			argc = 1;
		}
	} else asm volatile("break\n"); // OUT OF BOUNDS, UNDEFINED ITEM!

	int terminate = 0;
	int opsDone = 0;

	if (downloading == 1) {
		if (strstr("PS2ESDL.ELF",fn))
			DownloadList(device,path,"PS2ESDL.ELF");
		else if (strstr("PS2DOOM.ELF",fn))
			DownloadList(device,path,"PS2DOOM.ELF");
		else if (strstr("DOSBOX.ELF",fn))
			DownloadList(device,path,"DOSBOX.ELF");
		else {
			while (!terminate) { //a bit crude but it works..good enough for now
				guiDrawTerminal();

				sprintf(str,"* URL: %s \n", exec_args[0]);
				drawFont(35, 117, 0.32f, WhiteFont, str);

				sprintf(str,"* Path: %s \n", full_path);
				drawFont(35, 134, 0.32f, WhiteFont, str);

				if (!opsDone)
					ret = Download(exec_args[0],full_path);
				if (ret <= 0) {
					drawFont(35, 151, 0.32f, RedFont, "! Download Error!\n");
				} else {
					sprintf(str,"* File Size: %d bytes \n", ret);
					drawFont(35, 151, 0.32f, WhiteFont, str);

					if (!opsDone) {
						fd = open(full_path, O_RDONLY);
						size = lseek(fd, 0, SEEK_END);
						lseek(fd, 0, SEEK_SET);
						close(fd);
					}

					if (size >= 1) {
						sprintf(str,"* %s Exists! \n", full_path);
						drawFont(35, 168, 0.32f, WhiteFont, str);
					} else {
						sprintf(str,"* %s Does Not Exist! \n", full_path);
						drawFont(35, 168, 0.32f, RedFont, str);
					}
				}

				if (!opsDone)
					compareCRC();

				if (strstr(localcrc, remotecrc) != 0)
					drawFont(35, 185, 0.32f, RedFont, "Local and Remote CRC32 do not match!\n");
				else {
					sprintf(str, "CRC32 Verified! %s\n", localcrc);
					drawFont(35, 185, 0.32f, GreenFont, str);
				}
				drawFont(35, 380, 0.32f, WhiteFont, "Press X or O to continue.\n");

				guiRender();
				opsDone = 1;

				buttonStatts(0, 0);
				if ((new_pad & PAD_CROSS) || (new_pad & PAD_CIRCLE))
					terminate = 1;
			}
		}
	}

	if (checking == 1) {
		while (!terminate) {
			guiDrawTerminal();

			if (!opsDone) {
				snprintf(full_path, sizeof(full_path), "%s%s%s", device, path, fn);
				fd = open(full_path, O_RDONLY);
			}
			if (fd < 0) {
				sprintf(str,"! %s Does Not Exist!\n", full_path);
				drawFont(35, 117, 0.32f, RedFont, str);
			} else {
				sprintf(str,"* Local File Opened... %d \n", fd);
				drawFont(35, 117, 0.32f, GreenFont, str);
			}

			if (!opsDone) {
				size = lseek(fd, 0, SEEK_END);
				lseek(fd, 0, SEEK_SET);
			}

			if (size >= 1) {
				sprintf(str,"* File Size... %d \n", size);
				drawFont(35, 134, 0.32f, GreenFont, str);

				sprintf(str,"* Calculating CRC32 %s \n", full_path);
				drawFont(35, 151, 0.32f, GreenFont, str);
			} else
				sprintf(localcrc, "00000000");

			if (!opsDone) {
				strcpy(localcrc, file_crc32(device,path,fn));
				close(fd);
			}

			sprintf(str,"Local CRC32: %s\n", localcrc);
			drawFont(35, 168, 0.32f, GreenFont, str);
			drawFont(35, 380, 0.32f, WhiteFont, "Press X or O to continue.\n");

			guiRender();
			opsDone = 1;

			buttonStatts(0, 0);
			if ((new_pad & PAD_CROSS) || (new_pad & PAD_CIRCLE))
				terminate = 1;
		}
	}

	if (launching == 1) {
		//check file exists then launch
		fd = open(full_path, O_RDONLY);
		if (fd >= 0) {
			close(fd);
			deinit(0);
			LoadELFFromFile(full_path, argc, exec_args);
		}
	}
}

static char hbdl_path[256];

static char *set_hbdl_path(void)
{
	getcwd(hbdl_path, 256); //uncomment for release
	//sprintf(hbdl_path,"mc0:APPS/"); //hardcoded.

	strcat(hbdl_path,"VTSPS2-HBDL.TXT");

	return hbdl_path;
}

static void readcrc()
{
	char line[DBSIZE][128]; //maxlines
	char hbdl_path[256];
	//hardcoded path during dev. cwd is 'host' in PCSX2
	//char fname[25] = "mc0:APPS/VTSPS2-HBDL.TXT";

	char fname[16] = "VTSPS2-HBDL.TXT";	//uncomment for release builds
	getcwd(hbdl_path, 256);				//uncomment for release builds
	strcat(hbdl_path, fname);			//uncomment for release builds

	set_hbdl_path();
	FILE *fptr = fopen(hbdl_path, "r"); //fname dev, hbdl_path rls
	int i = 0;
	char *tmp = NULL;
	size_t len = 0;
	ssize_t read;

	if (fptr >= 0) {
		while((read = getline(&tmp, &len, fptr)) != -1)
			strcpy(line[i++], tmp);

		for(i = 0; i < DBSIZE; ++i)
			sprintf(CRC32DB[i]," %s\n", line[i]);

		fclose(fptr);
	} else
		printf("readcrc() error");
}

static void setDefaults(void)
{
	snprintf(action, sizeof(action), "CHECK");
	snprintf(device, sizeof(device), "mc0:/");
	snprintf(path, sizeof(path), "APPS/");
	snprintf(fn, sizeof(fn), downloadableApps[0].elfName);

	snprintf(mirror[0], sizeof(mirror[0]), "http://hbdl.vts-tech.org/");
	snprintf(mirror[1], sizeof(mirror[1]), "http://www.hwc.nat.cu/ps2-vault/ps2hbdl/");

	http_mirror = 0;
}

int main(int argc, char *argv[])
{
	// initialize
	init();

	guiInit();

	guiDrawLogo();

	drawFont(70, 171, 0.32f, TealFont, "Modules Loaded. Obtaining an IP Address ... \n");
	guiRender();

	dhcpmain(); // Setup Network Config With DHCP <dhcpmain.c>

	menuInitMenu();

	guiClear();

	guiDrawLogo();
	drawFont(70, 171, 0.32f, TealFont, "IP Address obtained. Downloading homebrew list from hbdl.vts-tech.org ... \n");
	guiRender();

	Download("http://hbdl.vts-tech.org/VTSPS2-HBDL.BIN", set_hbdl_path());

	setDefaults();
	sprintf(localcrc, "00000001");

	readcrc(); //populates CRC32DB[]
	guiClear();

	while (1)
	{
		guiDrawMenu();
		guiRender();
		menuHandleInput();
	}
}
