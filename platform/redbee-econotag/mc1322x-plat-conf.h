#ifndef __MC1322X_PLAT_CONF_H__
#define __MC1322X_PLAT_CONF_H__

/* mc1322x files */
#include "contiki-mc1322x-conf.h"

/* EUI64 generation */
/* Organizationally Unique Identifier */
#define OUI 0xacde48     /* if IAB is defined then OUI = 0x0050C2 */
#define IAB 0xA8C        /* IAB 0xA8C for use on Redwire products only */
//#undef IAB               /* do not define an IAB if you are using a full OUI */
//#define EXT_ID 0xdef123  /* lower 12-bits used if IAB is defined */ 
#undef  EXT_ID           /* if an extention id is not defined then one will be generated randomly */

/* Clock ticks per second */
#define CLOCK_CONF_SECOND 100
/* set to 1 to toggle the green led ever second */
/* FIXME setting this will break the sensor button (and other gpio) */
/* since leds_arch hits the entire gpio_data */
#define BLINK_SECONDS 0

#define CCIF
#define CLIF

/* Baud rate */
#define BRMOD 9999
/*  230400 bps, INC=767, MOD=9999, 24Mhz 16x samp */
/*  115200 bps, INC=767, MOD=9999, 24Mhz 8x samp */
#define BRINC 767  
/*  921600 bps, MOD=9999, 24Mhz 16x samp */
//#define INC 3071 
#define SAMP UCON_SAMP_8X
//#define SAMP UCON_SAMP_16X

#define uart_init uart1_init
#define dbg_putchar(x) uart1_putc(x)

#define USE_FORMATTED_STDIO         1
#define MACA_DEBUG                  0
#define CONTIKI_MACA_RAW_MODE       0
#define USE_32KHZ_XTAL              0

#define BLOCKING_TX 0

#endif /* end __MC1322X_PLAT_CONF_H__ */
