SIZE?=0x40000
STACK_START!=printf "0x%08x" $$(($(SIZE) + $(SIZE)))
MARCH?=rv32im
MABI?=ilp32
XLEN?=32
PROGRAM?=firmware



DUMMY_DIR?=$(PWD)
LIB_FILES?=
FIRMWARE_FILES?= ${DUMMY_DIR}/firmware/scheduler.c ${DUMMY_DIR}/firmware/engine.c ${DUMMY_DIR}/firmware/bdma.c ${DUMMY_DIR}/firmware/conv.c ${DUMMY_DIR}/firmware/sdp.c ${DUMMY_DIR}/firmware/cdp.c ${DUMMY_DIR}/firmware/pdp.c ${DUMMY_DIR}/firmware/rubik.c ${DUMMY_DIR}/firmware/cache.c ${DUMMY_DIR}/firmware/common.c ${DUMMY_DIR}/firmware/engine_data.c ${DUMMY_DIR}/firmware/engine_isr.c ${DUMMY_DIR}/firmware/engine_debug.c ${DUMMY_DIR}/firmware/nvdla_core_callbacks.c
INCLUDE_FILES?=-I${DUMMY_DIR}/firmware/ -I$(DUMMY_DIR)/firmware/include/ -I${DUMMY_DIR}/lib/ -I${DUMMY_DIR}/lib/test/
DEFINITIONS?=-DMAIN=0 -DATOMIC=0 -DTRAP=0 -DIRQ0=0 -DDLA_REG_TEST=1 -DMEM_TEST=1 -DTASK_TEST=1 -DNVDLA_INFO_ON=0 -DDEBUG_NETWORK_DATA=0 -DFPGA_LOG=0 -DSIM_LOG=1 -DQEMU_LOG=0
LINKING_FLAG := -ltapasco -lpthread -ldl -lrt -lpthread -L${DUMMY_DIR}/libtapasco -L/lib64

#RV_FLAGS:=-march=$(MARCH) -mabi=$(MABI) -fno-pic -Os -g --specs=nano.specs -nostartfiles -Werror -Wl,--relax -Wl,--gc-sections -mcmodel=medany $(INCLUDE_FILES) $(DEFINITIONS)
RV_FLAGS:=-march=$(MARCH) -mabi=$(MABI) -fno-pic -flto -g --specs=nosys.specs -nostartfiles -I$(DUMMY_DIR)/tapasco -Werror -Wl,--relax -Wl,--gc-sections $(INCLUDE_FILES) $(DEFINITIONS)

ifndef SWERV_CCM
	RV_FLAGS+= -T rv_$(SIZE).ld startup_$(SIZE)1.s trap.S
else
	RV_FLAGS+= -T rv_swerv.ld startup_swerv.s trap.S
endif

TOP:=`echo $(STACK_START) | cut -c3-7`
BOTTOM:=`echo $(STACK_START) | cut -c8-10`

program: setup
	riscv32-unknown-elf-gcc $(RV_FLAGS) $(DUMMY_DIR)/firmware/main.c -o firmware.elf $(LIB_FILES) $(FIRMWARE_FILES)
	      riscv32-unknown-elf-objcopy -O binary firmware.elf firmware.bin

setup:
	mkdir -p elf bin
	sed s/TOP/$(TOP)/ startup.s > startup_$(SIZE).s
	sed s/BOTTOM/$(BOTTOM)/ startup_$(SIZE).s > startup_$(SIZE)1.s
	sed s/SIZE/$(SIZE)/ rv.ld > rv_$(SIZE).ld


