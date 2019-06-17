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

#define PORT 4507 //端口号
#define LISTENQ 100  //最大连接队列
#define IP 192.168.28.167 //服务端IP地址
#define MAXSU 1024 //用户最大输入
#define ONLINE 1 //在线
#define NONLINE 0 //不在线

int user_number = 0 ;//记录有多少个用户
FILE *fp,*pf;

typedef struct user1{
	char name[20]; //用户名
	char password[20]; //密码
	struct user1 *next;
}UO;

typedef struct user2{
	char name[20]; //用户名
	int fr_number; //该用户的好友数量
	char fr_name[100][20]; //好友列表
	struct user2 *next;
}US;

typedef struct user3{
	char name[20]; //用户名
	int on; //on等于0说明不在线，等于1说明在线
	int tid; //该用户的socket套接字
	struct user3 *next;
}UT;

typedef struct info{
	char name[20]; //用户名
	char buf[MAXSU]; //数据缓冲区
	int tid; //socket套接字
}INFO;

typedef struct onuser{
	int on_user; //在线人数
	char on_user_name[100][20];
}ONUSE;

//错误处理函数
void my_err(const char *err_string , int line)
{
        fprintf(stderr,"line:%d",line);
        perror(err_string);
        exit(1);
}

//my_send函数
void my_send(int conn_fd , const char *string)
{
	if(send(conn_fd,string,strlen(string),0) < 0)
		my_err("send",__LINE__);
}

//将UF用户数据从文件中取出来
UO *f_quchu()
{
	UO *f_head = NULL;
        UO *p ,*temp;
        int i = 0;//判断取多少
        if((pf = fopen("user_number.dat","rb")) == NULL)
        {
		printf("无法打开user_number.dat文件\n");
		exit(1);
	}
        fread(&user_number,1,sizeof(int),pf);
        if(user_number == 0)
        {
                printf("文件中暂无用户数据，请添加!\n");
                fclose(pf);
                return 0;
        }
        if((fp = fopen("user_data.dat","rb"))  ==  0)
	{
		printf("无法打开文件user_data.dat\n");
		exit(1);
	}
        temp = NULL;
        //尾插法建立链表
        while(i < user_number)
        {
                p = (UO*)malloc(sizeof(UO));
                fread(p,1,sizeof(UO),fp);
                if(f_head == NULL)
			f_head = p;
                else
                        temp->next = p;
                temp = p;
                i++;
        }
        if(temp != NULL)
                temp->next = NULL;
        printf("现在共有%d个用户数据\n",user_number);
        fclose(fp);
        return f_head;
}

//将UF用户数据放入文件中
int f_add(UO *f_head)
{
        UO *p = f_head;
        if((pf = fopen("user_number.dat","wb")) == NULL)
        {
		printf("无法打开文件user_nuymber.dat\n");
		exit(1);
	}
        if((fp = fopen("user_data.dat","wb")) == NULL)
        {
		printf("无法打开文件user_dat.dat\n");
		exit(1);
	}
        while(p)
        {
                fwrite(p,sizeof(UO),1,fp);
                p = p->next;
        }
        printf("文件中现在有%d个数据\n",user_number);
        fwrite(&user_number,1,sizeof(int),pf);
        fclose(pf);
        fclose(fp);
        return 0;
}

//将US数据取出来
US *s_quchu()
{
	US *s_head = NULL;
        US *p ,*temp;
        int i = 0;//判断取多少
        if((pf = fopen("user_number.dat","rb")) == NULL)
        {
                printf("无法打开user_number.dat文件\n");
                exit(1);
        }
        fread(&user_number,1,sizeof(int),pf);
        if(user_number == 0)
        {
                printf("文件中暂无用户数据，请添加!\n");
                fclose(pf);
                return 0;
        }
        if((fp = fopen("user2_data.dat","rb"))  ==  0)
        {
                printf("无法打开文件user2_data.dat\n");
                exit(1);
        }
        temp = NULL;
        //尾插法建立链表
        while(i < user_number)
        {
                p = (US*)malloc(sizeof(US));
                fread(p,1,sizeof(US),fp);
                if(s_head == NULL)
                        s_head = p;
                else
                        temp->next = p;
                temp = p;
                i++;
        }
        if(temp != NULL)
                temp->next = NULL;
        printf("现在共有%d个用户数据\n",user_number);
        fclose(fp);
        return s_head;
}

