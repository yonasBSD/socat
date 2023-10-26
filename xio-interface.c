/* source: xio-interface.c */
/* Copyright Gerhard Rieger and contributors (see file CHANGES) */
/* Published under the GNU General Public License V.2, see file COPYING */

/* this file contains the source for opening addresses of raw socket type */

#include "xiosysincludes.h"

#if _WITH_INTERFACE

#include "xioopen.h"
#include "xio-socket.h"

#include "xio-interface.h"


static
int xioopen_interface(int argc, const char *argv[], struct opt *opts,
		      int xioflags, xiofile_t *xfd, groups_t groups, int pf,
		      int dummy2, int dummy3);

/*0 const struct optdesc opt_interface_addr    = { "interface-addr",    "address", OPT_INTERFACE_ADDR,    GROUP_INTERFACE, PH_FD, TYPE_STRING,   OFUNC_SPEC };*/
/*0 const struct optdesc opt_interface_netmask = { "interface-netmask", "netmask", OPT_INTERFACE_NETMASK, GROUP_INTERFACE, PH_FD, TYPE_STRING,   OFUNC_SPEC };*/
const struct optdesc opt_iff_up          = { "iff-up",          "up",          OPT_IFF_UP,          GROUP_INTERFACE, PH_INIT, TYPE_BOOL,     OFUNC_OFFSET_MASKS, XIO_OFFSETOF(para.interface.iff_opts), XIO_SIZEOF(para.interface.iff_opts), IFF_UP };
const struct optdesc opt_iff_broadcast   = { "iff-broadcast",   NULL,          OPT_IFF_BROADCAST,   GROUP_INTERFACE, PH_FD,   TYPE_BOOL,     OFUNC_OFFSET_MASKS, XIO_OFFSETOF(para.interface.iff_opts), XIO_SIZEOF(para.interface.iff_opts), IFF_BROADCAST };
const struct optdesc opt_iff_debug       = { "iff-debug"    ,   NULL,          OPT_IFF_DEBUG,       GROUP_INTERFACE, PH_FD,   TYPE_BOOL,     OFUNC_OFFSET_MASKS, XIO_OFFSETOF(para.interface.iff_opts), XIO_SIZEOF(para.interface.iff_opts), IFF_DEBUG };
const struct optdesc opt_iff_loopback    = { "iff-loopback" ,   "loopback",    OPT_IFF_LOOPBACK,    GROUP_INTERFACE, PH_FD,   TYPE_BOOL,     OFUNC_OFFSET_MASKS, XIO_OFFSETOF(para.interface.iff_opts), XIO_SIZEOF(para.interface.iff_opts), IFF_LOOPBACK };
const struct optdesc opt_iff_pointopoint = { "iff-pointopoint", "pointopoint",OPT_IFF_POINTOPOINT, GROUP_INTERFACE, PH_FD,   TYPE_BOOL,     OFUNC_OFFSET_MASKS, XIO_OFFSETOF(para.interface.iff_opts), XIO_SIZEOF(para.interface.iff_opts), IFF_POINTOPOINT };
const struct optdesc opt_iff_notrailers  = { "iff-notrailers",  "notrailers",  OPT_IFF_NOTRAILERS,  GROUP_INTERFACE, PH_FD,   TYPE_BOOL,     OFUNC_OFFSET_MASKS, XIO_OFFSETOF(para.interface.iff_opts), XIO_SIZEOF(para.interface.iff_opts), IFF_NOTRAILERS };
const struct optdesc opt_iff_running     = { "iff-running",     "running",     OPT_IFF_RUNNING,     GROUP_INTERFACE, PH_FD,   TYPE_BOOL,     OFUNC_OFFSET_MASKS, XIO_OFFSETOF(para.interface.iff_opts), XIO_SIZEOF(para.interface.iff_opts), IFF_RUNNING };
const struct optdesc opt_iff_noarp       = { "iff-noarp",       "noarp",       OPT_IFF_NOARP,       GROUP_INTERFACE, PH_FD,   TYPE_BOOL,     OFUNC_OFFSET_MASKS, XIO_OFFSETOF(para.interface.iff_opts), XIO_SIZEOF(para.interface.iff_opts), IFF_NOARP };
const struct optdesc opt_iff_promisc     = { "iff-promisc",     "promisc",     OPT_IFF_PROMISC,     GROUP_INTERFACE, PH_FD,   TYPE_BOOL,     OFUNC_OFFSET_MASKS, XIO_OFFSETOF(para.interface.iff_opts), XIO_SIZEOF(para.interface.iff_opts), IFF_PROMISC };
const struct optdesc opt_iff_allmulti    = { "iff-allmulti",    "allmulti",    OPT_IFF_ALLMULTI,    GROUP_INTERFACE, PH_FD,   TYPE_BOOL,     OFUNC_OFFSET_MASKS, XIO_OFFSETOF(para.interface.iff_opts), XIO_SIZEOF(para.interface.iff_opts), IFF_ALLMULTI };
#ifdef IFF_MASTER
const struct optdesc opt_iff_master      = { "iff-master",      "master",      OPT_IFF_MASTER,      GROUP_INTERFACE, PH_FD,   TYPE_BOOL,     OFUNC_OFFSET_MASKS, XIO_OFFSETOF(para.interface.iff_opts), XIO_SIZEOF(para.interface.iff_opts), IFF_MASTER };
#endif
#ifdef IFF_SLAVE
const struct optdesc opt_iff_slave       = { "iff-slave",       "slave",       OPT_IFF_SLAVE,       GROUP_INTERFACE, PH_FD,   TYPE_BOOL,     OFUNC_OFFSET_MASKS, XIO_OFFSETOF(para.interface.iff_opts), XIO_SIZEOF(para.interface.iff_opts), IFF_SLAVE };
#endif
const struct optdesc opt_iff_multicast   = { "iff-multicast",   NULL,          OPT_IFF_MULTICAST,   GROUP_INTERFACE, PH_FD,   TYPE_BOOL,     OFUNC_OFFSET_MASKS, XIO_OFFSETOF(para.interface.iff_opts), XIO_SIZEOF(para.interface.iff_opts), IFF_MULTICAST };
#ifdef IFF_PORTSEL
const struct optdesc opt_iff_portsel     = { "iff-portsel",     "portsel",     OPT_IFF_PORTSEL,     GROUP_INTERFACE, PH_FD,   TYPE_BOOL,     OFUNC_OFFSET_MASKS, XIO_OFFSETOF(para.interface.iff_opts), XIO_SIZEOF(para.interface.iff_opts), IFF_PORTSEL };
#endif
#ifdef IFF_AUTOMEDIA
const struct optdesc opt_iff_automedia   = { "iff-automedia",   "automedia",   OPT_IFF_AUTOMEDIA,   GROUP_INTERFACE, PH_FD,   TYPE_BOOL,     OFUNC_OFFSET_MASKS, XIO_OFFSETOF(para.interface.iff_opts), XIO_SIZEOF(para.interface.iff_opts), IFF_AUTOMEDIA };
#endif
/*const struct optdesc opt_iff_dynamic   = { "iff-dynamic",     "dynamic",     OPT_IFF_DYNAMIC,     GROUP_INTERFACE, PH_FD,   TYPE_BOOL,     OFUNC_OFFSET_MASKS, XIO_OFFSETOF(para.interface.iff_opts), XIO_SIZEOF(short), IFF_DYNAMIC };*/
#if LATER
const struct optdesc opt_route           = { "route",           NULL,          OPT_ROUTE,           GROUP_INTERFACE, PH_INIT, TYPE_STRING,   OFUNC_SPEC };
#endif

