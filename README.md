# Exploring Embedded Startup

This is a project that investigates some of the details of startup for microcontrollers. It is designed to run on the Ambiq Apollo3 microcontroller. SparkFun offers some [good development boards called 'Artemis'](https://www.sparkfun.com/artemis) that work well to try this demo. If not using a SparkFun board you may need to modify your work accordingly.

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

# The Startup Process
When learning how to write code most people start at ```main```. When using a scripting language like Python the ```main``` might be implied but regardless you are presented with a standard environment with a lot of support. For example C++ and Python both have built-in methods to display information. This means that there must be something going on behind the scenes but it doesn't really matter - we take it for granted. 

Not any more.

We're going to get into the nitty-gritty so there are no more questions about how we wound up at ```main```.

## How Computers Work (quickly)


## 


# Linker Scripts

# Explanation of Examples
