# Exploring Embedded Startup

This is a project that investigates some of the details of startup for microcontrollers. It is designed to run on the Ambiq Apollo3 microcontroller. SparkFun offers some [good development boards called 'Artemis'](https://www.sparkfun.com/artemis) that work well to try this demo. If not using a SparkFun board you may need to modify your work accordingly.

The text (below) that accompanies these examples focuses on the GNU tools GCC ad LD. 

## Contents
* **Getting Started**
* **Exlanation of Examples**

# Getting Started:

\* note: shell commands are bash-like - if using Windows CMD or PowerShell you may need to substitute equivalent commands

1. Get a working development environment that is able to compile and upload examples (choose *one* of these options, listed simplest to hardest)
    * Use the [Artemis Dev Platform](https://github.com/sparkfun/artemis_dev_platform)
    * Use [SparkFun's mirror of the Ambiq SDK](https://github.com/sparkfun/AmbiqSuiteSDK) **and** install required toolchain elements like ```arm-nne-eabi-gcc``` and ```make```.
    * Use Ambiq's [original SDK](https://s3-us-west-1.amazonaws.com/s3.ambiqmicro.com/downloads/AmbiqSuite-R2.3.2.zip), add [SparkFun's Board Support Packages](https://github.com/sparkfun/SparkFun_Apollo3_AmbiqSuite_BSPs) manually, and customize your installation of toolchain elements.
1. Set up paths

    Env Var. | Value | Required | Notes
    --- | --- | --- | ---
    BOARDPATH | $PATH_TO_BSPS/BOARD | Yes | BOARD may be ```redboard_artemis```, ```edge```, ```artemis_thing_plus``` etc...
    COMMONPATH | $PATH_TO_BSPS/common | When not using Artemis Dev Platform | This path should point to the common directory of the SparkFun BSPs
    SDKPATH | $PATH_TO_SDK | When not using Artemis Dev Platform | This path should point to the root of the AmbiqSuite SDK
        
    (do this by setting the variables temporarily e.g. ```export SDKPATH=/usr/AmbiqsuiteSDK```)
        

1. Build an example
    * Start at the root directory of this repo
    * Enter the example ```gcc``` build driectory ```cd $EXAMPLE/gcc```
    * Build the example using the ASB linkerscript (this utilizes the baked-in bootloader of the Apollo3) ```make asb```
        * (if you incorrectly specify the paths to important components you may see errors such as ```fatal error: am_mcu_apollo.h: No such file or directory``` or ```fatal error: am_bsp.h: No such file or directory```)
    
1. Upload the binary
    * Begin in the example's ```gcc``` directory
    * Suggested method: use or study the upload script included in the SparkFun BSPs
        ```./$PATH_TO_BSPS/common/scripts/upload_bin_asb.sh -f bin/$EXAMPLE_asb.bin -p /dev/ttyUSB0``` (on windows ```/dev/ttyUSB0``` --> ```COM4``` ) 
    * Detailed instructions coming later (you might need to ask for them)

# Background
When learning how to write code most people start at ```main```. When using a scripting language like Python the ```main``` might be implied but regardless you are presented with a standard environment with a lot of support. For example C++ and Python both have built-in methods to display information. This means that there must be something going on behind the scenes but it doesn't really matter - we take it for granted. 

Not any more.

We're going to get into the nitty-gritty so there are no more questions about how we wound up at ```main```.

## How Computers Work (quickly)
To gain a useful perspective we will need to start from the right vantage point - one in which we maintain a disdain for 'magic'. It will also be useful to assume that a computer has several capabilities which are all enabled by well-known physical processes.

1. Storage of information (data and/or instructions)
1. Execution of a set of pre-deefined instructions

More specifically when we consider the vast majority of computers around us we can say this about their operation:

1. Instructions and data are stored in a non-volatile form of memory
1. A 'program counter' keeps track of which instruction should be executed next
1. Instructions are followed to manipulate information in a volatile memory pace by a control unit and an arithmetic unit
1. A **reset circuit** is used to put the program counter into a known initial state.

Given these capabilities and a sufficient set of instructions the computer will be '[turing complete](https://en.wikipedia.org/wiki/Turing_completeness)' and therefore be able to handle all the same tasks as any other modern computer.

There are some additional concepts that while not necessarily being distinct parts of a computer are still helpful to call by name.

**(Sub)Routines**: Small sets of instructions that are re-usable and provide some oft-needed capability (e.g. a routine to add two numbers)
**Stack**: Memory that tracks the context of execution, particularly as one routine calls another and creates a nested structure
**Heap**: Memory that was not known until the program began to run and handle external inputs

## Compilation
For every source file in a project the compiler generates an object file which contains the necessary instructions as well as hints about missing information. A typical program on a microcontroller may not have any missing information so several object (or archive) files may need to be combined to fill in all the gaps. This is where the linker comes in.

## Linker
The GNU linker 'LD' ([manual](https://ftp.gnu.org/old-gnu/Manuals/ld-2.9.1/html_mono/ld.html)) is used to combine a number of object files into a single executable 'image.' As a means to that end the linker also arranges the memory resources of the specific computer for which the program is intended.

## Putting it All Together
Execution of a program is made possible thanks to the existing infrastructure of the computer and the ability to maniuplate memory manually (to load the program). When reset the CPU will begin following instructions at the agreed upon location and the sequence of instructions that follows defines the program. 

# The Startup Process
What we are referring to here is not the same 'hardware startup' process that the reset circuit takes care of. Instead it means the preparation of the software environment so that the remainder of the program can operate as expected. There are a few important tasks to complete before entering ```main```:

1. Initialize global and static variables 
1. Prepare the stack/heap
1. Call global/static contructors (in C++)

The examples in this repo explain how the compiler and linker work together to acheive these goals.

# Explanation of Examples

## The Framework:
The examples in this repo all use a common structure so that we can observe what changes when certain features are required by the progam in ```main.c```.

### startup_gcc.c
This file organizes all the code that is used to set up the software environment for the examples. 

#### Interrupt Handlers
The Cortex-M core of the Apollo3 can respond to asynchronous events by changing the program counter to a special value depending on the cause of the event. The startup file supports this by arranging functon calls at these special locations.

1. Named function declarations are made, most of which are aliased to a default handler weakly so that they can be overriden (that's compiler stuff - we don't worry about it right now)
1. These handlers are arranged in the correct order by placing them inside an array called ```g_am_pfnVectors```
1. That array is 'tagged' with an attribute that gives it a special input section name '.isr_vector' so that it can be located by the linker script

#### BLE Patch
A small section (16 4-byte words) is reserved for applying software updates to the BLE co-processor. This gets located directly after the ISR vector table.

#### Reset Handler
With help from the linker this function actually represents the first entry point of the program. Its purpose is to set up the needed SW environment and then call ```main``` so that the application code can run.

Within the reset handler there are six distinct tasks:

1. Set the vector table pointer (the Cortex-M core supports a relocatable vector table so we need to specify where it is before any interrupts occur)
1. Set the stack pointer (for flexibility the Cortex-M allows the stack to exist anywhere in memory)
1. Enable the Floating Point Unit (which provides HW acceleration for floating point arithmetic)
1. Copy non-trivial data from non-volatile memory to RAM (part of initializing global/static values)
1. Copy trivial data from non-volatile memory to RAM (the other part of initializing values)
1. Call ```main```

### AMA3B1KK.ld
This linker script works in concert with ```startup_gcc.c``` to set up the environment.

1. Specify the ENTRY point for the program 
    ```ENTRY(Reset_Handler)```
1. Specify available memory regions and their capabilitites
    ```
    MEMORY
    {
      flash (rx) : ORIGIN = 0x0000C000, LENGTH = 960K
      sram (rwx) : ORIGIN = 0x10000000, LENGTH = 384K
    }
    ```
1. Specify where in memory to locate various types of output from the compiler
    ```
    SECTIONS
    {
    ```
    1. Locate instructions (text) first, and specifically put the ISR vector and the BLE patch at the very first memory addresses of flash
    1. Store non-trivial data in flash memory next so that it can be recovered whenever the processor resets - also reserves a space for the same data to be stored in RAM
    1. Designate a space in RAM for trivially initialized values (a.k.a. zero-initialized)
    1. Define a symbol ```_sheap``` to indicate the initial location of the heap, which coincides with ```__end__``` (the end of initializer data)
    1. Define a symbol ```_sstack``` to indicate the initial location of the stack (which is pushed to the other end of RAM by the empty ```dmz``` section) 
    ```
    }
    ```

## [Example 1: Bare Bones](https://github.com/oclyke/exploration-embedded-startup/blob/master/example1_barebones/README.md)
This example shows how the framework responds to the trivial program:

``` c
/* example1_barebones */

int main()
{
    while(1){
        
    }
}
```

The only source files used are ```main.cpp``` and ```startup_gcc.c```.

Since there are no initialized values to handle both the data and bss loops run straight through in as few cycles as possible:

Assembly block | Number of Cycles
--- | ---
copy data segment | 5
zero fill bss | 7

## [Example 2: Data](https://github.com/oclyke/exploration-embedded-startup/blob/master/example2_data/README.md)
Now we add non-trivial initial values to see how the framework responds. Still, the only source files used are ```main.cpp``` and ```startup_gcc.c```.

The number of cycles taken in the copy loop (data section) is related to the number of unique bytes of initialization data 

Number of unique bytes | Number of Cycles in Copy Loop
--- | ---
0 | 5
1 | 10
2 | 10
3 | 10
4 | 10
5 | 15
6 | 15
7 | 15
8 | 15

## [Example 3: BSS](https://github.com/oclyke/exploration-embedded-startup/blob/master/example3_bss/README.md)
As a counterpart to the data section the BSS allows the compact expression of many trivially initialized variables. Like the previous examples only ```main.cpp``` and ```startup_gcc.c``` are used in the compilation.

Number of bytes | Number of Cycles in BSS Fill Loop
--- | ---
0 | 7
1 | 11
2 | 11
3 | 11
4 | 11
5 | 15
6 | 15
7 | 15
8 | 15
9 | 19

## [Example 4: Stack](https://github.com/oclyke/exploration-embedded-startup/blob/master/example4_stack/README.md)
This example adds a fair deal of complexity in order to test the operation of the stack. To the compilation we add two more source files which are provided in the SDK to handle printing to the UART (so that we can easily read test results)

The example first tests the maximumm recursive call depth - which relies on stack functionality. Because no memory is dynamically allocated nearly all of the system RAM can be dedicated to tracking context. 

## [Example 5: Heap](https://github.com/oclyke/exploration-embedded-startup/blob/master/example5_heap/README.md)
This example tests dynamic memory allocation using ```malloc```. It requires a definition of ```_sbrk``` which uses the stack and heap locations to determine if free memory exists. 

## [Example 6: Constructors](https://github.com/oclyke/exploration-embedded-startup/blob/master/example6_constructors/README.md)
A special example just for C++. Shows what it takes to ensure that gobal/static constructors are called before the user begins application code. 

# Going Further

## Helpful Links
https://ftp.gnu.org/old-gnu/Manuals/ld-2.9.1/html_chapter/ld_3.html
https://sourceware.org/binutils/docs/ld/Output-Section-LMA.html
https://onlinedisassembler.com/static/home/index.html
https://embeddedartistry.com/blog/2019/04/17/exploring-startup-implementations-newlib-arm/
https://bitbashing.io/embedded-cpp.html