//将US中的数据放入文件
int s_add(US *s_head)
{
	US *p = s_head;
        if((pf = fopen("user_number.dat","wb")) == NULL)
        {
                printf("无法打开文件user_nuymber.dat\n");
                exit(1);
        }
        if((fp = fopen("user2_data.dat","wb")) == NULL)
        {
                printf("无法打开文件user_dat.dat\n");
                exit(1);
        }
        while(p)
        {
                fwrite(p,sizeof(US),1,fp);
                p = p->next;
        }
        printf("文件中现在有%d个数据\n",user_number);
        fwrite(&user_number,1,sizeof(int),pf);
        fclose(pf);
        fclose(fp);
        return 0;
}

//将UT中的文件取出来
UT *t_quchu()
{
	UT *t_head = NULL;
        UT *p ,*temp;
        int i = 0;//判断取多少
        if((pf = fopen("user_number.dat","rb")) == NULL)
        {
                printf("无法打开user_number.dat文件\n");
                exit(1);
        }
        fread(&user_number,1,sizeof(int),pf);
        if(user_number == 0)
        {
                printf("文件中暂无用户数据，请添加!\n");
                fclose(pf);
                return 0;
        }
        if((fp = fopen("user3_data.dat","rb"))  ==  0)
        {
                printf("无法打开文件user3_data.dat\n");
                exit(1);
        }
        temp = NULL;
        //尾插法建立链表
        while(i < user_number)
        {
                p = (UT*)malloc(sizeof(UT));
                fread(p,1,sizeof(UT),fp);
                if(t_head == NULL)
                        t_head = p;
                else
                        temp->next = p;
                temp = p;
                i++;
        }
        if(temp != NULL)
                temp->next = NULL;
        printf("现在共有%d个用户数据\n",user_number);
        fclose(fp);
        return t_head;
}

//将UT中的数据放入文件中
int t_add(UT *t_head)
{
	UT *p = t_head;
        if((fp = fopen("user_number.dat","wb")) == NULL)
        {
                printf("无法打开文件user_nuymber.dat\n");
                exit(1);
        }
        if((pf = fopen("user3_data.dat","wb")) == NULL)
        {
                printf("无法打开文件user_dat.dat\n");
                exit(1);
        }
        while(p)
        {
                fwrite(p,sizeof(UT),1,pf);
                p = p->next;
        }
        printf("文件中现在有%d个数据\n",user_number);
        fwrite(&user_number,1,sizeof(int),fp);
        fclose(pf);
        fclose(fp);
        return 0;

}

//用户数据的增加
int user_add(UO *f, int conn_fd)
{
	printf("a\n");
	US *s,*s_temp,*s_head = NULL;
	UT *t,*t_temp,*t_head = NULL;
	UO *f_temp,*f_head =NULL;
	s = (US*)malloc(sizeof(US));
	strcpy(s->name,f->name);
	s->fr_number = 0;
	memset(s->fr_name,0,sizeof(s->fr_name));
	s_head = s_quchu();
	printf("b\n");
	if(s_head)
        {
		printf("c\n");
                s_temp = s_head;
                while(s_temp->next != NULL)
                        s_temp = s_temp->next;
                s_temp->next = s;
                s_temp->next->next = NULL;
        }
        else
        {
		printf("d\n");
                s_head = s;
                s_head->next = NULL;
        }
	s_add(s_head);
	t = (UT*)malloc(sizeof(UT));
	strcpy(t->name,f->name);
	t->on = ONLINE;
	t->tid = conn_fd;
	t_head = t_quchu();
	if(t_head)
        {
                t_temp = t_head;
                while(t_temp->next != NULL)
                        t_temp = t_temp->next;
                t_temp->next = t;
                t_temp->next->next = NULL;
        }
        else
        {
                t_head = t;
                t_head->next = NULL;
        }
	t_add(t_head);
	f_head = f_quchu();
	if(f_head)
	{
		f_temp = f_head;
		while(f_temp->next != NULL)
			f_temp = f_temp->next;
		f_temp->next = f;
		f_temp->next->next = NULL;
	}
	else
	{
		f_head = f;
		f_head->next = NULL;
	}
	user_number++;
	f_add(f_head);
	printf("注册成功!\n");
}

