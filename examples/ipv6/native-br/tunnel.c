int
devopen(const char *dev, int flags)
{
  char t[32];
  strcpy(t, "/dev/");
  strncat(t, dev, sizeof(t) - 5);
  return open(t, flags);
}

#ifdef linux
#include <linux/if.h>
#include <linux/if_tun.h>

int
tun_alloc(char *dev, int tap)
{
  struct ifreq ifr;
  int fd, err;

  if( (fd = open("/dev/net/tun", O_RDWR)) < 0 ) {
    return -1;
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
    return err;
  }
  strcpy(dev, ifr.ifr_name);
  return fd;
}
#else
int
tun_alloc(char *dev, int tap)
{
  return devopen(dev, O_RDWR);
}
#endif

void
ifconf(const char *tundev, const char *ipaddr)
{
#ifdef linux
  if (timestamp) stamptime();
  ssystem("ifconfig %s inet `hostname` up", tundev);
  if (timestamp) stamptime();
  ssystem("ifconfig %s add %s", tundev, ipaddr);
#else
  if (timestamp) stamptime();
  ssystem("ifconfig %s inet `hostname` %s up", tundev, ipaddr);
  if (timestamp) stamptime();
  ssystem("sysctl -w net.inet.ip.forwarding=1");
#endif /* !linux */

  if (timestamp) stamptime();
  ssystem("ifconfig %s\n", tundev);
}


  tunfd = tun_alloc(tundev, tap);
  if(tunfd == -1) err(1, "main: open");
  if (timestamp) stamptime();
  fprintf(stderr, "opened %s device ``/dev/%s''\n",
          tap ? "tap" : "tun", tundev);
