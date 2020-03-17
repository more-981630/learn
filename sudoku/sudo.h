#pragma once
#include<vector>
using namespace std;
class sudo
{
	public:
	void init();//初始化游戏的界面
	void inittable(int cnt);//初始化9*9全排列
	int random(int L, int R);//生成随机数
	void getcellr(int sx, int ex, int sy, int ey, int* hashr1, int* hashr2, int* hashr3);//获取行信息
	void getcellc(int sx, int ex, int sy, int ey, int* hashc1, int* hashc2, int* hashc3);//获取列信息

	int Nextcell(vector<int>& cellr, vector<int>& cellc);//根据目前的表格，查找下一个表格
	void drawcell(int sx, int ex, int sy, int ey, int index);//绘制一个单元格
	void setcell();//设置所有的单元格
	bool judge(int x, int y, int k);//判断(x,y)位置放k是否可行
	bool dfs(int cnt);//判断是否存在答案
	void draw();//画游戏的图
	bool judgeans(const int& my, const int& mx, char ch);//判断当前位置能否放ch
	bool judgemove(const int& my, const int& mx, const int& cury, const int& curx);//判断当前位置是否越界
	void run();//执行
	void test();//测试
	void printwin();//打印youwin
	void printlost();//打印youlost
public:
	sudo();
	virtual ~sudo();

private:
	char **initable;
	int remind;
	int numbers;
	char** table;
	char** curtable;
};


