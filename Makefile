VERSION = 0.32-GUI
NAME = VTSPS2-HBDL
EE_BIN = $(NAME).elf
EE_BIN_PACKED = $(NAME)-GUI.elf
EE_BIN_STRIPPED = $(NAME)-stripped.elf

####
# C File Objects
EE_OBJS = $(NAME).o ps2ipc.o gui.o menu.o pad.o textures.o background_png.o background2_png.o logo_png.o
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
EE_LIBS = -lc -ldebug -lpatches -Xlinker --start-group $(EE_LIBS_EXTRA) -lpadx -lmtap -lmc -lkernel -lpoweroff -lnetman -lps2ips -lfileXio -laudsrv -lelf-loader
EE_LIBS += -lgskit_toolkit -lgskit -ldmakit -L$(PS2SDK)/ports/lib -lpng -ljpeg -lz -Xlinker --end-group

EE_INCS += -I$(GSKIT)/include -I$(GSKIT)/ee/dma/include -I$(GSKIT)/ee/gs/include -I$(GSKIT)/ee/toolkit/include
# linker flags
EE_LIB_DIRS += -L$(GSKIT)/lib
EE_LIB_DIRS += -L$(PS2SDK)/ee/lib
EE_LDFLAGS += $(EE_LIB_DIRS)

all:
	@echo "======================================="
	@echo "=== Building $(NAME) v$(VERSION) ==="
	@echo "======================================="
	$(MAKE) $(EE_BIN_PACKED)

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

background_png.s: gfx/background.png
	bin2s $< $@ background_png

background2_png.s: gfx/background2.png
	bin2s $< $@ background2_png

logo_png.s: gfx/logo.png
	bin2s $< $@ logo_png

crc32.o: crc32.c checksum.h
	ee-gcc -c $< -o $@

misc.o: misc.c
	ee-gcc $(EE_INCS) -c $< -o $@

VTSPS2-CRC32.o: VTSPS2-CRC32.c VTSPS2-HBDL.h
	ee-gcc $(EE_INCS) -c $< -o $@

run: $(EE_BIN)
	ps2client execee host:$(EE_BIN)

reset:
	ps2client reset

$(EE_BIN_STRIPPED): $(EE_BIN)
	$(EE_STRIP) -o $@ $<

$(EE_BIN_PACKED): $(EE_BIN_STRIPPED)
	ps2-packer $< $@ > /dev/null

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