#if WITH_INTERFACE
const struct addrdesc xioaddr_interface = { "INTERFACE",    3, xioopen_interface, GROUP_FD|GROUP_SOCKET|GROUP_INTERFACE, PF_PACKET, 0, 0 HELP(":<interface>") };
#endif /* WITH_INTERFACE */


static
int _xioopen_interface(const char *ifname,
		       struct opt *opts, int xioflags, xiofile_t *xxfd,
		       groups_t groups, int pf) {
   xiosingle_t *xfd = &xxfd->stream;
   union sockaddr_union us = {{0}};
   socklen_t uslen;
   int socktype = SOCK_RAW;
   unsigned int ifidx;
   bool needbind = false;
   char *bindstring = NULL;
   struct sockaddr_ll sall = { 0 };
   int rc;

   if (ifindex(ifname, &ifidx, -1) < 0) {
      Error1("unknown interface \"%s\"", ifname);
      ifidx = 0;	/* desparate attempt to continue */
   }

   xfd->howtoend = END_INTERFACE;
   retropt_int(opts, OPT_SO_TYPE, &socktype);

   retropt_socket_pf(opts, &pf);

   /* ...res_opts[] */
   if (applyopts_single(xfd, opts, PH_INIT) < 0)  return -1;
   applyopts(-1, opts, PH_INIT);

   xfd->salen = sizeof(xfd->peersa);
   if (pf == PF_UNSPEC) {
      pf = xfd->peersa.soa.sa_family;
   }

   xfd->dtype = XIODATA_RECVFROM_SKIPIP;

   if (retropt_string(opts, OPT_BIND, &bindstring)) {
      needbind = true;
   }
   /*!!! parse by ':' */
   us.ll.sll_family = pf;
   us.ll.sll_protocol = htons(ETH_P_ALL);
   us.ll.sll_ifindex = ifidx;
   uslen = sizeof(sall);
   needbind = true;
   xfd->peersa = (union sockaddr_union)us;

   rc =
      _xioopen_dgram_sendto(needbind?&us:NULL, uslen,
			    opts, xioflags, xfd, groups, pf, socktype, 0, 0);
   if (rc < 0)
      return rc;

   strncpy(xfd->para.interface.name, ifname, IFNAMSIZ);
   _xiointerface_get_iff(xfd->fd, ifname, &xfd->para.interface.save_iff);
   _xiointerface_apply_iff(xfd->fd, ifname, xfd->para.interface.iff_opts);

#ifdef PACKET_IGNORE_OUTGOING
   /* Raw socket might also provide packets that are outbound - we are not
      interested in these and disable this "feature" in kernel if possible */
   if (Setsockopt(xfd->fd, SOL_PACKET, PACKET_IGNORE_OUTGOING, &one, sizeof(one)) < 0) {
      Warn2("setsockopt(%d, SOL_PACKET, PACKET_IGNORE_OUTGOING, {1}): %s",
	    xfd->fd, strerror(errno));
   }
#endif /*defined(PACKET_IGNORE_OUTGOING) */

   return 0;
}

