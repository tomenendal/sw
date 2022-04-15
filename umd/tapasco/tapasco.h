#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef enum {
  TlkmAccessExclusive = 0,
  TlkmAccessMonitor,
  TlkmAccessShared,
  TlkmAccessTypes,
} tlkm_access;

typedef struct Arc_OffchipMemory Arc_OffchipMemory;

/**
 * Description of a TaPaSCo device. Contains all relevant information and the operations
 * to interact with the device.
 *
 * Generated by [`TLKM.device_alloc`].
 *
 * [`TLKM.device_alloc`]: ../tlkm/struct.TLKM.html#method.device_alloc
 */
typedef struct Device Device;

/**
 * Helper structure to start and release PEs.
 * Deals with data transfer parameter handling.
 */
typedef struct Job Job;

/**
 * TLKM IOCTL convenience access
 *
 * This struct combines all basic interactions with TLKM
 * and makes them accessible to the outside world. During
 * initialization the driver is opened once and the file
 * handle is stored for future accesses.
 */
typedef struct TLKM TLKM;

typedef struct Vec_PEParameter Vec_PEParameter;

typedef uintptr_t PEId;

typedef Arc_OffchipMemory TapascoOffchipMemory;

typedef Vec_PEParameter JobList;

/**
 * Generic type for an address on the device.
 */
typedef uint64_t DeviceAddress;

typedef uint32_t DeviceId;

/**
 * Helper structure for device information
 *
 * Retrieved from TLKM and used to create a device structure
 * for use in the driver. Only used to provide information.
 */
typedef struct {
  DeviceId id;
  uint32_t vendor;
  uint32_t product;
  const char *name;
} DeviceInfo;

intptr_t tapasco_device_access(Device *dev, tlkm_access access);

Job *tapasco_device_acquire_pe(Device *dev, PEId id);

float tapasco_device_design_frequency(Device *dev);

PEId tapasco_device_get_pe_id(Device *dev, const char *name);

intptr_t tapasco_device_num_pes(Device *dev, PEId id);

TapascoOffchipMemory *tapasco_get_default_memory(Device *dev);

void tapasco_init_logging(void);

JobList *tapasco_job_param_alloc(Device *dev,
                                 uint8_t *ptr,
                                 uintptr_t bytes,
                                 bool to_device,
                                 bool from_device,
                                 bool free,
                                 bool uses_fixed,
                                 uint64_t fixed,
                                 JobList *list);

void tapasco_job_param_destroy(JobList *t);

JobList *tapasco_job_param_deviceaddress(DeviceAddress param, JobList *list);

JobList *tapasco_job_param_local(uint8_t *ptr,
                                 uintptr_t bytes,
                                 bool to_device,
                                 bool from_device,
                                 bool free,
                                 bool uses_fixed,
                                 uint64_t fixed,
                                 JobList *list);

JobList *tapasco_job_param_new(void);

JobList *tapasco_job_param_prealloc(Device *dev,
                                    uint8_t *ptr,
                                    DeviceAddress addr,
                                    uintptr_t bytes,
                                    bool to_device,
                                    bool from_device,
                                    bool free,
                                    JobList *list);

JobList *tapasco_job_param_single32(uint32_t param, JobList *list);

JobList *tapasco_job_param_single64(uint64_t param, JobList *list);

intptr_t tapasco_job_release(Job *job, uint64_t *return_value, bool release);

intptr_t tapasco_job_start(Job *job, JobList **params);

/**
 * Calculate the number of bytes in the last error's error message **not**
 * including any trailing `null` characters.
 */
int tapasco_last_error_length(void);

/**
 * Write the most recent error message into a caller-provided buffer as a UTF-8
 * string, returning the number of bytes written.
 *
 * # Note
 *
 * This writes a **UTF-8** string into the buffer. Windows users may need to
 * convert it to a UTF-16 "unicode" afterwards.
 *
 * If there are no recent errors then this returns `0` (because we wrote 0
 * bytes). `-1` is returned if there are any errors, for example when passed a
 * null pointer or a buffer of insufficient size.
 */
int tapasco_last_error_message(char *buffer, int length);

DeviceAddress tapasco_memory_allocate(TapascoOffchipMemory *mem, uintptr_t len);

DeviceAddress tapasco_memory_allocate_fixed(TapascoOffchipMemory *mem,
                                            uintptr_t len,
                                            uintptr_t offset);

intptr_t tapasco_memory_copy_from(TapascoOffchipMemory *mem,
                                  DeviceAddress addr,
                                  uint8_t *data,
                                  uintptr_t len);

intptr_t tapasco_memory_copy_to(TapascoOffchipMemory *mem,
                                const uint8_t *data,
                                DeviceAddress addr,
                                uintptr_t len);

void tapasco_memory_destroy(TapascoOffchipMemory *t);

intptr_t tapasco_memory_free(TapascoOffchipMemory *mem, DeviceAddress addr);

void tapasco_tlkm_destroy(TLKM *t);

Device *tapasco_tlkm_device_alloc(const TLKM *t, DeviceId id);

void tapasco_tlkm_device_destroy(Device *t);

intptr_t tapasco_tlkm_device_len(const TLKM *t);

intptr_t tapasco_tlkm_devices(const TLKM *t, DeviceInfo *di, uintptr_t len);

intptr_t tapasco_tlkm_devices_destroy(DeviceInfo *di, uintptr_t len);

TLKM *tapasco_tlkm_new(void);

int32_t tapasco_tlkm_version(const TLKM *t, char *vers, uintptr_t len);

uintptr_t tapasco_version(char *buffer, uintptr_t length);

uintptr_t tapasco_version_len(void);
