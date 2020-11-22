#include <kernel.h>
#include <gsKit.h>
#include <gsToolkit.h>

#include "include/gui.h"
#include "include/menu.h"
#include "include/textures.h"
#include "include/pad.h"

#include "VTSPS2-HBDL.h"

const u64 Black = GS_SETREG_RGBAQ(0x00,0x00,0x00,0x00,0x00);
const u64 White = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x00,0x00);
const u64 Blue = GS_SETREG_RGBAQ(0x01,0x50,0xA9,0x80,0x00);

const u64 WhiteFont = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x80,0x00);
const u64 RedFont = GS_SETREG_RGBAQ(0xFF,0x00,0x00,0x80,0x00);
const u64 GreenFont = GS_SETREG_RGBAQ(0x00,0xFF,0x00,0x80,0x00);
const u64 TealFont = GS_SETREG_RGBAQ(0x00,0xFF,0xFF,0x80,0x00);
const u64 YellowFont = GS_SETREG_RGBAQ(0xFF,0xFF,0x00,0x80,0x00);

// should really fill this from VTSPS2-HBDL.TXT but I cbf right now
targets_t downloadableApps[NUM_APPS] = {
	{"AURA.ELF", "Aura For Laura", "Demo", "4.87MB"},
	{"DOSBOX.ELF", "DosBox", "0.74", "3.71MB"},
	{"EJECT.ELF", "Eject", "0.6", "0.03MB"},
	{"ESR.ELF", "ESR", "R10F DVD Direct", "0.01MB"},
	{"FRUITY.ELF", "Fruity", "1.0", "0.70MB"},
	{"GSM.ELF", "GSM", "0.38", "0.09MB"},
	{"HDL.ELF", "HDL", "0.8C", "0.60MB"},
	{"HERMES.ELF", "Hermes", "1.10", "0.95MB"},
	{"INFOGB.ELF", "InfoGB", "0.5K", "0.54MB"},
	{"LBFN.ELF", "Lbfn", "0.07.19", "0.54MB"},
	{"NEOCD.ELF", "NeoCD", "0.6c", "0.32MB"},
	{"OPL.ELF", "Open PS2 Loader", "r1562: #41f7cf0", "1.12MB"},
	{"PGEN.ELF", "PGEN", "1.5.1S", "2.90MB"},
	{"PS2DOOM.ELF", "PS2 Doom", "1.0.5.0", "5.12MB"},
	{"PS2ESDL.ELF", "PS2ESDL", "0.810 BETA", "0.18MB"},
	{"PS2SX.ELF", "PS2SX", "1.3", "0.05MB"},
	{"PSMS.ELF", "PSMS", "0.6.3 Reloaded", "0.23MB"},
	{"PVCS.ELF", "PVCS", "0.3 Reloaded", "0.87MB"},
	{"RA_2048.ELF", "RetroArch 2048", "1.9.0", "0.85MB"},
	{"RA_FCEU.ELF", "RetroArch FCEUmm", "1.9.0", "0.99MB"},
	{"RA_MGBA.ELF", "RetroArch mGBA", "1.9.0", "1.03MB"},
	{"RA_PICO.ELF", "RetroArch PicoDrive", "1.9.0", "1.00MB"},
	{"RA_QNES.ELF", "RetroArch QuickNES", "1.9.0", "0.92MB"},
	{"SMS.ELF", "Simple Media System (SMS)", "2.9 REV4", "1.36MB"},
	{"SNES9X.ELF", "SNES9X", "1.53 (2011)", "3.65MB"},
	{"SNESSTN.ELF", "SNES Station", "0.26C", "0.83MB"},
	{"TESTMODE.ELF", "Test Mode", "0.3", "0.03MB"},
	{"WLE.ELF", "wLaunch Elf", "4.43A: #970ca992", "0.40MB"},
	{"XUMP.ELF", "XUMP", "1.10", "0.64MB"},
	{"ZONELDR.ELF", "ZONELDR", "0.5 BETA", "0.13MB"}
};

// CLEAN UP vv
extern char action[32], device[32], path[256], fn[16];

extern char mirror[2][128];
// CLEAN UP ^^

static GSGLOBAL *gsGlobal;
static GSFONTM *gsFontM;
static GSTEXTURE gsTexture[TEXTURES_COUNT];

static int callbackId;
static int semaId;

static int guiCallback(void)
{
	ee_sema_t sema;
	iReferSemaStatus(semaId, &sema);
	if(sema.count < sema.max_count)
		iSignalSema(semaId);

	ExitHandler();
	return 0;
}

