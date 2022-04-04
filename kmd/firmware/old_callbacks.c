/*
 * Copyright (c) 2017-2018, NVIDIA CORPORATION. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation; or, when distributed
 * separately from the Linux kernel or incorporated into other
 * software packages, subject to the following license:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of NVIDIA CORPORATION nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdarg.h>

#include <nvdla_interface.h>
#include <nvdla_riscv.h>
#include <string.h>
//#include "femto.h"

uint32_t dla_irq_notifier = 0;

uint32_t task_notifier = 0;
static void irq0_handler(void)
{
  uint32_t check;
    //check = glb_reg_read(S_INTR_STATUS);
    //debug(IRQ0, "irq0_handler.");
    if (check)
    {
      // debug(IRQ0, "new task.");
      //atomic_set(task_notifier,(*task_notifier)+1);
      //(*(volatile typeof(*(&task_notifier)) *)(&task_notifier) = (*&task_notifier) + 1);
      //notify_dla_irq(&task_notifier);
      //glb_reg_write(S_INTR_STATUS, check);

    }
    else
    {
      // debug(IRQ0, "dla intr.");
        //nvdla_engine_isr(0, &get_nvdla_dev());
    }
}

static struct simple_device *nvdla_dev;
static struct nvdla_submit_task *local_task;

void* get_nvdla_dev(void)
{
    return &nvdla_dev;
}

void* get_local_task(void)
{
    return &local_task;
}

void update_submit_task_params(void)
{
    //struct nvdla_submit_task *local_task;
    //uint32_t addrl;
    //asm volatile("lw %0, 0(%1)" : "=r" (addrl) : "r" (0xf8000064));
    //uint32_t addrh;
    //asm volatile("lw %0, 0(%1)" : "=r" (addrh) : "r" (0xf8000068));
    //uint32_t addr = (addrh << 16) + addrl;
    //local_task = get_local_task();
    //memcpy(local_task, (void *)(uint32_t)addr, sizeof(struct nvdla_submit_task));
}

static struct nvdla_config nvdla_config_os_initial = {
	.atom_size = 32,
	.bdma_enable = true,
	.rubik_enable = true,
	.weight_compress_support = true,
};

static struct nvdla_config nvdla_config_small = {
	.atom_size = 8,
	.bdma_enable = false,
	.rubik_enable = false,
	.weight_compress_support = false,
};

void dla_debug(const char *str, ...)
{
#if (NVDLA_INFO_ON==1)
	va_list args;
	va_start(args, str);
	vprintf(str, args);
	va_end(args);
#else
#endif
}

void dla_info(const char *str, ...)
{
#if (NVDLA_INFO_ON==1)
	va_list args;
	va_start(args, str);
	vprintf(str, args);
	va_end(args);
#else
#endif
}

void dla_warn(const char *str, ...)
{
#if (NVDLA_INFO_ON==1)
	va_list args;
	va_start(args, str);
	vprintf(str, args);
	va_end(args);
#else
#endif
}

void dla_error(const char *str, ...)
{
#if (NVDLA_INFO_ON==1)
	va_list args;
	va_start(args, str);
	vprintf(str, args);
	va_end(args);
#else
#endif
}

void *dla_memset(void *src, int ch, uint32_t len)
{
	return memset(src, ch, len);
}

void *dla_memcpy(void *dest, const void *src, uint32_t len)
{
	return memcpy(dest, src, len);
}

int32_t dla_get_time_us(void)
{
    return 0;
}

// DLA has normal memory master(DMA) which could be conflict with cpu
// e.g. 
// cpu write data to normal memory
// data actually resides in cache
// cpu writel DLA regs to start  <----fence w,o should be added so that data flushed before DLA start
// DLA read data from normal memory
void dla_reg_write(void *driver_context, uint32_t addr, uint32_t reg)
{
	struct nvdla_device *nvdla_dev =
			(struct nvdla_device *)driver_context;

	if (!nvdla_dev)
		return;
    //dla_isr_handler(nvdla_dev->engine_context);
    volatile uint8_t rega;
    volatile uint8_t regb;
    volatile uint8_t regc;
    volatile uint8_t regd;
    rega = (((uintptr_t)reg) & ((uintptr_t)0xFF000000))>>24;
    regb = (((uintptr_t)reg) & ((uintptr_t)0x00FF0000))>>16;
    regc = (((uintptr_t)reg) & ((uintptr_t)0x0000FF00))>>8;
    regd = (((uintptr_t)reg) & ((uintptr_t)0x000000FF));

    volatile uint32_t *read_int = (uint32_t *)(nvdla_dev->base +addr);
    volatile uint32_t ri;
    ri = read_int[0];
    asm volatile("sw %0, 0(%1)" :: "r"(reg), "r"(nvdla_dev->base + addr));
    ri = read_int[0];
    volatile uint32_t *base_add = (uint32_t *)nvdla_dev->base;
    volatile uint32_t *intr_mask = (void *)(uint32_t)(nvdla_dev->base + 0x1004);
    volatile uint32_t *intr_set = (void *)(uint32_t)(nvdla_dev->base + 0x1008);
    volatile uint32_t *intr_status = (void *)(uint32_t)(nvdla_dev->base + 0x100c);
    volatile uint32_t ia;
    volatile uint32_t ib;
    volatile uint32_t ic;
    ia = intr_mask[0];
    ib = intr_set[0];
    ic = intr_status[0];
    if (ic!=0 && dla_irq_notifier==0)
    {
        dla_irq_notifier = 1;
        dla_isr_handler(nvdla_dev->engine_context);
        dla_irq_notifier = 0;
    }

    memcpy((int *)(nvdla_dev->base + addr), &reg, sizeof(reg));
    volatile uint32_t check;
    asm volatile("lw %0, 0(%1)" : "=r" (check) : "r" (nvdla_dev->base + addr));

}

uint32_t dla_reg_read(void *driver_context, uint32_t addr)
{
  uint32_t ret;
  uint32_t ter;
	struct nvdla_device *nvdla_dev =
			(struct nvdla_device *)driver_context;

	if (!nvdla_dev)
		return 0;
    //ret = (uint32_t *)(nvdla_dev->base + addr);
    //dla_isr_handler(nvdla_dev->engine_context);
    memcpy(&ret, (void *)(uint32_t)(nvdla_dev->base + addr), sizeof(uint32_t));
    volatile uint8_t *read_byte = (uint8_t *)(nvdla_dev->base +addr);
    volatile uint32_t *read_int = (uint32_t *)(nvdla_dev->base +addr);
    volatile uint32_t *cdma = (uint32_t *)(nvdla_dev->base +0x5000);
    volatile uint32_t *csc = (uint32_t *)(nvdla_dev->base +0x6000);
    volatile uint32_t *cmaca = (uint32_t *)(nvdla_dev->base +0x7000);
    volatile uint32_t *cmacb = (uint32_t *)(nvdla_dev->base +0x8000);
    volatile uint32_t *cacc = (uint32_t *)(nvdla_dev->base +0x9000);
    volatile uint32_t *sdp = (uint32_t *)(nvdla_dev->base +0xb000);
    volatile uint32_t *pdp = (uint32_t *)(nvdla_dev->base +0xd000);
    volatile uint32_t *cdp = (uint32_t *)(nvdla_dev->base +0xf000);
    volatile uint32_t *rubik = (uint32_t *)(nvdla_dev->base +0x10000);

    volatile uint8_t ra;
    volatile uint8_t rb;
    volatile uint8_t rc;
    volatile uint8_t rd;
    volatile uint32_t ri;
    volatile uint32_t rcdma;
    volatile uint32_t rcsc;
    volatile uint32_t rcmaca;
    volatile uint32_t rcmacb;
    volatile uint32_t rcacc;
    volatile uint32_t rsdp;
    volatile uint32_t rpdp;
    volatile uint32_t rcdp;
    volatile uint32_t rrubik;

    ri = read_int[0];
    ra = read_byte[0];
    rb = read_byte[1];
    rc = read_byte[2];
    rd = read_byte[3];
    rcdma = cdma[0];
    rcsc = csc[0];
    rcmaca = cmaca[0];
    rcmacb = cmacb[0];
    rcacc = cacc[0];
    rsdp = sdp[0];
    rpdp = pdp[0];
    rcdp = cdp[0];
    rrubik = rubik[0];

    volatile uint32_t *base_add = (uint32_t *)nvdla_dev->base;
    volatile uint32_t *intr_mask = (void *)(uint32_t)(nvdla_dev->base + 0x1004);
    volatile uint32_t *intr_set = (void *)(uint32_t)(nvdla_dev->base + 0x1008);
    volatile uint32_t *intr_status = (void *)(uint32_t)(nvdla_dev->base + 0x100c);
    volatile uint32_t ia;
    volatile uint32_t ib;
    volatile uint32_t ic;
    volatile uint32_t id;
    ia = intr_mask[0];
    ib = intr_set[0];
    ic = intr_status[0];
    id = base_add[0];
    if (ic!=0 && dla_irq_notifier==0)
    {
        dla_irq_notifier=1;
        dla_isr_handler(nvdla_dev->engine_context);
        dla_irq_notifier=0;
    }

    asm volatile("lw %0, 0(%1)" : "=r" (ret) : "r" (nvdla_dev->base + addr));
    //ter = ((ret>>12)&0xf) | ((ret<<4)&0xf00) | ((ret>>4)&0xf0) | ((ret<<12)&0xf000);
    ter = ((((uint32_t)ra)<<24) | (((uint32_t)rb)<<16) | (((uint32_t)rc)<<8) | (((uint32_t)rd)));
    return (uint32_t)ret;
}

void  nvdla_engine_isr(int32_t irq, void *data)
{
	unsigned long flags;
	struct simple_device *nvdla_dev = (struct simple_device *)data;

	if (!nvdla_dev)
		return;

	dla_isr_handler(nvdla_dev->engine_context);

    //atomic_set(dla_irq_notifier,(*dla_irq_notifier)+1);
	//(*(volatile typeof(*(&dla_irq_notifier)) *)(&dla_irq_notifier) = (*&dla_irq_notifier) + 1);
    //notify_dla_irq(&dla_irq_notifier);

    return;
}


static int32_t dla_read_dma_address(void *driver_context, void *task_data,
						int16_t index, void *dst)
{
	int32_t ret = 0;
	struct simple_mem_handle *handles;
	uint32_t *phys_addr = (uint32_t *)(dst);
	struct simple_task *task = (struct simple_task *)task_data;

	if (index == -1 || index > task->num_addresses)
		return -1;

    handles = task->address_list;

    ////copy address at index from address_list to dst
    ////next line is identical
    //*phys_addr = handles[index].paddr;
    memcpy(dst, &handles[index].paddr, sizeof(struct simple_mem_handle));

	return ret;
}

static int32_t dla_read_cpu_address(void *driver_context, void *task_data,
						int16_t index, void *dst)
{
	uint32_t *temp = (uint32_t *)dst;
	struct nvdla_task *task = (struct nvdla_task *)task_data;

	if (index == -1 || index > task->num_addresses)
		return -1;
    ////writes index of desired obeject at address_list to dst and checks if it is valid
	*temp = (uint32_t)index;
	return 0;
}

int32_t dla_get_dma_address(void *driver_context, void *task_data,
					int16_t index, void *dst_ptr,
					uint32_t destination)
{
	int32_t ret = 0;

    dla_debug("dla_get_dma_address.\n");

	if (destination == DESTINATION_PROCESSOR) {
		ret = dla_read_cpu_address(driver_context, task_data,
						index, dst_ptr);
	} else if (destination == DESTINATION_DMA) {
		ret = dla_read_dma_address(driver_context, task_data,
						index, dst_ptr);
	} else {
		ret = -1;
	}

	return ret;
}

int32_t dla_data_write(void *driver_context, void *task_data,
				void *src, uint32_t dst,
				uint32_t size, uint32_t offset)
{
	int32_t ret;
	struct nvdla_mem_handle *handles;
	struct nvdla_task *task = (struct nvdla_task *)task_data;

    handles = task->address_list;


    //TODO: confirm data actually flushed, be aware if smmu exists
	//memcpy((void *)(uint32_t)(handles[dst].paddr + handles[dst].offset + offset), src, size);
    ////copy data from DCCM to tapasco-memory
    ////dst-address comes from address_list
    memcpy((void *)(uint32_t)(handles[dst].paddr + offset), src, size);
    asm volatile ("fence" ::: "memory");

	return ret;
}

int32_t dla_data_read(void *driver_context, void *task_data,
				uint32_t src, void *dst,
				uint32_t size, uint32_t offset)
{
	int32_t ret;
	struct simple_mem_handle *handles;
	struct simple_task *task = (struct simple_task *)task_data;

    handles = task->address_list;

    //TODO: confirm data actually flushed, be aware if smmu exists
    ////copy data from tapasco-memory to DCCM
    ////src-address comes from address_list
	//memcpy(dst, (void *)(uint32_t)(handles[src].paddr + handles[src].offset + offset), size);
    //memcpy(dst, (void *)(uint32_t)(handles[src].paddr + offset), size);
    memcpy(dst, (void *)(uint32_t)(src + offset), size);

	return 0;
}


uint32_t nvdla_task_submit(int list_start,int address_count)
{
	int32_t err = 0;
	uint32_t task_complete = 0;
    struct simple_device *nvdla_dev = get_nvdla_dev();
    struct simple_task *task = malloc(sizeof(struct simple_task));

    //volatile unsigned int address = 0x800f0e80;
    volatile unsigned int address = list_start;
    uint32_t task_count = 1;
    volatile uint8_t *read_byte = (uint8_t *)address;
    volatile uint8_t ra;
    volatile uint8_t rb;
    volatile uint8_t rc;
    volatile uint8_t rd;
    //struct nvdla_mem_handle mem_list[address_count];
    struct simple_mem_handle *mem_handles = malloc(address_count * sizeof(struct simple_mem_handle));
    struct nvdla_mem_handle *fuze_paddr = malloc(sizeof(struct nvdla_mem_handle));
    volatile uint32_t *mem_address = (uint32_t *)mem_handles;
    volatile uint32_t *write_byte = (uint32_t *)mem_address;
    uint32_t i;
    //for (i = 0; i < task_count; i++) {
        uint32_t j;

        for (j = 0; j < address_count; j++) {
            read_byte = (uint8_t *)address;
            write_byte = (uint32_t *)mem_address;
            ra = read_byte[0];
            rb = read_byte[1];
            rc = read_byte[2];
            rd = read_byte[3];
            //mem_list[j].paddr = ((((uint32_t)ra)<<24) | (((uint32_t)rb)<<16) | (((uint32_t)rc)<<8) | (((uint32_t)rd)));
            fuze_paddr->paddr = ((((uint32_t)ra)<<24) | (((uint32_t)rb)<<16) | (((uint32_t)rc)<<8) | (((uint32_t)rd)));
            address += 0x00000004;
            read_byte = (uint8_t *)address;
            ra = read_byte[0];
            rb = read_byte[1];
            rc = read_byte[2];
            rd = read_byte[3];
            //mem_list[j].offset = ((((uint32_t)ra)<<24) | (((uint32_t)rb)<<16) | (((uint32_t)rc)<<8) | (((uint32_t)rd)));
            fuze_paddr->offset = ((((uint32_t)ra)<<24) | (((uint32_t)rb)<<16) | (((uint32_t)rc)<<8) | (((uint32_t)rd)));
            address += 0x00000004;
            //write_byte[0] = fuze_paddr->paddr + fuze_paddr->offset;
            mem_address[0] = fuze_paddr->paddr + fuze_paddr->offset;
            //mem_handles[j]->paddr = (fuze_paddr->paddr + fuze_paddr->offset);
           mem_address += 0x00000001;
        }

    task->num_addresses = address_count;
    task->nvdla_dev = nvdla_dev;
    task->address_list = mem_handles;
    nvdla_dev->task = task;
    task->nvdla_dev = nvdla_dev;

    volatile uint8_t iga;
    volatile uint8_t igb;
    volatile uint8_t igc;
    volatile uint8_t *hw_version = (uint8_t *)nvdla_dev->base+0x1000;
    iga = hw_version[0];
    igb = hw_version[1];
    igc = hw_version[2];



	err = dla_execute_task(nvdla_dev->engine_context, (void *)task, nvdla_dev->config_data);
	if (err) {
		return err;
	}

    while(1)
    {
        dla_isr_handler(nvdla_dev->engine_context);
        err = dla_process_events(nvdla_dev->engine_context, &task_complete);
        if (err || task_complete)
            break;
    }


	dla_clear_task(nvdla_dev->engine_context);

	return task_complete;
}

static int32_t nvdla_fill_task_desc(struct nvdla_submit_task *local_task, struct nvdla_task *task)
{
	struct nvdla_mem_handle *handles;

	/* update task desc fields */
	task->num_addresses = local_task->num_addresses;

	handles = malloc(local_task->num_addresses *
				sizeof(struct nvdla_mem_handle));
	if (handles == NULL)
		return -1;

    /* local_task points to data in dram, the actual data is list of nvdla_mem_handle */
    memcpy(handles, (void *)(uint32_t)local_task->base_address, local_task->num_addresses * sizeof(struct nvdla_mem_handle));

	task->address_list = handles;

	return 0;
}


