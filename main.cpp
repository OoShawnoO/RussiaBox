#include <cstdio>
#include <cstdlib>
#include <dos.h>
#include <graphics.h>
#include <conio.h>
#include <iostream>
#include <windows.h>


/* 按键码: */
/*
i=87,j=87	W
i=83,j=83	S
i=65,j=65	A
i=68,j=68	D
*/
/*
i=87,j=119	W-87 w-119
i=83,j=115	S-83 s-115
i=65,j=97	A-65 a-97
i=68,j=100	D-68 d-100
i = 27, j =27		ESC
*/
#define W 87
#define S 83
#define A 65
#define D 68
#define ESC 27



/* 定义常量 */
#define MAX_BOX 19      //19种方块
#define BSIZE 20        //游戏方块边长20个像素
#define SYS_X 160       //左上角方块界面x坐标
#define SYS_Y 25        //左上角方块界面y坐标
#define Horizontal_boxs 10      //横向界面相对方块大小 （10个方块）
#define Vertical_boxs 15        //纵向界面向对方块大小  (15个方块)
#define Begin_boxs_x Horizontal_boxs/2 //第一个方块起始位置
#define FgColor WHITE     //前景颜色
#define BgColor BLACK      //背景颜色
#define LeftWin_x SYS_X+Horizontal_boxs*BSIZE+46	//右边状态栏的x坐标
#define false 0
#define true 1
#define MoveLeft 1
#define MoveRight 2
#define MoveDown 3
#define MoveRoll 4

/*全局变量定义*/

int current_box_numb;       //当前方块编号
int Curbox_x = SYS_X + Begin_boxs_x * BSIZE, Curbox_y = SYS_Y;        //x,y当前坐标
int flag_newbox = false;
int speed = 1000;
int score = 0;

/*
1.预处理
	1.调用<windows.h><conio.h><stdio.h><stdlib.h><graphics.h><dos.h><iostream>库
	2.通过test1.cpp 进行W、S、A、D、ESC的键值获取，并进行宏定义。
	3.定义程序所需常量，全局变量。
2.主函
	main()函数；
		初始化画布 设定前景与背景颜色 调用 graphics.h 库中的rectangle,line函数进行游戏界面构造
		调用Initialize，ShowScore，ShowSpeed,ShowHelp函数初始化图形界面，调用 stdlib.h 中的 rand 函数获取随机boxnumber 构造第一个Box
		调用ShowNextBox 显示当前游戏方块与下一游戏方块。PreGraph 修正地板着色。
					进入循环---->按键按下与否----->相应操作。
3.数据结构
	1.游戏界面:
		结构体BORAD 所有方块都具有的性质：被占（1）空（0） 颜色：color
		构建		Horizontal_boxs * Vertical_boxs 的游戏底板
	2.方块形状:
		结构体 SHAPE
		利用二进制的0/1表示方格为空或非空(□/■)
		二维数组储存十九种方块的形状
		■ ■ ■ ■ //8个字节为一组 11110000
		□ □ □ □
		□ □ □ □	// 00000000
		□ □ □ □

4.游戏主体函数：
	------------------------------
	1.方块移动函数： Move(int x, int y, int boxnumber, int direction)
		接受主函数switch-case 的参数
		Curbox_x,Curbox_y,box_id,direction 获取当前游戏方块左上角坐标
		调用EraseBox 擦除 原有方块 并使其X，Y坐标+BSIZE 同时 调用 ShowNextBox显示移动后的方块
	2.满行消除函数：DelFullRow(int y)
		需要接受满行坐标
		上一行的Table_board.var 覆盖下一行的 Table_board.var
	3.方块擦除函数：EraseBox(int x, int y, int box_number)
		接受主函数传入的方块所在坐标，对该方块所在区域进行重写
		利用line函数覆盖原有方块线条。
	4.方格显示函数：ShowNextBox(int x, int y, int box_number, int color)
		接受主函数传入的坐标与游戏方块编号，在该坐标绘制相应游戏方块。
	5.界面初始化: Initialize(int x, int y, int m, int n)
		绘制游戏底板，Horizontal_boxs * Vertical_boxs 个方块的底板。
		同时调用ShowScore,ShowSpeed函数 初始化 分数速度显示。
	6.寻找满行函数: setFullRow(int t_boardy)
		接受传入的t_boardy --- 方块左上角相对于游戏界面左上角的坐标（方块个数）。
		通过对其+1 +2 +3 +4 遍历当游戏方块无法继续下落是其所属的每行的var值判断是否满行。
		满行传入DelFullRow函数进行满行消除
	7.新方块生成函数： MkNextBox(int box_number)
		当上一游戏方块无法继续下落且未到达顶层时，设置flag_newbox = 1 调用新方块生成函数。
		接受传入的游戏方块编号，在底板顶部调用ShowNextBox绘制相应新游戏方块。
		同时进行随机选取下一游戏方块的游戏编号，调用ShowNextBox在下一方块预览界面绘制下一方块。
	8.显示分数函数: ShowScore(int score)
		在游戏的显示界面显示当前分数。
	9.显示游戏速度函数: ShowSpeed(int speed)
		在游戏的显示界面显示当前游戏速度
	10.显示游戏提示函数： ShowHelp(int xs, int ys)
		在游戏的显示界面显示游戏提示
	11.下一方块更新函数:	ErasePreBox(int x, int y, int box_number)
		擦除上一方块预览
	------------------------------

5.补充函数：
	-------------------------------
	1.地板着色修正函数：PreGraph(int x, int y)
		地板可能因为其他ShowNextBox或EraseBox影响导致其原本的颜色失真，在其调用时加入
		地板着色修正函数进行修正
	2.杂线覆盖函数：clean()
		在程序运行时出现预料之外的线条，调用clean函数对其进行修正
	3.真值修正函数: ClearBoard()
		当地板的var值被修改后，需要准确判断其var值及其他地板var值 并对玩家进行反馈
		调用ClearBoard函数修正真值 并 用 颜色标记后反馈给玩家。
	-------------------------------


*/

