/* example5_heap */

// This example will verify the function of malloc and free

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

// #include <stdint.h>
#include <stdlib.h>     // provides malloc / free
#include <sys/types.h>  // provides caddr_t /* for caddr_t (typedef char * caddr_t;) */s

#define MEMORY_HEADSPACE 4096 // How many bytes to leave untouched between stack and heap

// Forward Declarations
void test_max_allocation( void );
void test_repeat_allocation( void );
void boost_mode_enable(bool bEnable);

static unsigned char* _eheap = NULL;    // our heap management tracks the end of the heap with this var
extern unsigned char _sheap;            // Provided by linker

int main()
{
    // Setup system clocks
    am_hal_clkgen_control(AM_HAL_CLKGEN_CONTROL_SYSCLK_MAX, 0);

    // Set the default cache configuration
    am_hal_cachectrl_config(&am_hal_cachectrl_defaults);
    am_hal_cachectrl_enable();

    // Configure the board for low power operation.
    am_bsp_low_power_init();

    // Enable the UART print interface.
    am_bsp_uart_printf_enable();

    // Clear the terminal and print the banner.
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("Example5_heap\n");
    am_util_stdio_printf("=============\n");

    am_util_stdio_printf("Testing Repeat Allocation\n");
    test_repeat_allocation();

    am_util_stdio_printf("Testing Maximum Allocation (malloc() and free())\n");
    test_max_allocation();

    while(1){

    }
}

void test_max_allocation( void ){
    void* mem = NULL;
    size_t len = 0;
    boost_mode_enable(true);
    do {
        len++;
        mem = (void*)malloc( len * sizeof(uint8_t));
        free(mem);
    } while (mem != NULL);
    am_util_stdio_printf("\n\nLargest allocated space: 0x%08X = %d = %d K\n\n\n", len, len, len/1024);
    boost_mode_enable(false); 
}

void test_repeat_allocation( void ){
    size_t len = 0;
    if (_eheap == NULL) { _eheap = (unsigned char*)&_sheap; }
    am_util_stdio_printf("_eheap: 0x%08X\n", (uint32_t)_eheap);

    len = 1;
    void* mymem1 = (void*)malloc(len * sizeof(uint8_t));
    if( mymem1 != NULL ){ am_util_stdio_printf("Allocated %d bytes of memory at address 0x%08X. _eheap: 0x%08X\n", len, (uint32_t)mymem1, (uint32_t)_eheap ); }

    len = 5;
    void* mymem2 = (void*)malloc(len * sizeof(uint8_t));
    if( mymem2 != NULL ){ am_util_stdio_printf("Allocated %d bytes of memory at address 0x%08X. _eheap: 0x%08X\n", len, (uint32_t)mymem2, (uint32_t)_eheap ); }

    len = 0x1500;
    void* mymem3 = (void*)malloc(len * sizeof(uint8_t));
    if( mymem3 != NULL ){ am_util_stdio_printf("Allocated %d bytes of memory at address 0x%08X. _eheap: 0x%08X\n", len, (uint32_t)mymem3, (uint32_t)_eheap ); }

    len = 8;
    void* mymem4 = (void*)malloc(len * sizeof(uint8_t));
    if( mymem4 != NULL ){ am_util_stdio_printf("Allocated %d bytes of memory at address 0x%08X. _eheap: 0x%08X\n", len, (uint32_t)mymem4, (uint32_t)_eheap ); }

    free(mymem3);
    am_util_stdio_printf("Freed the large block\n");

    len = 0x1400;
    void* mymem5 = (void*)malloc(len * sizeof(uint8_t));
    if( mymem5 != NULL ){ am_util_stdio_printf("Allocated %d bytes of memory at address 0x%08X. _eheap: 0x%08X\n", len, (uint32_t)mymem5, (uint32_t)_eheap ); }

    len = 0x80;
    void* mymem6 = (void*)malloc(len * sizeof(uint8_t));
    if( mymem6 != NULL ){ am_util_stdio_printf("Allocated %d bytes of memory at address 0x%08X. _eheap: 0x%08X\n", len, (uint32_t)mymem6, (uint32_t)_eheap ); }

    len = 0x10;
    void* mymem7 = (void*)malloc(len * sizeof(uint8_t));
    if( mymem7 != NULL ){ am_util_stdio_printf("Allocated %d bytes of memory at address 0x%08X. _eheap: 0x%08X\n", len, (uint32_t)mymem7, (uint32_t)_eheap ); }


    free(mymem1);
    free(mymem2);
    free(mymem4);
    free(mymem5);
    free(mymem6);
    free(mymem7);
    am_util_stdio_printf("Freed allocted memory\n");
    am_util_stdio_printf("Repeat Allocation Test Complete\n\n");
}

/* Excellent insight from Martin T. https://embdev.net/topic/linker-error-undefined-reference-to-_sbrk */
extern "C" caddr_t _sbrk ( int incr ){
    unsigned char* prev_heap;
    unsigned char  local;

    // Initialize the heap pointer
    if (_eheap == NULL) {
        _eheap = (unsigned char*)&_sheap;
    }
    prev_heap = _eheap;

    // Check that the requested amount of memory is available
    if( ((&local)-(_eheap)) >= (incr + MEMORY_HEADSPACE) ){
        _eheap += incr;
    }else{
        // set errno to ENOMEM
        prev_heap = (unsigned char*)-1;
    }

    // Return the previous heap (or the error code)
    return (caddr_t) prev_heap;
}

// burst mode enable
void boost_mode_enable(bool bEnable){
    am_hal_burst_avail_e          eBurstModeAvailable;
    am_hal_burst_mode_e           eBurstMode;

    // Check that the Burst Feature is available.
    if (AM_HAL_STATUS_SUCCESS == am_hal_burst_mode_initialize(&eBurstModeAvailable)){
        if (AM_HAL_BURST_AVAIL == eBurstModeAvailable){
            am_util_stdio_printf("Apollo3 Burst Mode is Available\n");
        }
        else{
            am_util_stdio_printf("Apollo3 Burst Mode is Not Available\n");
            while(1){};
        }
    }
    else{
        am_util_stdio_printf("Failed to Initialize for Burst Mode operation\n");
    }

    // Make sure we are in "Normal" mode.
    if (AM_HAL_STATUS_SUCCESS == am_hal_burst_mode_disable(&eBurstMode)){
        if (AM_HAL_NORMAL_MODE == eBurstMode){
            am_util_stdio_printf("Apollo3 operating in Normal Mode (48MHz)\n");
        }
    }
    else{
        am_util_stdio_printf("Failed to Disable Burst Mode operation\n");
    }

    // Put the MCU into "Burst" mode.
    if (bEnable)
    {
        if (AM_HAL_STATUS_SUCCESS == am_hal_burst_mode_enable(&eBurstMode)){
            if (AM_HAL_BURST_MODE == eBurstMode){
                am_util_stdio_printf("Apollo3 operating in Burst Mode (96MHz)\n");
            }
        }
        else{
            am_util_stdio_printf("Failed to Enable Burst Mode operation\n");
        }
    }
}