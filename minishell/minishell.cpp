#include<cstdio>
#include<cstring>
#include<unistd.h>
#include<cstdlib>
#include<sys/wait.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#include<signal.h>
using namespace std;

//宏定义
#define CMDLEN 1024  //命令的长度
#define FILELEN 128  //文件名的长度
#define APP 1	//追加输出重定向
#define TRUNC 0	//截断输出重定向
#define STDIN_FD 1000 //标准输入描述符#
#define STDOUT_FD 1001 //标准输出描述符
#define STDERR_FD 1002 //标准错误描述符
#define DIRLEN 1024 //目录长度

//数据
char inputcom[CMDLEN];//终端输入的命令

struct idxtem //命令的位置
{
	char *argv[CMDLEN];//拆分简单命令存放的位置
	int in_file;//输入重定向文件描述符
	int out_file;//输出重定向文件描述符
	int APPorTRUNC;//判断输出重定向是追加还是截断，默认截断
};
struct idxlist//命令表
{
	char Out_blank_com[CMDLEN];//规格化命令存储
	idxtem tem[CMDLEN];//简单命令存储
	int last;//最后一个命令的下标，同时代表简单命令的个数
}list;

int background_jobs;//后台工作
struct sigaction act;//注册信号

//测试函数
void test()
{
	for(int i = 0; i <= list.last;i++)
	{
		int k = 0;
		while(list.tem[i].argv[k]!=NULL)
			printf("[%s] ",list.tem[i].argv[k++]);
		printf("\n");
		printf("in_file:%d\n",list.tem[i].in_file);
		printf("out_file:%d\n",list.tem[i].out_file);
		printf("输出重定向类别:%d\n",list.tem[i].APPorTRUNC);
	}
}

//函数
void sys_err(const char * err);//错误处理
void init();//初始化

void Read_command();//读取命令
void Parse_command();//解析命令
void executive_command();//执行命令

void Remove_blank();//分离空白字符
void Isexit();//判断是否退出程序
bool check();//处理特殊字符

void Break_up_pipe();//分离管道
void simple_command();//处理简单命令
void Split_input_redirection();//分离输入重定向
void Split_output_redirection();//分离输出重定向
void Split_background_jobs();//分离后台执行

void setsig();//设置信号

int main(int argc, char *argv[])
{
	if(dup2(STDIN_FILENO,STDIN_FD)==-1)//备份标注输入输出错误文件描述符
		sys_err("dup2 STDIN_FD");
	if(dup2(STDOUT_FILENO,STDOUT_FD)==-1)
		sys_err("dup2 STDOUT_FD");
	if(dup2(STDERR_FILENO,STDERR_FD)==-1)
		sys_err("dup2 STDERR_FD");
	setsig();
	while(1)
	{
		//		printf("456\n");
		init();//初始化
		Read_command();//读取命令
		Parse_command();//解析命令
//		test();
		executive_command();//执行命令
	}
	return 0;
}

void sys_err(const char * err)//错误处理
{
	perror(err);//打印errno值
	exit(1);
}

void setsig()//设置信号
{
	//	act.sa_handler = sig_chld;
	//	sigemptyset(&act.sa_mask);
	//	sigaddset(&act.sa_mask,SIGCHLD);
	//	sigaction(SIGCHLD,&act,NULL);
	act.sa_handler = SIG_IGN;//默认忽略
	if(sigemptyset(&act.sa_mask)==-1)//清空act
		sys_err("sigemptyset error");
	if(sigaddset(&act.sa_mask,SIGINT)==-1)//将SIGINT加入act中
		sys_err("sigaddset error");
	if(sigaddset(&act.sa_mask,SIGQUIT)==-1)
		sys_err("sigaddset error");
	if(sigaddset(&act.sa_mask,SIGTSTP)==-1)
		sys_err("sigaddset error");
	if(sigaction(SIGQUIT,&act,NULL)==-1)//注册SIGINT
		sys_err("sigaction error");
	if(sigaction(SIGINT,&act,NULL)==-1)
		sys_err("sigaction error");
	if(sigaction(SIGTSTP,&act,NULL)==-1)
		sys_err("sigaction error");
}