//用户修改密码
int xiugai(UO *f)
{
	UO *f_temp,*f_head;
	f_head = f_quchu();
	f_temp = f_head;
	int i;
	for(i=0;i<user_number;i++)
	{
		if((strcmp(f_temp->name,f->name) == 0))
		{
			strcpy(f_temp->password,f->password);
			printf("修改成功!\n");
			break;
		}
	}
	f_add(f_head);
	return 1;
}

//用户删除好友
int fr_del(const char *a,const char *b)
{
	US *s,*s_head;
	int i;
	s_head = s_quchu();
	s = s_head;
	while(s != NULL)
	{
		if((strcmp(s->name,a)) == 0)
		{
			for(i=0;i < s->fr_number;i++)
			{
				if((strcmp(s->fr_name[i],b)) == 0)
				{
					if(i == s->fr_number-1)
					{
						memset(s->fr_name[i],0,sizeof(s->fr_name[i]));
						s->fr_number--;
					}
					else
					{
						while(i < s->fr_number-1)
							strcpy(s->fr_name[i],s->fr_name[i+1]);
						s->fr_number--;
					}
				}
				break;
			}
			if(i >= s->fr_number)
			{	
				printf("没有id为%s的好友\n",b);
				return 0;
			}
			break;
		}
		s = s->next;
	}
	s = s_head;
        while(s != NULL)
        {
                if((strcmp(s->name,b)) == 0)
                {
                        for(i=0;i < s->fr_number;i++)
                        {
                                if((strcmp(s->fr_name[i],a)) == 0)
                                {
                                        if(i == s->fr_number-1)
                                        {
                                                memset(s->fr_name[i],0,sizeof(s->fr_name[i]));
                                                s->fr_number--;
                                        }
                                        else
                                        {
                                                while(i < s->fr_number-1)
                                                        strcpy(s->fr_name[i],s->fr_name[i+1]);
                                                s->fr_number--;
                                        }
                                }
                                break;
                        }
                        if(i >= s->fr_number)
                        {
                                printf("没有id为%s的好友\n",a);
                                return 0;
                        }
			break;
                }
                s = s->next;
        }
	s_add(s_head);
	printf("删除成功!\n");
	return 1;
}

//添加好友
int fr_add(const char *a,const char *b)
{
	US *s,*s_head;
	s_head = s_quchu();
	s = s_head;
	while(s != NULL)
	{
		if(strcmp(s->name,a) == 0)
		{
			strcpy(s->fr_name[s->fr_number],b);
			s->fr_number++;
		}
		s = s->next;
	}
	s = s_head;
	while(s != NULL)
	{
		if(strcmp(s->name,b) == 0)
		{
			strcpy(s->fr_name[s->fr_number],a);
			s->fr_number++;
		}
		s = s->next;
	}
	s_add(s_head);
	printf("添加成功!\n");
}

