/* example4_stack */

// In this example we will compute part of the Fibonacci sequence using recursion
// Along the way we will print information about the stack!


#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

#include <stdint.h>
#include <stdbool.h>

#define TEST_DEPTH
// #define TEST_FIBONACCI

#define MEMORY_HEADSPACE 4096

// Forward Declarations
uint32_t free_memory( void );       // return number of bytes of unused memory between stack and heap
void update_stack_info( void );
void print_stack_info( void );   
void boost_mode_enable(bool bEnable);   
uint32_t fibonacci(uint32_t end);

void test_fibonacci( void );
void test_depth( void );

// Globals
uint32_t stack_pointer;
uint32_t min_stack_pointer = 0xFFFFFFFF;
uint32_t free_mem;
uint32_t min_free_mem = 0xFFFFFFFF;
bool go_deeper = true;
uint32_t max_depth = 0;

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

    // Enable Burst Mode (MOAR POWAH)
    boost_mode_enable(true); 

    // Clear the terminal and print the banner.
    am_util_stdio_terminal_clear();
    am_util_stdio_printf("Example4_stack\n");

    am_util_stdio_printf("\nInitial State\n");
    am_util_stdio_printf("=============\n");
    print_stack_info();

#ifdef TEST_FIBONACCI
    test_fibonacci();
#endif

#ifdef TEST_DEPTH
    test_depth();
#endif

    while(1){
        am_util_stdio_printf("\nTest Finished");
        am_util_delay_ms(10000);
    }
}


extern unsigned char _sheap;
uint32_t free_memory( void ){
    // Without an implementation of _sbrk (heap management) we are assuming 
    // that the heap has zero size. If there was heap management then you 
    // would compute the distance to the program break (the end of the heap)
    void* local;
    return (((uint32_t)&local) - ((uint32_t)&_sheap));
}

void update_stack_info( void ){
    void* local;
    stack_pointer = (uint32_t)(&local);
    free_mem = free_memory();
    min_free_mem = (free_mem < min_free_mem) ? free_mem : min_free_mem;
    min_stack_pointer = (stack_pointer < min_stack_pointer) ? stack_pointer : min_stack_pointer;
}

void print_stack_info( void ){
    update_stack_info();
    am_util_stdio_printf("Stack Info Snapshot:  ");
    am_util_stdio_printf("SP:           0x%08X, ", stack_pointer);
    am_util_stdio_printf("Free Memory:  0x%08X, ", free_mem);
    // am_util_stdio_printf("\n");
}


uint32_t fibonacci(uint32_t n){
    if(free_memory() < MEMORY_HEADSPACE){
        update_stack_info();
        am_util_stdio_printf("Out of Memory\n");
        return 0;
    }
    if(n == 0){
        update_stack_info();
        return 0;
    }
    if(n == 1){
        update_stack_info();
        return 1;
    }
    return fibonacci(n-1) + fibonacci(n-2);
}

void test_fibonacci( void ){
    uint32_t count = 0;
    uint32_t F_n = 0;
    while(1){

        F_n = fibonacci(count);
        am_util_stdio_printf("\nF(%d) = %d. min_free_mem = %d", count, F_n, min_free_mem);
        min_free_mem = 0xFFFFFFFF;

        count++;
    } // while(1)
}


void deep_horizon( void ){
    update_stack_info();
    if(free_memory() < MEMORY_HEADSPACE){
        go_deeper = false;
        return;
    }
    if( go_deeper ){
        deep_horizon();
    }
    max_depth++;
}

void test_depth( void ){
    go_deeper = true;
    min_free_mem = 0xFFFFFFFF;
    max_depth = 0;
    deep_horizon();
    am_util_stdio_printf("\nReached maximum depth! min_stack_pointer = 0x%08X, min_free_mem = %d, max_depth: %d\n", min_stack_pointer, min_free_mem, max_depth);
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