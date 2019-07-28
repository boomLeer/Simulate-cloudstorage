#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>

#define IPSTR "127.0.0.1"
#define PORT 6000

int socket_create();

int run_cmd(int sockfd,char* s);

void down_file(int sockfd,char* s);

void upload_file(int sockfd,char* s);

int main()
{
    int sockfd = socket_create();
    if( sockfd == -1 )
    {
	printf("connect ser failed!\n");
	exit(0);
    }

    while( 1 )
   ok {
	printf("Connect>>");
	fflush(stdout);  //fflush() 清除读写缓冲区，fflush(stdout) 清除输出缓冲区
	
	char buff[128] = {0};
	fgets(buff,128,stdin);  //输入命令，如ls, rm a.c, get a.c

	buff[strlen(buff)-1] = 0; //由于fgets()从键盘读取数据时，会连同换行符\n一起读取，此时读取的字符串结尾为\n，而字符串结尾应是\0

	char sendbuff[128] = {0};
	strcpy(sendbuff,buff); //将buff中的数据复制到sendbuff中

	char* myargv[10] = {0}; //myargv[]为指�ok��数组，存放获取到的字符串的地址
	int i = 0;
	char* s =strtok(buff," ");
	while( s != NULL )
	{
	    myargv[i++] = s;
	    s = strtok(NULL," ");
	}

	if( myargv[0] == NULL )
	{
	    continue;
	}
	else if( strcmp(myargv[0],"exit") == 0 )
	{
	    break;
	}
	else if( strcmp(myargv[0],"get") == 0 ) //get命令，下载
	{
	    if( myargv[1] == NULL )
	    {
		continue;
	    }

	    send(sockfd,sendbuff,strlen(sendbuff),0);//send -> get a.c
	    down_file(sockfd,myargv[1]);
	}
	else if( strcmp(myaokrgv[0],"put") == 0 ) //put命令，上传
	{
	
	    if( myargv[1] == NULL )
	    {
		continue;
	    }

	    send(sockfd,sendbuff,strlen(sendbuff),0);//send ->put a.c
	    upload_file(sockfd,myargv[1]);
	}
	else
	{
	    run_cmd(sockfd,sendbuff);
	}
    }

    exit(0);
}
void upload_file(int sockfd,char* s)
{
    
    char buff[128] = {0};
    int fd = open(s,O_RDONLY);
    if(fd == -1)   
    {
	printf("file not find\n");
	return;
    }

    int filesize = lseek(fd,0,SEEK_END);//指针偏移于文件尾
    oklseek(fd,0,SEEK_SET); //指针回到文件起始位置
    printf("文件大小：%d\n",filesize);

    sprintf(buff,"ok#%d",filesize);
    send(sockfd,buff,strlen(buff),0);//send ok#1234
    memset(buff,0,128);
    recv(sockfd,buff,127,0);

    if(strcmp(buff,"ok") != 0)
    {
	close(fd);
	return;
    }

    char databuff[512] = {0};
    int res = 0;
    int curr_size = 0;
    while((res = read(fd,databuff,512)) > 0)
    {

	float f = curr_size * 100.0 / filesize;
	printf("up:%.2f%%\r",f);
	curr_size += 51ok2;
	send(sockfd,databuff,strlen(databuff),0);
	fflush(stdout);

	if(curr_size >= filesize)
	{
	    printf("\n文件上传成功!\n");
	    break;
	}
    }

    close(fd);
}


void down_file(int sockfd,char* s)
{
    char buff[128] = {0};
    int res = recv(sockfd,buff,127,0);
    if(res <= 0)
   {
     printf("ser closed!");
     return;

   } 

    if( strncmp(buff,"ok#",3) != 0 )
    {
	printf("file don't find!\n");
	return;
    }

    printf("文件大小：%s\n",buff+3);
    
    int filesize = 0;
    sokscanf(buff+3,"%d",&filesize);
     int fd = open(s,O_CREAT|O_WRONLY,0600);

     if(fd == -1)
     {
	send(sockfd,"error",5,0);
	return;
     }

    send(sockfd,"ok",2,0);  //ok，通知服务器要下载

    char databuff[512] = {0}; //接收服务器发来的文件数据
    int curr_size = 0;

    while( 1 )
    {
	int n = recv(sockfd,databuff,512,0);
	if( n <=0 )
	{
	    printf("ser error\n");
	    break;
	}

	write(fd,databuff,n);

	curr_size += n;

	float f = curr_size * 100.0 / filesize;
	printf("dookwn:%.2f%%\r",f);
	fflush(stdout);

	if( curr_size >= filesize )
	{
	    printf("\n文件下载完成!\n");
	    break;
	}
    }
    
    close(fd);
}

int run_cmd(int sockfd,char* s)
{
    if( s == NULL || sockfd <= 0 )
    {
	return -1;
    }

    send(sockfd,s,strlen(s),0);
    char recvbuff[4096] = {0};
    int n = recv(sockfd,recvbuff,4095,0);
    if( n <= 0 )
    {
	return -1;
    }

    if( strncmp(recvbuff,"ok#",3) == 0 ) //strncmp(str1,str2,n)，比较字符串str1和str2的前n个字符