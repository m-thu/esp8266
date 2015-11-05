/* toggles GPIO9 (pin 18) */

#include "osapi.h"
#include "os_type.h"
#include "gpio.h"
#include "user_interface.h"
#include "mem.h"

/* ICACHE_FLASH_ATTR macro: put functions in .irom0 segment in flash */

#define MESSAGE_QUEUE_DEPTH 4

static os_timer_t os_timer;
/*os_event_t *queue;*/
static os_event_t queue[MESSAGE_QUEUE_DEPTH];

__attribute__((unused)) static void ICACHE_FLASH_ATTR user_rf_preinit()
{
}

static void timer()
{
	if (GPIO_REG_READ(GPIO_OUT_ADDRESS) & BIT9) {
		GPIO_OUTPUT_SET(GPIO_ID_PIN(9), 0);
	} else {
		GPIO_OUTPUT_SET(GPIO_ID_PIN(9), 1);
	}
}

static void os_task(__attribute__((unused)) os_event_t *event)
{
	os_delay_us(10);
}

void user_init()
{
	/* select GPIO9 function of pin 18
	 * (see ESP8266__Pin_List*.xlsx, ESP8266_Interface_GPIO*.pdf) */
	gpio_init();
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_DATA2_U, FUNC_GPIO9);
	/* configure as output and disable
	 * (set_mask, clear_mask, enable_mask, disable_mask) */
	gpio_output_set(0, BIT9, BIT9, 0);

	/* set callback function for timer */
	os_timer_disarm(&os_timer);
	os_timer_setfn(&os_timer, (os_timer_func_t *)timer, NULL);
	/* call timer function every 1000 ms */
	os_timer_arm(&os_timer, 500, 1);

	/* set up task with priority 0 */
	/*queue = (os_event_t *)os_malloc(MESSAGE_QUEUE_DEPTH * sizeof(os_event_t));*/
	system_os_task(os_task, USER_TASK_PRIO_0, queue, MESSAGE_QUEUE_DEPTH);
}
