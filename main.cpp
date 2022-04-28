#include <cstdio>
#include <cstdlib>
#include <dos.h>
#include <graphics.h>
#include <conio.h>
#include <iostream>
#include <windows.h>


/* ������: */
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



/* ���峣�� */
#define MAX_BOX 19      //19�ַ���
#define BSIZE 20        //��Ϸ����߳�20������
#define SYS_X 160       //���ϽǷ������x����
#define SYS_Y 25        //���ϽǷ������y����
#define Horizontal_boxs 10      //���������Է����С ��10�����飩
#define Vertical_boxs 15        //���������Է����С  (15������)
#define Begin_boxs_x Horizontal_boxs/2 //��һ��������ʼλ��
#define FgColor WHITE     //ǰ����ɫ
#define BgColor BLACK      //������ɫ
#define LeftWin_x SYS_X+Horizontal_boxs*BSIZE+46	//�ұ�״̬����x����
#define false 0
#define true 1
#define MoveLeft 1
#define MoveRight 2
#define MoveDown 3
#define MoveRoll 4

/*ȫ�ֱ�������*/

int current_box_numb;       //��ǰ������
int Curbox_x = SYS_X + Begin_boxs_x * BSIZE, Curbox_y = SYS_Y;        //x,y��ǰ����
int flag_newbox = false;
int speed = 1000;
int score = 0;

