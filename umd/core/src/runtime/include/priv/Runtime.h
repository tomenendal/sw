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

#ifndef NVDLA_PRIV_RUNTIME_H
#define NVDLA_PRIV_RUNTIME_H

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <algorithm>

#include "priv/Type.h"

#include "nvdla/ILoadable.h"
#include "nvdla/IRuntime.h"

#include "priv/EMUInterface.h"

#define NUM_MAX_BDMA_OPS	20

struct nvdla_mem_handle {
    void* handle;
    uint64_t offset;
};
struct dla_network_desc {
    int16_t operation_desc_index;
    int16_t surface_desc_index;

    int16_t dependency_graph_index;
    int16_t lut_data_index;

    int16_t roi_array_index;
    int16_t surface_index;

    int16_t stat_list_index;
    int16_t reserved1;

    int16_t op_head[6];

    uint16_t num_rois;
    uint16_t num_operations;

    uint16_t num_luts;
    uint16_t num_addresses;

    int16_t input_layer;
    uint8_t dynamic_roi;
    uint8_t reserved0;
};
struct dla_bdma_transfer_desc {
    int16_t source_address;
    int16_t destination_address;

    uint32_t line_size;

    uint32_t line_repeat;

    uint32_t source_line;

    uint32_t destination_line;

    uint32_t surface_repeat;

    uint32_t source_surface;

    uint32_t destination_surface;
};
struct dla_cvt_param {
    int16_t  scale;
    uint8_t  truncate;
    uint8_t  enable;

    int32_t  offset;
};
struct dla_bdma_op_desc {
    uint16_t num_transfers;
    uint16_t reserved0;
};
struct dla_bdma_surface_desc {
    uint8_t source_type;
    uint8_t destination_type;
    uint16_t num_transfers;

    struct dla_bdma_transfer_desc transfers[NUM_MAX_BDMA_OPS];
};
struct dla_data_cube {
    uint16_t type; /* dla_mem_type */
    int16_t address; /* offset to the actual IOVA in task.address_list */

    uint32_t offset;
    uint32_t size;

    /* cube dimensions */
    uint16_t width;
    uint16_t height;

    uint16_t channel;
    uint16_t reserved0;

    /* stride information */
    uint32_t line_stride;
    uint32_t surf_stride;

    /* For Rubik only */
    uint32_t plane_stride;
};
struct dla_conv_op_desc {
    /* Performance parameters */

    /* dla_conv_mode */
    uint8_t conv_mode;
    uint8_t data_reuse;
    uint8_t weight_reuse;
    uint8_t skip_data_rls;

    uint8_t skip_weight_rls;
    uint8_t reserved0;
    uint16_t entry_per_slice;

    /* dla_data_format */
    uint8_t data_format;
    /* dla_pixel_mapping */
    uint8_t pixel_mapping;
    /* number of free slices before fetch */
    uint16_t fetch_grain;

    uint8_t reserved_b[8];

    /* batch_num */
    uint8_t batch;
    /* dla_weight_format */
    uint8_t weight_format;
    uint8_t data_bank;
    uint8_t weight_bank;

    /* the offset in bytes of each data cube in a batch */
    uint32_t batch_stride;

    uint8_t post_extension;
    uint8_t pixel_override;
    /* number of slices need to be released */
    uint16_t release;

    /* The input cube dimension for CSC */
    uint16_t input_width_csc;
    uint16_t input_height_csc;

    uint16_t input_channel_csc;
    uint16_t kernel_width_csc;

    uint16_t kernel_height_csc;
    uint16_t kernel_channel_csc;

    /* The input cube dimension for CMAC */
    uint16_t input_width_cmac;
    uint16_t input_height_cmac;

    /* actual size in bytes */
    uint32_t bytes_per_kernel;

    /* Algorithm parameters */

    int16_t mean_ry; /* mean value for red in RGB or Y in YUV */
    int16_t mean_gu; /* mean value for green in RGB or U in YUV */

    int16_t mean_bv; /* mean value for blue in RGB or V in YUV */
    int16_t mean_ax;

    uint8_t mean_format; /* dla_mean_format */
    uint8_t conv_stride_x;
    uint8_t conv_stride_y;
    uint8_t pad_x_left;

    uint8_t pad_x_right;
    uint8_t pad_y_top;
    uint8_t pad_y_bottom;
    uint8_t dilation_x;

    uint8_t dilation_y;
    uint8_t reserved2[2];

