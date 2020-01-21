import os

CFLAGS= ' --static -nostartfiles -std=c11 -g3 -Os'
CFLAGS += ' -fno-common -ffunction-sections -fdata-sections'
CFLAGS += ' -I./libopencm3/include -L./libopencm3/lib'
CFLAGS += ' -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group'
CFLAGS += ' -Wl,-T ld.psoc4.basic'
CFLAGS += ' -mcpu=cortex-m0 -mthumb -msoft-float'

PREFIX='arm-none-eabi-'

CC = PREFIX + 'gcc'

def run(cmdline):
    print(cmdline)
    os.system(cmdline)

run('rm firm.elf firm.hex')
run(CC + CFLAGS +  ' main.c' + ' -o firm.elf')