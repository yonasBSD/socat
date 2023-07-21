/* Source: xio-namespaces.c */
/* Copyright Gerhard Rieger and contributors (see file CHANGES) */
/* Published under the GNU General Public License V.2, see file COPYING */

/* This file contains Linux namespace related code */

#include "xiosysincludes.h"
#include "xioopen.h"

#include "xio-namespaces.h"

#if WITH_NAMESPACES

const struct optdesc opt_set_netns   = { "netns",      NULL, OPT_SET_NETNS,   GROUP_PROCESS, PH_INIT, TYPE_STRING, OFUNC_SET_NAMESPACE,   0, 0, 0 };


/* Set the given namespace. Requires root or the appropriate CAP_*-
   Returns 0 on success, or -1 on error. */
int xio_set_namespace(
	const char *nstype,
	const char *nsname)
{
	char nspath[PATH_MAX];
	int nsfd;
	int rc;

	if (!xioparms.experimental) {
		Error1("option \"%s\" requires use of --experimental", nstype);
	}

	snprintf(nspath, sizeof(nspath)-1, "/run/%s/%s", nstype, nsname);
	Info1("switching to net namespace \"%s\"", nsname);
	nsfd = Open(nspath, O_RDONLY|O_CLOEXEC, 000);
	if (nsfd < 0) {
		Error2("open(%s, O_RDONLY|O_CLOEXEC): %s", nspath, strerror(errno));
		return -1;
	}
	rc = Setns(nsfd, CLONE_NEWNET);
	if (rc < 0) {
		Error2("setns(%d, CLONE_NEWNET): %s", nsfd, strerror(errno));
		Close(nsfd);
	}
	Close(nsfd);
	return 0;
}

/* Sets the given namespace to that of process 1, this is assumed to be the
   systems default.
   Returns 0 on success, or -1 on error. */
int xio_reset_namespace(
	const char *nstype)
{
	char nspath[PATH_MAX];
	int nsfd;
	int rc;

	snprintf(nspath, sizeof(nspath)-1, "/proc/1/ns/%s", nstype);
	Info("switching back to default namespace");
	nsfd = Open(nspath, O_RDONLY|O_CLOEXEC, 000);
	if (nsfd < 0) {
		Error2("open(%s, O_RDONLY|O_CLOEXEC): %s", nspath, strerror(errno));
		return -1;
	}
	rc = Setns(nsfd, CLONE_NEWNET);
	if (rc < 0) {
		Error2("setns(%d, CLONE_NEWNET): %s", nsfd, strerror(errno));
		Close(nsfd);
	}
	Close(nsfd);
	return 0;
}

#endif /* WITH_NAMESPACES */