    /* Precision parameters */
    uint8_t pra_truncate;

    uint8_t in_precision;
    /* The output precision from CONV, it's the MAC processing precison */
    uint8_t out_precision;
    int16_t pad_val;

    /* input converter parameters */
    struct dla_cvt_param in_cvt;
    /* output converter parameters, support truncate only */
    struct dla_cvt_param out_cvt;

};
struct dla_conv_surface_desc {
    /* Data cube */
    struct dla_data_cube weight_data;
    struct dla_data_cube wmb_data;
    struct dla_data_cube wgs_data;
    struct dla_data_cube src_data;
    struct dla_data_cube dst_data;

    /**
     * u_addr = input_data.source_addr + offset_u
     * this field should be set when YUV is not interleave format
     *
     */
    int32_t offset_u;

    /* line stride for 2nd plane, must be 32bytes aligned */
    uint32_t in_line_uv_stride;
};
struct dla_sdp_cvt {
    struct dla_cvt_param alu_cvt;
    struct dla_cvt_param mul_cvt;
};

struct dla_sdp_op {
    uint8_t enable;
    uint8_t alu_type; /* dla_sdp_alu_op_type */
    uint8_t type; /* dla_sdp_op_type */
    uint8_t mode; /* dla_sdp_op_mode */

    uint8_t act; /* dla_act_type */
    uint8_t shift_value; /* left shift */
    uint8_t truncate;
    uint8_t precision;

    int32_t alu_operand;
    int32_t mul_operand;

    struct dla_sdp_cvt  cvt;
};
struct dla_sdp_op_desc {
        /* Precision parameters */
        /* dla_precision */
        uint8_t src_precision;
        uint8_t dst_precision;
        int16_t lut_index;

        struct dla_cvt_param out_cvt;

        /* Performance parameters */
        /* dla_conv_mode */
        uint8_t conv_mode;
        uint8_t batch_num;
        uint16_t reserved0;

        uint32_t batch_stride;	/* will be used when batch_num > 1 */

        /* Algorithm parameters */
        struct dla_sdp_op x1_op;
        struct dla_sdp_op x2_op;
        struct dla_sdp_op y_op;
};
struct dla_sdp_surface_desc {
    /* Data cube */
    /* source input cube, available when SDP working on offline mode */
    struct dla_data_cube src_data;

    /* X1 input cube */
    struct dla_data_cube x1_data;

    /* X2 input cube */
    struct dla_data_cube x2_data;

    /* Y input cube */
    struct dla_data_cube y_data;

    /* Output cube */
    struct dla_data_cube dst_data;
};
#define PDP_PAD_VAL_NUM	7
struct dla_pdp_op_desc {
    /* Performance parameters */
    uint16_t  partial_in_width_first;
    uint16_t  partial_in_width_mid;

    uint16_t  partial_in_width_last;
    uint16_t  partial_width_first;

    uint16_t  partial_width_mid;
    uint16_t  partial_width_last;

    uint8_t   split_num;

    /* Algorithm parameters */
    uint8_t  pool_mode; /* dla_pool_mode */
    uint8_t  pool_width; /* dla_pool_width */
    uint8_t  pool_height; /* dla_pool_height */

    uint8_t  stride_x;
    uint8_t  stride_y;

    /**
     * The left/right padding size,
     * pad_right might be less than pad_left
     */
    uint8_t  pad_left;
    uint8_t  pad_right;

    /* The top/bottom padding size */
    uint8_t  pad_top;
    uint8_t  pad_bottom;

    /* Precision parameters */
    uint8_t  precision; /* dla_precision */
    uint8_t  reserved0;
    /**
     * if input has non-zero "offset", this value should be set
     * There'll be 7 different paddding values, the relationship between
     * those versions are:
     * padding_value[0] = -offset*scaling;
     * padding_value[1] = 2*padding_value[0]
     * padding_value[2] = 3*padding_value[0]
     * ...
     * The purpose is to avoid ucode implement FP16
     * multiplier(for FP16 mode)
     */
    int32_t  padding_value[PDP_PAD_VAL_NUM];
};
struct dla_pdp_surface_desc {
    /* Data cube */
    struct dla_data_cube src_data;

    struct dla_data_cube dst_data;
};
struct dla_cdp_op_desc {
    /* Precision parameters */

    /* dla_precision */
    uint8_t  in_precision;
    uint8_t  out_precision;
    int16_t  lut_index;

    struct dla_cvt_param in_cvt;
    struct dla_cvt_param out_cvt;