int32_t nvdla_submit(struct nvdla_submit_task *local_task)
{
	int32_t err = 0;
    struct nvdla_task task;
    struct nvdla_device nvdla_dev;
    //nvdla_dev= get_nvdla_dev();
    

	/*task = malloc(sizeof(*task));
	if (task == NULL)
		return -1;*/

	nvdla_dev.task = &task;

	task.nvdla_dev = &nvdla_dev;

	/* update task desc fields */
	err = nvdla_fill_task_desc(local_task, &task);
    /*volatile unsigned int address = 0x84409d00;
    volatile uint8_t *read_byte = (uint8_t *)address;
    volatile uint8_t ra;
    volatile uint8_t rb;
    volatile uint8_t rc;
    volatile uint8_t rd;
    struct nvdla_mem_handle mem_list[task_count][address_count];
    uint32_t i;
    for (i = 0; i < task_count; i++) {
        uint32_t j;

        for (j = 0; j < address_count; j++) {
            read_byte = (uint8_t *)address;
            ra = read_byte[0];
            rb = read_byte[1];
            rc = read_byte[2];
            rd = read_byte[3];
            mem_list[i][j].paddr = ((((uint32_t)ra)<<24) | (((uint32_t)rb)<<16) | (((uint32_t)rc)<<8) | (((uint32_t)rd)));
            address += 0x00000004;
            read_byte = (uint8_t *)address;
            ra = read_byte[0];
            rb = read_byte[1];
            rc = read_byte[2];
            rd = read_byte[3];
            mem_list[i][j].offset = ((((uint32_t)ra)<<24) | (((uint32_t)rb)<<16) | (((uint32_t)rc)<<8) | (((uint32_t)rd)));
            address += 0x00000004;
        }
    }
    task->num_addresses = local_task->num_addresses;
    task->address_list =*/

	if (err)
		goto free_task_desc;

	//err = nvdla_task_submit();

    free(task.address_list);

free_task_desc:
	//free(&task);
	return err;
}

void nvdla_init()
{
    struct simple_device *nvdla_dev = get_nvdla_dev();
    /* TODO: make this configurable */
    nvdla_dev->config_data = &nvdla_config_small;

    /* initialize io base */
    nvdla_dev->base = (volatile void *)(0x12000000);
    //volatile uint8_t *base_add = (uint8_t *)nvdla_dev->base;

    /* register irq function */
    //register_irq_handler(IRQ_ID_IRQ0, irq0_handler);

    dla_register_driver(&nvdla_dev->engine_context, (void *)nvdla_dev);
    dla_clear_task(nvdla_dev->engine_context);
    //*return_dev = *nvdla_dev;
    //return return_dev;
}