void init()//初始化
{	
	for(int i = 0; i < STDIN_FD;i++)//关闭除备份文件描述符之外的所谓文件描述符
		close(i);
	if(dup2(STDIN_FD,STDIN_FILENO)==-1)//重置标准输入
		sys_err("dup2 STDIN_FD error");
	if(dup2(STDOUT_FD,STDOUT_FILENO)==-1)//重置标准输出
		sys_err("dup2 STDOUT_FD error");
	if(dup2(STDERR_FD,STDERR_FILENO)==-1)//重置标准错误
		sys_err("dup2 STDERR_FD error");
	//	printf("789\n");
	memset(&list,0,sizeof(list));//初始化list，
	for(int i = 0; i < CMDLEN;i++)
	{
		list.tem[i].in_file = list.tem[i].out_file = -1;//代表当前没有指定输入输出
		list.tem[i].APPorTRUNC = TRUNC;//默认截断方式输出重定向
	}
	list.last = -1;//代表当前没有命令
	memset(&inputcom,0,sizeof(inputcom));
	background_jobs = 0;//默认没有后台运行
	return ;
}
void Read_command()//读取命令
{
	do
	{
		if(getcwd(inputcom,sizeof(inputcom))==NULL)//获取当前目录
			sys_err("getcwd error");
		if(strstr(inputcom,"/root")!=NULL)//如果当前目录是在root之下，终端用户显示为~
			printf("GQ@葫芦娃兄弟的混天绫:~%s# ",inputcom+5);
		else
			printf("GQ@葫芦娃兄弟的混天绫:%s# ",inputcom);//如果不在，则原样显示
		if(fflush(stdin)==EOF)//刷新流
			sys_err("fflush error");
		while(fgets(inputcom,sizeof(inputcom),stdin)==NULL)//读取命令
		{
		//		printf("errno:%d\n",errno);
			if(errno==EBADF)//如果Ctrl+D，发送一个EOF
				exit(0);
			else if(errno==EINTR)//如果信号中断，则继续读取
				continue;
			else
				sys_err("fgets error");
		}
		//	printf("ONE:   errno:%d\n",errno);
		Remove_blank();//移除空白字符，规范化命令
		Isexit();//判断是否为exit命令
	}while(check());//check判断是是否为特殊命令
	return ;
}

void Remove_blank()//分离空白字符
{
	int len = strlen(inputcom);
	int index = 0,i = 0;
	int flag = 0;
	char *ptr = inputcom;
	//删除空白字符
	while(*ptr==' '||*ptr=='\t')//去除前置空白字符
	{
		ptr++,i++;
	}
	for(; i <= len;i++)
	{
		if(inputcom[i]==' '||inputcom[i]=='\t')//如果有空白字符将flag标记为真
		{
			flag = 1;
			continue;
		}
		if(flag)//如果flag为真，代表当前非空白字符之前有空白，无论前边有多个还是一个，这里都变成一个
		{
			inputcom[index++] = ' ';//加一个空白字符
			flag = 0;
		}
		inputcom[index++] = inputcom[i];
	}
	len = strlen(inputcom);
	for(i = len-1;i>=0;i--)
	{
		if(inputcom[i]==' '||inputcom[i]=='\t'||inputcom[i]=='\n'||inputcom[i]=='\0')//去除后置空白字符
			inputcom[i]='\0';
		else
			break;
	}
	//	printf("%s\n",inputcom);
	index = 0;
	len = strlen(inputcom);
	//添加空白字符
	for(i = 0; i <= len; i++)
	{
		if(inputcom[i]=='>'&&inputcom[i+1]=='>')//如果是>>符号，将其前后都加一个‘ ’空格
		{
			if(list.Out_blank_com[index-1]!=' ')//如果前边没有一个‘ ’空格，加一个‘ ’空格，如果有就不加了
				list.Out_blank_com[index++] = ' ';
			list.Out_blank_com[index++] = inputcom[i];//复制有效字符
			list.Out_blank_com[index++] = inputcom[i+1];
			i++;
			if(inputcom[i+1]!=' ')//如果后边没有一个‘ ’空格，加一个‘ ’空格，如果有就不加了
				list.Out_blank_com[index++] = ' ';
		}
		else if(inputcom[i]=='<'||inputcom[i]=='|'||inputcom[i]=='>')//如果是>,|,<符号，将其前后都加一个‘ ’空格
		{
			if(list.Out_blank_com[index-1]!=' ')
				list.Out_blank_com[index++] = ' ';
			list.Out_blank_com[index++] = inputcom[i];
			if(inputcom[i+1]!=' ')
				list.Out_blank_com[index++] = ' ';
		}
		else if(inputcom[i]=='&')//如果是&符号，将其前边加一个‘ ’空格
		{
			if(list.Out_blank_com[index-1]!=' ')
				list.Out_blank_com[index++] = ' ';
			list.Out_blank_com[index++] = inputcom[i];
		}
		else
			list.Out_blank_com[index++] = inputcom[i];//其他字符都复制下来
	}
	//	printf("%s",list.Out_blank_com);
	return ;
}

void Isexit()//判断是否退出程序
{
	if(strcmp(list.Out_blank_com,"exit")==0)//如果是exit，退出
	{
		printf("logout\n");
		exit(0);
	}
}

