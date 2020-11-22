#include "VTSPS2-HBDL.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/gui.h"
#include "include/menu.h"
#include "include/pad.h"

typedef struct menu_item
{
    char *text;
    int id;
} menu_item_t;

typedef struct menu_list
{
    struct menu_item item;

    struct menu_list *prev, *next, *current, *pagestart;
} menu_list_t;

static menu_list_t *menu;

// CLEAN UP vv
extern char action[32], device[32], path[256], fn[16];

static char ELF_NO_EXT[128];
static char PATH_ELF[256];
static char PATH_APP[256];
// CLEAN UP ^^

static menu_list_t *menuAllocItem(char *text, int id)
{
	menu_list_t *it = (menu_list_t *)malloc(sizeof(menu_list_t));

	it->prev = NULL;
	it->next = NULL;
	it->item.text = text;
	it->item.id = id;

	return it;
}

static menu_list_t *menuAddItem(menu_list_t **menu, char *text, int id)
{
	if (*menu == NULL) {
		*menu = menuAllocItem(text, id);
		return *menu;
	}

	menu_list_t *cur = *menu;

	// traverse till the end
	while (cur->next)
		cur = cur->next;

	// create new item
	menu_list_t *newitem = menuAllocItem(text, id);

	// link
	cur->next = newitem;
	newitem->prev = cur;

	return newitem;
}

static void menuDestroy(menu_list_t **menu)
{
	// destroy sub menu
	menu_list_t *cur = *menu;

	while (cur) {
		menu_list_t *td = cur;
		cur = cur->next;

		free(td);
	}

	*menu = NULL;
}

void menuInitMenu(void)
{
	int i;

	if (menu)
		menuDestroy(&menu);

	for (i = 0; i < NUM_APPS; i++)
		menuAddItem(&menu, downloadableApps[i].longName, i);

	menu->current = menu;
	menu->pagestart = menu->current;
}

void menuEnd(void)
{
	if (menu)
		menuDestroy(&menu);
}

void menuRender(void)
{
	menu_list_t *ps = menu->pagestart;
	u64 color;
	int others = 0;
	int spacing = 20;
	int y = 96;

	while (ps && (others++ < 15))
	{
		if (ps == menu->current)
			color = Blue;
		else
			color = WhiteFont;

		drawFont(35, y, 0.45f, color, downloadableApps[ps->item.id].longName);
		drawFont(315, y, 0.45f, color, downloadableApps[ps->item.id].version);
		drawFont(535, y, 0.45f, color, downloadableApps[ps->item.id].size);

		y += spacing;
		ps = ps->next;
	}
}

static void menuNavDown(void)
{
	menu_list_t *cur = menu->current;

	if (cur && cur->next) {
		menu->current = cur->next;

		// if the current item is beyond the page start, move the page start one page down
		cur = menu->pagestart;
		int itms = 15 + 1;
		while (--itms && cur)
			if (menu->current == cur)
				return;
			else
				cur = cur->next;

		menu->pagestart = menu->current;
	}
}

static void menuNavUp(void)
{
	menu_list_t *cur = menu->current;

	if (cur && cur->prev) {
		menu->current = cur->prev;

		// if the current item is on the page start, move the page start one page up
		if (menu->pagestart == cur) {
			int itms = 15 + 1; // +1 because the selection will move as well
			while (--itms && menu->pagestart->prev)
				menu->pagestart = menu->pagestart->prev;
		}
	}
}

void menuHandleInput(void)
{
	//check to see if the pad is still connected
	checkPadConnected();
	//read pad 0
	buttonStatts(0, 0);

	if(new_pad & PAD_SQUARE) {
		if (strcmp(action,"CHECK") == 0) {
			strcpy(action, "DOWNLOAD");
		} else if (strcmp(action,"DOWNLOAD") == 0) {
			strcpy(action, "LAUNCH");
		} else if (strcmp(action,"LAUNCH") == 0) {
			strcpy(action, "CHECK");
		}
	}

	if (new_pad & PAD_UP)
		menuNavUp();

	if (new_pad & PAD_DOWN)
		menuNavDown();

	if(new_pad & PAD_CIRCLE) {
		int id = menu->current->item.id;
		strcpy(fn, downloadableApps[id].elfName);

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
	}

	if(new_pad & PAD_TRIANGLE) {
		if (strcmp(device,"mc0:/") == 0) {
			strcpy(device,"mc1:/");
		} else if (strcmp(device,"mc1:/") == 0) {
			strcpy(device,"mass:/");
		} else {
			strcpy(device,"mc0:/");
		}
	}

	if (new_pad & PAD_R1)
		deinit(1);

	if (new_pad & PAD_L1) {
		if (http_mirror == 0) {
			http_mirror = 1;
		} else if (http_mirror == 1) {
		 	http_mirror = 0;
		}
	}

	if (new_pad & PAD_CROSS) {
		int id = menu->current->item.id;
		strcpy(fn, downloadableApps[id].elfName);

		if (strcmp(action,"CHECK") == 0) {
			DoTask(1);
		} else if (strcmp(action,"DOWNLOAD") == 0) {
			DoTask(2);
		} else if (strcmp(action,"LAUNCH") == 0) {
			DoTask(3);
		}
	}
}
