#include <errno.h>
#include <loadfile.h>
#include <sbv_patches.h>

#include "malloc.h"
#include "VTSPS2-HBDL.h"
#include "include/pad.h"

// Modules
extern unsigned char freesio2[];
extern unsigned int size_freesio2;

extern unsigned char iomanX[];
extern unsigned int size_iomanX;

extern unsigned char fileXio[];
extern unsigned int size_fileXio;

extern unsigned char freepad[];
extern unsigned int size_freepad;

extern unsigned char poweroff[];
extern unsigned int size_poweroff;

extern unsigned char mcman[];
extern unsigned int size_mcman;

extern unsigned char mcserv[];
extern unsigned int size_mcserv;

extern unsigned char ps2dev9[];
extern unsigned int size_ps2dev9;

extern unsigned char netman[];
extern unsigned int size_netman;

extern unsigned char smap[];
extern unsigned int size_smap;

extern unsigned char ps2ipnm[];
extern unsigned int size_ps2ipnm;

extern unsigned char ps2ips[];
extern unsigned int size_ps2ips;

extern unsigned char ps2http[];
extern unsigned int size_ps2http;

extern unsigned char usbd[];
extern unsigned int size_usbd;

extern unsigned char usbhdfsd[];
extern unsigned int size_usbhdfsd;

static void ResetIOP()
{
	SifInitRpc(0);           //Initialize SIFRPC and SIFCMD. Although seemingly unimportant, this will update the addresses on the EE, which can prevent a crash from happening around the IOP reboot.
	while(!SifIopReset("", 0)){};      //Reboot IOP with default modules (empty command line)
	while(!SifIopSync()){}   //Wait for IOP to finish rebooting.
	SifInitRpc(0);           //Initialize SIFRPC and SIFCMD.
	SifLoadFileInit();       //Initialize LOADFILE RPC.
	// SBV Patches Are Not part of a Normal IOP Reset.
	sbv_patch_enable_lmb(); //SBV Patches
	sbv_patch_disable_prefix_check(); //SBV Patch Load Executable IRX And ELF Files From User-Writable Storage
	//sbv_patch_user_mem_clear(0x00100000); // You Can Specify a Starting Address for the Wipe
	//sbv_patch_user_mem_clear(0x02000000); // Disable Clear Memory With LoadExecPS2() when 0x02000000 is passed as an arg
}

static void gotoOSDSYS(void)
{
	ResetIOP();
	LoadExecPS2("rom0:OSDSYS", 0, NULL);
}

// might be better off immediatley returning and exiting if certain modules don't load idk but this looks neater.. for now..
int loadModules(void)
{
	int ret;

	ret = SifExecModuleBuffer(&freesio2, size_freesio2, 0, NULL, NULL);
	if (ret < 0)
		printf("Failed to Load freesio2 sw module");

	ret = SifExecModuleBuffer(&iomanX, size_iomanX, 0, NULL, NULL);
	if (ret < 0)
		printf("Failed to Load iomanx sw module");

	ret = SifExecModuleBuffer(&freepad, size_freepad, 0, NULL, NULL);
	if (ret < 0)
		printf("Failed to Load freepad sw module");

	ret = SifExecModuleBuffer(&mcman, size_mcman, 0, NULL, NULL);
	if (ret < 0)
		printf("Failed to Load mcman sw module");

	ret = SifExecModuleBuffer(&mcserv, size_mcserv, 0, NULL, NULL);
	if (ret < 0)
		printf("Failed to Load mcserv sw module");

	ret = SifExecModuleBuffer(&ps2dev9, size_ps2dev9, 0, NULL, NULL);
	if (ret < 0)
		printf(" Could not load ps2dev9.IRX! %d\n", ret);

	ret = SifExecModuleBuffer(&netman, size_netman, 0, NULL, NULL);
	if (ret < 0)
		printf(" Could not load netman.IRX! %d\n", ret);

	ret = SifExecModuleBuffer(&smap, size_smap, 0, NULL, NULL);
	if (ret < 0)
		printf(" Could not load smap.IRX! %d\n", ret);

	ret = SifExecModuleBuffer(&ps2ipnm, size_ps2ipnm, 0, NULL, NULL);
	if (ret < 0)
		printf(" Could not load ps2ip.IRX! %d\n", ret);

	ret = SifExecModuleBuffer(&ps2ips, size_ps2ips, 0, NULL, NULL);
	if (ret < 0)
		printf(" Could not load ps2ips.IRX! %d\n", ret);

	ps2ip_init(); //interesting place to put it..

	ret = SifExecModuleBuffer(&ps2http, size_ps2http, 0, NULL, NULL);
	if (ret < 0)
		printf(" Could not load ps2http.IRX! %d\n", ret);

	ret = SifExecModuleBuffer(&usbd, size_usbd, 0, NULL, NULL);
	if (ret < 0)
		printf(" Could not load usbd.IRX! %d\n", ret);

	ret = SifExecModuleBuffer(&usbhdfsd, size_usbhdfsd, 0, NULL, NULL);
	if (ret < 0)
		printf(" Could not load usbhdfsd.IRX! %d\n", ret);

	return ret;
}

void init(void)
{
	int ret;

	ResetIOP();

	ret = loadModules();
	if (ret < 0)
		gotoOSDSYS();

	pad_init();
}

void deinit(int browser)
{
	menuEnd();
	guiEnd();

	if (browser)
		gotoOSDSYS();
}

// This function is public domain -- Will Hartung 4/9/09 */
// Modifications, public domain as well, by Antti Haapala, 11/10/17
// - Switched to getc on 5/23/19 */

// if typedef doesn't exist (msvc, blah)
//typedef intptr_t ssize_t;
ssize_t getline(char **lineptr, size_t *n, FILE *stream)
{
    size_t pos;
    int c;

    if (lineptr == NULL || stream == NULL || n == NULL) {
        errno = EINVAL;
        return -1;
    }

    c = getc(stream);
    if (c == EOF) {
        return -1;
    }

    if (*lineptr == NULL) {
        *lineptr = malloc(128);
        if (*lineptr == NULL) {
            return -1;
        }
        *n = 128;
    }

    pos = 0;
    while(c != EOF) {
        if (pos + 1 >= *n) {
            size_t new_size = *n + (*n >> 2);
            if (new_size < 128) {
                new_size = 128;
            }
            char *new_ptr = realloc(*lineptr, new_size);
            if (new_ptr == NULL) {
                return -1;
            }
            *n = new_size;
            *lineptr = new_ptr;
        }

        ((unsigned char *)(*lineptr))[pos ++] = c;
        if (c == '\n') {
            break;
        }
        c = getc(stream);
    }

    (*lineptr)[pos] = '\0';
    return pos;
}
