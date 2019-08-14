/** Name: SnackCoreTest*/
#include"Snack.h"
#include<iostream>
#include<graphics.h>
#include<conio.h>
int MAP_LINE = 30;
int MAP_COL = 40;
int BLOCK_SIZE = 25;
using namespace std;
void DrawMap(Snack& mygame);
MOV_DIRECT GetMove();
int main(int argc, char * argv[])
{
	HWND hwnd = initgraph(BLOCK_SIZE*MAP_COL, BLOCK_SIZE*MAP_LINE);
	setbkcolor(RGB(200,200,200));
	setbkmode(TRANSPARENT);
	settextcolor(RGB(120,220,120));
	cleardevice();
	if (MessageBox(hwnd, "是否默认大小？", "游戏设置：大小设置", MB_YESNO) == IDNO)
	{
		char read[20] = { 0 };
		InputBox(read,20-1,"请输入大小（长*宽）：","大小设置");
		int line = 0, col = 0;
		if (sscanf(read, "%d*%d", &line, &col) == 2)
		{
			MAP_LINE = line;
			MAP_COL = col;
			if (line<25)
				BLOCK_SIZE = (25 - line) / 5 * 10 + 25;
			hwnd = initgraph(BLOCK_SIZE*MAP_COL, BLOCK_SIZE*MAP_LINE);
			setbkcolor(RGB(200, 200, 200));
			setbkmode(TRANSPARENT);
			settextcolor(RGB(120, 220, 120));
			cleardevice();
		}

	}
	bool Autoplay = true;
	bool Wallopen = false;
	if (MessageBox(hwnd, "是否玩家玩？", "游戏设置：玩家设置", MB_YESNO) == IDYES)
		Autoplay = false;
	else
		Autoplay = true;
	if (MessageBox(hwnd, "是否去除边界？", "游戏设置：边界设置", MB_YESNO) == IDYES)
		Wallopen = false;
	else
		Wallopen = true;
	while (1)
	{
		Snack mygame(MAP_LINE, MAP_COL, Wallopen);
		while (1)
		{
			DrawMap(mygame);
			if (!Autoplay)
			{
				MOV_DIRECT direct = GetMove();
				if (!mygame.Run(direct))
					break;
				Sleep(80);
			}
			else
			{
				if (!mygame.AutoRun())
					break;
				Sleep(10);
			}
		}
		if (MessageBox(hwnd, "是否重玩？", "游戏提示：游戏结束", MB_YESNO) == IDNO)
			break;
	}
	return 0;
}
MOV_DIRECT GetMove()
{
	char move = '\0';
	if (kbhit())
	{
		move = getch();
		if (move == 'w' || move == '8' || move == 'W')
			return MOV_UP;
		if (move == 's' || move == '5' || move == 'S')
			return MOV_DOWN;
		if (move == 'a' || move == '4' || move == 'A')
			return MOV_LEFT;
		if (move == 'd' || move == '6' || move == 'D')
			return MOV_RIGHT;
	}
	else
		return MOV_NULL;

}
void DrawMap(Snack& mygame)
{
	BeginBatchDraw();
	cleardevice();
	for (int i = 0; i < mygame.GetLines(); i++)
	{
		for (int j = 0; j < mygame.GetCols(); j++)
		{
			int elem = mygame.MapAt(i, j);
			cout.width(2);
			if (elem == ELEM_VECANT)
			{
				setfillcolor(RGB(255,255,255));
			}
			else if (elem == ELEM_GIFT)
			{
				if(mygame.IsNormalGift())
					setfillcolor(RGB(255, 255, 0));
				else
					setfillcolor(RGB(255, 120, 0));
			}
			else if (elem == ELEM_SNACK)
			{
				setfillcolor(RGB(0, 255, 0));
			}
			else if (elem == ELEM_SNACK_HEAD)
			{
				setfillcolor(RGB(255, 0, 0));
			}
			else if (elem == ELEM_SNACK_TAIL)
			{
				setfillcolor(RGB(100, 100, 100));
			}
			else if (elem==ELEM_WALL)
			{
				setfillcolor(RGB(0,0,0));
			}
			solidcircle(j*BLOCK_SIZE + BLOCK_SIZE / 2, i*BLOCK_SIZE + BLOCK_SIZE / 2, BLOCK_SIZE/2);
		}
	}
	char temp[20] = { 0 };
	sprintf(temp,"Score : %d\0",mygame.GetScore());
	outtextxy(0,BLOCK_SIZE,temp);
	sprintf(temp, "Length : %d\0", mygame.GetSnackLen());
	outtextxy(0, 2*BLOCK_SIZE, temp);
	EndBatchDraw();
}