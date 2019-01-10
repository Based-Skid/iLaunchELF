EE_BIN = httpBrew.elf
EE_BIN_PACKED = http-elf-ldr-packed.elf
####
# C File Objects
EE_OBJS = main.o loader_elf.o asm.o ps2ipc.o
# SW Module Objects
EE_OBJS += freesio2.o iomanX.o fileXio.o freepad.o mcman.o mcsrv.o
# Network Module
EE_OBJS += ps2dev9.o ps2ip-nm.o ps2ips.o netman.o smap.o ps2http.o
# Other IRX
EE_OBJS += poweroff.o
# SBV Shit
EE_INCS = -I$(PS2SDK)/ports/include -I$(PS2SDK)/sbv/include
EE_LDFLAGS = -L$(PS2SDK)/sbv/lib 
####
EE_LIBS = -lpadx -lmtap -ldebug -lmc -lc -lpatches -ldebug -lkernel -lpoweroff -lnetman -lps2ips


all: $(EE_BIN)
	rm -rf *.o *.s

# Un Comment to Enable Compression of the ELF. you will need ps2packer in the project dir

# all: $(EE_BIN)
	# ps2_packer/ps2_packer -p zlib $(EE_BIN) $(EE_BIN_PACKED)
	# cp -f --remove-destination $(EE_BIN_PACKED) $(EE_BIN_DIR)/$(EE_BIN)
	# rm -rf *.o *.s


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

	
include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
