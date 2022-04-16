/*
 * Copyright (c) 2017-2018, NVIDIA CORPORATION. All rights reserved.
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

#define _GNU_SOURCE

#include <dlaerror.h>
#include <dlatypes.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <drm.h>
#include <drm_mode.h>

#include "nvdla.h"
#include "nvdla_inf.h"
#include "nvdla_ioctl.h"
#include "nvdla_os_inf.h"

#include "tapasco.h"


#define NVDLA_DEVICE_NODE "/dev/dri/renderD128"

#define NVDLA_MEM_READ (PROT_READ)
#define NVDLA_MEM_WRITE (PROT_WRITE)

NvDlaError
NvDlaFreeMem(void *session_handle, void *device_handle, void *mem_handle, void *pData, NvU32 size)
{
    int err;
    struct nvdla_gem_destroy_args args;
    NvDlaMemHandle hMem = (NvDlaMemHandle)mem_handle;
    NvDlaDeviceHandle hDlaDev = (NvDlaDeviceHandle)device_handle;

    if (hMem == 0)
        return NvDlaError_BadParameter;

    /* unmap data */
    err = munmap(pData, size);
    if (err != 0) {
        printf("Failed to unmap memory err=%d, errno=%d\n",err, errno);
        return NvDlaError_BadParameter;
    }

    /* Close the file handle corresponding to that mem */
    if (hMem->fd != 0)
        (void) close(hMem->fd);

    args.handle = hMem->prime_handle;

    err = ioctl(hDlaDev->fd, DRM_IOCTL_NVDLA_GEM_DESTROY, &args);
    if (err) {
        printf("Failed to destroy handle err=%d errno=%d\n", err, errno);
        return NvDlaError_IoctlFailed;
    }

    free(hMem);

    return NvDlaSuccess;
}

NvDlaError
NvDlaInitialize(void **session_handle)
{
    if(!session_handle)
        return NvDlaError_BadParameter;
    *session_handle = NULL;

    return NvDlaSuccess;
}

void
NvDlaDestroy(void *session_handle)
{
    (void)session_handle;

    return;
}

NvDlaError
NvDlaOpen(void *session_handle, NvU32 instance, void **device_handle)
{
    NvDlaContext *pContext = NULL;
    NvDlaError e = NvDlaSuccess;

    if (instance > 0)
        return NvDlaError_BadParameter;

    if (!device_handle)
        return NvDlaError_BadParameter;

    pContext = (NvDlaContext*)NvDlaAlloc(sizeof(NvDlaContext));
    if (!pContext) {
        return NvDlaError_InsufficientMemory;
    }

    NvDlaMemset(pContext, 0, sizeof(NvDlaContext));

    /*pContext->fd = open(NVDLA_DEVICE_NODE, O_RDWR);
    perror(NVDLA_DEVICE_NODE);
    if (pContext->fd < 0) {
        e = NvDlaError_ResourceError;
        goto fail;
    }*/

    *device_handle = (void *)pContext;

    return NvDlaSuccess;

fail:
    NvDlaFree(pContext);
    return e;
}

void
NvDlaClose(void *hDlaDevice)
{
    NvDlaDeviceHandle device_handle = (NvDlaDeviceHandle)hDlaDevice;

    if (hDlaDevice == NULL)
        return;

    if (device_handle->fd != -1)
        (void)close(device_handle->fd);

    NvDlaFree(device_handle);
    return;
}
