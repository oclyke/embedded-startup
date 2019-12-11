The stack keeps track of the context of execution. 

A special register / variable is used to keep track of the stack - this is called the stack pointer.

On the Apollo3 (and Cortex-M4 and most other ARM processor--based microcontrollers) the stack pointer
is manipulated with instructions such as:
push
pop

These functions make it easy to manage the stack and hence easy to keep track of the context of calling 
functions and arguments.

The Stack may be placed anywhere in RAM but to afford that flexibility one of the first things that 
needs to be taken care of in startup is setting the initial stack pointer value. In this example that
is taken care of like this:

- The linker script defines regions in the RAM
  - data: uniquely-initialized variables, fixed length
  - bss: trivially initialized variables, fixed length
  - heap: initially zero-length region that can be dynamically allocated from low to high addresses
  - dmz: an empty section filled to the remaining length of the RAM so that the next section is at the end
  - stack: initially zero-length region that can be dynamically used from high to low addresses (growing down)
- Within the 'stack' region the linker provides the address for two variables:
  - '_sstack' denotes the start of the stack - in this example it is the highest accessible memory address
  - '_estack' denoted the end of the stack -it starts with the same value as the start of the stack but it can move as items are added or removed from the stack. (in this example, however, '_estack' does not change value during execution because the stack is largely managed in the processor hardware)
- During startup the vector table and stack pointers are set from known locations
  - The vector table provides all the important 'known locations' and it is given a tag for the '.isr_vector' section which is used to ensure that it begins at memory location 0x00000000.
  ```c
  __attribute__ ((section(".isr_vector")))
  void (* const g_am_pfnVectors[])(void) =
  {
      (void (*)(void))(&_sstack),             // The initial stack pointer (provided by linker script)
      Reset_Handler,                          // The reset handler
      ...
      am_clkgen_isr,                          // 31: CLKGEN
  };
  ```
  - Setting the vector table pointer is done by placing the address of 'g_am_pfnVectors' (the vector table) into the memory address '0xE000ED08' (still need to verify this against the datasheet to be 100% sure)
    ```
        ldr    r0, =0xE000ED08
        ldr    r1, =g_am_pfnVectors
        str    r1, [r0]
    ```
  - Setting the stack pointer is done by storing the first value in the vector table to the stack pointer
    ```
        ldr    sp, [r1]
    ```

## Key Info
