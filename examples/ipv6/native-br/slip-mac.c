/*

  This is the mac driver for a slip interface to work with a network driver.

  In this case, there is no RDC, radio, or framer. The data from the
  slip connection is passed into the NETSTACK.network
  (e.g. sicslowpan.c)

 */

/* don't want slip input to go through tcpip */
/* instead we want to send the data straight through the netstack */

#define SLIP_CONF_TCPIP_INPUT slip_mac_input

#include "contiki.h"
#include "net/netstack.h"
#include "dev/slip.h"
#include "dev/uart1.h"

#define SLIP_END     0300

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

static void
slip_input_callback(void)
{

}

static void
init(void)
{
  slip_arch_init(BAUD2UBR(115200));
  process_start(&slip_process, NULL);
  slip_set_input_callback(slip_input_callback);
}

void
slip_mac_input(void)
{
	printf("slip_mac_input\n\r");
	NETSTACK_NETWORK.input();
}

static int
on(void)
{
  return NETSTACK_RDC.on();
}

static int
off(int keep_radio_on)
{
  return NETSTACK_RDC.off(keep_radio_on);
}

static unsigned short
channel_check_interval(void)
{
  return 0;
}

const struct mac_driver slipmac_driver = {
  "nullmac",
  init,
  slip_send,
  slip_mac_input,
  on,
  off,
  channel_check_interval,
};


