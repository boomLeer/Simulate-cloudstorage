#include"thread.h"
#include<sys/wait.h>
#include<fcntl.h>

void recv_file(int c,char* name)
{   
    
    char buff[128] = {0};
    int res = recv(c,buff,127,0);
    if(res <= 0)
    {
	send(c,"error",5,0);
	return;
    }

    if( strncmp(buff,"ok#",3) != 0)
    {
	send(c,"error",5,0);
    }

    int filesize = 0;
    sscanf(buff+3,"%d",&filesize);

    int fd = open(name,O_CREAT|O_WRONLY,0600);
    if( fd == -1 )
    {
	send(c,"error",5,0); //失败，不下载
	return;
    }

    send(c,"ok",2,0);  //ok，通知服务器要下载

    char databuff[512] = {0}; //接收服务器发来的文件数据
    int curr_size = 0;

    while( 1 )
    {
	int n = recv(c,databuff,512,0);
	if( n <=0 )
	{
	    //printf("ser error\n");
	    break;
	}
	curr_size += 512;
	if(curr_size >= filesize)
	{
	    break;
	}
	write(fd,databuff,n);
    }
    close(fd);
}

void send_file(int c,char * name)
{
    int fd = open(name,O_RDONLY);
    if(fd == -1)   
    {
	send(c,"err",3,0);
	return;
    }

    int filesize = lseek(fd,0,SEEK_END);//指针偏移于文件尾
    lseek(fd,0,SEEK_SET); //指针回到文件起始位置

    char buff[128] = {0};
    sprintf(buff,"ok#%d",filesize);

    send(c,buff,strlen(buff),0);//send ok#1234
    memset(buff,0,128);
    recv(c,buff,127,0);

    if(strcmp(buff,"ok") != 0)
    {
	close(fd);
	return;
    }

    char databuff[512] = {0};
    int res = 0;

    while((res = read(fd,databuff,512)) > 0)
    {
	send(c,databuff,res,0);
    }
   close(fd);
}

void* thread_work(void* arg)
{
    int c = (int)arg;
    if(c < 0)
    {
	pthread_exit(NULL);
    }

    while(1)
    {
	char buff[128] = {0};
	int n = recv(c,buff,127,0);
	if( n <= 0)
	{
	    break;
	}
	
	char* myargv[10] = {0};
	char* ptr = NULL;
	int i = 0;
	char * s = strtok_r(buff," ",&ptr);

	while(s != NULL)
	{
	    myargv[i++] = s;
	    s = strtok_r(NULL," ",&ptr);
	}
	if( myargv[0] == NULL )
	{
	    send(c,"error",5,0);
	    continue;
	}

	else if( strcmp(myargv[0],"get") == 0)
	{
	    //下载
	    send_file(c,myargv[1]);
	}
	else if( strcmp(myargv[0],"put") == 0)
	{
	    //上传
	    recv_file(c,myargv[1]);
	}
	else
	{
	    int pipefd[2];
	    pipe(pipefd);

	    pid_t pid = fork();

	    if( pid == -1)
	    {
		send(c,"err",3,0);
		continue;
	    }

	    if(pid == 0)
	    {
		dup2(pipefd[1],1);//1 标准输出
		dup2(pipefd[1],2);//2 标准错误输出 
                // 使用管道中的数据覆盖1，2，之后所有输出只从管道中进行
		execvp(myargv[0],myargv);
		perror("exec error");
		exit(0);
	    }
	    close(pipefd[1]);
	    wait(NULL);

	    char readbuff[4096] = {"ok#"};
	    read(pipefd[0],readbuff+3,4092);

	    send(c,readbuff,strlen(readbuff),0);
	    close(pipefd[0]);// 0 读 1 写
	}
    }
    close(c);
    printf("one client over\n");
}

void thread_start(int c)
{
    pthread_t id;
    pthread_create(&id,NULL,thread_work,(void*)c);
}
