#include <tamtypes.h>
#include <errno.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
//#include <fileio.h>
#include <libmc.h>
#include <stdio.h>
#include <string.h>
#include "libpad.h"
#include <debug.h>
#include <libpwroff.h>
#include <iopcontrol.h>
#include <stdlib.h>
#include "malloc.h"
#include <libcdvd.h>
#include <iopheap.h>
//#include <io_common.h>
#include <fcntl.h>
#include <syscallnr.h>
#include <unistd.h>
//#include <fileXio.h>
//#include <fileXio_rpc.h>
#include "checksum.h"

// IP Shit.
#include <netman.h>
#include <ps2ips.h>

// SBV
#include <sbv_patches.h>


//ps2ip.c
int dhcpmain();

//VTSPS2-CRC32.c
void substring(char s[], char sub[], int p, int l);
char* file_crc32(char device[], char path[], char fn[]);

//misc.c
int getFileSize(int fd);
void LoadElf(const char *elf, char* path);
ssize_t getline(char **lineptr, size_t *n, FILE *stream);
void ResetIOP();

//pad.c
void buttonStatts(int port, int slot);
void checkPadConnected(void);

// Modules
extern void freesio2;
extern void iomanX;
extern void fileXio;
extern void freepad;
extern void poweroff;
extern void mcman;
extern void mcserv;
extern void ps2dev9;
extern void netman;
extern void smap;
extern void ps2ipnm;
extern void ps2ips;
extern void ps2http;
extern void usbd;
extern void usbhdfsd;

extern u32 size_poweroff;
extern u32 size_freesio2;
extern u32 size_iomanX;
extern u32 size_fileXio;
extern u32 size_freepad;
extern u32 size_mcman;
extern u32 size_mcserv;
extern u32 size_ps2dev9;
extern u32 size_netman;
extern u32 size_smap;
extern u32 size_ps2ipnm;
extern u32 size_ps2ips;
//extern u32 size_dnsirx;
extern u32 size_ps2http;
extern u32 size_usbd;
extern u32 size_usbhdfsd;

//PAD VARIABLES
//check for multiple definitions
//#define DEBUG

#if !defined(ROM_PADMAN) && !defined(NEW_PADMAN)
#define ROM_PADMAN
#endif

#if defined(ROM_PADMAN) && defined(NEW_PADMAN)
#error Only one of ROM_PADMAN & NEW_PADMAN should be defined!
#endif

#if !defined(ROM_PADMAN) && !defined(NEW_PADMAN)
#error ROM_PADMAN or NEW_PADMAN must be defined!
#endif


//pad buffer
static char padBuf[256] __attribute__((aligned(64)));
//rumblers
static char actAlign[6];
static int actuators;
//button status
struct padButtonStatus buttons;
u32 paddata;
u32 old_pad;
u32 new_pad;
int port, slot;
extern void readPad(void);

void LoadModules(void);
void initialize(void);
int LoadIRX();


#define TYPE_XMC

