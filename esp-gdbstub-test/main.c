/* Test project for the ESP8266 GDB debug stub (toggles GPIO2)
 * (https://github.com/espressif/esp-gdbstub)
 *
 * Build project without gdbstub:
 * 	make DEBUG=n
 *
 * Build project with gdbstub:
 * 	make DEBUG=y
 *
 * Test with:
 * 	make DEBUG=y
 * 	make flash
 * 	make debug
 */

#include "osapi.h"
#include "os_type.h"
#include "gpio.h"
#include "user_interface.h"
#include "eagle_soc.h"

#ifdef DEBUG
#include "../esp-gdbstub/gdbstub.h"
#endif

/* ICACHE_FLASH_ATTR macro: put functions in .irom0 segment in flash */

#define MESSAGE_QUEUE_DEPTH 1

static os_timer_t os_timer;
static os_event_t queue[MESSAGE_QUEUE_DEPTH];

__attribute__((unused)) static void ICACHE_FLASH_ATTR user_rf_preinit()
{
}

static void timer(__attribute__((unused)) void *param)
{
	if (GPIO_REG_READ(GPIO_OUT_ADDRESS) & BIT2) {
		GPIO_OUTPUT_SET(GPIO_ID_PIN(2), 0);
	} else {
		GPIO_OUTPUT_SET(GPIO_ID_PIN(2), 1);
	}
}

static void ICACHE_FLASH_ATTR os_task(__attribute__((unused)) os_event_t *event)
{
	os_delay_us(10);
}

void ICACHE_FLASH_ATTR user_init(void)
{
#ifdef DEBUG
	/* set UART0 baudrate to 115200 */
	uart_div_modify(0, UART_CLK_FREQ/115200);
	gdbstub_init();
#endif

	/* select GPIO2 function of pin 14
	 * (see ESP8266__Pin_List*.xlsx, ESP8266_Interface_GPIO*.pdf) */
	gpio_init();
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
	/* configure as output and disable
	 * (set_mask, clear_mask, enable_mask, disable_mask) */
	gpio_output_set(0, BIT2, BIT2, 0);

	/* set callback function for timer */
	os_timer_disarm(&os_timer);
	os_timer_setfn(&os_timer, (os_timer_func_t *)timer, NULL);
	/* call timer function every 1000 ms */
	os_timer_arm(&os_timer, 1000, 1);

	/* set up task with priority 0 */
	system_os_task(os_task, USER_TASK_PRIO_0, queue, MESSAGE_QUEUE_DEPTH);
}
