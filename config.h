#pragma once

#define MLCDCOM GPIO6, 1
#define MLCDCS GPIO6, 2
#define MLCDMOSI GPIO0, 1
#define MLCDSCLK GPIO0, 0

#define LORA_USE_TCXO 1
#define LORA_PIN_NSS GPIO4, 3
#define LORA_PIN_BUSY GPIO4, 7
#define LORA_PIN_DIO_1 GPIO4, 6
#define LORA_PIN_RESET GPIO5, 7
#define LORA_PIN_MOSI GPIO4, 0
#define LORA_PIN_MISO GPIO4, 1
#define LORA_PIN_SCLK GPIO4, 2
//#define LORA_PIN_ANT_SWITCH_POWER GPIO6, 1

#define ENABLE_HZK12