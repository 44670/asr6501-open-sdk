#pragma once
#include "global.h"
#include "cydevice_trm.h"


#define PSOC_4100S
#define CORTEX_M0


#define GPIO0 CYDEV_GPIO_PRT0_BASE
#define GPIO1 CYDEV_GPIO_PRT1_BASE
#define GPIO2 CYDEV_GPIO_PRT2_BASE
#define GPIO3 CYDEV_GPIO_PRT3_BASE
#define GPIO4 CYDEV_GPIO_PRT4_BASE
#define GPIO5 CYDEV_GPIO_PRT5_BASE
#define GPIO6 CYDEV_GPIO_PRT6_BASE



#define GPIO_PC(p) *(vu32 *)(p + 8)
#define GPIO_PS(p) *(vu32 *)(p + 4)
#define GPIO_DR(p) *(vu32 *)(p)

#define GPIO_MODE_OFF (0x0)
#define GPIO_MODE_INPUT (0x1)
#define GPIO_DM_0_PU (0x2)
#define GPIO_DM_PD_1 (0x3)
#define GPIO_DM_0_Z (0x4)
#define GPIO_DM_Z_1 (0x5)
#define GPIO_DM_0_1 (0x6)
#define GPIO_DM_PD_PU (0x7)
#define GPIO_MODE_OUTPUT GPIO_DM_0_1
#define GPIO_MODE_ANALOG GPIO_MODE_OFF

static void gpioSetPinMode(u32 gpioport, u8 pin, u8 dm)
{
    uint32_t PCVAL = GPIO_PC(gpioport);
    dm &= 0b111;                      // Ensure drive mode is in range
    PCVAL &= ~(((u32)0b111) << (u32)(3 * pin)); // Clear the current value
    PCVAL |= ((u32)dm) << (u32)(3 * pin);         // Update the value for the pin
    GPIO_PC(gpioport) = PCVAL;        // Write the new pin value
}

static FORCE_INLINE void gpioWritePin(u32 gpioPort, u8 pin, u8 val)
{
    if (val)
    {
        GPIO_DR(gpioPort) |= (1 << pin);
    }
    else
    {
        GPIO_DR(gpioPort) &= ~(1 << pin);
    }
}

static FORCE_INLINE u8 gpioReadPin(u32 gpioPort, u8 pin) {
    return (GPIO_PS(gpioPort) & (1 << pin)) ? 1 : 0;
}


// The watchdog counter is a free-running counter that cannot be disabled. 
// However, it is possible to disable the watchdog reset by writing a key '0xACED8865' to the WDT_DISABLE_KEY register. 
static void socWdtDisable() {
    MMIO32(CYREG_WDT_DISABLE_KEY) = 0xACED8865u;
}

// 0: ILO, 1: WCO
// Only the ILO can be the source for LFCLK in the PSoC 4100S and PSoC 4100S Plus 256 KB device.  
/*
static void clkSetLfclkSource(u32 source) {
    MMIO32(CYREG_WCO_WDT_CONFIG) &= ~((u32)3 << CYFLD_WCO_LFCLK_SEL__OFFSET);
    MMIO32(CYREG_WCO_WDT_CONFIG) |= (u32)source << CYFLD_WCO_LFCLK_SEL__OFFSET;
}
*/


static void delayUs(u32 us) {
    vu32 i = us;
    while(i) {
        i --;
    }
}

static void delayMs(u32 ms) {
    volatile u32 i;
    for (i = 0; i < ms; i++) {
        delayUs(1000);
    }
}