struct BOARD
{
    int var;
    int color;
}Table_board[Vertical_boxs][Horizontal_boxs];

struct SHAPE
{
    char box[2];
    int color;      //颜色
    int next;       //下一个编号
};

struct SHAPE shape_box[MAX_BOX] = {
        {0x88,0xc0,CYAN,1},
        {0xe8,0x0,CYAN,2},
        {0xc4,0x40,CYAN,3},
        {0x2e,0x0,CYAN,0},

        {0x44,0xc0,MAGENTA,5},
        {0x8e,0x0,MAGENTA,6},
        {0xc8,0x80,MAGENTA,7},
        {0xe2,0x0,MAGENTA,4},

        {0x8c,0x40,BLUE,9},
        {0x6c,0x0,BLUE,8},

        {0x4c,0x80,BROWN,11},
        {0xc6,0x0,BROWN,10},

        {0x4e,0x0,GREEN,13},
        {0x8c,0x80,GREEN,14},
        {0xe4,0x0,GREEN,15},
        {0x4c,0x40,GREEN,12},

        {0x88,0x88,RED,17},
        {0xf0,0x0,RED,16},

        {0xcc,0x0,BLUE,18}
};

void ShowScore(int score)
{
    int x, y;
    char score_str[5];
    setfillstyle(EMPTY_FILL, BgColor);
    x = LeftWin_x + 10;
    y = 100;
    bar(x - BSIZE, y, x + BSIZE * 3, y + BSIZE * 3);
    sprintf_s(score_str, "%3d", score);
    outtextxy(x, y, "分数:");
    outtextxy(x, y - 50, "俄罗斯方块");
    outtextxy(x + 40, y, score_str);
}

