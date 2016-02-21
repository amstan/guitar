//Bitbanging ws2812 implementation
//Requires disabled interrupts to run
//Dependent on system clock being 48MHz
//Here be dragons, don't touch

#include <libopencm3/cm3/cortex.h>

__attribute__((optimize("unroll-loops")))
__attribute__((always_inline))
inline void ws2812_write_byte(uint8_t byte) {
	for(int j = 7; j >= 0; j--)
	{
		bool bit = (byte & (1 << j)) == 0;
		gpio_set(GPIOC,GPIO12);
		GPIOC_BSRR = 1 << 12;
		if(bit)
			GPIOC_BSRR = 1 << 12 << 16;
		__asm__("nop");
		__asm__("nop");
		__asm__("nop");
		__asm__("nop");
		__asm__("nop");
		__asm__("nop");
		GPIOC_BSRR = 1 << 12 << 16;
	}
}

__attribute__((optimize("unroll-loops")))
void ws2812_sendarray(uint8_t *buf, uint16_t size) {
	for(volatile int i = 0; i < 100; i++);

	cm_disable_interrupts();
	for(uint16_t i = 0; i < size; i+=3)
	{
		ws2812_write_byte(buf[i+1]);
		ws2812_write_byte(buf[i+0]);
		ws2812_write_byte(buf[i+2]);
	}
	cm_enable_interrupts();
}
