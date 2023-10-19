TOOL_CHAIN_BASE=/home/orlando/Desktop/toolchain-mips_24kc_gcc-7.3.0_musl

GCC=$(TOOL_CHAIN_BASE)/bin/mips-openwrt-linux-gcc
GPP=$(TOOL_CHAIN_BASE)/bin/mips-openwrt-linux-g++

LIB=$(TOOL_CHAIN_BASE)/lib/gcc/mips-openwrt-linux-musl/7.3.0
INCLUDE_DIR=$(TOOL_CHAIN_BASE)/include


build:
	$(GPP) -I$(INCLUDE_DIR) -L$(LIB) -o recvCSI csi_fun.c main.c

clean:
	rm csi_loop_ping