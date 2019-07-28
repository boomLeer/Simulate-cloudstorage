#include <assert.h>
#include "socket.h"

#define port_ser 6000   //端口号
#define inet_addr_ser "127.0.0.1"

int create_socket()
{
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    assert( sockfd != -1);

    struct sockaddr_in saddr;
    memset(&saddr,0,sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port_ser);
    saddr.sin_addr.s_addr = inet_addr(inet_addr_ser);

    int res = bind(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
    assert( res != -1);

    listen(sockfd,5);

    return sockfd;
}