void ShowSpeed(int speed)
{
    int x, y;
    char speed_str[20];
    setfillstyle(EMPTY_FILL, BgColor);
    x = LeftWin_x + 10;
    y = 150;
    bar(x - BSIZE, y, x + BSIZE * 3, y + BSIZE * 3);
    outtextxy(x, y, "速度:              ms/20pixes");
    sprintf_s(speed_str, "%3d", speed);
    outtextxy(x + 40, y, speed_str);
    outtextxy(x, y + 50, "下一个方块是:");
}

void PreGraph(int x, int y)
{
    setlinecolor(WHITE);
    int m, n, oldx = x;
    for (m = 0; m < 4; m++)
    {
        for (n = 0; n < 4; n++)
        {
            line(x, y, x + BSIZE, y);
            line(x, y, x, y + BSIZE);
            line(x, y + BSIZE, x + BSIZE, y + BSIZE);
            line(x + BSIZE, y, x + BSIZE, y + BSIZE);
            x += BSIZE;
        }
        x = oldx;
        y += BSIZE;
    }
}

void Initialize(int x, int y, int m, int n)
{
    int i, j, oldx;
    oldx = x;
    for (j = 0; j < n; j++)
    {
        for (i = 0; i < m; i++)
        {
            Table_board[j][i].var = 0;
            Table_board[j][i].color = FgColor;
            line(x, y, x + BSIZE, y);
            line(x, y, x, y + BSIZE);
            line(x, y + BSIZE, x + BSIZE, y + BSIZE);
            line(x + BSIZE, y, x + BSIZE, y + BSIZE);
            x += BSIZE;
        }
        y += BSIZE;
        x = oldx;
    }
    Curbox_x = x;
    Curbox_y = y;
    flag_newbox = false;
    ShowScore(score);
    ShowSpeed(speed);
}

void ShowHelp(int xs, int ys)
{
    char help_str[100];
    setfillstyle(EMPTY_FILL, BgColor);
    setcolor(WHITE);
    /*sprintf_s(help_str, " ↑旋转  ↓向下加速");
    outtextxy(xs + 40, ys + 30, help_str);
    sprintf_s(help_str, " ←向左移动 向右移动→");
    outtextxy(xs + 40, ys + 45, help_str);
    outtextxy(xs + 80, ys + 60, "ESC-EXIT ");*/
    sprintf_s(help_str, "W ：旋转        A : 左移       ESC : 退出游戏");
    outtextxy(xs - 45, ys + 15, help_str);
    sprintf_s(help_str, "S : 下移        D : 右移       Notice : 注意大小写");
    outtextxy(xs - 45, ys + 45, help_str);
}

void clean()
{
    int x = SYS_X + Horizontal_boxs * BSIZE, y = SYS_Y, i;
    for (i = 0; i < Vertical_boxs; i++)
    {
        setlinecolor(BLACK);
        line(x, y, x + BSIZE, y);
        setlinecolor(WHITE);
        line(x - Horizontal_boxs * BSIZE, y, x - Horizontal_boxs * BSIZE, y + BSIZE);
        y += BSIZE;
    }
    x = LeftWin_x + 4 * BSIZE, y = SYS_Y + 200;
    setlinecolor(BLACK);
    line(x, y, x + BSIZE, y);
}

void ClearBoard(int x, int y)
{
    int i, j;
    for (i = 0; i < Vertical_boxs; i++)
    {
        for (j = 0; j < Horizontal_boxs; j++)
        {
            if (!Table_board[i][j].var)
            {
                setlinecolor(WHITE);
                line(x + j * BSIZE, y + i * BSIZE, x + j * BSIZE + BSIZE, y + i * BSIZE);
                line(x + j * BSIZE, y + i * BSIZE, x + j * BSIZE + BSIZE, y + i * BSIZE);
                line(x + j * BSIZE + BSIZE, y + i * BSIZE, x + j * BSIZE + BSIZE, y + i * BSIZE + BSIZE);
                line(x + j * BSIZE, y + i * BSIZE + BSIZE, x + j * BSIZE + BSIZE, y + i * BSIZE + BSIZE);
            }
            else
            {
                setlinecolor(RED);
                line(x + j * BSIZE, y + i * BSIZE, x + j * BSIZE + BSIZE, y + i * BSIZE);
                line(x + j * BSIZE, y + i * BSIZE, x + j * BSIZE + BSIZE, y + i * BSIZE);
                line(x + j * BSIZE + BSIZE, y + i * BSIZE, x + j * BSIZE + BSIZE, y + i * BSIZE + BSIZE);
                line(x + j * BSIZE, y + i * BSIZE + BSIZE, x + j * BSIZE + BSIZE, y + i * BSIZE + BSIZE);

            }
        }
    }
}

