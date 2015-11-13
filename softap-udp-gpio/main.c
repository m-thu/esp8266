/* Soft-AP mode, receives UDP packets on port 1234
 * SSID: ESP8266
 * IP address: 172.16.0.1
 *
 * Turn GPIO2 on:
 *    bash -c 'echo -n 1 >/dev/udp/172.16.0.1/1234'
 * Turn GPIO2 off:
 *    bash -c 'echo -n 0 >/dev/udp/172.16.0.1/1234'
 */

#include "osapi.h"
#include "os_type.h"
#include "gpio.h"
#include "user_interface.h"
#include "espconn.h"

/* ICACHE_FLASH_ATTR macro: put functions in .irom0 segment in flash */

#define MESSAGE_QUEUE_DEPTH 1

static struct softap_config softap_cfg = {
	"ESP8266", /* ssid */
	"",        /* password */
	0,         /* ssid_len, 0: zero terminated string */
	1,         /* channel */
	AUTH_OPEN, /* authmode */
	0,         /* ssid_hidden */
	4,         /* max_connection */
	100        /* beacon_interval (ms) */
};

static struct ip_info ip;
static struct espconn udp_conn;
static esp_udp udp;

static os_event_t queue[MESSAGE_QUEUE_DEPTH];

__attribute__((unused)) static void ICACHE_FLASH_ATTR user_rf_preinit(void)
{
}

static void ICACHE_FLASH_ATTR
os_task(__attribute__((unused)) os_event_t *event)
{
	os_delay_us(10);
}

static void ICACHE_FLASH_ATTR
udp_receive_callback(__attribute__((unused)) void *arg, char *pdata,
                 __attribute__((unused)) unsigned short len)
{
	if (pdata[0] == '1') {
		GPIO_OUTPUT_SET(GPIO_ID_PIN(2), 1);
	} else {
		GPIO_OUTPUT_SET(GPIO_ID_PIN(2), 0);
	}
}

void ICACHE_FLASH_ATTR user_init(void)
{
	/* select GPIO2 function of pin 14
	 * (see ESP8266__Pin_List*.xlsx, ESP8266_Interface_GPIO*.pdf) */
	gpio_init();
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
	/* configure as output and disable
	 * (set_mask, clear_mask, enable_mask, disable_mask) */
	gpio_output_set(0, BIT2, BIT2, 0);

	/* check if WiFi operating mode is set to soft-AP */
	if (wifi_get_opmode() != 0x02)
		/* set operating mode to soft-AP, don't save to flash */
		wifi_set_opmode_current(0x02);

	/* set soft-AP configuration, don't save to flash */
	wifi_softap_set_config_current(&softap_cfg);

	/* disable DHCP server */
	wifi_softap_dhcps_stop();

	/* static IP configuration */
	IP4_ADDR(&ip.ip, 172, 16, 0, 1);
	IP4_ADDR(&ip.gw, 172, 16, 0, 1);
	IP4_ADDR(&ip.netmask, 255, 240, 0, 0);
	wifi_set_ip_info(SOFTAP_IF, &ip);

	/* listen on UDP port 1234 */
	udp.local_port = 1234;
	udp_conn.type = ESPCONN_UDP;
	udp_conn.state = ESPCONN_NONE;
	udp_conn.proto.udp = &udp;
	/* register callback function for receiving UDP packets */
	espconn_regist_recvcb(&udp_conn, udp_receive_callback);
	espconn_create(&udp_conn);

	/* set up task with priority 0 */
	system_os_task(os_task, USER_TASK_PRIO_0, queue, MESSAGE_QUEUE_DEPTH);
}
