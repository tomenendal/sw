#include "../rv_pe.h"
#include <stdlib.h>
#include <stdint.h>
#include "nvdla_riscv.h"
#include "nvdla_interface.h"
#include "riscv.h"

#define BASE_UARTLITE 0x11010000
#define UARTLITE_RX *((volatile uint8_t*)(BASE_UARTLITE))
#define UARTLITE_TX *((volatile uint8_t*)(BASE_UARTLITE + 4))
#define UARTLITE_STAT *((volatile uint32_t*)(BASE_UARTLITE + 8))
#define UARTLITE_CTRL *((volatile uint32_t*)(BASE_UARTLITE + 12))


char str[50];

void trap_entry();

void sendchar(char c) {
    //wait for UARTLITE to have sending FIFO space
    while(UARTLITE_STAT & (1<<3));
    //send character
    UARTLITE_TX = c;
}

void sendstr(char* str) {
    int i = 0;
    char c;
    while(c = str[i]) {
        i++;
        sendchar(c);
    }
}

void trap() {
    intr_trap();
}


int main()
{
    csr_set(mtvec, trap_entry);
    csr_set(mie, MIE_MTIE);
    csr_set(mstatus, MSTATUS_MPP | MSTATUS_MIE);
	
    //get start address and size from address_list to read it correctly
    uint32_t add_list_start = readFromCtrl(ARG1);
    uint32_t num_add = readFromCtrl(ARG2);
    uint32_t complete = (uint32_t)nvdla_task_submit(add_list_start,num_add);
    //return nvdla_complete value
    writeToCtrl(RETL, complete);

    setIntr();
    return 0;
}
