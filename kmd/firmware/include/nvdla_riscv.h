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

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>


/**
 * struct nvdla_mem_handle structure for memory handles
 *
 * @handle		handle to DMA buffer allocated in userspace
 * @reserved		Reserved for padding
 * @offset		offset in bytes from start address of buffer
 *
 */
struct nvdla_mem_handle {
	uint32_t handle;
    uint32_t reserved;
    uint64_t offset;
};
struct nvdla_handle {
    uint32_t handle;
};
struct nvdla_reserved {
    uint32_t reserved;
};
struct nvdla_offset {
    uint64_t offset;
};

/**
 * struct nvdla_ioctl_submit_task structure for single task information
 *
 * @num_addresses		total number of entries in address_list
 * @reserved			Reserved for padding
 * @address_list		pointer to array of struct nvdla_mem_handle
 *
 */
struct nvdla_ioctl_submit_task {
#define NVDLA_MAX_BUFFERS_PER_TASK (6144)
    uint32_t num_addresses;
#define NVDLA_NO_TIMEOUT    (0xffffffff)
    uint32_t timeout;
    uint64_t address_list;
};

/**
 * struct nvdla_submit_args structure for task submit
 *
 * @tasks		pointer to array of struct nvdla_ioctl_submit_task
 * @num_tasks		number of entries in tasks
 * @flags		flags for task submit, no flags defined yet
 * @version		version of task structure
 *
 */
struct nvdla_submit_args {
    uint64_t tasks;
    uint16_t num_tasks;
#define NVDLA_MAX_TASKS_PER_SUBMIT	24
#define NVDLA_SUBMIT_FLAGS_ATOMIC	(1 << 0)
    uint16_t flags;
    uint32_t version;
};

/**
 * struct nvdla_gem_create_args for allocating DMA buffer through GEM
 *
 * @handle		handle updated by kernel after allocation
 * @flags		implementation specific flags
 * @size		size of buffer to allocate
 */
struct nvdla_gem_create_args {
    uint32_t handle;
    uint32_t flags;
    uint64_t size;
};

/**
 * struct nvdla_gem_map_offset_args for mapping DMA buffer
 *
 * @handle		handle of the buffer
 * @reserved		reserved for padding
 * @offset		offset updated by kernel after mapping
 */
struct nvdla_gem_map_offset_args {
    uint32_t handle;
    uint32_t reserved;
    uint64_t offset;
};

/**
 * struct nvdla_gem_destroy_args for destroying DMA buffer
 *
 * @handle		handle of the buffer
 */
struct nvdla_gem_destroy_args {
    uint32_t handle;
};

struct nvdla_task {
    uint32_t num_addresses;
    struct nvdla_device *nvdla_dev;
    struct nvdla_mem_handle *address_list;
};

struct riscv_task {
    uint32_t num_addresses;
    struct riscv_device *nvdla_dev;
    struct nvdla_handle *handle_list;
    struct nvdla_offset *offset_list;
};

struct nvdla_config
{
    uint32_t atom_size;
    bool bdma_enable;
    bool rubik_enable;
    bool weight_compress_support;
};

struct nvdla_device {
    int32_t irq;
    void volatile *base;
    struct nvdla_task *task;
    struct nvdla_config *config_data;

    void *engine_context;
};

struct riscv_device {
    int32_t irq;
    void volatile *base;
    struct riscv_task *task;
    struct nvdla_config *config_data;

    void *engine_context;
};


int32_t nvdla_task_submit(int list_start,int address_count);
struct riscv_device * nvdla_init(void);
void intr_trap(void);
void* get_nvdla_dev(void);
void set_nvdla_dev(struct riscv_device *dev);
void* get_local_task(void);
