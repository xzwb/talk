#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>

#define PORT 4507
#define MAXSU 1024

typedef struct user1{
        char name[20]; //用户名
        char password[20]; //密码
        struct user1 *next;
}UO;

typedef struct onuser{
        int on_user; //在线人数
        char on_user_name[100][20];
}ONUSE;

typedef struct info{
        char name[20]; //用户名
        char buf[MAXSU]; //数据缓冲区
        int tid; //socket套接字
}INFO;

pthread_t tid[2];

char *s_gets(char *a , int n)
{
	char *b;
	int i = 0;
	b = fgets(a,n,stdin);
	if(b)
	{
		while(a[i] != '\n' && a[i] != '\0')
			i++;
		if(a[i] == '\n')
			a[i] = '\0';
		else
			while(getchar() != '\n')
				continue;
	}
	return b;
}

//错误处理函数
void my_err(const char *err_string , int line)
{
        fprintf(stderr,"line:%d",line);
        perror(err_string);
        exit(1);
}

//菜单
void caidan()
{
	printf("\033c");
	printf("********************哈哈哈哈聊天室********************\n"
               "**                                                  **\n"
               "**                  1. 私聊	                    **\n"
               "**                  2. 删除好友 	            **\n"
               "**                  3. 查看当前在线好友             **\n"
               "**                  4. 发送好友申请 	            **\n"
               "**                  5. 接受好友申请                 **\n"
	       "**                  6. 退出登录                     **\n"
	       "**                                                  **\n"
               "******************************************************\n");
}

void *my_send(void *conn_fd)
{
	int i;
	INFO *buff;
	UO *f;
	char rev;
	char sed_buf[20];
	f = (UO*)malloc(sizeof(UO));
	char choose;
	char a;
	buff = (INFO*)malloc(sizeof(INFO));
	while(1)
	{
		printf("请选择:\n1.登录   2.注册\n");
		choose = getchar();
		getchar();
		if(send(*(int*)conn_fd,&choose,sizeof(char),0) < 0)
			my_err("send",__LINE__);
		if(choose == '1')
		{
			printf("请输入\n用户名:\n");
		//	scanf("%s",f->name);
			s_gets(f->name,20);
		//	getchar();
			printf("密码:\n");
			s_gets(f->password,20);
		//	scanf("%s",f->password);
		//	getchar();
			printf("%s\n %s\n",f->name,f->password);
			if(send(*(int*)conn_fd,f,sizeof(UO),0) < 0)
				my_err("send",__LINE__);
		}
		else if(choose == '2')
		{
			printf("请输入\n用户名:\n");
			s_gets(f->name,20);
		//	scanf("%s",f->name);
		//	getchar();
			printf("密码:\n");
			s_gets(f->password,20);
		//	scanf("%s",f->password);
		//	getchar();
			if(send(*(int*)conn_fd , f , sizeof(UO), 0) < 0)
				my_err("send",__LINE__);
		}
		caidan();
A:
		printf("请选择您需要的功能:\n");
		choose = getchar();
		getchar();
		if(choose < '1' || choose > '6')
		{
			printf("输入错误!\n");
			goto A;
		}
		if(send(*(int*)conn_fd,&choose,sizeof(char),0) < 0)
			my_err("send",__LINE__);
		switch(choose)
		{
			case '1':
				{
					printf("请输入要发送的好友昵称:\n");
					s_gets(buff->name,20);
				//	scanf("%s",buff->name);
				//	getchar();
					printf("消息内容:");
				//	scanf("%s",buff->buf);
				//	getchar();
					s_gets(buff->buf,1024);
					if(send(*(int*)conn_fd,buff,sizeof(INFO),0) < 0)
						my_err("send",__LINE__);
					goto A;
				}
			case '2':
				{
					printf("输入要删除的好友的昵称:\n");
					s_gets(sed_buf,20);
				//	scanf("%s",sed_buf);
				//	getchar();
					if(send(*(int*)conn_fd , sed_buf , sizeof(sed_buf) , 0) < 0)
						my_err("send",__LINE__);
					goto A;
				}
			case '3':
				{
					goto A;
				}
			case '4':
				{
					printf("情输入想要添加的用户昵称:\n");
					s_gets(sed_buf,20);
				//	scanf("%s",sed_buf);
				//	getchar();
					if(send(*(int*)conn_fd,sed_buf,sizeof(sed_buf),0) < 0)
						my_err("send",__LINE__);
					goto A;
				}
			case '5':
				{
					a = getchar();
					getchar();
                                        if(send(*(int*)conn_fd,&a,sizeof(char),0) < 0)
                                                my_err("send",__LINE__);
					goto A;
				}
			case '6':
				{
					close(*(int*)conn_fd);
					pthread_cancel(tid[1]);
					exit(0);
				}
			default:exit(1);
		}
	}
}

