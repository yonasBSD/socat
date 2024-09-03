cmake_minimum_required(VERSION 3.20)

project(socat)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall")

find_package(PkgConfig)

set(SOURCES
src/dalan.c
src/error.c
src/filan.c
src/fdname.c
src/hostan.c
src/nestlex.c
src/snprinterr.c
src/socat.c
src/sslcls.c
src/sycls.c
src/sysutils.c
src/utils.c
src/vsnprintf_r.c
src/xio-ascii.c
src/xio-creat.c
src/xio-dccp.c
src/xio-exec.c
src/xio-fd.c
src/xio-fdnum.c
src/xio-file.c
src/xio-fs.c
src/xio-gopen.c
src/xio-interface.c
src/xio-ip.c
src/xio-ip4.c
src/xio-ip6.c
src/xio-ipapp.c
src/xio-listen.c
src/xio-named.c
src/xio-namespaces.c
src/xio-openssl.c
src/xio-pipe.c
src/xio-posixmq.c
src/xio-process.c
src/xio-progcall.c
src/xio-proxy.c
src/xio-pty.c
src/xio-rawip.c
src/xio-readline.c
src/xio-sctp.c
src/xio-shell.c
src/xio-socket.c
src/xio-socketpair.c
src/xio-socks.c
src/xio-socks5.c
src/xio-stdio.c
src/xio-streams.c
src/xio-system.c
src/xio-tcp.c
src/xio-tcpwrap.c
src/xio-termios.c
src/xio-tun.c
src/xio-udp.c
src/xio-udplite.c
src/xio-unix.c
src/xio-vsock.c
src/xioclose.c
src/xiodiag.c
src/xioexit.c
src/xiohelp.c
src/xioinitialize.c
src/xiolayer.c
src/xiolockfile.c
src/xioopen.c
src/xioopts.c
src/xioparam.c
src/xioread.c
src/xioshutdown.c
src/xiosigchld.c
src/xiosignal.c
src/xiowrite.c
)

add_executable(socat ${SOURCES})

# from http://websvn.kde.org/trunk/KDE/kdeedu/cmake/modules/FindReadline.cmake
# http://websvn.kde.org/trunk/KDE/kdeedu/cmake/modules/COPYING-CMAKE-SCRIPTS
# --> BSD licensed
#
# GNU Readline library finder
if(READLINE_INCLUDE_DIR AND READLINE_LIBRARY AND NCURSES_LIBRARY)
  set(READLINE_FOUND TRUE)
else(READLINE_INCLUDE_DIR AND READLINE_LIBRARY AND NCURSES_LIBRARY)
  FIND_PATH(READLINE_INCLUDE_DIR readline/readline.h
    /usr/include/readline
  )
  
# 2008-04-22 The next clause used to read like this:
#
#  FIND_LIBRARY(READLINE_LIBRARY NAMES readline)
#        FIND_LIBRARY(NCURSES_LIBRARY NAMES ncurses )
#        include(FindPackageHandleStandardArgs)
#        FIND_PACKAGE_HANDLE_STANDARD_ARGS(Readline DEFAULT_MSG NCURSES_LIBRARY READLINE_INCLUDE_DIR READLINE_LIBRARY )
#
# I was advised to modify it such that it will find an ncurses library if
# required, but not if one was explicitly given, that is, it allows the
# default to be overridden. PH 

  FIND_LIBRARY(READLINE_LIBRARY NAMES readline)
        include(FindPackageHandleStandardArgs)
        FIND_PACKAGE_HANDLE_STANDARD_ARGS(Readline DEFAULT_MSG READLINE_INCLUDE_DIR READLINE_LIBRARY )

  MARK_AS_ADVANCED(READLINE_INCLUDE_DIR READLINE_LIBRARY)
endif(READLINE_INCLUDE_DIR AND READLINE_LIBRARY AND NCURSES_LIBRARY)

if(READLINE_FOUND)
	message(STATUS "Compiling with readline support")
	target_link_libraries(socat ${READLINE_LIBRARY})
endif(READLINE_FOUND)

include_directories(/usr/local/include)

find_package(OpenSSL REQUIRED)

message("OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
message("OpenSSL libraries: ${OPENSSL_LIBRARIES}")

include_directories(${OPENSSL_INCLUDE_DIR})
target_link_libraries(socat ${OPENSSL_LIBRARIES})

target_link_libraries(socat wrap)
target_link_libraries(socat util)
