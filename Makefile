VERSION = 0.21
NAME = VTSPS2-HBDL
EE_BIN = $(NAME).elf
EE_BIN_PACKED = $(NAME)-packed.elf
EE_BIN_STRIPPED = $(NAME)-stripped.elf
####
# C File Objects
EE_OBJS = main.o loader_elf.o ps2ipc.o
# SW Module Objects
EE_OBJS += freesio2.o iomanX.o fileXio.o freepad.o mcman.o mcsrv.o
# Network Module
EE_OBJS += ps2dev9.o ps2ip-nm.o ps2ips.o netman.o smap.o ps2http.o crc32.o
# Other IRX
EE_OBJS += poweroff.o
# SBV Shit
EE_INCS = -I$(PS2SDK)/ports/include -I$(PS2SDK)/sbv/include
EE_LDFLAGS = -L$(PS2SDK)/sbv/lib 
####
EE_LIBS = -lpadx -lmtap -ldebug -lmc -lc -lpatches -ldebug -lkernel -lpoweroff -lnetman -lps2ips -lfileXio


all:
	@echo "======================================="
	@echo "=== Building $(NAME) v$(VERSION) ==="
	@echo "======================================="
	$(MAKE) $(EE_BIN_PACKED)

$(EE_BIN_STRIPPED): $(EE_BIN)
	@echo "================="
	@echo "=== Stripping ==="
	@echo "================="
	$(EE_STRIP) -o $@ $<
	
$(EE_BIN_PACKED): $(EE_BIN_STRIPPED)
# Uncomment to compress ELF. Adjust path to match your environment
	@echo "==================="
	@echo "=== Compressing ==="
	@echo "==================="
	~/ps2homebrew/ps2-packer/ps2-packer -v $< $@
	rm -f *.o *.s

clean:
	rm -f *.elf *.o *.s loader/*.o loader/*.elf


#wLaunchELF's loader.elf

loader/loader.elf: loader
	$(MAKE) -C $<

loader_elf.s: loader/loader.elf
	bin2s $< $@ loader_elf

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

crc32: crc32.c crc.h
	$(CC) $(CFLAGS) -o $@ $<
	
include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
