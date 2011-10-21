/*
 * Copyright (c) 2010, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id: slip-bridge.c,v 1.6 2011/01/17 20:05:51 joxe Exp $
 */

/**
 * \file
 *         TUN fallback interface
 * \author
 *         (borrowed from tunslip6.c)
 *         Mariano Alvira <mar@devl.org>         
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include <sys/ioctl.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <err.h>

#include "net/uip.h"
#include "net/uip-ds6.h"

#include <linux/if.h>
#include <linux/if_tun.h>

#define UIP_IP_BUF     ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])

int
ssystem(const char *fmt, ...)
{
  char cmd[128];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(cmd, sizeof(cmd), fmt, ap);
  va_end(ap);
  printf("%s\n", cmd);
  fflush(stdout);
  return system(cmd);
}

static int fd;
static void
init(void)
{
  struct ifreq ifr;
  int err;
  int tap = 0;
  char dev[32] = { "" };
  char *ipaddr = "aaaa::1/64";

  printf("opening tun device as UIP fallback interface\n\r");

  if(*dev == '\0') {
    /* Use default. */
    if(tap) {
      strcpy(dev, "tap0");
    } else {
      strcpy(dev, "tun0");
    }
  }

  if( (fd = open("/dev/net/tun", O_RDWR | O_NONBLOCK)) < 0 ) {
	  printf("failed to open tun device\n\r");
	  return;
  }

  memset(&ifr, 0, sizeof(ifr));

  /* Flags: IFF_TUN   - TUN device (no Ethernet headers)
   *        IFF_TAP   - TAP device
   *
   *        IFF_NO_PI - Do not provide packet information
   */
  ifr.ifr_flags = (tap ? IFF_TAP : IFF_TUN) | IFF_NO_PI;
  if(*dev != 0)
    strncpy(ifr.ifr_name, dev, IFNAMSIZ);

  if((err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0 ) {
    close(fd);
    printf("failed to set TUNSETIFF\n\r");
    printf("maybe you don't have permission to create a tun device\n\r");
    return;
  }
  strcpy(dev, ifr.ifr_name);

  ssystem("ifconfig %s inet `hostname` up", dev);
  ssystem("ifconfig %s add %s", dev, ipaddr);
  ssystem("ifconfig %s\n", dev);

}

static void
output(void)
{
	printf("output len: %d\n\r", uip_len);
	write(fd, (uint8_t *)UIP_IP_BUF, uip_len);
}

uint8_t tunbuf[2000];
unsigned int idx = 0;

void
tun_arch_read(void)
{
	struct timeval tv;
	int i, len;
	tv.tv_sec = 0;
	tv.tv_usec = 1000;
	

//	if(select(1, &slipfds, NULL, NULL, &tv) < 0) {
//		perror("select");
//	} else if(FD_ISSET(slipfd, &slipfds)) {

	if(len = read(fd, &tunbuf, 2000) > 0) {
		memcpy((uint8_t *)UIP_IP_BUF, (uint8_t *)tunbuf, UIP_BUFSIZE);
		uip_len = (((uint8_t*)UIP_IP_BUF)[4] << 8) + ((uint8_t *)UIP_IP_BUF)[5];
		printf("len %d\n", uip_len);			
		tcpip_input();
	}

//	}

}


const struct uip_fallback_interface rpl_interface = {
  init, output
};

