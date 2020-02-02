#pragma once
#include "global.h"
#define MLCD_COMMAND_BYTE_1 (0x01)
#define MLCD_HEIGHT (176)
#define MLCD_WIDTH (176)
#define MLCD_LINEBUF_SIZE (MLCD_WIDTH / 8 * 4)
#define MLCD_FB_LINES (12)
#define MLCD_ADD_COM(x) (mlcdCom ? ((x) + 2) : (x))

#define COLOR_BLACK (0b000)
#define COLOR_RED (0b001)
#define COLOR_GREEN (0b010)
#define COLOR_BLUE (0b100)
#define COLOR_WHITE (0b111)

#include "vgafont8.h"

u8 fb[MLCD_LINEBUF_SIZE * MLCD_FB_LINES];
u8 mlcdCom;
u8 fgColor;
u8 bgColor;

static u8 bitswap8(u8 dat)
{
    u8 ret = 0;
    u8 i;
    for (i = 0; i < 8; i++)
    {
        ret |= (((dat >> i) & 1) ? 1 : 0) << (7 - i);
    }
    return ret;
}
static FORCE_INLINE void mlcdWrite8(u8 dat)
{
    u8 i;
    for (i = 0; i < 8; i++)
    {
        gpioWritePin(MLCDMOSI, (dat & 0x1));
        gpioWritePin(MLCDSCLK, 1);
        dat >>= 1;
        gpioWritePin(MLCDSCLK, 0);
    }
}

static void mlcdWriteBuf(u8 *buf, u8 len)
{
    while (len)
    {
        mlcdWrite8(*buf);
        buf++;
        len--;
    }
}

static void mlcdWriteLines(u8 y, u8 height, u8 *lines)
{
    y += 1; // row starts from 1
    gpioWritePin(MLCDCS, 1);
    delayUs(3);
    // 4 bit write
    mlcdWrite8(MLCD_ADD_COM(1 + 8));
    while (height)
    {
        mlcdWrite8(bitswap8(y));
        mlcdWriteBuf(lines, MLCD_LINEBUF_SIZE);
        mlcdWrite8(0x00);
        y++;
        lines += MLCD_LINEBUF_SIZE;
        height--;
    }
    mlcdWrite8(0x00); // trailing paddings
    delayUs(1);

    gpioWritePin(MLCDCS, 0);
    delayUs(1); // can I delete this delay?
}

static void mlcdToggleCom()
{
    mlcdCom = !mlcdCom;
#ifdef MLCDCOM
    gpioWritePin(MLCDCOM, mlcdCom);
#endif
}

static void mlcdInit()
{
    gpioSetPinMode(MLCDCS, GPIO_MODE_OUTPUT);
    gpioSetPinMode(MLCDSCLK, GPIO_MODE_OUTPUT);
    gpioSetPinMode(MLCDMOSI, GPIO_MODE_OUTPUT);
    gpioWritePin(MLCDCS, 0);
    gpioWritePin(MLCDSCLK, 0);
    gpioWritePin(MLCDMOSI, 0);

#ifdef MLCDCOM
    gpioSetPinMode(MLCDCOM, GPIO_MODE_OUTPUT);
    gpioWritePin(MLCDCOM, 0);
#endif
}

u16 fbCursorX, fbCursorY;

static FORCE_INLINE void fbSetPixel(u16 x, u16 y, u8 color)
{
    color &= 7;

    if (y >= MLCD_FB_LINES)
    {
        return;
    }
    if (x >= MLCD_WIDTH)
    {
        return;
    }
    // 00001111 22223333
    u8 *ptr = fb + y * MLCD_LINEBUF_SIZE + x / 2;
    if ((x & 1) == 0)
    {
        *ptr &= 0xf0;
        *ptr |= color;
    }
    else
    {
        *ptr &= 0x0f;
        *ptr |= color << 4;
    }
}

static void fbDrawCharAscii8(u8 ch)
{
    const u8 *ptr = vgafont8 + ch * 8;
    for (u16 row = 0; row < 8; row++)
    {
        u8 t = *ptr;
        ptr++;
        for (u16 col = 0; col < 8; col++)
        {
            fbSetPixel(fbCursorX + col, fbCursorY + row, (t & 0x80) ? fgColor : bgColor);
            t <<= 1;
        }
    }
    fbCursorX += 8;
}

#ifdef ENABLE_HZK12

#define HZK12_COMPRESSED_FONT_SIZE (3 * 6)
#define FIRSTPLANE_ONLY

#include "font_hzk12c_firstplane.h"

static void fbDrawCharGb2312Hzk12Compressed(u8 ch1, u8 ch2)
{
    u8 qu = ch1 - 0xA0;
    u8 wei = ch2 - 0xA0;
    u8 outOfFont = 0;
    if ((qu > 88) || (wei > 94))
    {
        outOfFont = 1;
    }
#ifdef FIRSTPLANE_ONLY
    if (!((qu >= 16) && (qu <= 55)))
    {
        outOfFont = 1;
    }
#endif
    if (outOfFont)
    {
        fbDrawCharAscii8(ch1);
        fbDrawCharAscii8(ch2);
        return;
    }
    if (qu > 15)
    {
        qu -= 6;
    }
#ifdef FIRSTPLANE_ONLY
    qu -= 9;
#endif
    qu -= 1; wei -= 1;

    u8 *ptr = (u8 *)(hzk12c_firstplane + HZK12_COMPRESSED_FONT_SIZE * (qu * 94 + wei));

    u16 cnt = 0;
    u8 t = 0;
    for (u16 row = 0; row < 12; row++)
    {
        for (u16 col = 0; col < 12; col++)
        {
            if ((cnt % 8) == 0)
            {
                t = *ptr;
                ptr++;
            }
            fbSetPixel(fbCursorX + col, fbCursorY + row, (t & 0x80) ? fgColor : bgColor);
            t <<= 1;
            cnt++;
        }
    }
    fbCursorX += 12;
}

static u8 isCharInGB2312(u8 ch, u8 ch2)
{
    if (((ch >= 0xA1) && (ch <= 0xF8)))
    {
        if ((ch2 >= 0xA1) && (ch2 <= 0xFE))
        {
            return 1;
        }
    }
    return 0;
}
#endif

static void fbDrawString(const char *str)
{
    while (*str)
    {
        u8 ch = (u8)(*str);
#ifdef ENABLE_HZK12
        u8 ch2 = (u8)(*(str + 1));
        if (isCharInGB2312(ch, ch2))
        {
            fbDrawCharGb2312Hzk12Compressed(ch, ch2);
            str++;
        }
        else
        {
            fbDrawCharAscii8(ch);
        }
#else
     fbDrawCharAscii8(ch);
#endif

        str++;
    }
}

static void fbSetCursor(u8 x, u8 y)
{
    fbCursorX = x;
    fbCursorY = y;
}

static void fbClear()
{
    memset(fb, 0, sizeof(fb));
    fbSetCursor(0, 0);
}