//线程函数 
//1为登陆成功 0为失败 2为私聊 3为好友请求 4为删除好友 5为查看当前在线人数 
void *my_recv(void *conn_fd)
{
	pthread_detach(pthread_self());
	int ret;
	char c;
	int i;
	char rev;
	char sed;
	UO *f , *f_temp,*f_head;
	US *s,*s_head;
	UT *t,*t_head;
	ONUSE *oun;
	char b[20];
	INFO *buff;
	oun = (ONUSE*)malloc(sizeof(ONUSE));
	buff = (INFO*)malloc(sizeof(INFO));
	f = (UO*)malloc(sizeof(UO));
	while(1)
	{
		if((ret = recv(*(int*)conn_fd , &rev , sizeof(char) , 0)) < 0)
			my_err("recv",__LINE__);
		if(rev == '1')
		{
			if((ret = recv(*(int*)conn_fd , f , sizeof(UO), 0)) < 0)
				my_err("recv",__LINE__);
			f_head = f_quchu();
			f_temp = f_head;
			while(f_temp != NULL)
			{
				if(strcmp(f_temp->name,f->name) == 0)
				{
					if(strcmp(f_temp->password , f->password) == 0)
					{
						sed = '1';
						if(send(*(int*)conn_fd,&sed,sizeof(char),0) < 0)
                			                my_err("send",__LINE__);
						my_send(*(int*)conn_fd , "匹配成功!");
						break;	
					}
					else
					{
						sed = '0';
						if(send(*(int*)conn_fd,&sed,sizeof(char),0) < 0)
               				                 my_err("send",__LINE__);
						my_send(*(int*)conn_fd , "密码错误!");
						close(*(int*)conn_fd);
						exit(0);
					}
				}
				f_temp = f_temp->next;
			}
			if(f_temp == NULL)
			{
				sed = '0';
				if(send(*(int*)conn_fd,&sed,sizeof(char),0) < 0)
                           	    	 my_err("send",__LINE__);
				my_send(*(int*)conn_fd , "没有该用户信息快去注册吧!");
				close(*(int*)conn_fd);
				exit(0);
			}
		}
		else if(rev == '2')
		{
			if((ret = recv(*(int*)conn_fd , f , sizeof(UO) , 0)) < 0)
				my_err("recv",__LINE__);
			user_add(f,*(int*)conn_fd);
			sed = '1';
			if(send(*(int*)conn_fd,&sed,sizeof(char),0) < 0)
				my_err("send",__LINE__);
			my_send(*(int*)conn_fd , "注册成功已登录!");
			f_temp = f;	
		}
		t_head = t_quchu();
		t = t_head;
		while(t != NULL)
		{
			if(strcmp(f_temp->name , t->name) == 0)
			{
				t->tid = *(int*)conn_fd;
				t->on = 1;
			}
			t = t->next;
		}
		t_add(t_head);
A:
		if((ret = recv(*(int*)conn_fd , &rev , sizeof(char) , 0)) < 0)
			my_err("recv",__LINE__);
		//rev 1是私聊，2是删好友，3是查看当前好友在线人员，4是加好友，5退出
		switch(rev)
		{
			case '1':
				{
					while(1)
					{	
						if((ret = recv(*(int*)conn_fd , buff , sizeof(INFO) , 0)) < 0)
							my_err("recv",__LINE__);
						s_head = s_quchu();
						s = s_head;
						while(s != NULL)
						{
							if(strcmp(f_temp->name,s->name) == 0)
							{
								for(i=0;i<s->fr_number;i++)
								{
									if(strcmp(buff->name,s->fr_name[i]) == 0)
									{
										t_head = t_quchu();
										t = t_head;
										while(t != NULL)
										{
											if(strcmp(t->name,s->fr_name[i]) == 0)
											{
												if(t->on == 0)
												{
													sed = '0';
													if(send(*(int*)conn_fd,&sed,sizeof(char),0) < 0)
														my_err("send",__LINE__);
													my_send(*(int*)conn_fd,"对方不在线!");
													goto A;
												}
												else
												{
													strcpy(buff->name,f_temp->name);
													buff->tid = t->tid;
													sed = '2';
													if(send(buff->tid,&sed,sizeof(char),0) < 0)
														my_err("send",__LINE__);
													if(send(buff->tid,buff,sizeof(INFO),0) < 0)
														my_err("send",__LINE__);
													goto A;
												}
											}
											t = t->next;
										}
									}
								}
								if(i >= s->fr_number)
								{
									sed = '0';
									if(send(*(int*)conn_fd,&sed,sizeof(char),0) < 0)
                   						             my_err("send",__LINE__);
									my_send(*(int*)conn_fd,"这个人不是你的好友!");
									goto A;
								}
							}
							s = s->next;
						}
					}	
				}
			case '2':
				{
					memset(b,0,sizeof(b));
					if(recv(*(int*)conn_fd,b,sizeof(b),0) < 0)
						my_err("recv",__LINE__);
					if(fr_del(f_temp->name , b) == 0)
					{
						sed = '0';
						if(send(*(int*)conn_fd,&sed,sizeof(char),0) < 0)
							my_err("send",__LINE__);
						my_send(*(int*)conn_fd,"删除失败!");
						goto A;
					}
					else
					{
						sed = '0';
						if(send(*(int*)conn_fd,&sed,sizeof(char),0) < 0)
							my_err("send",__LINE__);
						my_send(*(int*)conn_fd,"删除成功!\n");
						goto A;
					}
				}
			case '3':
				{
					memset(oun,0,sizeof(ONUSE));
					s_head = s_quchu();
					s = s_head;
					while(s != NULL)
					{
						if(strcmp(s->name,f_temp->name) == 0)
						{
							for(i=0;i<s->fr_number;i++)
							{
								t_head = t_quchu();
								t = t_head;
								while(t != NULL)
								{
									if(strcmp(s->fr_name[i],t->name) == 0)
									{
										if(t->on = 1)
										{
											strcpy(oun->on_user_name[oun->on_user],t->name);
											oun->on_user++;
											break;
										}
									}
									t = t->next;
								}
							}
						}
						s = s->next;
					}
					sed = '4';
					if(send(*(int*)conn_fd,&sed,sizeof(char) , 0) < 0)
						my_err("send",__LINE__);
					if(send(*(int*)conn_fd,oun,sizeof(ONUSE),0) < 0)
						my_err("send",__LINE__);
					goto A;
				}
			case '4':
				{
					memset(b,0,sizeof(b));
					if(recv(*(int*)conn_fd,b,sizeof(b),0) < 0)
						my_err("recv",__LINE__);
					t_head = t_quchu();
					t = t_head;
					while(t != NULL)
					{
						if(strcmp(t->name,b) == 0)
						{
							printf("%d\n",t->tid);
							if(t->on == 0)
							{
								sed = '0';
                                                                if(send(*(int*)conn_fd,&sed,sizeof(char),0) < 0)
                                                                my_err("send",__LINE__);
                                                                my_send(*(int*)conn_fd,"对方不在线");
								goto A;
							}
							else
							{
								sed = '3';
								if(send(t->tid,&sed,sizeof(char),0) < 0)
									my_err("send",__LINE__);
								if(send(t->tid,f_temp->name,sizeof(f_temp->name),0) < 0)
									my_err("send",__LINE__);
								if(recv(t->tid,&c,sizeof(char),0) < 0)
                                                                        my_err("recv",__LINE__);
								memset(&c,0,sizeof(char));
								if(recv(t->tid,&c,sizeof(char),0) < 0)
									my_err("recv",__LINE__);
								if(c == 'n')
								{
									sed = '0';
									if(send(*(int*)conn_fd,&sed , sizeof(char) , 0) < 0)
										my_err("send",__LINE__);
									my_send(*(int*)conn_fd,"对方拒绝添加好友请求!");
									goto A;
								}
								else
								{
									sed = '0';
									if(send(*(int*)conn_fd,&sed,sizeof(char),0) < 0)
										my_err("send",__LINE__);
									my_send(*(int*)conn_fd,"添加成功!\n");
									fr_add(f_temp->name,t->name);
									goto A;
								}
							}
						}
						t = t->next;
					}
					if(t == NULL)
					{
						sed = '0';
						if(send(*(int*)conn_fd,&sed,sizeof(char),0) < 0)
							my_err("send",__LINE__);
						my_send(*(int*)conn_fd,"没有找到要添加的这个人!");
						goto A;
					}
				}
			case '6':
				{
					memset(b,0,sizeof(b));
					if(recv(*(int*)conn_fd,&b,sizeof(b),0) < 0)
						my_err("recv",__LINE__);
					t_head = t_quchu();
					t = t_head;
					while(t != NULL)
					{
						if(strcmp(t->name,b) == 0)
						{	
							t->on = 0;
							break;
						}
						t = t->next;
					}
					close(*(int*)conn_fd);
					break;
				}
			default:exit(0);
		}
		break;
	}
}


