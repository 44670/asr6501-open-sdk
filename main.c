#include "vector.h"
#include "global.h"
#include "mlcd.h"
#include "sx126x.h"
#include "xsprintf.h"

vu32 t1 = 0x11223344;
vu32 t2 = 0x11223344;
vu32 t3 = 0x3;
int main()
{
    int i;
    socWdtDisable();
    delayMs(100);
    mlcdInit();
    fgColor = COLOR_WHITE;
    bgColor = COLOR_BLACK;
    fbSetCursor(0, 0);
    for (i = 0; i < 10; i++)
    {
        mlcdWriteLines(i * 16, 16, fb);
    }
    loraInit();
    char buf[10];
    while (1)
    {
        delayMs(1000);
        mlcdToggleCom();
    }
}