static
int xioopen_interface(int argc, const char *argv[], struct opt *opts,
		      int xioflags, xiofile_t *xxfd, groups_t groups,
		      int pf, int dummy2, int dummy3) {
   xiosingle_t *xfd = &xxfd->stream;
   int result;

   if (argc != 2) {
      Error2("%s: wrong number of parameters (%d instead of 1)",
	     argv[0], argc-1);
      return STAT_NORETRY;
   }

   if ((result =
	_xioopen_interface(argv[1], opts, xioflags, xxfd, groups, pf))
       != STAT_OK) {
      return result;
   }

   xfd->dtype = XIOREAD_RECV|XIOWRITE_SENDTO;
   if (pf == PF_INET) {
      xfd->dtype |= XIOREAD_RECV_SKIPIP;
   }

   xfd->para.socket.la.soa.sa_family = xfd->peersa.soa.sa_family;

   _xio_openlate(xfd, opts);
   return STAT_OK;
}


/* Retrieves the interface flags related to sockfd */
int _xiointerface_get_iff(
	int sockfd,
	const char *name,
	short *save_iff)
{
   struct ifreq ifr;

   memset(&ifr, 0, sizeof(ifr));
   strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name));
   if (Ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0) {
      Error3("ioctl(%d, SIOCGIFFLAGS, {\"%s\"}: %s",
	     sockfd, ifr.ifr_name, strerror(errno));
   }
   *save_iff = ifr.ifr_flags;
   return 0;
}

/* Applies the interface flags to the socket FD.
   Used by INTERFACE and TUN
*/
int _xiointerface_set_iff(
	int sockfd,
	const char *name,
	short new_iff)
{
   struct ifreq ifr;

   memset(&ifr, 0, sizeof(ifr));
   strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name));
   if (Ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0) {
      Error3("ioctl(%d, SIOCGIFFLAGS, {\"%s\"}: %s",
	     sockfd, ifr.ifr_name, strerror(errno));
   }
   ifr.ifr_flags = new_iff;
   if (Ioctl(sockfd, SIOCSIFFLAGS, &ifr) < 0) {
      Error4("ioctl(%d, SIOCSIFFLAGS, {\"%s\", %hd}: %s",
	     sockfd, ifr.ifr_name, ifr.ifr_flags, strerror(errno));
   }
   return 0;
}

/* Applies the interface flags to the socket FD
   Used by INTERFACE and TUN
 */
int _xiointerface_apply_iff(
	int sockfd,
	const char *name,
	short iff_opts[2])
{
   struct ifreq ifr;

   memset(&ifr, 0, sizeof(ifr));
   strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name));
   if (Ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0) {
      Error3("ioctl(%d, SIOCGIFFLAGS, {\"%s\"}: %s",
	     sockfd, ifr.ifr_name, strerror(errno));
   }
   Debug2("\"%s\": system set flags: 0x%hx", ifr.ifr_name, ifr.ifr_flags);
   ifr.ifr_flags |= iff_opts[0];
   ifr.ifr_flags &= ~iff_opts[1];
   Debug2("\"%s\": xio merged flags: 0x%hx", ifr.ifr_name, ifr.ifr_flags);
   if (Ioctl(sockfd, SIOCSIFFLAGS, &ifr) < 0) {
      Error4("ioctl(%d, SIOCSIFFLAGS, {\"%s\", %hd}: %s",
	     sockfd, ifr.ifr_name, ifr.ifr_flags, strerror(errno));
   }
   ifr.ifr_flags = 0;
   if (Ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0) {
      Error3("ioctl(%d, SIOCGIFFLAGS, {\"%s\"}: %s",
	     sockfd, ifr.ifr_name, strerror(errno));
   }
   Debug2("\"%s\": resulting flags: 0x%hx", ifr.ifr_name, ifr.ifr_flags);
   return 0;
}

#endif /* _WITH_INTERFACE */
