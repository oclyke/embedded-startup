//*****************************************************************************
//
//! @file blinky.c
//!
//! @brief Simple blinky example for the Apollo EVK.
//!
//! This example runs various patterns on the EVB LEDs and sleeps in between
//! the pattern updates.
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright 2015-2017
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "apollo1.h"
#include <core_cm4.h>


//*****************************************************************************
//
// Define LEDs, mapping them to their respective GPIO numbers.
//
//*****************************************************************************
//
// The Apollo1 EVB (circa 2017) uses the following LEDs:
// D90 = GPIO34 (PADREGI, GPIOCFGE)
// D91 = GPIO33 (PADREGI, GPIOCFGE)
// D92 = GPIO28 (PADREGH, GPIOCFGD)
// D93 = GPIO26 (PADREGG, GPIOCFGD)
// D94 = GPIO25 (PADREGG, GPIOCFGD)
//
#define EVB_LED0        34
#define EVB_LED1        33
#define EVB_LED2        28
#define EVB_LED3        26
#define EVB_LED4        25

//*****************************************************************************
//
// Globals
//
//*****************************************************************************
//
// Software timer updated by ISR.
//
volatile uint32_t ui32_SoftwareCounter = 0;

//*****************************************************************************
//
// Constant data
//
//*****************************************************************************
//
// Keep the LED pattern array in FLASH instead of SRAM.
//
static const uint8_t led_pattern[64] =
{
    //
    // Binary count up
    //
      1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16,
     17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,  0,

    //
    // Rotote left pattern
    //
    0x01, 0x02, 0x04, 0x08, 0x10, 0x01, 0x02, 0x04, 0x08, 0x10, 0x00,

    //
    // Rotate right pattern
    //
    0x10, 0x08, 0x04, 0x02, 0x01, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00,

    //
    // Springing pattern
    //
    0x15, 0x0A, 0x15, 0x0A, 0x15, 0x0A, 0x15, 0x0A, 0x15, 0x00
};

//*****************************************************************************
//
// Set or clear an LED.
// The LEDs on the EVB are common anode (anodes tied high).
// Therefore, they are turned on when the GPIO is cleared.
// Likewise, they are turned off when the GPIO is set.
//
//*****************************************************************************
static void
LED_on(uint32_t ui32LED, bool bOn)
{
    uint32_t ui32Mask;

    if ( ui32LED <= 31 )
    {
        ui32Mask = 0x01 << (ui32LED - 0);

        if ( bOn )
        {
            GPIO->WTCA = ui32Mask;
        }
        else
        {
            GPIO->WTSA = ui32Mask;
        }
    }
    else if ( ui32LED <= 49 )
    {
        ui32Mask = 0x01 << (ui32LED - 32);

        if ( bOn )
        {
            GPIO->WTCB = ui32Mask;
        }
        else
        {
            GPIO->WTSB = ui32Mask;
        }
    }
    else
    {
        // ERROR.
        while (1);
    }

} // LED_on()

//*****************************************************************************
//
// Set a PADREG FNCSEL field.
//
//*****************************************************************************
static void
padreg_funcsel_set(uint32_t ui32GPIOnum, uint32_t ui32Func)
{
    uint32_t ui32Shift;
    uint32_t volatile *pui32Reg;

    //
    // Unlock writes to the GPIO and PAD configuration registers.
    //
    GPIO->PADKEY = 0x73;

    //
    // Determine and configure the PADREG.
    // Each PADREG configures 4 GPIOs, each field divided into bytes.
    //
    ui32Shift = (ui32GPIOnum & 0x3) * 8;

    //
    // Since there are 4 GPIOs per PADREG, the GPIO number also happens to
    // be a byte offset to the PADREG address. Since we have a word ptr, we
    // need to make it a word offset (so divide by 4).
    //
    pui32Reg   = &(GPIO->PADREGA) + (ui32GPIOnum / 4);
    *pui32Reg &= ~(0xFF << ui32Shift);

    //
    // The FNCSEL field lsb is 3 bits into the PADREG field.
    // FNCSEL can be 2 or 3 bits wide.
    //
    *pui32Reg |= (ui32Func & 0x7) << (ui32Shift + 3);

    //
    // Lock PAD configuration registers.
    //
    GPIO->PADKEY = 0;
}

//*****************************************************************************
//
// Write a GPIO CFG field.
//
//*****************************************************************************
static void
gpio_cfg_set(uint32_t ui32GPIOnum, uint32_t ui32CFG)
{
    uint32_t ui32Shift;
    uint32_t volatile *pui32Reg;

    //
    // Unlock writes to the GPIO and PAD configuration registers.
    //
    GPIO->PADKEY = 0x73;

    //
    // Configure the GPIO as push pull outputs for use with an LED.
    // Each GPIOCFG configures 8 GPIOs, each divided into nibbles.
    //
    ui32Shift  = (ui32GPIOnum & 0x7) * 4;
    pui32Reg   = &(GPIO->CFGA) + ((ui32GPIOnum / 8));
    *pui32Reg &= ~(0xF << ui32Shift);

    //
    // The OUTCFG field lsb is 1 bit into the CFG field.
    // OUTCFG is 2 bits wide.
    //
    *pui32Reg |= ((ui32CFG & 0x3) << (ui32Shift + 1));

    //
    // Lock PAD configuration registers.
    //
    GPIO->PADKEY = 0;

} // gpio_cfg_set()

