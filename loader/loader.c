//--------------------------------------------------------------
//File name:    loader.c
//--------------------------------------------------------------
//dlanor: This subprogram has been modified to minimize the code
//dlanor: size of the resident loader portion. Some of the parts
//dlanor: that were moved into the main program include loading
//dlanor: of all IRXs and mounting pfs0: for ELFs on hdd.
//dlanor: Another change was to skip threading in favor of ExecPS2
/*==================================================================
==											==
==	Copyright(c)2004  Adam Metcalf(gamblore_@hotmail.com)		==
==	Copyright(c)2004  Thomas Hawcroft(t0mb0la@yahoo.com)		==
==	This file is subject to terms and conditions shown in the	==
==	file LICENSE which should be kept in the top folder of	==
==	this distribution.							==
==											==
==	Portions of this code taken from PS2Link:				==
==				pkoLoadElf						==
==				wipeUserMemory					==
==				(C) 2003 Tord Lindstrom (pukko@home.se)	==
==				(C) 2003 adresd (adresd_ps2dev@yahoo.com)	==
==	Portions of this code taken from Independence MC exploit	==
==				tLoadElf						==
==				LoadAndRunHDDElf					==
==				(C) 2003 Marcus Brown <mrbrown@0xd6.org>	==
==											==
==================================================================*/
#include "tamtypes.h"
#include "debug.h"
#include "kernel.h"
#include "iopcontrol.h"
#include "sifrpc.h"
#include "loadfile.h"
#include "string.h"
#include "iopheap.h"
#include "errno.h"
//--------------------------------------------------------------
#define GS_BGCOLOR *((volatile unsigned long int*)0x120000E0)

//--------------------------------------------------------------
//End of data declarations
//--------------------------------------------------------------
//Start of function code:
//--------------------------------------------------------------
// Clear user memory
// PS2Link (C) 2003 Tord Lindstrom (pukko@home.se)
//         (C) 2003 adresd (adresd_ps2dev@yahoo.com)
//--------------------------------------------------------------
static void wipeUserMem(void)
{
    int i;
    for (i = 0x100000; i < GetMemorySize(); i += 64) {
        asm volatile(
            "\tsq $0, 0(%0) \n"
            "\tsq $0, 16(%0) \n"
            "\tsq $0, 32(%0) \n"
            "\tsq $0, 48(%0) \n" ::"r"(i));
    }
}

//--------------------------------------------------------------
//End of func:  void wipeUserMem(void)
//--------------------------------------------------------------
// *** MAIN ***
//--------------------------------------------------------------
void main(int argc, char *argv[])
{
    static t_ExecData elfdata;
    int ret = 0;
    int args;
    // Initialize
    SifInitRpc(0);
    wipeUserMem();
    if(argc == 0) { // Fatal, dunno wtf to load
		GS_BGCOLOR = 0xFF0000;
		asm volatile("break\n");
	} else if(argc == 1) { // Missing binary type arg, loading target as ELF. Launch arg will be ELF URI.
		ret = SifLoadElf(argv[0], &elfdata);
	} else {
		ret = SifLoadElf(argv[0], &elfdata);
		argc -= 1;
		for(args = 0; args < argc; args++) argv[args] = argv[args+1];
	}
    if (ret == 0) {
        SifExitRpc();

        FlushCache(0);
        FlushCache(2);

        ExecPS2((void *)elfdata.epc, (void *)elfdata.gp, argc, argv);
        asm volatile("break\n");
    } else {
		GS_BGCOLOR = 0xFFFF00;
		asm volatile("break\n");
    }
}
//--------------------------------------------------------------
//End of func:  int main(int argc, char *argv[])
//--------------------------------------------------------------
//End of file:  loader.c
//--------------------------------------------------------------
