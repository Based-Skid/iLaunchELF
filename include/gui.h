#ifndef __GUI_H
#define __GUI_H

#include <tamtypes.h>

#define NUM_APPS 30

typedef struct
{
	char *elfName;
	char *longName;
	char *version;
	char *size;
	char *rcrc;
} targets_t;

targets_t downloadableApps[NUM_APPS];

const u64 Black;
const u64 White;
const u64 Blue;

const u64 WhiteFont;
const u64 RedFont;
const u64 GreenFont;
const u64 TealFont;
const u64 YellowFont;

void drawFont(float x, float y, float scale, u64 colour, char *text);

void guiInit(void);
void guiEnd(void);

void guiDrawLogo(void);
void guiDrawMenu(void);
void guiDrawTerminal(void);

void guiClear(void);
void guiRender(void);

#endif