//*****************************************************************************
//
// Configure a GPIO for use with an LED.
//
//*****************************************************************************
void
LED_gpio_cfg(uint32_t ui32GPIOnum)
{
    if ( ui32GPIOnum > 49 )
    {
        //
        // Error
        //
        while(1);
    }

    //
    // Configure the PAD for GPIO (FUNCSEL=3).
    //
    padreg_funcsel_set(ui32GPIOnum, 3);

    //
    // Configure the GPIO output for PUSHPULL (OUTCFG=1).
    //
    gpio_cfg_set(ui32GPIOnum, 1);

} // LED_gpio_cfg()

//*****************************************************************************
//
// Initialize GPIO PADS to drive the LEDs and enable SWO
//
//*****************************************************************************
void
GPIO_init(void)
{
    //
    // Configure pads for LEDs as GPIO functions.
    //
    //
    LED_gpio_cfg(EVB_LED0);
    LED_gpio_cfg(EVB_LED1);
    LED_gpio_cfg(EVB_LED2);
    LED_gpio_cfg(EVB_LED3);
    LED_gpio_cfg(EVB_LED4);

    //
    // While we're at it, also configure GPIO41 for SWO.
    //
    padreg_funcsel_set(41, 2);

    //
    // Initialize the LEDs to all on.
    //
    LED_on(EVB_LED0, true);
    LED_on(EVB_LED1, true);
    LED_on(EVB_LED2, true);
    LED_on(EVB_LED3, true);
    LED_on(EVB_LED4, true);

} // GPIO_init()

//*****************************************************************************
//
// CTIMER_init
//
//*****************************************************************************
void
CTIMER_init(void)
{
    //
    // Setup timer A0 to count LFRC clocks at 32Hz (0xB)
    //
    CTIMER->CTRL0 = CTIMER_CTRL0_TMRA0CLR_Msk | CTIMER_CTRL0_TMRB0CLR_Msk;
    CTIMER->CTRL0 = (3 << CTIMER_CTRL0_TMRA0FN_Pos)  |       // PWM REPEAT  MODE 3
                    CTIMER_CTRL0_TMRA0IE_Msk         |       // Interrupt Enable
                    (0xB << CTIMER_CTRL0_TMRA0CLK_Pos);      // LFRC 32Hz, i.e. LFRC divided by 32 = 32 Hz

    //
    // Setup timer A0 to count to 1Hz and interrupt (divide by 32)
    // Period = 32 on = 16 for mode 3.
    //
    CTIMER->CMPRA0 = 0x00200010;

    //
    // Clear the timer interrupt.
    //
    CTIMER->INTCLR = 0x000000FF;

    //
    // Set the interrupt enable for Timer A0.
    //
    CTIMER->INTEN = 0x00000001;

    //
    // Enable CTIMER interrupts in the NVIC.
    //
    NVIC_EnableIRQ(CTIMER_IRQn);

    //
    // Start the timer.
    //
    CTIMER->CTRL0 |= CTIMER_CTRL0_TMRA0EN_Msk;

} // CTIMER_init()

//*****************************************************************************
//
// setSleepMode
//
//*****************************************************************************
void
setSleepMode (int bSetDeepSleep)
{
  if (bSetDeepSleep)
  {
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
  }
  else
  {
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
  }

} // setSleepMode()

//*****************************************************************************
//
// Timer interrupt service routine: increment software counter at 1 Second
//                                  intervals and output to the LEDs
//                                  Entered once per second
//
//*****************************************************************************
void
am_ctimer_isr(void)
{
    uint32_t ui32Value;

    //
    // Clear timer interrupt.
    //
    CTIMER->INTCLR = 0x00000001;    // AM_REG_CTIMER_INT_TIMERA0

    //
    // look up next interesting pattern
    //
    ui32Value = led_pattern[ui32_SoftwareCounter];

    //
    // Increment software counter
    //
    ui32_SoftwareCounter = ++ui32_SoftwareCounter & 0x0000003F;

    //
    // Copy software counter to LEDs.
    //
    LED_on( EVB_LED0, (bool)(ui32Value & 0x00000001) );
    LED_on( EVB_LED1, (bool)(ui32Value & 0x00000002) );
    LED_on( EVB_LED2, (bool)(ui32Value & 0x00000004) );
    LED_on( EVB_LED3, (bool)(ui32Value & 0x00000008) );
    LED_on( EVB_LED4, (bool)(ui32Value & 0x00000010) );

} // am_ctimer_isr()

//*****************************************************************************
//
// Main
//
//*****************************************************************************
int
main(void)
{
    //
    // Initialize GPIO PADs we will use.
    //
    GPIO_init();

    //
    // Initialize timer.
    //
    CTIMER_init();

    //
    // Configure type of sleep (0=normal sleep, 1=deep sleep).
    //
    setSleepMode(1);

    //
    // Enable interrupts in NVIC.
    //
    __enable_irq();

    //
    // Wait for the timer interrupt.
    //
    while(1)
    {
        //
        // Sleep here
        //
        __disable_irq();
        __WFI();
        __enable_irq();
    }
}
