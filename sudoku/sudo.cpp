#define _CRT_SECURE_NO_WARNINGS
#include<cstdio>
#include<cstring>
#include<cstdlib>
#include<cstddef>
#include<random>
#include<unistd.h>
#include<ctime>
#include <locale.h>
#include<ncurses.h>
#include<algorithm>
using namespace std;
#include "sudo.h"

const int MAX = 362879;
const int MIN = 0;

void sudo::test()//测试
{
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
			printf("%c", table[i][j]);
		printf("\n");
	}	
	return;
}
/*
		Welcome to Sudoku  			17
Please enter s to start the game or q to exit game	50
	    author: 葫芦娃兄弟的混天绫			26
		  version:1.0.0				13
*/
void sudo::init()//初始化游戏的界面
{
	setlocale(LC_ALL,"");
	initscr();
	cbreak();
	noecho();
	int y,x;
	char ch;
	getmaxyx(stdscr,y,x);
	mvprintw(y/2-1,(x-17)/2,"Welcome to Sudoku");
	mvprintw(y/2,(x-50)/2,"Please enter s to start the game or q to exit game");
	mvprintw(y/2+1,(x-26)/2,"author: 葫芦娃兄弟的混天绫");
	mvprintw(y/2+2,(x-13)/2,"version:1.0.0");
	inittable(0);
	ch = getch();
	while(ch!='s'&& ch!='S')
	{
		if(ch=='q'||ch=='Q')
		{
			endwin();
			exit(0);
		}
		beep();
		ch = getch();
	}
	return;
}
void sudo::inittable(int cnt)//初始化9*9全排列
{
	static bool used[11] = { false };
	static char ttable[11];
	if (cnt == 9)
	{
		ttable[cnt] = '\0';
		strcpy(initable[numbers], ttable);
		numbers++;
		return;
	}
	for (int i = 1; i <= 9; i++)
	{
		if (!used[i])
		{
			used[i] = true;
			ttable[cnt] = '0' + i;
			inittable(cnt+1);
			ttable[cnt] = '\0';
			used[i] = false;
		}
	}
}
int sudo::random(int L, int R)//生成随机数
{
	static default_random_engine e(unsigned(time(0)));
	uniform_int_distribution<unsigned> u(L, R);
	return u(e);

}
void sudo::drawcell(int sx,int ex,int sy,int ey,int index)//绘制一个单元格
{
	for (int i = sx; i < ex; i++)
	{
		for (int j = sy; j < ey; j++)
		{
			table[i][j] = *(*(initable+ index)+(i - sx) * 3 + (j - sy));
		}
	}
	return;
}
void sudo::getcellr(int sx, int ex, int sy, int ey, int* hashr1, int* hashr2, int* hashr3)
{
	for (int i = sx; i < ex; i++)
		for (int j = sy; j < ey; j++)
		{
			if (i-sx == 0)	hashr1[table[i][j] - '0']++;
			else if (i-sx == 1)hashr2[table[i][j] - '0']++;
			else if (i-sx == 2)hashr3[table[i][j] - '0']++;
		}
}
void sudo::getcellc(int sx, int ex, int sy, int ey, int* hashc1, int* hashc2, int* hashc3)
{
	for (int i = sx; i < ex; i++)
		for (int j = sy; j < ey; j++)
		{
			if (j-sy == 0)	hashc1[table[i][j] - '0']++;
			else if (j-sy == 1)hashc2[table[i][j] - '0']++;
			else if (j-sy == 2)hashc3[table[i][j] - '0']++;
		}
}
int sudo::Nextcell(vector<int>& cellr, vector<int>& cellc)//根据目前的表格，查找下一个表格
{
	vector<int> query;//存放在query内
	int hashr1[11] = { 0 }, hashr2[11] = { 0 }, hashr3[11] = { 0 };
	int hashc1[11] = { 0 }, hashc2[11] = { 0 }, hashc3[11] = { 0 };
	for (size_t i = 0; i < cellr.size(); i++)
	{
		switch (cellr[i])
		{
		case 1:
			getcellr(0, 3, 0, 3, hashr1, hashr2, hashr3);
			break;
		case 2:
			getcellr(0, 3, 3, 6, hashr1, hashr2, hashr3);
			break;
		case 3:
			getcellr(0, 3, 6, 9, hashr1, hashr2, hashr3);
			break;
		case 4:
			getcellr(3, 6, 0, 3, hashr1, hashr2, hashr3);
			break;
		case 5:
			getcellr(3, 6, 3, 6, hashr1, hashr2, hashr3);
			break;
		case 6:
			getcellr(3, 6, 6, 9, hashr1, hashr2, hashr3);
			break;
		case 7:
			getcellr(6, 9, 0, 3, hashr1, hashr2, hashr3);
			break;
		case 8:
			getcellr(6, 9, 3, 6, hashr1, hashr2, hashr3);
			break;
		}
	}
	for (size_t i = 0; i < cellc.size(); i++)
	{
		switch (cellc[i])
		{
		case 1:
			getcellc(0, 3, 0, 3, hashc1, hashc2, hashc3);
			break;
		case 2:
			getcellc(0, 3, 3, 6, hashc1, hashc2, hashc3);
			break;
		case 3:
			getcellc(0, 3, 6, 9, hashc1, hashc2, hashc3);
			break;
		case 4:
			getcellc(3, 6, 0, 3, hashc1, hashc2, hashc3);
			break;
		case 5:
			getcellc(3, 6, 3, 6, hashc1, hashc2, hashc3);
			break;
		case 6:
			getcellc(3, 6, 6, 9, hashc1, hashc2, hashc3);
			break;
		case 7:
			getcellc(6, 9, 0, 3, hashc1, hashc2, hashc3);
			break;
		case 8:
			getcellc(6, 9, 3, 6, hashc1, hashc2, hashc3);
			break;
		}
	}
	for (int i = 0; i <= MAX; i++)
	{
		bool flag = true;
		for (int j = 0; j < 3; j++)
			if (hashr1[initable[i][j] - '0'])
				flag = false;
		for (int j = 3; j < 6; j++)
			if (hashr2[initable[i][j] - '0'])
				flag = false;
		for (int j = 6; j < 9; j++)
			if (hashr3[initable[i][j] - '0'])
				flag = false;
		for (int j = 0; j < 9; j += 3)
			if (hashc1[initable[i][j] - '0'])
				flag = false;
		for (int j = 1; j < 9; j += 3)
			if (hashc2[initable[i][j] - '0'])
				flag = false;
		for (int j = 2; j < 9; j += 3)
			if (hashc3[initable[i][j] - '0'])
				flag = false;
		if (flag)
			query.push_back(i);
	}
	if (query.size() == 0)
		return -1;
	return query[random(0, query.size()-1)];
}
void sudo::setcell()//设置单元格
{
	vector<int> cellr;//存放需要查询的行单元格
	vector<int> cellc;//存放需要查询的列单元格
	int curorder[11] = { 0,1,2,3,4,7,5,6,8,9 };//排放单元格的顺序
	int index = 0;
	int cur = 1;
	while (cur<=7)
	{
		switch (curorder[cur])
		{
		case 1://排放‘1’
			drawcell(0, 3, 0, 3, random(MIN, MAX));
			cur++;
			break;
		case 2:
			cellr.clear();
			cellc.clear();
			cellr.push_back(1);
			index = Nextcell(cellr, cellc);
			if (index == -1)
				cur--;
			else
			{
				drawcell(0, 3, 3, 6, index); cur++;
			}
			break;
		case 3:
			cellr.clear();
			cellc.clear();
			cellr.push_back(1);
			cellr.push_back(2);
			index = Nextcell(cellr, cellc);
			if (index == -1)
				cur--;
			else
			{
				drawcell(0, 3, 6, 9, index); cur++;
			}
			break;
		case 4:
			cellr.clear();
			cellc.clear();
			cellc.push_back(1);
			index = Nextcell(cellr, cellc);
			if (index == -1)
				cur--;
			else
			{
				drawcell(3, 6, 0, 3, index); cur++;
			}
			break;
		case 5:
			cellr.clear();
			cellc.clear();
			cellr.push_back(4);
			cellc.push_back(2);
			index = Nextcell(cellr, cellc);
			if (index == -1)
				cur--;
			else
			{
				drawcell(3, 6, 3, 6, index); cur++;
			}
			break;
		case 7:
			cellr.clear();
			cellc.clear();
			cellc.push_back(1);
			cellc.push_back(4);
			index = Nextcell(cellr, cellc);
			if (index == -1)
				cur--;
			else
			{
				drawcell(6, 9, 0, 3, index); cur++;
			}
			break;
		default:
			vector<int> query6;//存放单元格6的方案
			vector<int> query8;//存放单元格8的方案
			//查找满足‘6’位置的单元格
			int hashr1[11] = { 0 }, hashr2[11] = { 0 }, hashr3[11] = { 0 };
			int hashc1[11] = { 0 }, hashc2[11] = { 0 }, hashc3[11] = { 0 };
			getcellr(3, 6, 0, 3, hashr1, hashr2, hashr3);
			getcellr(3, 6, 3, 6, hashr1, hashr2, hashr3);
			getcellc(0, 3, 6, 9, hashc1, hashc2, hashc3);
			for (int i = 0; i <= MAX; i++)
			{
				bool flag = true;
				for (int j = 0; j < 3; j++)
					if (hashr1[initable[i][j] - '0'])
						flag = false;
				for (int j = 3; j < 6; j++)
					if (hashr2[initable[i][j] - '0'])
						flag = false;
				for (int j = 6; j < 9; j++)
					if (hashr3[initable[i][j] - '0'])
						flag = false;
				for (int j = 0; j < 9; j += 3)
					if (hashc1[initable[i][j] - '0'])
						flag = false;
				for (int j = 1; j < 9; j += 3)
					if (hashc2[initable[i][j] - '0'])
						flag = false;
				for (int j = 2; j < 9; j += 3)
					if (hashc3[initable[i][j] - '0'])
						flag = false;
				if (flag)
					query6.push_back(i);
			}
			if (query6.size() == 0)
			{
				cur--;
				break;
			}
			//查找满足‘6’位置的单元格
			for (int i = 0; i <= 10; i++)
				hashr1[i] = hashr2[i] = hashr3[i] = hashc1[i] = hashc2[i] = hashc3[i] = 0;
			getcellc(0, 3, 3, 6, hashc1, hashc2, hashc3);
			getcellc(3, 6, 3, 6, hashc1, hashc2, hashc3);
			getcellr(6, 9, 0, 3, hashr1, hashr2, hashr3);
			for (int i = 0; i <= MAX; i++)
			{
				bool flag = true;
				for (int j = 0; j < 3; j++)
					if (hashr1[initable[i][j] - '0'])
						flag = false;
				for (int j = 3; j < 6; j++)
					if (hashr2[initable[i][j] - '0'])
						flag = false;
				for (int j = 6; j < 9; j++)
					if (hashr3[initable[i][j] - '0'])
						flag = false;
				for (int j = 0; j < 9; j += 3)
					if (hashc1[initable[i][j] - '0'])
						flag = false;
				for (int j = 1; j < 9; j += 3)
					if (hashc2[initable[i][j] - '0'])
						flag = false;
				for (int j = 2; j < 9; j += 3)
					if (hashc3[initable[i][j] - '0'])
						flag = false;
				if (flag)
					query8.push_back(i);
			}
			if (query8.size() == 0)
			{
				cur--;
				break;
			}
			bool fdfs = false;
			for (size_t i = 0; i < query6.size(); i++)
			{
				drawcell(3, 6, 6, 9, query6[i]);
				for (size_t j = 0; j < query8.size(); j++)
				{
					drawcell(6, 9, 3, 6, query8[j]);
					if (fdfs = dfs(0))
					{
						cur++; break;
					}
				}
				if (fdfs)
					break;
			}
			if(!fdfs)
				cur=1;
			break;
		}
	}
	return;
}
bool sudo::judge(int x, int y, int k)
{
	for (int i = 0; i < 6; i++)
		if (table[i][y] - '0' == k)
			return false;
	for (int i = 0; i < 6; i++)
		if (table[x][i] - '0' == k)
			return false;
	return true;
}
bool sudo::dfs(int cnt)//判断是否存在答案
{
	static bool used[11] = { false };
	if (cnt == 9)
	{
		return true;
	}
	for (int i = 1; i <= 9; i++)
	{
		if(!used[i])
		{ 
			used[i] = true;
			if (judge(6 + cnt / 3, 6 + cnt % 3, i))
			{
				table[6 + cnt / 3][6 + cnt % 3] = '0'+i;
				if (dfs(cnt + 1))
				{
					for (int k = 0; k <= 10; k++)//为了下一次使用，这里置零
						used[k] = false;
					return true;
				}
			}
			used[i] = false;
		}
	}
	return false;
}