    /* Performance parameters */

    /* Algorithm parameters */
    uint8_t  local_size;
    uint8_t  bypass_sqsum;
    uint8_t  bypass_out_mul;
    uint8_t  reserved0;
};
struct dla_cdp_surface_desc {
    /* Data cube */
    struct dla_data_cube src_data;

    struct dla_data_cube dst_data;
};
struct dla_rubik_op_desc {
    /* Precision parameters */
    uint8_t mode;
    uint8_t precision;
    uint8_t stride_x;
    uint8_t stride_y;
};
struct dla_rubik_surface_desc {
    /* Data cube */
    struct dla_data_cube src_data;

    struct dla_data_cube dst_data;
};
union dla_surface_container {
    struct dla_bdma_surface_desc bdma_surface;
    struct dla_conv_surface_desc conv_surface;
    struct dla_sdp_surface_desc sdp_surface;
    struct dla_pdp_surface_desc pdp_surface;
    struct dla_cdp_surface_desc cdp_surface;
    struct dla_rubik_surface_desc rubik_surface;
};
union dla_quad_surface_container {
    struct dla_bdma_surface_desc bdma_surface_a;
    struct dla_conv_surface_desc conv_surface_a;
    struct dla_sdp_surface_desc sdp_surface_a;
    struct dla_pdp_surface_desc pdp_surface_a;
    struct dla_cdp_surface_desc cdp_surface_a;
    struct dla_rubik_surface_desc rubik_surface_a;

    struct dla_bdma_surface_desc bdma_surface_b;
    struct dla_conv_surface_desc conv_surface_b;
    struct dla_sdp_surface_desc sdp_surface_b;
    struct dla_pdp_surface_desc pdp_surface_b;
    struct dla_cdp_surface_desc cdp_surface_b;
    struct dla_rubik_surface_desc rubik_surface_b;

    struct dla_bdma_surface_desc bdma_surface_c;
    struct dla_conv_surface_desc conv_surface_c;
    struct dla_sdp_surface_desc sdp_surface_c;
    struct dla_pdp_surface_desc pdp_surface_c;
    struct dla_cdp_surface_desc cdp_surface_c;
    struct dla_rubik_surface_desc rubik_surface_c;

    struct dla_bdma_surface_desc bdma_surface_d;
    struct dla_conv_surface_desc conv_surface_d;
    struct dla_sdp_surface_desc sdp_surface_d;
    struct dla_pdp_surface_desc pdp_surface_d;
    struct dla_cdp_surface_desc cdp_surface_d;
    struct dla_rubik_surface_desc rubik_surface_d;
};
union dla_operation_container {
    struct dla_bdma_op_desc bdma_op;
    struct dla_conv_op_desc conv_op;
    struct dla_sdp_op_desc sdp_op;
    struct dla_pdp_op_desc pdp_op;
    struct dla_cdp_op_desc cdp_op;
    struct dla_rubik_op_desc rubik_op;
};

namespace nvdla
{
class ITensor;

namespace priv
{

class Loadable;
class Runtime;


class RuntimeFactory
{
public:
    typedef PrivPair<IRuntime *, Runtime*> RuntimePrivPair;

    static RuntimePrivPair newRuntime();
    static void deleteRuntime(IRuntime *);

    static Runtime *priv(IRuntime *);
    static IRuntime *i(Runtime *);
    static IRuntime *self(void *s);

protected:
    static BiMap<IRuntime *, Runtime *> s_priv;
    static BiMap<void *, IRuntime *> s_self;

};

class Runtime : public IRuntime
{
public: // externally facing

    // device interfaces
    virtual NvU16 getMaxDevices();
    virtual NvU16 getNumDevices();

    virtual void read_binary_file(std::string filename, std::vector<char> &buffer);
    virtual NvDlaError TapascoTransmit(NvU32 taskcount, NvDlaTask *tasks, std::string binName);
    virtual NvDlaError TapascoCopyTo(void* Buffer, void* Data, int size);
    virtual NvDlaError TapascoCopyFrom(void* Buffer, void* Data, int size);

    virtual bool initEMU(void);
    virtual void stopEMU(void);

    virtual bool load(NvU8 *buf, int instance);
    virtual void unload(void);
    virtual NvDlaError allocateSystemMemory(void **h_mem, NvU64 size, void **pData);
    virtual void freeSystemMemory(void *phMem, NvU64 size);

