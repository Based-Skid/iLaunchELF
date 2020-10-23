VERSION = 0.32
NAME = VTSPS2-HBDL
EE_BIN = $(NAME).elf
EE_BIN_PACKED = $(NAME)-packed.elf
EE_BIN_STRIPPED = $(NAME)-stripped.elf
GSKITSRC = /homebrew/gsKit

include $(GSKITSRC)/ee/Rules.make

LIBGSKIT = $(GSKIT)/lib/libgskit.a
LIBDMAKIT = $(GSKIT)/lib/libdmakit.a
LIBGSKIT_TOOLKIT = $(GSKIT)/lib/libgskit_toolkit.a

####
# C File Objects
EE_OBJS = $(NAME).o ps2ipc.o
# SW Module Objects
EE_OBJS += freesio2.o iomanX.o freepad.o mcman.o mcsrv.o
# Network Module
EE_OBJS += ps2dev9.o ps2ip-nm.o ps2ips.o netman.o smap.o ps2http.o
# Other IRX
EE_OBJS += poweroff.o usbd.o usbhdfsd.o misc.o crc32.o VTSPS2-CRC32.o
# SBV Shit
EE_INCS = -I$(PS2SDK)/ports/include -I$(PS2SDK)/sbv/include
EE_LDFLAGS = -L$(PS2SDK)/sbv/lib 
####
EE_LIBS = -lc -ldebug -lpatches -Xlinker --start-group $(EE_LIBS_EXTRA) -lpadx -lmtap -lmc -lkernel -lpoweroff -lnetman -lps2ips -lfileXio
EE_LIBS += -lgskit_toolkit -lgskit -ldmakit -Xlinker --end-group

EE_INCS += -I$(GSKITSRC)/ee/gs/include -I$(GSKITSRC)/ee/dma/include
EE_INCS += -I$(GSKITSRC)/ee/toolkit/include
# linker flags
EE_LIB_DIRS += -L$(GSKIT)/lib
EE_LIB_DIRS += -L$(PS2SDK)/ee/lib
EE_LDFLAGS += $(EE_LIB_DIRS)



all:
	@echo "======================================="
	@echo "=== Building $(NAME) v$(VERSION) ==="
	@echo "======================================="
	$(MAKE) $(EE_BIN)

clean:
	rm -f *.elf *.o *.s


#poweroff Module

poweroff.s:
	bin2s $(PS2SDK)/iop/irx/poweroff.irx poweroff.s poweroff

#IRX Modules

freesio2.s:
	bin2s $(PS2SDK)/iop/irx/freesio2.irx freesio2.s freesio2
	
iomanX.s:
	bin2s $(PS2SDK)/iop/irx/iomanX.irx iomanX.s iomanX
	
fileXio.s:
	bin2s $(PS2SDK)/iop/irx/fileXio.irx fileXio.s fileXio

freepad.s:
	bin2s $(PS2SDK)/iop/irx/freepad.irx freepad.s freepad

mcman.s:
	bin2s $(PS2SDK)/iop/irx/mcman.irx mcman.s mcman

mcsrv.s:
	bin2s $(PS2SDK)/iop/irx/mcserv.irx mcsrv.s mcserv
	
ps2dev9.s:
	bin2s $(PS2SDK)/iop/irx/ps2dev9.irx ps2dev9.s ps2dev9
	
ps2ip-nm.s:
	bin2s $(PS2SDK)/iop/irx/ps2ip-nm.irx ps2ip-nm.s ps2ipnm
	
ps2ips.s:
	bin2s $(PS2SDK)/iop/irx/ps2ips.irx ps2ips.s ps2ips

netman.s:
	bin2s $(PS2SDK)/iop/irx/netman.irx netman.s netman

smap.s:
	bin2s $(PS2SDK)/iop/irx/smap.irx smap.s smap

ps2http.s:
	bin2s $(PS2SDK)/iop/irx/ps2http.irx ps2http.s ps2http

#thx KrahJohlito
usbd.s:
	bin2s $(PS2SDK)/iop/irx/usbd.irx usbd.s usbd
	
usbhdfsd.s:
	bin2s $(PS2SDK)/iop/irx/usbhdfsd.irx usbhdfsd.s usbhdfsd

crc32.o: crc32.c checksum.h
	ee-gcc -c $< -o $@
	
misc.o: misc.c
	ee-gcc $(EE_INCS) -c $< -o $@
	
VTSPS2-CRC32.o: VTSPS2-CRC32.c VTSPS2-HBDL.h
	ee-gcc $(EE_INCS) -c $< -o $@
	
include $(PS2SDK)/samples/Makefile.pref
include Makefile.eeglobal
