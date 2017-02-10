/*
 * light weight WS2812 lib - ARM Cortex M0/M0+ version
 *
 * Created: 07.07.2013
 *  Author: Tim (cpldcpu@gmail.com)
 */

#include "light_ws2812_cortex.h"
/*
* The total length of each bit is 1.25us (25 cycles @ 20Mhz)
* At 0us the dataline is pulled high.
* To send a zero the dataline is pulled low after 0.375us
* To send a one the dataline is pulled low after 0.625us
*/

#define ws2812_ctot	(((ws2812_cpuclk/1000)*1525)/1000000) //1250
#define ws2812_t1	(((ws2812_cpuclk/1000)*494 )/1000000)		// floor //375
#define ws2812_t2	(((ws2812_cpuclk/1000)*783+500000)/1000000) // ceil //625

#define w1 (ws2812_t1-2)
#define w2 (ws2812_t2-ws2812_t1-2)
#define w3 (ws2812_ctot-ws2812_t2-5)

#define ws2812_DEL1 "	nop		\n\t"
#define ws2812_DEL2 "	b	.+2	\n\t"
#define ws2812_DEL4 ws2812_DEL2 ws2812_DEL2
#define ws2812_DEL8 ws2812_DEL4 ws2812_DEL4
#define ws2812_DEL16 ws2812_DEL8 ws2812_DEL8


__attribute__((optimize(0)))
__attribute__((optimize("unroll-loops")))
void ws2812_sendarray(uint8_t *data,int datlen)
{
	uint32_t maskhi = ws2812_mask_set;
	uint32_t masklo = ws2812_mask_clr;
	volatile uint32_t *set = ws2812_port_set;
	volatile uint32_t *clr = ws2812_port_clr;
	uint32_t i = 0;
	uint32_t curbyte;

	while (datlen--) {
		curbyte=*data++;

	asm volatile(
			"		lsl %[dat],#24				\n\t"
			"		movs %[ctr],#8				\n\t"
			"ilop%=:							\n\t"
			"		lsl %[dat], #1				\n\t"
			"		str %[maskhi], [%[set]]		\n\t"
#if (w1&1)
			ws2812_DEL1
#endif
#if (w1&2)
			ws2812_DEL2
#endif
#if (w1&4)
			ws2812_DEL4
#endif
#if (w1&8)
			ws2812_DEL8
#endif
#if (w1&16)
			ws2812_DEL16
#endif
			"		bcs one%=					\n\t"
			"		str %[masklo], [%[clr]]		\n\t"
			"one%=:								\n\t"
#if (w2&1)
			ws2812_DEL1
#endif
#if (w2&2)
			ws2812_DEL2
#endif
#if (w2&4)
			ws2812_DEL4
#endif
#if (w2&8)
			ws2812_DEL8
#endif
#if (w2&16)
			ws2812_DEL16
#endif
			"		sub %[ctr], #1				\n\t"
			"		str %[masklo], [%[clr]]		\n\t"
			"		beq	end%=					\n\t"
#if (w3&1)
			ws2812_DEL1
#endif
#if (w3&2)
			ws2812_DEL2
#endif
#if (w3&4)
			ws2812_DEL4
#endif
#if (w3&8)
			ws2812_DEL8
#endif
#if (w3&16)
			ws2812_DEL16
#endif

			"		b 	ilop%=					\n\t"
			"end%=:								\n\t"
			:	[ctr] "+r" (i)
			:	[dat] "r" (curbyte), [set] "r" (set), [clr] "r" (clr), [masklo] "r" (masklo), [maskhi] "r" (maskhi)
			);
	}
}

