/* source: procan.c */
/* Copyright Gerhard Rieger and contributors (see file CHANGES) */
/* Published under the GNU General Public License V.2, see file COPYING */

/* the subroutine procan makes a "PROCess ANalysis". It gathers information
   about the process environment it is running in without modifying its state
   (almost).
 */

#include "xiosysincludes.h"
#include "mytypes.h"
#include "compat.h"
#include "error.h"
#include "sycls.h"
#include "sysutils.h"
#include "sched.h"
#include "filan.h"

#include <sys/resource.h>

#include "procan.h"


int procan(FILE *outfile) {

   /*filan(0, outfile);*/

   /* controlling terminal */
   fprintf(outfile, "process id = "F_pid"\n", Getpid());
   fprintf(outfile, "process parent id = "F_pid"\n", Getppid());
   {
      int fd;
      if ((fd = Open("/dev/tty", O_NOCTTY, 0)) < 0) {
	 fprintf(outfile, "controlling terminal: -\n");
      } else {
#if 1
	 fprintf(outfile, "controlling terminal: \"%s\"\n", Ttyname(fd));
#else
	 char procpath[PATH_MAX], devpath[PATH_MAX+1];
	 int devlen;
	 sprintf(procpath, "/proc/"F_pid"/fd/%d", Getpid(), 0 /*! fd*/);
	 if ((devlen = Readlink(procpath, devpath, sizeof(devpath))) < 0) {
	    ;
	 } else {
	    devpath[devlen] = '\0';
	    fprintf(outfile, "controlling terminal: \"%s\"\n", devpath);
	 }
#endif
      }
   }
   fprintf(outfile, "process group id = "F_pid"\n", Getpgrp());
#if HAVE_GETSID
   fprintf(outfile, "process session id = "F_pid"\n", Getsid(0));
#endif
   fprintf(outfile, "process group id if fg process / stdin = "F_pid"\n", Tcgetpgrp(0));
   fprintf(outfile, "process group id if fg process / stdout = "F_pid"\n", Tcgetpgrp(1));
   fprintf(outfile, "process group id if fg process / stderr = "F_pid"\n", Tcgetpgrp(2));
   {
      int fd;
      if ((fd = Open("/dev/tty", O_RDWR, 0600)) >= 0) {
	 fprintf(outfile, "process has a controlling terminal\n");
	 Close(fd);
      } else {
	 fprintf(outfile, "process does not have a controlling terminal\n");
      }
   }

   /* process owner, groups */
   fprintf(outfile, "user id  = "F_uid"\n", Getuid());
   fprintf(outfile, "effective user id  = "F_uid"\n", Geteuid());
   fprintf(outfile, "group id = "F_gid"\n", Getgid());
   fprintf(outfile, "effective group id = "F_gid"\n", Getegid());

   /* Simple process features */
   fprintf(outfile, "\n");
   {
      mode_t mask;
#if LATER
      char procpath[PATH_MAX];
      sprintf(procpath, "/proc/"F_pid"/status", Getpid());
      if (Stat()) {

      } else
#endif
      {
	 mask = Umask(0066);
	 Umask(mask);
      }
      fprintf(outfile, "umask = "F_mode"\n", mask);
   }

   {
      struct rlimit rlim;

      fprintf(outfile, "\n/* Resource limits */\n");
      fprintf(outfile, "resource                                 current                 maximum\n");
      if (getrlimit(RLIMIT_CPU, &rlim) < 0) {
	 Warn2("getrlimit(RLIMIT_CPU, %p): %s", &rlim, strerror(errno));
      } else {
	 fprintf(outfile,
		 "cpu time (seconds)      %24"F_rlim_max"%24"F_rlim_max"\n",
		 rlim.rlim_cur, rlim.rlim_max);
      }
      if (getrlimit(RLIMIT_FSIZE, &rlim) < 0) {
	 Warn2("getrlimit(RLIMIT_FSIZE, %p): %s", &rlim, strerror(errno));
      } else {
	 fprintf(outfile,
		 "file size (blocks)      %24"F_rlim_max"%24"F_rlim_max"\n",
		 rlim.rlim_cur, rlim.rlim_max);
      }
      if (getrlimit(RLIMIT_DATA, &rlim) < 0) {
	 Warn2("getrlimit(RLIMIT_DATA, %p): %s", &rlim, strerror(errno));
      } else {
	 fprintf(outfile,
		 "data seg size (kbytes)  %24"F_rlim_max"%24"F_rlim_max"\n",
		 rlim.rlim_cur, rlim.rlim_max);
      }
      if (getrlimit(RLIMIT_STACK, &rlim) < 0) {
	 Warn2("getrlimit(RLIMIT_STACK, %p): %s", &rlim, strerror(errno));
      } else {
	 fprintf(outfile,
		 "stack size (blocks)     %24"F_rlim_max"%24"F_rlim_max"\n",
		 rlim.rlim_cur, rlim.rlim_max);
      }
      if (getrlimit(RLIMIT_CORE, &rlim) < 0) {
	 Warn2("getrlimit(RLIMIT_CORE, %p): %s", &rlim, strerror(errno));
      } else {
	 fprintf(outfile,
		 "core file size (blocks) %24"F_rlim_max"%24"F_rlim_max"\n",
		 rlim.rlim_cur, rlim.rlim_max);
      }
#ifdef RLIMIT_RSS	/* Linux, AIX; not Cygwin */
      if (getrlimit(RLIMIT_RSS, &rlim) < 0) {
	 Warn2("getrlimit(RLIMIT_RSS, %p): %s", &rlim, strerror(errno));
      } else {
	 fprintf(outfile,
		 "max resident set size   %24"F_rlim_max"%24"F_rlim_max"\n",
		 rlim.rlim_cur, rlim.rlim_max);
      }
#endif
#ifdef RLIMIT_NPROC	/* Linux, not AIX, Cygwin */
      if (getrlimit(RLIMIT_NPROC, &rlim) < 0) {
	 Warn2("getrlimit(RLIMIT_NPROC, %p): %s", &rlim, strerror(errno));
      } else {
	 fprintf(outfile,
		 "max user processes      %24"F_rlim_max"%24"F_rlim_max"\n",
		 rlim.rlim_cur, rlim.rlim_max);
      }
#endif
#ifdef RLIMIT_NOFILE	/* not AIX 4.1 */
      if (getrlimit(RLIMIT_NOFILE, &rlim) < 0) {
	 Warn2("getrlimit(RLIMIT_NOFILE, %p): %s", &rlim, strerror(errno));
      } else {
	 fprintf(outfile,
		 "open files              %24"F_rlim_max"%24"F_rlim_max"\n",
		 rlim.rlim_cur, rlim.rlim_max);
      }
#endif
#ifdef RLIMIT_MEMLOCK	/* Linux, not AIX, Cygwin */
      if (getrlimit(RLIMIT_MEMLOCK, &rlim) < 0) {
	 Warn2("getrlimit(RLIMIT_MEMLOCK, %p): %s", &rlim, strerror(errno));
      } else {
	 fprintf(outfile,
		 "max locked-in-memory\n  address space         %24"F_rlim_max"%24"F_rlim_max"\n",
		 rlim.rlim_cur, rlim.rlim_max);
      }
#endif
#ifdef RLIMIT_AS
      if (getrlimit(RLIMIT_AS, &rlim) < 0) {
	 Warn2("getrlimit(RLIMIT_AS, %p): %s", &rlim, strerror(errno));
      } else {
	 fprintf(outfile,
		 "virtual memory (kbytes) %24"F_rlim_max"%24"F_rlim_max"\n",
		 rlim.rlim_cur, rlim.rlim_max);
      }
#endif
      fputc('\n', outfile);

   }

#ifdef CC
   fprintf(outfile, "// CC:                      "CC"\n");
#endif
#ifdef __STDC_VERSION__
   fprintf(outfile, "#define __STDC_VERSION__    %ld\n", __STDC_VERSION__);
#endif
#ifdef SIZE_MAX
   fprintf(outfile, "SIZE_MAX                  = "F_Zu" /* maximum value of size_t */\n", SIZE_MAX);
#endif
#ifdef P_tmpdir
   fprintf(outfile, "P_tmpdir                  = \"%s\"\n", P_tmpdir);
#endif
#ifdef L_tmpnam
   fprintf(outfile, "L_tmpnam                  = %u\n", L_tmpnam);
#endif
#ifdef TMP_MAX
   fprintf(outfile, "TMP_MAX                   = %d\n", TMP_MAX);
#endif
#ifdef FD_SETSIZE
   fprintf(outfile, "FD_SETSIZE                = %d /* maximum number of FDs for select() */\n", FD_SETSIZE);
#endif
#ifdef PIPE_BUF
   fprintf(outfile, "PIPE_BUF                  = %-24d\n", PIPE_BUF);
#endif

   /* Name spaces */
   {
      char path[PATH_MAX];
      char link[PATH_MAX];
      snprintf(path, sizeof(path)-1, "/proc/"F_pid"/ns/net", getpid());
      if (readlink(path, link, sizeof(link)-1) >= 0) {
	 fprintf(outfile, "Network namespace: %s", link);
      }
   }

   /* file descriptors */

   /* what was this for?? */
   /*Sleep(1);*/
   return 0;
}