void *my_recv(void *conn_fd)
{
	char a;
	char rev = 0;
	char ref[20];
	char ref_buf[100];
	INFO *buff;
	int i;
	buff = (INFO*)malloc(sizeof(INFO));
	ONUSE *online;
        online = (ONUSE*)malloc(sizeof(ONUSE));
	while(1)
	{
		if(recv(*(int*)conn_fd,&rev,sizeof(char),0) < 0)
			my_err("recv",__LINE__);
		switch(rev)
		{
			case '0':
				{
					memset(ref_buf,0,sizeof(ref_buf));
					if(recv(*(int*)conn_fd,ref_buf,sizeof(ref_buf),0) < 0)
						my_err("recv",__LINE__);
					printf("%s\n",ref_buf);
					break;
				}
			case '1':
				{
					memset(ref_buf,0,sizeof(ref_buf));
                                        if(recv(*(int*)conn_fd,ref_buf,sizeof(ref_buf),0) < 0)
                                                my_err("recv",__LINE__);
                                        printf("%s\n",ref_buf);
                                        break;
				}
			case '2':
				{
					memset(online,0,sizeof(INFO));
					if(recv(*(int*)conn_fd,buff,sizeof(INFO),0) < 0)
						my_err("recv",__LINE__);
					printf("接收到%s的消息:%s\n",buff->name,buff->buf);
					break;
				}
			case '3':
				{
					memset(ref,0,sizeof(ref));
					if(recv(*(int*)conn_fd,ref,sizeof(ref),0) < 0)
						my_err("recv",__LINE__);
					printf("接收到%s的好友请求!\ny(接受)   n(拒绝)\n",ref);
					break;
				}
			case '4':
				{
					if(recv(*(int*)conn_fd,online,sizeof(ONUSE),0) < 0)
						my_err("recv",__LINE__);
					printf("当前好友在线人数为%d\n",online->on_user);
					for(i=0;i<online->on_user;i++)
						printf("%s\n",online->on_user_name[i]);
					break;
				}
			default:exit(0);
		}
	}
}

int main(int argc , char *argv[])
{
        int sock_fd , ret;
        int haha;
        struct sockaddr_in seve;
        if((sock_fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
                my_err("socket",__LINE__);
        seve.sin_family = AF_INET;
        seve.sin_port = htons(PORT);
        inet_aton("192.168.43.23",&seve.sin_addr);
        if(connect(sock_fd,(struct sockaddr*)&seve,sizeof(struct sockaddr_in)) < 0)
                my_err("connect",__LINE__);
	if((ret = pthread_create(&tid[0],NULL,my_send,(void*)&sock_fd)) < 0)
                my_err("pthread_creat",__LINE__);
        if((ret = pthread_create(&tid[1],NULL,my_recv,(void*)&sock_fd)) < 0)
                my_err("pthread_creat",__LINE__);
        pthread_join(tid[0],NULL);
        pthread_join(tid[1],NULL);
        return 0;
}