int main(int argc , char *argv[])
{
	int sock_fd,ret,fd[100];
        pthread_t tid[100];
        int i = 0;
        int len;
        struct sockaddr_in seve,cli;
        if((sock_fd = socket(AF_INET , SOCK_STREAM , 0)) < 0)
                my_err("socket",__LINE__);
        int opt = 1;
        setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
        memset(&seve,0,sizeof(struct sockaddr_in));
        seve.sin_family = AF_INET;
        seve.sin_port = htons(PORT);
        seve.sin_addr.s_addr = htonl(INADDR_ANY);
        if(bind(sock_fd,(struct sockaddr*)&seve,sizeof(struct sockaddr_in)) < 0)
                my_err("bind",__LINE__);
        if(listen(sock_fd,LISTENQ) < 0)
                my_err("listen",__LINE__);

        while(1)
        {
                len = sizeof(struct sockaddr_in);
                if((fd[i] = accept(sock_fd,(struct sockaddr *)&cli,&len)) < 0)
                        my_err("accept",__LINE__);
                printf("accept %d , port %d\n",fd[i],seve.sin_port);
                if((ret = pthread_create(&tid[i],NULL,my_recv,(void*)&fd[i])) < 0)
                        my_err("pthread_creat",__LINE__);
                i++;
        }
        close(sock_fd);
        return 0;
}