    virtual bool bindInputTensor (int index, void *hMem);
    virtual bool bindOutputTensor(int index, void *hMem);

    virtual NvDlaError getNetworkDataType(uint8_t *) const;

    virtual NvDlaError getNumInputTensors(int *);
    virtual NvDlaError getInputTensorDesc(int id, IRuntime::NvDlaTensor *);
    virtual NvDlaError setInputTensorDesc(int id, const IRuntime::NvDlaTensor *);

    virtual NvDlaError getNumOutputTensors(int *);
    virtual NvDlaError getOutputTensorDesc(int id, IRuntime::NvDlaTensor *);
    virtual NvDlaError setOutputTensorDesc(int id, const IRuntime::NvDlaTensor *);

    virtual bool submit(std::string binaryName);

public: // internally facing
    Runtime();

    virtual NvU16 getFactoryType() const;

protected:

    friend class RuntimeFactory;

    virtual ~Runtime();

    inline bool debugMemoryLayout() const  { return false; }
    inline bool debugTasks() const { return false; }
    inline bool debugVersions() const { return false; }
    inline bool debugLoadables() const { return false; }
    inline bool debugBinding() const { return false; }
    inline bool debugStrideRewrite() const { return false; }

    NvDlaError submitInternal(std::string binName);

    virtual void *getDLADeviceContext(size_t sel_i);
    size_t getMaxDLADevices() { return 1; }

    void *m_dla_handle;
    void *m_dla_device_handles[2];
    Emulator *m_emu_engine;

    void *h_network_desc_mem;
    void *h_op_desc_mem;
    void *h_surf_desc_mem;
    void *h_dependency_list_mem;

    std::vector<ILoadable::TaskListEntry> m_task_entries;
    std::vector<ILoadable::SubmitListEntry> m_submit_entries;
    std::vector<ILoadable::MemoryListEntry> m_memory_entries;
    std::vector<ILoadable::AddressListEntry> m_address_entries;
    std::vector<ILoadable::EventListEntry> m_event_entries;
    std::vector<ILoadable::TensorDescListEntry> m_tensor_desc_entries;
    std::vector<ILoadable::RelocEntry> m_reloc_entries;

    class Task  {
    public:
        Task() { }
        Task(const ILoadable::TaskListEntry &e) : mEntry(e)        { }
        Task(const Task &o)                     : mEntry(o.mEntry) { }
        NvU16 id() const { return mEntry.id; }
        NvU32 interface() const { return mEntry.interface; }
        NvS16 instance()  const { return mEntry.instance; }
        std::vector<NvU16> &address_list() { return mEntry.address_list; }
        std::vector<NvU16> &preactions() { return mEntry.preactions; }
        std::vector<NvU16> &postactions() { return mEntry.postactions; }
    protected:
        friend class Runtime;
        ILoadable::TaskListEntry mEntry;

    };

    class Submit {
    public:
        Submit() { }
        Submit(const ILoadable::SubmitListEntry &e) : mEntry(e) { }
        Submit(const Submit &o) : mEntry(o.mEntry) { }
        NvU16 id() const { return mEntry.id; }
        std::vector<NvU16> &tasks() { return mEntry.tasks; }
    protected:
        friend class Runtime;
        ILoadable::SubmitListEntry mEntry;
    };

    class Memory {
    public:
        Memory() : hMem(0), pVirtAddr(0) { }
        Memory(const ILoadable::MemoryListEntry &e) : hMem(0), pVirtAddr(0), mEntry(e) { }
        Memory(const Memory &o)                     : hMem(o.hMem), pVirtAddr(0), mEntry(o.mEntry) { }
        inline NvU16 id() { return mEntry.id; }
        inline NvU64 size() { return mEntry.size; }
        inline NvU32 alignment() { return mEntry.alignment; }
        inline NvU8 domain() { return mEntry.domain; }
        inline bool bound() { return hMem != 0; }
        inline NvU8 flags() { return mEntry.flags; }
        inline void setHandle(void *h) { hMem = h; }
        inline void *getHandle() const { return hMem; }
        inline void setVirtAddr(void *addr) { pVirtAddr = addr; }
        inline void *getVirtAddr() const { return pVirtAddr; }
        inline std::vector<std::string> & contents() { return mEntry.contents; }
        inline std::vector<uint64_t> & offsets() { return mEntry.offsets; }
        inline int inputBindId() const {
            if ( mEntry.flags & mEntry.flags_input() ) {
                return (int) mEntry.bind_id;
            }
            return -1;
        }
        inline int outputBindId() const {
            if ( mEntry.flags & mEntry.flags_output() ) {
                return (int) mEntry.bind_id;
            }
            return -1;
        }
        inline bool bindable() const {
            return !!(mEntry.flags & (mEntry.flags_input() | mEntry.flags_output()));
        }
        inline int bindId(IOD &which) const
        {
            // there should be only one valid.  but if not take in order of
            // input, output
            if ( mEntry.flags & mEntry.flags_input() ) {
                which = IOD_Input;
                return (int) mEntry.bind_id;
            } else if ( mEntry.flags & mEntry.flags_output() ) {
                which = IOD_Output;
                return (int) mEntry.bind_id;
            }
            which = IOD_Max;
            return -1;
        };
        inline int tensorDescId() const {
            if ( mEntry.flags & ( mEntry.flags_input() | mEntry.flags_output() ) ) {
                return (int) mEntry.tensor_desc_id;
            }
            return -1;
        }

