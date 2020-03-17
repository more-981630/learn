## 前言
&emsp;&emsp;在一位大佬的博客看到的这个项目，感觉大佬写的这个项目功能较少，有可完善之处，本着完善的理念写了这个项目。
## 项目介绍
&emsp;&emsp;minishell是一个命令处理器，以程序的方式运行在终端中，接受终端输入的命令并进行解析执行结果。
### 功能介绍
&emsp;&emsp;实现的功能包括命令的读入、解析与执行。minishell可接受的命令是有一定的格式，可以接受带有管道、输入重定向、输出重定向、后台运行等组合外部命令。
&emsp;&emsp;**格式：简单命令 [\< 文件名] [ | 简单命令] ... [ >/>> 文件名] [&]** 
&emsp;&emsp;其中[]表示可选项，/表示前后仅可选择一个。
&emsp;&emsp;除了外部命令外，还实现了内部命令cd，cd命令的**格式：cd 相对路径**
&emsp;&emsp;查看一个命令是否为内部命令可以在bash终端使用type 命令
### 模块分析
#### 注册信号
&emsp;&emsp;使用setsig注册SIGINT、SIGQUIT、SIGTSTP三个信号为SIG_IGN
#### 初始化
&emsp;&emsp;初始化数据。将打开的文件描述符关闭。重置标准输入、标准输出、标准错误文件描述符。
#### 读取命令
&emsp;&emsp;从标准输入读取命令，使用Remove_blank函数处理空白字符进行规格化命令。然后使用Isexit判断是否为exit，使用check判断命令是否为cd命令等内部命令。exit和cd是minishell实现内部命令。
#### 解析命令
&emsp;&emsp;分五步进行解析终端输入的命令。
1. 使用Split_background_jobs解析后台命令。
2. 使用Break_up_pipe解析管道命令，将命令分解成简单命令。
3. 使用Split_input_redirection解析输入重定向命令，去除简单命令的输入重定向。
4. 使用Split_output_redirection解析输出重定向命令，去除简单命令的输出重定向。
5. 使用simple_command解析剩余简单命令。
#### 执行命令
&emsp;&emsp;首先为第一个简单命令指定标准输入描述符与和最后一个命令指定标准输出描述符，如果有多个简单命令，那么就使用pipe创建管道，用于连接简单命令的标准输出与标准输入的数据的对接。然后调用fork创建进程调用execvp执行简单命令。对于后台运行的命令，改变其进程组，使后台运行程序的到后台运行，父进程也不再回收。
### 遇到的问题
&emsp;&emsp;管道读取失败：当管道写端不关闭，读端读完数据将阻塞等待，查阅资料得知：
1. 读管道
		a. 有数据：读取端返回读取的字节数
		b. 无数据：如果写端全关闭：读取端读取到EOF。否则：阻塞等待。
2. 写管道
		a. 读端全关闭：进程异常终止，系统发送SIGPIPE信号。
		b. 有读端打开：管道未满：写数据，写端返回写进的字符个数，否则：阻塞等待(少见)


&emsp;&emsp;0、1、2文件描述符：程序需要不断地从标准输入读取命令，由于含有管道的命令也需要从文件描述符0读入，不得不让出0号文件描述符，让出的0号文件描述符怎么才能重新指向标准输入呢？这里采用将0、1、2号文件描述符复制到1000、1001、1002号文件描述符，保留文件指针。
&emsp;&emsp;cd命令的实现：cd命令是内部命令，调用execvp函数无法实现相应的功能，这里调用chdir系统函数，以相对路径的方式进行修改目录。
&emsp;&emsp;后台运行：让程序在后台运行，让出当前终端，这里采用修改进程组，父进程也不再回收。
### 功能扩展
&emsp;&emsp;虽然扩展了一些命令，但与Linux标准的bash差的还有十万八千个筋斗云的十万八千里，但为了程序的完善，这里指出程序的不足和待完善之处。
&emsp;&emsp;目前仅支持外部命令和个别内部命令，可以扩展更多内部命令。
&emsp;&emsp;信号事件待改进，可以让minishell终端忽略SIGINT、SIGQUIT、SIGTSTP三个信号，让其子进程执行相应的事件。
&emsp;&emsp;命令不能自动补全，可以采用读取单字符的方式获取已经输入的字符，根据已经输入的字符到系统中寻找对应的命令。