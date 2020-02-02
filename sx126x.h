#pragma once
#include "global.h"
#define SPI_CPOL 0
#define SPI_CPHA 0
#define SPI_MOSI LORA_PIN_MOSI
#define SPI_MISO LORA_PIN_MISO
#define SPI_SCLK LORA_PIN_SCLK
#define SPI_PREFIX(a) lora##a
#include "swspi_template.h"

#include "sx126x_def.h"

#define SX1262_PA_SELECT (2)
bool UseTCXO = false;
u8 gPaOptSetting;

static void BoardDisableIrq(void)
{
    //TODO: disable irq
}

static void BoardEnableIrq(void)
{
    //TODO: enable irq
}

static void SX126xWaitOnBusy(void)
{
    while (gpioReadPin(LORA_PIN_BUSY))
        ;
}

static void sx126xCheckReadyAndAcquireCS()
{
    SX126xCheckDeviceReady();

    gpioWritePin(LORA_PIN_NSS, 0);
}

static void sx126xReleaseCSAndWaitBusy()
{
    gpioWritePin(LORA_PIN_NSS, 1);

    // Wait for chip to be ready.
    SX126xWaitOnBusy();
}

static void SX126xReset(void)
{
    gpioSetPinMode(LORA_PIN_RESET, GPIO_MODE_OUTPUT);

    delayMs(20);
    gpioWritePin(LORA_PIN_RESET, 0);
    delayMs(40);
    gpioWritePin(LORA_PIN_RESET, 1);
    delayMs(20);
    //gpioSetPinMode(LORA_PIN_RESET, GPIO_MODE_ANALOG);
}

void SX126xWakeup(void)
{
    BoardDisableIrq();

    gpioWritePin(LORA_PIN_NSS, 0);
    loraXchg8(RADIO_GET_STATUS);
    loraXchg8(0x00);
    gpioWritePin(LORA_PIN_NSS, 1);

    // Wait for chip to be ready.
    SX126xWaitOnBusy();

    BoardEnableIrq();
}

void SX126xWriteRegisters(uint16_t address, uint8_t *buffer, uint16_t size)
{
    sx126xCheckReadyAndAcquireCS();
    loraXchg8(RADIO_WRITE_REGISTER);

    loraXchg8((address & 0xFF00) >> 8);
    loraXchg8(address & 0x00FF);

    for (uint16_t i = 0; i < size; i++)
    {
        loraXchg8(buffer[i]);
    }
    sx126xReleaseCSAndWaitBusy();
}

static void SX126xWriteRegister(uint16_t address, uint8_t value)
{
    SX126xWriteRegisters(address, &value, 1);
}

void SX126xReadRegisters(uint16_t address, uint8_t *buffer, uint16_t size)
{
    sx126xCheckReadyAndAcquireCS();

    gpioWritePin(LORA_PIN_NSS, 0);

    loraXchg8(RADIO_READ_REGISTER);
    loraXchg8((address & 0xFF00) >> 8);
    loraXchg8(address & 0x00FF);
    loraXchg8(0);
    for (uint16_t i = 0; i < size; i++)
    {
        buffer[i] = loraXchg8(0);
    }
    gpioWritePin(LORA_PIN_NSS, 1);

    sx126xReleaseCSAndWaitBusy();
}

uint8_t SX126xReadRegister(uint16_t address)
{
    uint8_t data;
    SX126xReadRegisters(address, &data, 1);
    return data;
}

void SX126xWriteBuffer(uint8_t offset, uint8_t *buffer, uint8_t size)
{

    sx126xCheckReadyAndAcquireCS();

    loraXchg8(RADIO_WRITE_BUFFER);
    loraXchg8(offset);
    for (uint16_t i = 0; i < size; i++)
    {
        loraXchg8(buffer[i]);
    }
    sx126xReleaseCSAndWaitBusy();
}

void SX126xReadBuffer(uint8_t offset, uint8_t *buffer, uint8_t size)
{
    sx126xCheckReadyAndAcquireCS();

    loraXchg8(RADIO_READ_BUFFER);
    loraXchg8(offset);
    loraXchg8(0);
    for (uint16_t i = 0; i < size; i++)
    {
        buffer[i] = loraXchg8(0);
    }
    sx126xReleaseCSAndWaitBusy();
}

void SX126xSetRfTxPower(int8_t power)
{
    SX126xSetTxParams(power, RADIO_RAMP_200_US);
}

uint8_t SX126xGetPaSelect(uint32_t channel)
{
    (void)channel;
    return SX1262_PA_SELECT; //SX1262
}

uint8_t SX126xGetPaOpt()
{
    return gPaOptSetting;
}

void SX126xSetPaOpt(uint8_t opt)
{
    if (opt > 3)
        return;

    gPaOptSetting = opt;
}

void SX126xAntSwOn(void)
{
}

void SX126xAntSwOff(void)
{
}

bool SX126xCheckRfFrequency(uint32_t frequency)
{
    (void)frequency;
    // Implement check. Currently all frequencies are supported
    return 1;
}

void DelayMsMcu(uint32_t ms)
{
    delayMs(ms);
}

static void loraInit()
{
    gpioWritePin(LORA_PIN_NSS, 1);
    gpioSetPinMode(LORA_PIN_NSS, GPIO_MODE_OUTPUT);
    gpioSetPinMode(LORA_PIN_BUSY, GPIO_MODE_INPUT);
    gpioSetPinMode(LORA_PIN_DIO_1, GPIO_MODE_INPUT);
    ;
    gpioWritePin(LORA_PIN_MOSI, 0);
    gpioSetPinMode(LORA_PIN_MOSI, GPIO_MODE_OUTPUT);
    gpioSetPinMode(LORA_PIN_MISO, GPIO_MODE_INPUT);
    gpioWritePin(LORA_PIN_SCLK, SPI_CPOL);
    gpioSetPinMode(LORA_PIN_SCLK, GPIO_MODE_OUTPUT);
    gpioWritePin(LORA_PIN_RESET, 1);
    gpioSetPinMode(LORA_PIN_RESET, GPIO_MODE_OUTPUT);
    delayMs(10);
    SX126xReset();
    delayMs(10);
}

#undef SPI_MOSI
#undef SPI_MISO
#undef SPI_SCLK
#undef SPI_CPOL
#undef SPI_CPHA
#undef SPI_PREFIX