bool check()////处理特殊命令
{
	int len = strlen(inputcom);
	if(len==0)//如果没有输入或者输入为空白字符
		return true;
	else if(inputcom[0]=='c'&&inputcom[1]=='d')//如果是cd命令
	{
		if(chdir(inputcom+3)==-1)//项目目录的方式更改目录
			perror("chdir error");
		return true;
	}
	return false;
}

void Parse_command()//解析命令
{
	//后台运行
	Split_background_jobs();
	//先分离管道
	Break_up_pipe();
	//输入重定向
	Split_input_redirection();
	//输出重定向
	Split_output_redirection();
	//处理单个命令
	simple_command();

	return ;
}


void executive_command()//执行命令
{
	if(list.tem[0].in_file==-1)//如果没有输入重定向，则第一个命令的输入是标准输入
		list.tem[0].in_file = STDIN_FD;
	if(list.tem[list.last].out_file ==-1)//如果没有输出重定向，则最后一个命令的输出是标准输出
		list.tem[list.last].out_file = STDOUT_FD;
	int fd[2];
	//初始标准读入与标准输出
	for(int i = 0; i < list.last;i++)
	{
		if(pipe(fd)==-1)//注册管道
			sys_err("pipe error");
		list.tem[i+1].in_file = fd[0];//管道的读端给下一个命令的输入端
		list.tem[i].out_file = fd[1];//管道的写端给当前命令的输出端
		//管道起连接两个命令之间的数据的传输
		if(dup2(list.tem[i].out_file,STDOUT_FILENO)==-1)//输出到下一个进程的入口管道中
			sys_err("dup2 error");
		if(dup2(list.tem[i].in_file,STDIN_FILENO)==-1)//从上一个进程的出口管道中读入
			sys_err("dup2 error");

		//	printf("i = %d in_file:%d out_file:%d\n",i,list.tem[i].in_file,list.tem[i].out_file);
		pid_t pid = fork();//创建进程
		if(pid==-1)
			sys_err("fork error");
		else if(pid==0)//子进程
		{
			if(execvp(list.tem[i].argv[0],list.tem[i].argv)==-1)//调用execvp函数执行命令
			{
				dprintf(STDOUT_FD,"%s: command not found\n",list.tem[list.last].argv[0]);
				exit(1);
			}
		}
		else//父进程
		{
	//		dprintf(STDOUT_FD,"pid:%d in_file:%d out.file:%d\n", getpid(),list.tem[i].in_file,list.tem[i].out_file);
			if(list.tem[i].in_file!=STDIN_FD && list.tem[i].in_file!=STDOUT_FD && list.tem[i].in_file!=STDERR_FD)
				close(list.tem[i].in_file);//如果不是标准输入输出错误的备份文件描述符就关闭
			if(list.tem[i].out_file!=STDIN_FD && list.tem[i].out_file!=STDOUT_FD && list.tem[i].out_file!=STDERR_FD)
				close(list.tem[i].out_file);//如果不是标准输入输出错误的备份文件描述符就关闭
			//char c = -1;
			//write(list.tem[i].out_file,(void *)&c,1);
			if(wait(NULL)==-1)//阻塞等待回收
				sys_err("wait error");
		}
	}
	if(dup2(list.tem[list.last].out_file,STDOUT_FILENO)==-1)//输出到下一个进程的入口管道中
		sys_err("dup2 error");
	if(dup2(list.tem[list.last].in_file,STDIN_FILENO)==-1)//从上一个进程的出口管道中读入
		sys_err("dup2 error");

	
//	dprintf(STDOUT_FD,"pid:%d in_file:%d out.file:%d\n", getpid(),list.tem[list.last].in_file,list.tem[list.last].out_file);
	pid_t pid = fork();

	if(pid==-1)
		sys_err("fork error");
	else if(pid==0)
	{
		if(background_jobs==1&&setpgid(getpid(),getpid())==-1)//如果是后台运行，设置进程组
			sys_err("setpgid error");
		if(execvp(list.tem[list.last].argv[0],list.tem[list.last].argv)==-1)
		{
			dprintf(STDOUT_FD,"%s: command not found\n",list.tem[list.last].argv[0]);
			exit(1);
		}
	}
	else
	{
		if(list.tem[list.last].in_file!=STDIN_FD && list.tem[list.last].in_file!=STDOUT_FD && list.tem[list.last].in_file!=STDERR_FD)
			close(list.tem[list.last].in_file);
		if(list.tem[list.last].out_file!=STDIN_FD && list.tem[list.last].out_file!=STDOUT_FD && list.tem[list.last].out_file!=STDERR_FD)
			close(list.tem[list.last].out_file);
		//父进程
		if(background_jobs==0&&wait(NULL)==-1)//如果不是后台运行，阻塞回收
			sys_err("wait error");
	}
//		printf("123\n");
	return ;
}