int DelFullRow(int y)
{
    int n, top = 0;
    /*top 保存最高点的纵坐标 由下往上出现一行全空就是最高点处*/
    register int m, total;
    for (n = y; n >= 0; n--)
    {
        total = 0;
        for (m = 0; m < Horizontal_boxs; m++)
        {
            if (!Table_board[n][m].var) total++;
            if (Table_board[n][m].var != Table_board[n - 1][m].var)
            {
                Table_board[n][m].var = Table_board[n - 1][m].var;
                /*Table_board[n][m].color = Table_board[n - 1][m].color;*/
            }
        }
        if (total == Horizontal_boxs)
        {
            top = n;
            break;
        }
    }
    return top;
}

void setFullRow(int t_boardy)
{
    /*t_boardy 是游戏方块的左上角的坐标相对于游戏界面左上角坐标的纵向距离*/
    int n, fullnumber = 0, top = 0;
    /*top 保存的是当前游戏主板消除满行后的最高点*/
    register int m;
    for (n = t_boardy + 3; n >= t_boardy; n--)
    {
        if (n < 0 || n >= Vertical_boxs)  continue;
        for (m = 0; m < Horizontal_boxs; m++)
        {
            if (!Table_board[n + fullnumber][m].var)  break;
        }
        if (m == Horizontal_boxs)
        {
            if (n == t_boardy + 3)
            {
                top = DelFullRow(n + fullnumber);
                ClearBoard(SYS_X, SYS_Y);
            }
            else
            {
                DelFullRow(n + fullnumber);
                ClearBoard(SYS_X, SYS_Y);
                fullnumber++;
            }
        }
    }
    if (fullnumber)
    {
        int oldx, x = SYS_X, y = top * BSIZE + SYS_Y;
        oldx = x;
        score = score + fullnumber * 10;
        for (n = top; n < t_boardy + 4; n++)
        {
            if (n >= Vertical_boxs)	continue;
            /*for (m = 0; m < Horizontal_boxs; m++)
            {
                if (Table_board[n][m].var)
                    setfillstyle(SOLID_FILL, Table_board[n][m].color);
                else
                {
                    setfillstyle(SOLID_FILL, BgColor);
                    bar(x, y, x + BSIZE, y + BSIZE);
                    line(x, y, x + BSIZE, y);
                    line(x, y, x, y + BSIZE);
                    line(x, y + BSIZE, x + BSIZE, y + BSIZE);
                    line(x + BSIZE, y, x + BSIZE, y + BSIZE);
                    x += BSIZE;
                }
                y += BSIZE;
                x = oldx;
            }*/
        }
        ShowScore(score);
        int i = 30;
        if (speed != 1000 - (score / 30) * 100)
        {
            speed = 1000 - (score / 30) * 100;
            ShowSpeed(speed);
        }
        else
        {
            ShowSpeed(speed);
        }
    }
}

