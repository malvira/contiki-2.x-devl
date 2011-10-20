/*

  This is the network driver for a generic slip radio

 */

/* don't want slip input to go through tcpip */
/* instead we want to send the data straight through the netstack */
#define SLIP_CONF_TCPIP_INPUT slip_radio_input

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

AUTOSTART_PROCESSES(&slip_process);

static void
packet_sent(void *ptr, int status, int num_tx)
{
  
}

void 
slip_radio_input(void)
{
	NETSTACK_MAC.send(packet_sent, NULL);
}

u8_t
slip_packetbuf_send(void)
{
  u16_t i;
  u8_t *ptr;
  u8_t c;

  slip_arch_writeb(SLIP_END);

  ptr = (u8_t *)packetbuf_dataptr();
  for(i = 0; i < packetbuf_datalen(); ++i) {
    c = *ptr++;
    slip_arch_writeb(c);
  }
  slip_arch_writeb(SLIP_END);

}

static void
input(void)
{
 slip_packetbuf_send();
}

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

const struct network_driver slip_driver = { "Slip network driver", init, input };

