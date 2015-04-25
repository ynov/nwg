#ifndef NWG_COMMON_SOCKET_INCLUDE_H_
#define NWG_COMMON_SOCKET_INCLUDE_H_

#if !defined(__unix__) && (defined(__APPLE__) && defined(__MACH__))
#define __unix__ 1
#endif

#ifdef __unix__
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif /* __unix__ */

#ifdef _WIN32
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0600

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
typedef int socklen_t;

#undef FD_SETSIZE
#define FD_SETSIZE 2048

#endif /* _WIN32 */

#include <fcntl.h>
#include <unistd.h>

#endif /* NWG_COMMON_SOCKET_INCLUDE_H_ */
