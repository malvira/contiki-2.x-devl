/*

  This is the network driver for a generic slip radio

 */

/* don't want slip input to go through tcpip */
/* instead we want to send the data straight through the netstack */
#define SLIP_CONF_TCPIP_INPUT slip_radio_input

#include "contiki.h"
#include "net/uip.h"
#include "net/netstack.h"
#include "net/packetbuf.h"
#include "dev/slip.h"
#include "dev/uart1.h"
#include <string.h>

#define SLIP_END     0300
#define SLIP_ESC     0333
#define SLIP_ESC_END 0334
#define SLIP_ESC_ESC 0335

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
	uint8_t *ptr;	
	packetbuf_clear();
	ptr = (uint8_t *)packetbuf_dataptr();
	memcpy(ptr,&uip_buf[UIP_LLH_LEN],
	       uip_len);
	packetbuf_set_datalen(uip_len);
	       
	PRINTF("slip radio input\n\r");
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
  /* need to back the pointer up to put the 802.15.4 header back on */
  ptr -= packetbuf_attr(PACKETBUF_ATTR_FRAMEHDR_LEN);

  for(i = 0; i < packetbuf_datalen() + packetbuf_attr(PACKETBUF_ATTR_FRAMEHDR_LEN); ++i) {
//  for(i = 0; i < packetbuf_datalen(); ++i) {
    c = *ptr++;

    printf("%x ", c);

    if(c == SLIP_END) {
      slip_arch_writeb(SLIP_ESC);
      c = SLIP_ESC_END;
    } else if(c == SLIP_ESC) {
      slip_arch_writeb(SLIP_ESC);
      c = SLIP_ESC_ESC;
    }
    slip_arch_writeb(c);
  }
  slip_arch_writeb(SLIP_END);

}

static void
input(void)
{
	printf("slip-radio input\n\r");
	
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

