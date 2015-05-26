#define LED_GREEN  (GPIOD, GPIO12)
#define LED_ORANGE (GPIOD, GPIO13)
#define LED_RED    (GPIOD, GPIO14)
#define LED_BLUE   (GPIOD, GPIO15)

#include "timer.c"
#include "usb.c"
#include "pins.c"
#include "i2c.c"
#include <stdio.h>

static void clock_setup(void)
{
	rcc_clock_setup_hse_3v3(&hse_8mhz_3v3[CLOCK_3V3_120MHZ]);
}

struct __attribute__ ((__packed__)) tp_object {
	uint8_t type;
	uint16_t start;
	uint8_t size;
	uint8_t instances;
	uint8_t num_report_ids;
};

struct __attribute__ ((__packed__)) tp_info_header {
	uint8_t family;
	uint8_t variant;
	uint8_t version;
	uint8_t build;
	uint8_t x;
	uint8_t y;
	uint8_t objects_count;
};

struct __attribute__ ((__packed__)) tp_info {
	struct tp_info_header header;
	struct tp_object objects[30];
} tp;

uint8_t tp_buffer[30*6];

struct touch_message {
	uint8_t status;
	uint16_t x;
	uint16_t y;
	uint8_t area;
	uint8_t amplitude;
	uint8_t vector1;
	uint8_t vector2;
};

int main(void) {
	clock_setup();
	gpio_setup();
	systick_setup();
	usb_init();

// 	while(1) {
// 		gpio_toggle(GPIOD, GPIO12);
// // 		printf("ping\n");
// 		#define TP_ADDRESS 0x4B
// 		for (unsigned int i=0 ; i < 128 ; i++) {
// 			printf("%02x: %d\n", i, i2c_probe(i));
// 			msleep(2);
// 		}
//
// 		printf("\n");
// 		msleep(1000);
// 	}

	#define TP_ADDRESS 0x4B<<1
	while(i2c_probe(TP_ADDRESS)) {
		printf("Error connecting!");
		//set_bit(OUT(LED_R_PORT),LED_G);
// 		while(1);
	}

	i2c_read_data(TP_ADDRESS, 0, (uint8_t*)&tp.header, 7);
	printf("family=%02x\n", tp.header.family);
	printf("variant=%02x\n", tp.header.variant);
	printf("version=%02x\n", tp.header.version);
	printf("build=%02x\n", tp.header.build);
	printf("x=%d\n", tp.header.x);
	printf("y=%d\n", tp.header.y);
	printf("objects_count=%d\n", tp.header.objects_count);

	struct tp_object *touch_object;
	struct tp_object *msg_object;

	i2c_read_data(TP_ADDRESS, 7, (uint8_t*)&tp.objects, 6 * tp.header.objects_count);
	for (int i=0; i<tp.header.objects_count; i++) {
		struct tp_object *o = &tp.objects[i];

		//printf("i%02d ",i);
		printf("T%d ",o->type);
		printf("s=0x%04x ",o->start);
		printf("i=%d",o->instances);
		printf("(r/%d) ",o->num_report_ids);
		printf("size %d ",o->size);

		if(o->type == 9) {
			touch_object = o;
		}
		if(o->type == 5) {
			msg_object = o;
		}

		printf("\n");
	}
	printf("\n");

	//disable movfilter
// 	i2c_write_register(TP_ADDRESS, touch_object->start+13, 0x80);

	//increase jumplimit
	i2c_write_register(TP_ADDRESS, touch_object->start+30, 0x0c*4);

	printf("Multi touch properties[0x%04x]:\n", touch_object->start);
	i2c_read_data(TP_ADDRESS, touch_object->start, tp_buffer, touch_object->size);
	for (int i=0; i<touch_object->size; i++) {
		printf("T9[%d]=0x%02x\n",i,tp_buffer[i]);
	}
	printf("\n");

	unsigned char touch_id;

// 	char buf[64];
// 	while(1) {
// 		int len = usbd_ep_read_packet(global_usbd_dev, ENDPOINT_FAST_IN, buf, 64);
// 		if (len) {
// 			for(unsigned int i=0;i<len;i++) {
// 				printf("%02x ", buf[i]);
// 			}
// 		}
// 		printf("\n");
// 	}





	printf("data:\n");
	while (1) {


// 		printf("msg[0x%04x]:\n", msg_object->start);
		i2c_read_data(TP_ADDRESS, msg_object->start, tp_buffer, msg_object->size);

		if(tp_buffer[0]!=0xff) {
			if(tp_buffer[0] <= touch_id)
				printf("\n");

			touch_id = tp_buffer[0];

			struct touch_message tm;

			tm.status = tp_buffer[1];
			tm.x = (tp_buffer[2]<<8) + (tp_buffer[4] & 0xf0);
			tm.y = (tp_buffer[3]<<8) + ((tp_buffer[4] & 0x0f)<<4);
			tm.area = tp_buffer[5];
			tm.amplitude = tp_buffer[6];
			tm.vector1 = (tp_buffer[7] & 0xf0)>>4;
			tm.vector2 = (tp_buffer[7] & 0x0f);

			for(int k = 7; k >= 0; k--) {
				printf("%c", tm.status & (1<<k) ? (k + '0') : '.');
				if (k == 4)
					printf(" ");
			}

			printf(" %5d %5d %3d %3d %2d %2d | ",
				tm.x/16,
				tm.y/16,
				tm.area,
				tm.amplitude,
				tm.vector1,
				tm.vector2
			);

// 			for(unsigned int i = 0; i < 8; i++) {
// 				printf("%c", tp_buffer[i]);
// 			}

			fflush(0);
		}

		//toggle_bit(OUT(LED_R_PORT),LED_R);
// 		delay(500 ms);
	}
}
