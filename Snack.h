#ifndef _SNACK_H_
#define _SNACK_H_
#include<stdlib.h>
#include<time.h>
typedef int ELEM_TYPE;
#define ELEM_VECANT 0
#define ELEM_WALL  1
#define ELEM_SNACK 2
#define ELEM_GIFT  3
#define ELEM_SNACK_HEAD 4
#define ELEM_SNACK_TAIL 5
typedef int MOV_DIRECT;
#define MOV_NULL 0
#define MOV_UP 8
#define MOV_DOWN 5
#define MOV_LEFT 4
#define MOV_RIGHT 6
typedef struct
{
	int line;
	int col;
}Point;
typedef struct node
{
	Point point;
	struct node * next;
}PointNode;
typedef PointNode SnackHead;
typedef struct
{
	Point point;
	int reward;
}Gift;
class Snack
{
public:
	Snack(int lines=10,int cols=10,bool haveWall=true);
	~Snack();
	ELEM_TYPE & MapAt(int rindex, int cindex);
	int GetLines();
	int GetCols();
	bool Run(MOV_DIRECT direct);
	int GetScore();
	int GetSnackLen();
	void Clear();
	bool IsNormalGift();
	bool AutoRun();
private:
	struct
	{
		Point np;
		ELEM_TYPE ne;
	}NextInfo;
	bool Move(MOV_DIRECT direct);
	void AddNode(Point& np);
	void GetNextElem(MOV_DIRECT direct);
	void DeleteTail();
	void DrawToMap();
	void CleanMap();
	void CreateGift();
	void DeleteSnack();
	PointNode * CreatePointNode();
	int Lines;
	int Cols;
	int * Map;
	PointNode * SnackBody;
	Gift gift;
	int Score;
	int SnackLen;
	MOV_DIRECT LastDirect;
	bool HaveWall;
};

#endif // !_SNACK_H_
