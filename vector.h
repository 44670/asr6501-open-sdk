/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2012 chrysn <chrysn@fsfe.org>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *
 * Definitions for handling vector tables.
 *
 * This implements d0002_efm32_cortex-m3_reference_manual.pdf's figure 2.2
 * (from the EFM32 documentation at
 * http://www.energymicro.com/downloads/datasheets), and was seen analogously
 * in other ARM implementations' libopencm3 files.
 *
 * The structure of the vector table is implemented independently of the system
 * vector table starting at memory position 0x0, as it can be relocated to
 * other memory locations too.
 *
 * The exact size of a vector interrupt table depends on the number of
 * interrupts IRQ_COUNT, which is defined per family.
 */

#ifndef LIBOPENCM3_VECTOR_H
#define LIBOPENCM3_VECTOR_H


/** Type of an interrupt function. Only used to avoid hard-to-read function
 * pointers in the efm32_vector_table_t struct. */
typedef void (*vector_table_entry_t)(void);

typedef struct {
	unsigned int *initial_sp_value; /**< Initial stack pointer value. */
	vector_table_entry_t reset;
	vector_table_entry_t nmi;
	vector_table_entry_t hard_fault;
	vector_table_entry_t memory_manage_fault; /* not in CM0 */
	vector_table_entry_t bus_fault;           /* not in CM0 */
	vector_table_entry_t usage_fault;         /* not in CM0 */
	vector_table_entry_t reserved_x001c[4];
	vector_table_entry_t sv_call;
	vector_table_entry_t debug_monitor;       /* not in CM0 */
	vector_table_entry_t reserved_x0034;
	vector_table_entry_t pend_sv;
	vector_table_entry_t systick;
	vector_table_entry_t irq[8];
} vector_table_t;

/* Common symbols exported by the linker script(s): */
extern unsigned _data_loadaddr, _data, _edata, _ebss, _stack;
extern vector_table_t vector_table;

#endif


/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2010 Piotr Esden-Tempski <piotr@esden.net>,
 * Copyright (C) 2012 chrysn <chrysn@fsfe.org>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library. If not, see <http://www.gnu.org/licenses/>.
 */


/* Less common symbols exported by the linker script(s): */
typedef void (*funcp_t) (void);
extern funcp_t __preinit_array_start, __preinit_array_end;
extern funcp_t __init_array_start, __init_array_end;
extern funcp_t __fini_array_start, __fini_array_end;

int main(void);
void blocking_handler(void);
void null_handler(void);

void __attribute__ ((weak)) reset_handler(void)
{
	volatile unsigned *src, *dest;
	funcp_t *fp;

	for (src = &_data_loadaddr, dest = &_data;
		dest < &_edata;
		src++, dest++) {
		*dest = *src;
	}

	while (dest < &_ebss) {
		*dest++ = 0;
	}


	/* might be provided by platform specific vector.c */
	//pre_main();


	/* Call the application's entry point. */
	(void)main();


}

void blocking_handler(void)
{
	while (1);
}

void null_handler(void)
{
	/* Do nothing. */
}



__attribute__ ((section(".vectors")))
vector_table_t vector_table = {
	.initial_sp_value = &_stack,
	.reset = reset_handler,
	.nmi = null_handler,
	.hard_fault = blocking_handler,

/* Those are defined only on CM3 or CM4 */
#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
	.memory_manage_fault = mem_manage_handler,
	.bus_fault = bus_fault_handler,
	.usage_fault = usage_fault_handler,
	.debug_monitor = debug_monitor_handler,
#endif

	.sv_call = null_handler,
	.pend_sv = null_handler,
	.systick = null_handler,
	.irq = {
		null_handler,
        null_handler,
        null_handler,
        null_handler,
        null_handler,
        null_handler,
        null_handler,
        null_handler
	}
};
