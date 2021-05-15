/*
 * Copyright 2019 The Hafnium Authors.
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/BSD-3-Clause.
 */

#include "hf/io.h"
#include "hf/mm.h"
#include "hf/mpool.h"
#include "hf/plat/console.h"

/* clang-format off */

//#define UART_BASE (0x01C28000)

#define UART_RBR IO32_C(UART_BASE + 0x00)
#define UART_THR IO32_C(UART_BASE + 0x00)
#define UART_DLL IO32_C(UART_BASE + 0x00)
#define UART_IER IO32_C(UART_BASE + 0x04)
#define UART_DLM IO32_C(UART_BASE + 0x04)
#define UART_FCR IO32_C(UART_BASE + 0x08)
#define UART_LCR IO32_C(UART_BASE + 0x0C)
#define UART_MCR IO32_C(UART_BASE + 0x10)
#define UART_LSR IO32_C(UART_BASE + 0x14)
#define UART_MSR IO32_C(UART_BASE + 0x18)
#define UART_SCR IO32_C(UART_BASE + 0x1C)
/* clang-format on */

void plat_console_init(void)
{
}

void plat_console_mm_init(struct mm_stage1_locked stage1_locked,
			  struct mpool *ppool)
{
	mm_identity_map(stage1_locked, pa_init(UART_BASE),
                        pa_add(pa_init(UART_BASE), PAGE_SIZE),
                        MM_MODE_R | MM_MODE_W | MM_MODE_D, ppool);
}

void plat_console_putchar(char c)
{
	/* Print a carriage-return as well. */
	if (c == '\n') {
		plat_console_putchar('\r');
	}

	/* Wait until the transmitter is no longer busy. */
	while ((io_read32(UART_LSR) & 0x20) == 0) continue;

	/* Write data to transmitter FIFO. */
	memory_ordering_barrier();
	io_write32(UART_THR, c);
	memory_ordering_barrier();

}

char plat_console_getchar(void)
{
	/* Wait for the transmitter to be ready to deliver a byte. */
	while ((io_read32(UART_LSR) & 0x01) == 0) continue;

	/* Read data from transmitter FIFO. */
	return (char)(io_read32(UART_RBR));
}
