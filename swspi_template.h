#include "global.h"

#ifndef SPI_CPOL
#define SPI_CPOL 0
#endif
#ifndef SPI_CPHA
#define SPI_CPHA 0
#endif
#ifndef SPI_PREFIX
#define SPI_PREFIX(a) spi##a
#endif
#ifndef SPI_SIZE_T
#define SPI_SIZE_T u16
#endif

static FORCE_INLINE void SPI_PREFIX(Write8) (u8 dat){
    u8 i;
    for (i = 0; i < 8; i++)
    {
        gpioWritePin(SPI_MOSI, (dat & 0x80));
        gpioWritePin(SPI_SCLK, !SPI_CPOL);
        dat <<= 1;
        gpioWritePin(SPI_SCLK, SPI_CPOL);
    }
}

static void SPI_PREFIX(WriteBuf)(u8* buf, SPI_SIZE_T len) {
    while(len) {
        SPI_PREFIX(Write8)(*buf);
        buf ++;
        len --;
    }
}

#ifdef SPI_MISO
static FORCE_INLINE u8 SPI_PREFIX(Xchg8) (u8 dat){
    u8 i;
    u8 ret = 0;
    for (i = 0; i < 8; i++)
    {
        gpioWritePin(SPI_MOSI, (dat & 0x80));
        gpioWritePin(SPI_SCLK, !SPI_CPOL);
        dat <<= 1;
        ret <<= 1;
        ret |= gpioReadPin(SPI_MISO);
        gpioWritePin(SPI_SCLK, SPI_CPOL);
    }
    return ret;
}
static void SPI_PREFIX(XchgBuf)(u8* buf, SPI_SIZE_T len) {
    while(len) {
        SPI_PREFIX(Write8)(*buf);
        buf ++;
        len --;
    }
}

#endif

