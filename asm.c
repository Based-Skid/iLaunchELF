#include "main.h"


void PasteASM()
{
	DI();
	ee_kmode_enter();
	
	// Paste Compiled ASM Below

	*(u32*)0x000A0000 = 0x11111111; // "Write Access to User Memory Test"
	
	ee_kmode_exit();
	EI();
	scr_printf("Offline Patch Loaded Successfully!.\n");
}