    protected:
        friend class Runtime;
        void *hMem;
        void *pVirtAddr;
        ILoadable::MemoryListEntry mEntry;
    };

    class Event {
    public:
        NvU16 id()const { return mEntry.id; }
        NvU8  op() const { return mEntry.op; }
        NvU16 target()const { return mEntry.target; }
        NvU32 val() const { return mEntry.val; }
    protected:
        friend class Runtime;
        ILoadable::EventListEntry mEntry;
    };

    class Address {
    public:
        Address() { }
        Address(const ILoadable::AddressListEntry &e) : mEntry(e) { }
        Address(const Address &o) : mEntry(o.mEntry) { }
        NvU16 id() const { return mEntry.id; }
        NvU16 mem_id() const { return mEntry.mem_id; }
        NvU64 offset() const { return mEntry.offset; }
    public:
        friend class Runtime;
        ILoadable::AddressListEntry mEntry;
    };

public:
    class TensorDesc : public ILoadable::TensorDescListEntry {
    public:
        TensorDesc();
        TensorDesc(const ILoadable::TensorDescListEntry &e);
        IRuntime::NvDlaTensor bindTensorDesc() const;
    };

    bool bindTensorMemory(ITensor *, void *hMem);
    bool unbindTensorMemory(ITensor *, void *hMem);

protected:
    std::vector<Task> m_task;
    std::vector<Submit> m_submit;
    std::vector<Memory> m_memory;
    std::vector<Event> m_event;
    std::vector<Address> m_address;
    std::vector<TensorDesc> m_tensor_desc;

    Loadable * m_loaded;
    size_t m_loaded_instance;

    bool versionsCompatible(const ILoadable::Version &, const ILoadable::Version &);

    size_t m_numDLATasks;

    NvDlaError loadMemory(Loadable *, Memory *);
    void unloadMemory(Memory *);
    bool fillTaskAddressList(Task *task, NvDlaTask *);

    bool fillEMUTaskAddressList(Task *task, EMUTaskDescAccessor taskDescAcc);

    //
    // maintenance of ids/lookups for bind ids, associated memory, tensor descs
    //
    NvDlaError initBindableMemory();
    NvDlaError getMemoryFromBindId(IOD w, int id, Memory * &bound_mem);
    std::vector<std::vector<Memory *>> m_bindable_memory; // indexed on [iod][bind_id]
    std::vector<std::tuple<void *, void*, int>> m_copyback_memory;
    std::map<void *, void *> m_hmem_memory_map; // maintains 1-1 relation between hmem and mapped memory.
    std::map<void *, int> m_hmem_size_map;
    class MemoryId_BindId_Is // helper predicate
    {
    public:
        MemoryId_BindId_Is(int find_id) : m_find_id(find_id) { }
        bool operator () (Memory *&mem) const { IOD na; return m_find_id == mem->bindId(na); }
    protected:
        int m_find_id;
    };

    class Memory_BindId_LT_Compare // helper comparator
    {
    public:
        bool operator() (Memory* const& i, Memory* const& j) { IOD na; return i->bindId(na) < j->bindId(na); }
    };

    NvDlaError mergeSetTensorDesc(IOD w, int bindID, int tensorDescId, const IRuntime::NvDlaTensor *tdl);
    NvDlaError rewriteStrides(IOD w, int bindID, int tensorDescId, const NvU32 *);

};


} // nvdla::priv
} // nvdla

#endif // NVDLA_PRIV_RUNTIME_H