void Break_up_pipe()//分离管道
{
	int len = strlen(list.Out_blank_com);
	list.tem[++list.last].argv[0] = list.Out_blank_com;//第一条简单命令的起点
	for(int i = 0; i < len;i++)
	{
		if(list.Out_blank_com[i]=='|')//如果管道存在
		{
			list.Out_blank_com[i-1]='\0';//本条简单命令结束
			list.tem[++list.last].argv[0] = list.Out_blank_com+i+2;//吓一跳简单命令的开始
		}
	}
	//	for(int i = 0; i <= list.last; i++)
	//		printf("\n%s", list.tem[i].argv[0]);
	//	printf("\n");
	return ;
}

void simple_command()//处理简单命令
{
	for(int i = 0; i <= list.last; i++)
	{
		int len = strlen(list.tem[i].argv[0]);//第i个简单命令
		int cnt = 0;
		for(int j = 1;j<len;j++)//将简单命令分解
		{
			if(list.tem[i].argv[0][j-1]==' '&&list.tem[i].argv[0][j]!=' ')//如果前边是‘ ’空格，后边不是空格，则后边是命令参数的起点
				list.tem[i].argv[++cnt] = list.tem[i].argv[0]+j;//修改参数的指针
		}
		for(int j = 0; j <len; j++)
			if(list.tem[i].argv[0][j]==' ')//将所有‘ ’空白字符都变成‘\0’，代表每个命令及参数的结束标志
				list.tem[i].argv[0][j] = '\0';
	}
	return ;
}

void Split_input_redirection()//分离输入重定向
{
	char input_file[FILELEN]={0};
	for(int i = 0; i <= list.last;i++)
	{
		int len = strlen(list.tem[i].argv[0]);//在第i条简单命令里查找
		for(int j = 0; j < len;j++)
		{
			if(list.tem[i].argv[0][j]=='<')//如果存在输入重定向
			{
				list.tem[i].argv[0][j] = ' ';//将<这个位置变成‘ ’空格
				for(int k = j+2; ;k++)//从<之后的第二个位置开始，因为第一个位置是‘ ’空格
				{
					if(list.tem[i].argv[0][k]==' '||list.tem[i].argv[0][k]=='\0')//如果碰到‘ ’或者‘\0’则表示文件名复制完成
					{
						input_file[k-j-2]='\0';//文件名之后加一个‘\0’，表示文件名到此结束
						break;
					}
					input_file[k-j-2] = list.tem[i].argv[0][k];//按字节复制文件名
					list.tem[i].argv[0][k] = ' ';//将原位置变成‘ ’空格
				}
				list.tem[i].in_file = open(input_file,O_RDONLY);//打开输入重定向文件
				if(list.tem[i].in_file==-1)//如果失败
					sys_err("open error");
				break;
			}
		}
	}
	return ;
}

void Split_output_redirection()//分离输出重定向
{
	char output_file[FILELEN];
	for(int i = 0; i <= list.last; i++)
	{
		int len = strlen(list.tem[i].argv[0]);//在第i条简单命令里查找
		for(int j = 0; j < len; j++)
		{
			if(list.tem[i].argv[0][j]=='>')//如果存在输入重定向
			{
				list.tem[i].argv[0][j] = ' ';//将>这个位置变成‘ ’空格
				if(list.tem[i].argv[0][j+1]=='>')//判断是否为追加重定向
				{
					j++;
					list.tem[i].argv[0][j] = ' ';//将>这个位置变成‘ ’空格
					list.tem[i].APPorTRUNC = APP;//输出重定向文件打开方式标记为追加
				}
				for(int k = j+2; ;k++)
				{
					if(list.tem[i].argv[0][k]==' '||list.tem[i].argv[0][k]=='\0')
					{
						output_file[k-j-2] = '\0';
						break;
					}
					output_file[k-j-2] = list.tem[i].argv[0][k];
					list.tem[i].argv[0][k] = ' ';
				}
				if(list.tem[i].APPorTRUNC==TRUNC)//如果输出重定向文件打开方式为截断
				{
					list.tem[i].out_file = open(output_file,O_WRONLY|O_CREAT|O_TRUNC,0664);//以截断的方式打开
				}
				else
				{
					list.tem[i].out_file =open(output_file,O_WRONLY|O_CREAT|O_APPEND,0664);//以追加的方式打开
				}
				if(list.tem[i].out_file ==-1)//如果打开失败
					sys_err("open out_file error");
				break;
			}
		}
	}
	return ;
}

void Split_background_jobs()//分离后台执行
{
	int len = strlen(list.Out_blank_com);
	for(int i = len-1; i >= 0; i--)//从后开始遍历，找&符号，
	{
		if(list.Out_blank_com[i]=='&')//如果找到
		{
			background_jobs = 1;//后台运行标记为真
			list.Out_blank_com[i-1]='\0';//将&前置为‘\0’，代表&前边的命令的结束位置
			break;
		}
	}
	return ;
}

