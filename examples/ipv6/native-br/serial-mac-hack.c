#include "contiki.h"


PROCESS(serial_mac_hack, "Serial MAC hack");

static void
sent_callback(void *dummy)
{
  
}

PROCESS_THREAD(serial_mac_hack, ev, data)
{
  PROCESS_BEGIN();

  while(1) {
    PROCESS_PAUSE();
    
    xxx_slip_input();

    packetbuf_clear();
    memcpy(packetbuf_dataptr(), the_slip_intpu_buffer_thing);
    packetbuf_set_datalen(the_slip_intpu_buffer_thing_lengt);
    NETSTACK_MAC.send(sent_callback, NULL);
  }

  PROCESS_END();
}

static void
init(void)
{
  process_start(&serial_mac_hack, NULL);
}

static void
input(void)
{
  serial_print_out_the_packet(packetbuf_dataptr(), packetbuf_datalen());
}

struct network_driver the_mac_hacK_driver_thingy = { "Machack", init, input };
