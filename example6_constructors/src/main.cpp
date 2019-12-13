/* example5_heap */

// This example will demonstrate the C-runtime setup by calling constructors

#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"

#define WAIT_TIME 500 //msec
#define LED AM_BSP_GPIO_LED_BLUE

// Declare the libc init array function
extern "C" void __libc_init_array(void);

/* A very simple class to test constructor execution */
class LEDSTATE {
private:
protected:
public:
    uint8_t state;          // Trivial initialization is 0
    LEDSTATE(uint8_t init);
};

LEDSTATE::LEDSTATE(uint8_t init){
    state=init;
}

// This object should be initialized with a non-zero state
LEDSTATE    state(true);
LEDSTATE    state2(false);



//
// Call Constructors

// https://bitbashing.io/embedded-cpp.html
static void callConstructors()
{
  // Start and end points of the constructor list,
  // defined by the linker script.
  extern void (*__init_array_start)();
  extern void (*__init_array_end)();

  // Call each function in the list.
  // We have to take the address of the symbols, as __init_array_start *is*
  // the first function pointer, not the address of it.
  for (void (**p)() = &__init_array_start; p < &__init_array_end; ++p) {
      (*p)();
  }

  // The main remaining questions are:
  // How does GCC locate the function pointers in this section?
  // How does GCC ensure proper calling convention for these functions?
  // Does GCC ever use different section names for constructors? 
  //    (Will the rule capture all constructors into the .init_array output section?)


  // For reference here is the relevant part of SEGGER
  // startup assembly code:

  //   ldr r0, =__ctors_start__
  //   ldr r1, =__ctors_end__
  // ctor_loop:
  //   cmp r0, r1
  //   beq ctor_end
  //   ldr r2, [r0]
  //   adds r0, #4
  //   push {r0-r1}  
  //   blx r2
  //   pop {r0-r1}
  //   b ctor_loop
  // ctor_end:

  // (guessing here)
  // Since the init_array only deals with global constructors they must have 'static scope'
  // (am I using that term correctly?)
  // This means (I think) that the compiler is able to store all arguments in a preamble to 
  // the function. Thus there is no need to pass any arguments to the constuctor - and that
  // is why the function pointers can all have the same "void (*)(void)" signature - and also
  // why the assembly code can simply push {r0-r1} onto the stack and just branch to the 
  // address pointed to by r2. 
}



int main()
{
    // // Call cnstructors
    // __libc_init_array();
    callConstructors();

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
    am_util_stdio_printf("Example6_constructors\n");
    am_util_stdio_printf("=====================\n");

    am_util_stdio_printf("state: %d\n", state.state);
    am_util_stdio_printf("state2: %d\n", state2.state);

    while(1){

    }
}