void guiInit(void)
{
	dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
				D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);

	// Initialize the GS
	gsGlobal = gsKit_init_global();

	// Force NTSC (cause I cbf dealing with various screenHeights)
	gsGlobal->Mode = GS_MODE_NTSC;
	gsGlobal->Width = 640;
	gsGlobal->Height = 448;
	gsGlobal->Interlace = GS_INTERLACED;
	gsGlobal->Field = GS_FIELD;
	gsGlobal->PSM = GS_PSM_CT24;

	gsGlobal->PrimAAEnable = GS_SETTING_ON;
	gsGlobal->PrimAlphaEnable = GS_SETTING_ON;
	gsGlobal->DoubleBuffering = GS_SETTING_ON; 
	gsGlobal->ZBuffering = GS_SETTING_OFF;

	gsFontM = gsKit_init_fontm();
	gsKit_fontm_upload(gsGlobal, gsFontM);
	gsFontM->Spacing = 0.95f;

	ee_sema_t sema;
	sema.init_count = 0;
	sema.max_count = 1;
	sema.attr = sema.option = 0;
	semaId = CreateSema(&sema);

	callbackId = gsKit_add_vsync_handler(&guiCallback);
	gsKit_init_screen(gsGlobal);
	gsKit_mode_switch(gsGlobal, GS_ONESHOT);

	gsKit_set_test(gsGlobal, GS_ZTEST_OFF);
	gsKit_set_primalpha(gsGlobal, GS_SETREG_ALPHA(0, 1, 0, 1, 0), 0);
	gsKit_clear(gsGlobal, GS_SETREG_RGBAQ(0x00,0x00,0x00,0x80,0x00));
	gsKit_sync_flip(gsGlobal);

	// Load all textures
	int i;
	for (i = 0; i < TEXTURES_COUNT; i++)
	{
		gsTexture[i].Delayed = 1;
		texPngLoad(&gsTexture[i], i);
		gsTexture[i].Filter = GS_FILTER_LINEAR;
	}
}

void drawFont(float x, float y, float scale, u64 colour, char *text)
{
	gsKit_fontm_print_scaled(gsGlobal, gsFontM, x, y, 3, scale, colour, text);
}

static void drawBackground(GSTEXTURE *gsTexture)
{
	u64 texCol = GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00);

	gsKit_TexManager_bind(gsGlobal, gsTexture);

	gsKit_prim_sprite_texture(gsGlobal, gsTexture,
						0.0f,  // X1
						0.0f,  // Y2
						0.0f,  // U1
						0.0f,  // V1
						gsGlobal->Width, // X2
						gsGlobal->Height, // Y2
						gsGlobal->Width, // U2
						gsGlobal->Height, // V2
						2,
						texCol);
}

static void drawTexture(GSTEXTURE *gsTexture, float fx, float fy)
{
	u64 texCol = GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00);

	gsKit_TexManager_bind(gsGlobal, gsTexture);

	gsKit_prim_sprite_texture(gsGlobal, gsTexture,
						fx,  // X1
						fy - gsTexture->Height,  // Y2
						0.0f,  // U1
						0.0f,  // V1
						fx + gsTexture->Width, // X2
						fy, // Y2
						gsTexture->Width, // U2
						gsTexture->Height, // V2
						2,
						texCol);
}

void guiDrawLogo(void)
{
	drawTexture(&gsTexture[LOGO], 70, 150);
}

void guiDrawMenu(void)
{
	char str[256];

	drawBackground(&gsTexture[BACKGROUND]);

	sprintf(str, "IP Address: %s\n", getIpAddress());
	drawFont(35, 23, 0.34f, TealFont, str);

	sprintf(str, "Mirror: %s\n", mirror[http_mirror]);
	drawFont(37, 32, 0.34f, TealFont, str);

	sprintf(str,"Mode: %s", action);
	drawFont(35, 52, 0.32f, TealFont, str);

	sprintf(str,"Device: %s", device);
	drawFont(265, 52, 0.32f, TealFont, str);

	sprintf(str,"Path: %s", path);
	drawFont(485, 52, 0.32f, TealFont, str);

	menuRender();
}

void guiDrawTerminal(void)
{
	char *appName = "VTSPS2-HBDL HomeBrew DownLoader v0.32-GUI 2020-11-13 (Commit: TBD)\n";
	char *appAuthor = "Forked from iLaunchElf by VTSTech github.com/Veritas83/VTSPS2-HBDL\nOriginal iLaunchElf By: krHACKen, Based_Skid, Copyright \xa9 2018 \n \n";
	/*char *appVer = "";
	char *appNotice = "Notice: This App Contains Code from uLaunchELF \n";*/

	drawBackground(&gsTexture[BACKGROUND2]);

	drawFont(35, 20, 0.32f, YellowFont, appName);
	drawFont(35, 35, 0.32f, YellowFont, appAuthor);
}

void guiClear(void)
{
	gsKit_clear(gsGlobal, Black);
}

void guiRender(void)
{
	gsKit_set_finish(gsGlobal);
	gsKit_queue_exec(gsGlobal);
	gsKit_finish();

	if(!gsGlobal->FirstFrame)
	{
		// Wait for vsync...
		PollSema(semaId);
		WaitSema(semaId);

		if (gsGlobal->DoubleBuffering == GS_SETTING_ON)
		{
			GS_SET_DISPFB2(gsGlobal->ScreenBuffer[gsGlobal->ActiveBuffer & 1] / 8192,
							gsGlobal->Width / 64, gsGlobal->PSM, 0, 0);

			gsGlobal->ActiveBuffer ^= 1;
		}
	}

	gsKit_setactive(gsGlobal);
	gsKit_TexManager_nextFrame(gsGlobal);
}

void guiEnd(void)
{
	DeleteSema(semaId);
}
