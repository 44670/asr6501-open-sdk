import os

CFLAGS= ' --static -nostartfiles -std=c11 -g3 -Os'
CFLAGS += ' -fno-common -ffunction-sections -fdata-sections'
#CFLAGS += ' -I./libopencm3/include -L./libopencm3/lib'
CFLAGS += ' -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group'
CFLAGS += ' -Wl,-T ld.psoc4.basic'
CFLAGS += ' -Wl,--gc-sections'
#CFLAGS += ' -Wl,--print-gc-sections'
CFLAGS += ' -mcpu=cortex-m0 -mthumb -msoft-float'
CFLAGS += ' -Wall -Wno-unused-function'

PREFIX='arm-none-eabi-'

CC = PREFIX + 'gcc'
OD = PREFIX + 'objdump'
OC = PREFIX + 'objcopy'

def run(cmdline):
    print(cmdline)
    os.system(cmdline)

run('rm firm.elf firm.hex firm.asm')
run(CC + CFLAGS +  ' main.c' + ' -o firm.elf')
run(OD + ' -d firm.elf > firm.disasm.s')
run(OC + ' firm.elf -O ihex firm.hex')