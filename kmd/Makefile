SIZE?=0x10000
STACK_START!=printf "0x%08x" $$(($(SIZE) + $(SIZE)))
MARCH?=rv32im
MABI?=ilp32
XLEN?=32
PROGRAM?=firmware



FIRMWARE_DIR?=$(PWD)
LIB_FILES?=
FIRMWARE_FILES?= ${FIRMWARE_DIR}/firmware/scheduler.c ${FIRMWARE_DIR}/firmware/engine.c ${FIRMWARE_DIR}/firmware/bdma.c ${FIRMWARE_DIR}/firmware/conv.c ${FIRMWARE_DIR}/firmware/sdp.c ${FIRMWARE_DIR}/firmware/cdp.c ${FIRMWARE_DIR}/firmware/pdp.c ${FIRMWARE_DIR}/firmware/rubik.c ${FIRMWARE_DIR}/firmware/cache.c ${FIRMWARE_DIR}/firmware/common.c ${FIRMWARE_DIR}/firmware/engine_data.c ${FIRMWARE_DIR}/firmware/engine_isr.c ${FIRMWARE_DIR}/firmware/engine_debug.c ${FIRMWARE_DIR}/firmware/nvdla_core_callbacks.c
INCLUDE_FILES?=-I${FIRMWARE_DIR}/firmware/ -I$(FIRMWARE_DIR)/firmware/include/ -I${FIRMWARE_DIR}/lib/ -I${FIRMWARE_DIR}/lib/test/
DEFINITIONS?=-DMAIN=0 -DATOMIC=0 -DTRAP=0 -DIRQ0=0 -DDLA_REG_TEST=1 -DMEM_TEST=1 -DTASK_TEST=1 -DNVDLA_INFO_ON=0 -DDEBUG_NETWORK_DATA=0 -DFPGA_LOG=0 -DSIM_LOG=1 -DQEMU_LOG=0
LINKING_FLAG := -lpthread -ldl -lrt -lpthread -L/lib64

RV_FLAGS:=-march=$(MARCH) -mabi=$(MABI) -fno-pic -flto -g --specs=nosys.specs -nostartfiles -Werror -Wl,--relax -Wl,--gc-sections $(INCLUDE_FILES) $(DEFINITIONS)

ifndef SWERV_CCM
	RV_FLAGS+= -T rv_$(SIZE).ld startup_$(SIZE)1.s trap.S
else
	RV_FLAGS+= -T rv_swerv.ld startup_swerv.s trap.S
endif

TOP:=`echo $(STACK_START) | cut -c3-7`
BOTTOM:=`echo $(STACK_START) | cut -c8-10`

program: setup
	riscv32-unknown-elf-gcc $(RV_FLAGS) $(FIRMWARE_DIR)/firmware/main.c -o firmware.elf $(LIB_FILES) $(FIRMWARE_FILES)
	riscv32-unknown-elf-objcopy -O binary firmware.elf firmware.bin
	mv startup_$(SIZE).s out/
	mv startup_$(SIZE)1.s out/
	mv rv_$(SIZE).ld out/
	mv firmware.bin out/
	mv firmware.elf out/

setup:
	mkdir -p out
	sed s/TOP/$(TOP)/ startup.s > startup_$(SIZE).s
	sed s/BOTTOM/$(BOTTOM)/ startup_$(SIZE).s > startup_$(SIZE)1.s
	sed s/SIZE/$(SIZE)/ rv.ld > rv_$(SIZE).ld

clean:
	rm -rf out/
