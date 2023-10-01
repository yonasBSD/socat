/* source: hostan.c */
/* Copyright Gerhard Rieger and contributors (see file CHANGES) */
/* Published under the GNU General Public License V.2, see file COPYING */

/* the subroutine hostan makes a "HOST ANalysis". It gathers information
   about the host environment it is running in without modifying its state
   (almost).
 */

#include "xiosysincludes.h"
#include "mytypes.h"
#include "compat.h"
#include "error.h"
#include "sycls.h"
#include "sysutils.h"
#include "filan.h"

#include "hostan.h"


static int iffan(FILE *outfile);
static int vsockan(FILE *outfile);


int hostan(FILE *outfile) {
   fprintf(outfile, "\nC TYPE SIZES\n");
   fprintf(outfile, "sizeof(char)      = %u\n", (unsigned int)sizeof(char));
   fprintf(outfile, "sizeof(short)     = %u\n", (unsigned int)sizeof(short));
   fprintf(outfile, "sizeof(int)       = %u\n", (unsigned int)sizeof(int));
   fprintf(outfile, "sizeof(long)      = %u\n", (unsigned int)sizeof(long));
#if HAVE_TYPE_LONGLONG
   fprintf(outfile, "sizeof(long long) = %u\n", (unsigned int)sizeof(long long));
#endif
   fprintf(outfile, "sizeof(size_t)    = %u\n", (unsigned int)sizeof(size_t));
   {
      union {
	 uint16_t s;
	 uint8_t c[2];
      } bo;
      bo.c[0] = 0x05;
      bo.c[1] = 0xa0;
      if (bo.s == 0x05a0) {
	 fprintf(outfile, "host byte order: network (BE \"big endian\", most significast byte first)\n");
      } else if (bo.s == 0xa005) {
	 fprintf(outfile, "host byte order: intel (LE \"little endian\", least significast byte first\n");
      } else {
	 fprintf(outfile, "host byte order: unknown\n");
	 fprintf(stderr, "failed to determine host byte order");
      }
   }

#include <sys/time.h>	/* select(); OpenBSD: struct timespec */
   fprintf(outfile, "sizeof(struct timespec)      = %u\n", (unsigned int)sizeof(struct timespec));
   fprintf(outfile, "sizeof(struct diag_dgram)      = %u\n", (unsigned int)sizeof(struct diag_dgram));
   fprintf(outfile, "((struct diag_dgram *)0)->op-((struct diag_dgram *)0)     = %u\n", (unsigned int)((char *)(&((struct diag_dgram *)0)->op)-(char *)((struct diag_dgram *)0)));
   fprintf(outfile, "((struct diag_dgram *)0)->now-((struct diag_dgram *)0)     = %u\n", (unsigned int)((char *)(&((struct diag_dgram *)0)->now)-(char *)((struct diag_dgram *)0)));
   fprintf(outfile, "((struct diag_dgram *)0)->exitcode-((struct diag_dgram *)0)     = %u\n", (unsigned int)((char *)(&((struct diag_dgram *)0)->exitcode)-(char *)((struct diag_dgram *)0)));
   fprintf(outfile, "((struct diag_dgram *)0)->text-((struct diag_dgram *)0)     = %u\n", (unsigned int)((((struct diag_dgram *)0)->text)-(char *)((struct diag_dgram *)0)));
#if _WITH_SOCKET && (_WITH_IP4 || _WITH_IP6)
   fprintf(outfile, "\nIP INTERFACES\n");
   iffan(outfile);
#endif
#if WITH_VSOCK
   vsockan(outfile);
#endif
   return 0;
}

#if _WITH_SOCKET && (_WITH_IP4 || _WITH_IP6)
static int iffan(FILE *outfile) {
   /* Linux: man 7 netdevice */
   /* FreeBSD, NetBSD: man 4 networking */
   /* Solaris: man 7 if_tcp */

/* currently we support Linux and a little FreeBSD */
#ifdef SIOCGIFCONF	/* not Solaris */

#define IFBUFSIZ 32*sizeof(struct ifreq) /*1024*/
   int s;
   unsigned char buff[IFBUFSIZ];
   struct ifconf ic;
   int i;

   if ((s = Socket(PF_INET, SOCK_DGRAM, IPPROTO_IP)) < 0) {
      Error1("socket(PF_INET, SOCK_DGRAM, IPPROTO_IP): %s", strerror(errno));
      return -1;
   }

   for (i=0; i < IFBUFSIZ; ++i) {
      buff[i] = 255;
   }
   ic.ifc_len = sizeof(buff);
   ic.ifc_ifcu.ifcu_buf = (caddr_t)buff;
   if (Ioctl(s, SIOCGIFCONF, &ic) < 0) {
      Error3("ioctl(%d, SIOCGIFCONF, %p): %s", s, &ic, strerror(errno));
      return -1;
   }

   for (i = 0; i < ic.ifc_len; i += sizeof(struct ifreq)) {
      struct ifreq *ifp = (struct ifreq *)((caddr_t)ic.ifc_req + i);
#if 0 || defined(SIOCGIFINDEX)	/* not NetBSD, OpenBSD */
      struct ifreq ifr;
#endif

#if 0 || defined(SIOCGIFINDEX)	/* not NetBSD, OpenBSD */
      strcpy(ifr.ifr_name, ifp->ifr_name);
      if (Ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
	 Error3("ioctl(%d, SIOCGIFINDEX, {\"%s\"}): %s",
		s, ifr.ifr_name, strerror(errno));
	 return 1;
      }
#if HAVE_STRUCT_IFREQ_IFR_INDEX
      fprintf(outfile, "%2d: %s\n", ifr.ifr_index, ifp->ifr_name);
#elif HAVE_STRUCT_IFREQ_IFR_IFINDEX
      fprintf(outfile, "%2d: %s\n", ifr.ifr_ifindex, ifp->ifr_name);
#endif /* HAVE_STRUCT_IFREQ_IFR_INDEX */
#else /* !defined(SIOCGIFINDEX) */
      fprintf(outfile, "%2d: %s\n", i/(int)sizeof(struct ifreq), ifp->ifr_name);
#endif /* defined(SIOCGIFINDEX) */
   }
   Close(s);
#endif /* defined(SIOCGIFCONF) */
   return 0;
}
#endif /* _WITH_SOCKET */


#if WITH_VSOCK
static int vsockan(FILE *outfile) {
	unsigned int cid;
	int vsock;
	if (Getuid() != 0) {
	   return 1;
	}
	if ((vsock = Open("/dev/vsock", O_RDONLY, 0)) < 0 ) {
		Warn1("open(\"/dev/vsock\", ...): %s", strerror(errno));
		return -1;
	} else if (Ioctl(vsock, IOCTL_VM_SOCKETS_GET_LOCAL_CID, &cid) < 0) {
		Warn2("ioctl(%d, IOCTL_VM_SOCKETS_GET_LOCAL_CID, ...): %s",
		      vsock, strerror(errno));
		return -1;
	} else {
		Notice1("VSOCK CID=%u", cid);
		fprintf(outfile, "\nVSOCK_CID        = %u\n", cid);
	}
	if (vsock >= 0) {
		Close(vsock);
	}
	return 0;
}
#endif /* WITH_VSOCK */
