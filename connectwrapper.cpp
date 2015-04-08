
#include "connectwrapper.h"

#include "sys/socket.h"

int socket_connect (int fd, __CONST_SOCKADDR_ARG addr, socklen_t len)
{

    connect(fd, addr, len);

}