/*
                +---+---+---+---+---+---+---+---+---+       37c
                | 1 | 2 | 3 | 1 | 2 | 3 | 1 | 2 | 3 |
                +---+---+---+---+---+---+---+---+---+
                | 2 | 2 | 3 | 1 | 2 | 3 | 1 | 2 | 3 |
                +---+---+---+---+---+---+---+---+---+
                | 3 | 2 | 3 | 1 | 2 | 3 | 1 | 2 | 3 |
                +---+---+---+---+---+---+---+---+---+
                | 4 | 2 | 3 | 1 | 2 | 3 | 1 | 2 | 3 |
    20r         +---+---+---+---+---+---+---+---+---+
                | 5 | 2 | 3 | 1 | 2 | 3 | 1 | 2 | 3 |
                +---+---+---+---+---+---+---+---+---+
                | 6 | 2 | 3 | 1 | 2 | 3 | 1 | 2 | 3 |
                +---+---+---+---+---+---+---+---+---+
                | 7 | 2 | 3 | 1 | 2 | 3 | 1 | 2 | 3 |
                +---+---+---+---+---+---+---+---+---+
                | 8 | 2 | 3 | 1 | 2 | 3 | 1 | 2 | 3 |
                +---+---+---+---+---+---+---+---+---+
                |   |   |   |   |   |   |   |   |   |
                +---+---+---+---+---+---+---+---+---+            
    
   A(左)、D(右)、W(上)、S(下)、1~9(数字)、R(重置)、Q(退出)、M(答案)    61c
*/