void ShowNextBox(int x, int y, int box_number, int color)
{
    int i, j, ls_x = x;
    if (box_number<0 || box_number>MAX_BOX)
    {
        box_number = MAX_BOX / 2;
    }
    setcolor(color);
    setlinecolor(color);
    for (j = 0; j < 2; j++)
    {
        int mask = 128;
        for (i = 0; i < 8; i++)
        {
            if (i % 4 == 0 && i != 0)
            {
                y += BSIZE;
                x = ls_x;
            }
            if (shape_box[box_number].box[j] & mask)
            {
                /*setfillcolor(color);
                bar(x, y, x + BSIZE, y + BSIZE);*/
                line(x, y, x + BSIZE, y);
                line(x, y, x, y + BSIZE);
                line(x, y + BSIZE, x + BSIZE, y + BSIZE);
                line(x + BSIZE, y, x + BSIZE, y + BSIZE);
            }
            x += BSIZE;
            mask /= 2;
        }
        y += BSIZE;
        x = ls_x;
    }


}

void EraseBox(int x, int y, int box_number)
{
    int mask = 128, n, m;
    setlinecolor(WHITE);
    setfillcolor(BLACK);
    for (n = 0; n < 4; n++)
    {
        for (m = 0; m < 4; m++)
        {
            if (shape_box[box_number].box[n / 2] & mask)
            {
                bar(x + m * BSIZE, y + n * BSIZE, x + m * BSIZE + BSIZE, y + n * BSIZE + BSIZE);
                line(x + m * BSIZE, y + n * BSIZE, x + m * BSIZE + BSIZE + BSIZE, y + n * BSIZE);
                line(x + m * BSIZE, y + n * BSIZE, x + m * BSIZE, y + n * BSIZE + BSIZE);
                line(x + m * BSIZE + BSIZE, y + n * BSIZE, x + m * BSIZE + BSIZE, y + n * BSIZE + BSIZE);
                line(x + m * BSIZE, y + n * BSIZE + BSIZE, x + m * BSIZE + BSIZE, y + n * BSIZE + BSIZE);
            }
            mask = mask / (2);
            if (mask == 0)
            {
                mask = 128;
            }
        }
    }
    ClearBoard(SYS_X, SYS_Y);
    clean();
}

void ErasePreBox(int x, int y, int box_number)
{
    int mask = 128, n, m;
    for (n = 0; n < 4; n++)
    {
        for (m = 0; m < 4; m++)
        {
            if (shape_box[box_number].box[n / 2] & mask)
            {
                setfillstyle(SOLID_FILL, WHITE);
                bar(x + m * BSIZE, y + n * BSIZE, x + m * BSIZE + BSIZE, y + n * BSIZE + BSIZE);
            }
            mask /= 2;
            if (mask == 0) mask = 128;

        }
    }
}

int MkNextBox(int box_number)
{
    int mask = 128, t_boardx, t_boardy, n, m;
    t_boardx = (Curbox_x - SYS_X) / BSIZE;
    t_boardy = (Curbox_y - SYS_Y) / BSIZE;
    for (n = 0; n < 4; n++)
    {
        for (m = 0; m < 4; m++)
        {
            if (shape_box[current_box_numb].box[n / 2] & mask)
            {
                Table_board[t_boardy + n][t_boardx + m].var = 1;
                Table_board[t_boardy + n][t_boardx + m].color = shape_box[current_box_numb].color;
            }
            mask /= 2;
            if (mask == 0) mask = 128;
        }
    }
    setFullRow(t_boardy);
    Curbox_x = SYS_X + Begin_boxs_x * BSIZE, Curbox_y = SYS_Y;
    if (box_number == -1)	box_number = rand() % MAX_BOX;
    current_box_numb = box_number;
    flag_newbox = false;
    return(rand() % MAX_BOX);
}

