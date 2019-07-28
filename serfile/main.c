#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "socket.h"
#include "thread.h"

int main()
{
   int sockfd = create_socket();
   assert(sockfd != -1);

   while ( 1 )
   {

       struct sockaddr_in caddr;
	int len = sizeof(caddr);
	int c = accept(sockfd,(struct sockaddr*)&caddr,&len);
	if(c < 0)
	{
	    continue;
	}
	thread_start(c);
   }
}
