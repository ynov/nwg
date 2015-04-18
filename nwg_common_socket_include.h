#ifndef NWG_COMMON_SOCKET_INCLUDE_H_
#define NWG_COMMON_SOCKET_INCLUDE_H_

#ifdef __unix__
#include <netinet/in.h>
#include <sys/socket.h>
#endif /* __unix__ */

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
typedef int socklen_t;

#undef FD_SETSIZE
#define FD_SETSIZE 2048
#endif /* _WIN32 */

#include <fcntl.h>
#include <unistd.h>

#endif /* NWG_COMMON_SOCKET_INCLUDE_H_ */