void sudo::draw()//画游戏的图
{
	clear();
	int y,x;
	getmaxyx(stdscr,y,x);
	int cury = (y-21)/2,curx=(x-37)/2;
	mvprintw(cury,curx,"+---+---+---+---+---+---+---+---+---+");
	mvprintw(cury+1,curx,"|   |   |   |   |   |   |   |   |   |");
	mvprintw(cury+2,curx,"+---+---+---+---+---+---+---+---+---+");
	mvprintw(cury+3,curx,"|   |   |   |   |   |   |   |   |   |");
	mvprintw(cury+4,curx,"+---+---+---+---+---+---+---+---+---+");
	mvprintw(cury+5,curx,"|   |   |   |   |   |   |   |   |   |");
	mvprintw(cury+6,curx,"+---+---+---+---+---+---+---+---+---+");
	mvprintw(cury+7,curx,"|   |   |   |   |   |   |   |   |   |");
	mvprintw(cury+8,curx,"+---+---+---+---+---+---+---+---+---+");
	mvprintw(cury+9,curx,"|   |   |   |   |   |   |   |   |   |");
	mvprintw(cury+10,curx,"+---+---+---+---+---+---+---+---+---+");
	mvprintw(cury+11,curx,"|   |   |   |   |   |   |   |   |   |");
	mvprintw(cury+12,curx,"+---+---+---+---+---+---+---+---+---+");
	mvprintw(cury+13,curx,"|   |   |   |   |   |   |   |   |   |");
	mvprintw(cury+14,curx,"+---+---+---+---+---+---+---+---+---+");
	mvprintw(cury+15,curx,"|   |   |   |   |   |   |   |   |   |");
	mvprintw(cury+16,curx,"+---+---+---+---+---+---+---+---+---+");
	mvprintw(cury+17,curx,"|   |   |   |   |   |   |   |   |   |");
	mvprintw(cury+18,curx,"+---+---+---+---+---+---+---+---+---+");

	mvprintw(cury+20,(x-69)/2,"A(左)、D(右)、W(上)、S(下)、1~9(数字)、C(清除)、R(重置)、Q(退出)、M(答案)");
	refresh();	
//	double st = clock();
	setcell();
//	double en = clock();
//	printf("clock:%.4f\n",(en-st)/CLOCKS_PER_SEC);
	cury = cury+1;
	curx = curx+2;
	move(cury,curx);
	int initfill = random(30,40);
	remind = 81 - initfill;
	for(int i = 0; i < initfill;i++)
	{
		move(cury,curx);
		int getx=-1,gety=-1;
		static default_random_engine e(unsigned(time(0)));
		static uniform_int_distribution<unsigned> u(0, 8);
		getx = u(e);
		gety = u(e);
		move(cury+2*gety,curx+4*getx);
		while(inch()!=' ')
		{
			getx = u(e);
			gety = u(e);
			move(cury+2*gety,curx+4*getx);
		}
		curtable[gety][getx]=table[gety][getx];
		addch(table[gety][getx]);
	}

	move(cury,curx);
	refresh();
	return;
}
bool sudo::judgeans(const int& my, const int& mx, char ch)
{
	char ccur = inch();
	if(ccur!=' ')
		return false;
	int y, x;
	getyx(stdscr,y, x);
	int cury = y, curx = mx;
	for (int i = 0; i < 9; i++)
	{
		move(cury, curx);
		char cur = inch();
		if (cur == ch)
			return false;
		curx += 4;
	}
	cury = my, curx = x;
	for (int i = 0; i < 9; i++)
	{
		move(cury, curx);
		char cur = inch();
		if (cur == ch)
			return false;
		cury += 2;
	}
	cury = (y - my) / 2;
	curx = (x - mx) / 4;
	cury = my+cury / 3 * 3 * 2;
	curx = mx + curx / 3 * 3*4;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			move(cury + 2 * i, curx + 4 * j);
			char cur = inch();
			if (cur == ch)
				return false;
		}
	}
	return true;
}
bool sudo::judgemove(const int& my, const int& mx, const int& cury, const int& curx)//判断当前位置是否越界
{
	int y = (cury - my) / 2;
	int x = (curx - mx) / 4;
	if (x >= 0 && x <= 8 && y >= 0 && y <= 8)
		return true;
	return false;
}
void sudo::run()//执行
{
Resetting:

	draw();
	int cy,cx;
	getyx(stdscr,cy,cx);
	int cury=cy,curx = cx;
	char ch;
	while(remind)
	{
		ch = getch();
		if(ch=='W'||ch=='w')
		{
			if(judgemove(cy,cx,cury-2,curx))
			{
				cury -=2;
				move(cury,curx);	
			}
			else
			{
				beep();
			}
			continue;
		}
		else if(ch=='a'||ch=='A')
		{
			if(judgemove(cy,cx,cury,curx-4))
			{
				curx-=4;
				move(cury,curx);
			}
			else
			{ 
				beep();
			}
			continue;
		}
		else if(ch=='s'||ch=='S')
		{
			if(judgemove(cy,cx,cury+2,curx))
			{
				cury +=2;
				move(cury,curx);
			}
			else
			{
				beep();
			}
			continue;
		}
		else if(ch=='d'||ch=='D')
		{
			if(judgemove(cy,cx,cury,curx+4))
			{
				curx +=4;
				move(cury,curx);
			}
			else
			{
				beep();
			}
			continue;
		}
		else if(ch>='1'&&ch<='9')
		{
			if(judgeans(cy,cx,ch))
			{
				move(cury,curx);
				addch(ch);
				remind--;
			}
			else
			{
				beep();
			}
			move(cury,curx);
			refresh();
			continue;
		}
		else if(ch=='c'||ch=='C')
		{
			char cur = inch();
			if(cur==' ')
			{
				beep();
			}
			else
			{
				if(curtable[(cury-cy)/2][(curx-cx)/4]==' ')
				{
					addch(' ');
					remind++;
					move(cury,curx);
				}
				else
				{
					beep();
				}
				//在curtable查找
			}
			continue;
		}
		else if(ch=='r'||ch=='R')
		{
			for(int ini = 0;ini<=10;ini++)
				for(int inj = 0; inj <= 10; inj++)
					table[ini][inj]=curtable[ini][inj] = ' ';
			goto Resetting;
		}
		else if(ch=='q'||ch=='Q')
		{
			endwin();
			exit(0);
		}
		else if(ch=='m'||ch=='M')
		{
			//给出table答案
			cury = cy;
			curx = cx;
			for(int iny = 0;iny<9;iny++)
			{
				curx = cx;
				for(int inx = 0; inx < 9; inx++)
				{
					move(cury,curx);
					addch(table[(cury-cy)/2][(curx-cx)/4]);
					curx += 4;
				}
				cury += 2;
			}
			//打印youlost
			printlost();
			//给出提示输入
			while(1)
			{
				char ch = getch();
				if(ch=='r'||ch=='R')
				{
					goto Resetting;
				}
				if(ch=='q'||ch=='Q')
				{
					endwin();
					exit(0);
				}
				else
					beep();
			}
		}
		else
		{
			beep();
		}
	}
	if(remind==0)
	{
		printwin();	
		while(1)
		{
			char ch = getch();
			if(ch=='r'||ch=='R')
			{
				goto Resetting;
			}
			if(ch=='q'||ch=='Q')
			{
				endwin();
				exit(0);
			}
			else
				beep();
		}
	}
	beep();
	return;
}
void sudo::printwin()//打印youwin
{
	int y,x;
	getmaxyx(stdscr,y,x);
	int cury = (y-21)/2,curx=(x-37)/2;
	mvprintw(cury+22,(x-60)/2,"+   +    ++    +  +       +           +   +    + +         +");	
	mvprintw(cury+23,(x-60)/2," + +    +  +   +  +       +     +     +        +   +       +");	
	mvprintw(cury+24,(x-60)/2,"  +     +  +   +  +        +   + +   +    +    +   +       +");	
	mvprintw(cury+25,(x-60)/2,"  +     +  +   +  +         + +   + +     +    +   +        ");	
	mvprintw(cury+26,(x-60)/2,"  +      ++     ++           +     +      +    +   +       +");	
	mvprintw(cury+28,(x-30)/2,"按Q退出，按R重新开始，请输入：");	

	return;
}
void sudo::printlost()//打印youlost
{
	
	int y,x;
	getmaxyx(stdscr,y,x);
	int cury = (y-21)/2,curx=(x-37)/2;
	mvprintw(cury+22,(x-60)/2,"+   +    ++    +  +       +        ++     ++    + + +      +");	
	mvprintw(cury+23,(x-60)/2," + +    +  +   +  +       +       +  +   +        +        +");	
	mvprintw(cury+24,(x-60)/2,"  +     +  +   +  +       +       +  +    ++      +        +");	
	mvprintw(cury+25,(x-60)/2,"  +     +  +   +  +       +       +  +      +     +         ");	
	mvprintw(cury+26,(x-60)/2,"  +      ++     ++        + + +    ++     ++      +        +");	
	mvprintw(cury+28,(x-30)/2,"按Q退出，按R重新开始，请输入：");	
	return;
}
sudo::sudo()
{
	initable = new char*[362881];
	for (int i = 0; i <= 362880; i++)
		initable[i] = new char[10];
	table = new char* [11];
	for (int i = 0; i <= 10; i++)
		table[i] = new char[11];
	curtable = new char* [11];
	for (int i = 0; i <= 10; i++)
		curtable[i] = new char[11];
	for(int i = 0; i <= 10; i++)
		for(int j = 0; j <= 10; j++)
			table[i][j]=curtable[i][j]=' ';
	numbers = 0;
	remind = 81;
}
sudo::~sudo()
{
	for (int i = 0; i <= 362880; i++)
		delete[] initable[i];
	delete[] initable;
	for (int i = 0; i <= 10; i++)
		delete[] table[i];
	delete[] table;
	for (int i = 0; i <= 10; i++)
		delete[] curtable[i];
	delete[] curtable;
	initable = NULL;
	remind = 0;
}