/*
1.Ԥ����
	1.����<windows.h><conio.h><stdio.h><stdlib.h><graphics.h><dos.h><iostream>��
	2.ͨ��test1.cpp ����W��S��A��D��ESC�ļ�ֵ��ȡ�������к궨�塣
	3.����������賣����ȫ�ֱ�����
2.����
	main()������
		��ʼ������ �趨ǰ���뱳����ɫ ���� graphics.h ���е�rectangle,line����������Ϸ���湹��
		����Initialize��ShowScore��ShowSpeed,ShowHelp������ʼ��ͼ�ν��棬���� stdlib.h �е� rand ������ȡ���boxnumber �����һ��Box
		����ShowNextBox ��ʾ��ǰ��Ϸ��������һ��Ϸ���顣PreGraph �����ذ���ɫ��
					����ѭ��---->�����������----->��Ӧ������
3.���ݽṹ
	1.��Ϸ����:
		�ṹ��BORAD ���з��鶼���е����ʣ���ռ��1���գ�0�� ��ɫ��color
		����		Horizontal_boxs * Vertical_boxs ����Ϸ�װ�
	2.������״:
		�ṹ�� SHAPE
		���ö����Ƶ�0/1��ʾ����Ϊ�ջ�ǿ�(��/��)
		��ά���鴢��ʮ���ַ������״
		�� �� �� �� //8���ֽ�Ϊһ�� 11110000
		�� �� �� ��
		�� �� �� ��	// 00000000
		�� �� �� ��

4.��Ϸ���庯����
	------------------------------
	1.�����ƶ������� Move(int x, int y, int boxnumber, int direction)
		����������switch-case �Ĳ���
		Curbox_x,Curbox_y,box_id,direction ��ȡ��ǰ��Ϸ�������Ͻ�����
		����EraseBox ���� ԭ�з��� ��ʹ��X��Y����+BSIZE ͬʱ ���� ShowNextBox��ʾ�ƶ���ķ���
	2.��������������DelFullRow(int y)
		��Ҫ������������
		��һ�е�Table_board.var ������һ�е� Table_board.var
	3.�������������EraseBox(int x, int y, int box_number)
		��������������ķ����������꣬�Ը÷����������������д
		����line��������ԭ�з���������
	4.������ʾ������ShowNextBox(int x, int y, int box_number, int color)
		�����������������������Ϸ�����ţ��ڸ����������Ӧ��Ϸ���顣
	5.�����ʼ��: Initialize(int x, int y, int m, int n)
		������Ϸ�װ壬Horizontal_boxs * Vertical_boxs ������ĵװ塣
		ͬʱ����ShowScore,ShowSpeed���� ��ʼ�� �����ٶ���ʾ��
	6.Ѱ�����к���: setFullRow(int t_boardy)
		���ܴ����t_boardy --- �������Ͻ��������Ϸ�������Ͻǵ����꣨�����������
		ͨ������+1 +2 +3 +4 ��������Ϸ�����޷�������������������ÿ�е�varֵ�ж��Ƿ����С�
		���д���DelFullRow����������������
	7.�·������ɺ����� MkNextBox(int box_number)
		����һ��Ϸ�����޷�����������δ���ﶥ��ʱ������flag_newbox = 1 �����·������ɺ�����
		���ܴ������Ϸ�����ţ��ڵװ嶥������ShowNextBox������Ӧ����Ϸ���顣
		ͬʱ�������ѡȡ��һ��Ϸ�������Ϸ��ţ�����ShowNextBox����һ����Ԥ�����������һ���顣
	8.��ʾ��������: ShowScore(int score)
		����Ϸ����ʾ������ʾ��ǰ������
	9.��ʾ��Ϸ�ٶȺ���: ShowSpeed(int speed)
		����Ϸ����ʾ������ʾ��ǰ��Ϸ�ٶ�
	10.��ʾ��Ϸ��ʾ������ ShowHelp(int xs, int ys)
		����Ϸ����ʾ������ʾ��Ϸ��ʾ
	11.��һ������º���:	ErasePreBox(int x, int y, int box_number)
		������һ����Ԥ��
	------------------------------

5.���亯����
	-------------------------------
	1.�ذ���ɫ����������PreGraph(int x, int y)
		�ذ������Ϊ����ShowNextBox��EraseBoxӰ�쵼����ԭ������ɫʧ�棬�������ʱ����
		�ذ���ɫ����������������
	2.���߸��Ǻ�����clean()
		�ڳ�������ʱ����Ԥ��֮�������������clean���������������
	3.��ֵ��������: ClearBoard()
		���ذ��varֵ���޸ĺ���Ҫ׼ȷ�ж���varֵ�������ذ�varֵ ������ҽ��з���
		����ClearBoard����������ֵ �� �� ��ɫ��Ǻ�������ҡ�
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
    int color;      //��ɫ
    int next;       //��һ�����
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
    outtextxy(x, y, "����:");
    outtextxy(x, y - 50, "����˹����");
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
    outtextxy(x, y, "�ٶ�:              ms/20pixes");
    sprintf_s(speed_str, "%3d", speed);
    outtextxy(x + 40, y, speed_str);
    outtextxy(x, y + 50, "��һ��������:");
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
    /*sprintf_s(help_str, " ����ת  �����¼���");
    outtextxy(xs + 40, ys + 30, help_str);
    sprintf_s(help_str, " �������ƶ� �����ƶ���");
    outtextxy(xs + 40, ys + 45, help_str);
    outtextxy(xs + 80, ys + 60, "ESC-EXIT ");*/
    sprintf_s(help_str, "W ����ת        A : ����       ESC : �˳���Ϸ");
    outtextxy(xs - 45, ys + 15, help_str);
    sprintf_s(help_str, "S : ����        D : ����       Notice : ע���Сд");
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
    /*top ������ߵ�������� �������ϳ���һ��ȫ�վ�����ߵ㴦*/
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
    /*t_boardy ����Ϸ��������Ͻǵ������������Ϸ�������Ͻ�������������*/
    int n, fullnumber = 0, top = 0;
    /*top ������ǵ�ǰ��Ϸ�����������к����ߵ�*/
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
    initgraph(640, 480);	//��ʼ������
    setbkcolor(BLACK);
    setcolor(WHITE);
    cleardevice();
    rectangle(SYS_X - 80, SYS_Y - 10, LeftWin_x + 120, SYS_Y + 320);
    rectangle(SYS_X - 80, SYS_Y + 320, LeftWin_x + 120, SYS_Y + 400);
    line(LeftWin_x - 20, SYS_Y - 10, LeftWin_x - 20, SYS_Y + 320);
    line(LeftWin_x - 20, SYS_Y + 70, LeftWin_x + 120, SYS_Y + 70);
    line(LeftWin_x - 20, SYS_Y + 150, LeftWin_x + 120, SYS_Y + 150);
    line(LeftWin_x - 20, SYS_Y + 110, LeftWin_x + 120, SYS_Y + 110);
    Initialize(SYS_X, SYS_Y, Horizontal_boxs, Vertical_boxs);	//��ʼ��ͼ�ν���
    nextboxnumber = MkNextBox(rand() % 19);	//��ȡ��һ������ı��
    //ShowNextBox(Curbox_x, Curbox_y, current_box_numb, shape_box[current_box_numb].color);	//��ʾ��Ϸ�з���
    //ShowNextBox(LeftWin_x, SYS_Y + 200, nextboxnumber, shape_box[nextboxnumber].color);		//��ʾ��һ������
    ShowHelp(SYS_X, SYS_Y + 320);		//��ʾ��ʾ
    PreGraph(LeftWin_x, SYS_Y + 200);
    while (1)	//���ܰ���ѭ����
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
            settextstyle(40, 25, _T("΢���ź�"));
            setcolor(RED);
            outtextxy(0, 200, "��Ϸ�������밴�س����˳���");
            getchar();
            break;
        }
    }
    printf("��Ϸ���������ķ�����:%d����л��Ϸ��", score);
    closegraph();
    getchar();
}