int Move(int x, int y, int boxnumber, int direction)
{
    int n, m, t_boardx, t_boardy;
    int mask;
    if (direction == MoveLeft)
    {
        mask = 128;
        x -= BSIZE;
        t_boardx = (x - SYS_X) / BSIZE;
        t_boardy = (y - SYS_Y) / BSIZE;
        for (n = 0; n < 4; n++)
        {
            for (m = 0; m < 4; m++)
            {
                if (shape_box[boxnumber].box[n / 2] & mask)
                {
                    if (x + BSIZE * m < SYS_X) return(false);
                    else if (Table_board[t_boardy + n][t_boardx + m].var)
                    {
                        return false;
                    }
                }
                mask /= 2;
                if (mask == 0) mask = 128;
            }
        }
        return true;
    }
    else if (direction == MoveRight)
    {
        mask = 128;
        x += BSIZE;
        t_boardx = (x - SYS_X) / BSIZE;
        t_boardy = (y - SYS_Y) / BSIZE;
        for (n = 0; n < 4; n++)
        {
            for (m = 0; m < 4; m++)
            {
                if (shape_box[boxnumber].box[n / 2] & mask)
                {
                    if (x + BSIZE * m >= SYS_X + Horizontal_boxs * BSIZE) return(false);
                    else if (Table_board[t_boardy + n][t_boardx + m].var)
                    {
                        return false;
                    }
                }
                mask /= 2;
                if (mask == 0) mask = 128;
            }
        }
        return true;
    }
    else if (direction == MoveDown)
    {
        y += BSIZE;
        t_boardx = (x - SYS_X) / BSIZE;
        t_boardy = (y - SYS_Y) / BSIZE;
        mask = 128;
        for (n = 0; n < 4; n++)
        {
            for (m = 0; m < 4; m++)
            {
                if (shape_box[boxnumber].box[n / 2] & mask)
                {
                    if ((y + BSIZE * n + BSIZE) > (SYS_Y + Vertical_boxs * BSIZE) || Table_board[t_boardy + n][t_boardx + m].var)
                    {
                        flag_newbox = true;
                        break;
                    }
                }
                mask = mask / 2;
                if (mask == 0) mask = 128;
            }
        }
        if (flag_newbox)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else if (direction == MoveRoll)
    {
        t_boardx = (x - SYS_X) / BSIZE;
        t_boardy = (y - SYS_Y) / BSIZE;
        mask = 128;
        for (n = 0; n < 4; n++)
        {
            for (m = 0; m < 4; m++)
            {
                if (shape_box[boxnumber].box[n / 2] & mask)
                {
                    if ((y + BSIZE * n) >= (SYS_Y + BSIZE * Vertical_boxs)) return false;
                    if ((x + BSIZE * n) >= (SYS_X + BSIZE * Horizontal_boxs)) return false;
                    if ((x + BSIZE * m) >= (SYS_X + BSIZE * Horizontal_boxs)) return false;
                    else if (Table_board[t_boardy + n][t_boardx + m].var)
                    {
                        return false;
                    }
                }
                mask = mask / 2;
                if (mask == 0) mask = 128;
            }
        }
        return true;
    }
    else
    {
        return false;
    }
    return false;
}

int main()
{
    int GameOver = 0, nextboxnumber, key, CurrentAction = 0;
    initgraph(640, 480);	//初始化画布
    setbkcolor(BLACK);
    setcolor(WHITE);
    cleardevice();
    rectangle(SYS_X - 80, SYS_Y - 10, LeftWin_x + 120, SYS_Y + 320);
    rectangle(SYS_X - 80, SYS_Y + 320, LeftWin_x + 120, SYS_Y + 400);
    line(LeftWin_x - 20, SYS_Y - 10, LeftWin_x - 20, SYS_Y + 320);
    line(LeftWin_x - 20, SYS_Y + 70, LeftWin_x + 120, SYS_Y + 70);
    line(LeftWin_x - 20, SYS_Y + 150, LeftWin_x + 120, SYS_Y + 150);
    line(LeftWin_x - 20, SYS_Y + 110, LeftWin_x + 120, SYS_Y + 110);
    Initialize(SYS_X, SYS_Y, Horizontal_boxs, Vertical_boxs);	//初始化图形界面
    nextboxnumber = MkNextBox(rand() % 19);	//获取下一个方块的编号
    //ShowNextBox(Curbox_x, Curbox_y, current_box_numb, shape_box[current_box_numb].color);	//显示游戏中方块
    //ShowNextBox(LeftWin_x, SYS_Y + 200, nextboxnumber, shape_box[nextboxnumber].color);		//显示下一个方块
    ShowHelp(SYS_X, SYS_Y + 320);		//显示提示
    PreGraph(LeftWin_x, SYS_Y + 200);
    while (1)	//接受按键循环体
    {
        CurrentAction = 0;
        flag_newbox = false;
        if (_kbhit())
        {
            key = _getch();

            switch (key)
            {
                case W:
                    if (Move(Curbox_x, Curbox_y, shape_box[current_box_numb].next, MoveRoll))
                    {
                        EraseBox(Curbox_x, Curbox_y, current_box_numb);
                        current_box_numb = shape_box[current_box_numb].next;
                        CurrentAction = MoveRoll;
                    }
                    break;


                case A:
                    if (Move(Curbox_x, Curbox_y, current_box_numb, MoveLeft))
                    {
                        EraseBox(Curbox_x, Curbox_y, current_box_numb);
                        Curbox_x -= BSIZE;
                        CurrentAction = MoveLeft;
                    }
                    break;


                case D:
                    if (Move(Curbox_x, Curbox_y, current_box_numb, MoveRight))
                    {
                        EraseBox(Curbox_x, Curbox_y, current_box_numb);
                        Curbox_x += BSIZE;
                        CurrentAction = MoveRight;
                    }
                    break;


                case S:
                    if (Move(Curbox_x, Curbox_y, current_box_numb, MoveDown))
                    {
                        EraseBox(Curbox_x, Curbox_y, current_box_numb);
                        Curbox_y += BSIZE;
                        CurrentAction = MoveDown;
                    }
                    break;

                case ESC:
                    GameOver = 1;
                    break;
                default:
                    break;
            }
            if (CurrentAction)
            {
                ShowNextBox(Curbox_x, Curbox_y, current_box_numb, shape_box[current_box_numb].color);
                CurrentAction = 0;
            }
            if (flag_newbox)
            {
                EraseBox(LeftWin_x, SYS_Y + 200, nextboxnumber);
                nextboxnumber = MkNextBox(nextboxnumber);
                ShowNextBox(LeftWin_x, SYS_Y + 200, nextboxnumber, shape_box[nextboxnumber].color);
                if (!Move(Curbox_x, Curbox_y, current_box_numb, MoveDown))
                {
                    ShowNextBox(Curbox_x, Curbox_y, current_box_numb, shape_box[current_box_numb].color);
                    GameOver = 1;
                }
                else
                {
                    flag_newbox = false;
                }
                CurrentAction = 0;
            }
        }
        else
        {
            if (Move(Curbox_x, Curbox_y, current_box_numb, MoveDown) && GameOver != 1)
            {
                EraseBox(Curbox_x, Curbox_y, current_box_numb);
                Move(Curbox_x, Curbox_y, current_box_numb, MoveDown);
                Curbox_y += BSIZE;
                ShowNextBox(Curbox_x, Curbox_y, current_box_numb, shape_box[current_box_numb].color);
                Sleep(speed
                );
            }
            if (flag_newbox)
            {
                EraseBox(LeftWin_x, SYS_Y + 200, nextboxnumber);
                nextboxnumber = MkNextBox(nextboxnumber);
                ShowNextBox(LeftWin_x, SYS_Y + 200, nextboxnumber, shape_box[nextboxnumber].color);
                if (!Move(Curbox_x, Curbox_y, current_box_numb, MoveDown))
                {
                    ShowNextBox(Curbox_x, Curbox_y, current_box_numb, shape_box[current_box_numb].color);
                    GameOver = 1;
                }
                else
                {
                    flag_newbox = false;
                }
            }
        }

        if (GameOver)
        {
            settextstyle(40, 25, _T("微软雅黑"));
            setcolor(RED);
            outtextxy(0, 200, "游戏结束，请按回车键退出。");
            getchar();
            break;
        }
    }
    printf("游戏结束，您的分数是:%d，感谢游戏。", score);
    closegraph();
    getchar();
}
