#include"Snack.h"

Snack::Snack(int lines, int cols,bool haveWall)
{
	srand((unsigned int)time(NULL));
	this->Lines = lines + 2;
	this->Cols = cols + 2;
	this->Map = (int *)malloc(sizeof(int)*this->Lines*this->Cols);
	CleanMap();
	this->SnackBody = CreatePointNode();
	this->SnackBody->next = CreatePointNode();
	this->SnackBody->next->point.line = this->Lines / 2;
	this->SnackBody->next->point.col = this->Cols / 2;
	this->Score = 0;
	this->SnackLen=1;
	this->HaveWall = haveWall;
	LastDirect = MOV_RIGHT;
	CreateGift();
	DrawToMap();
}

Snack::~Snack()
{
	free(Map);
	DeleteSnack();
}
bool Snack::IsNormalGift()
{
	return (this->gift.reward==1?true:false);
}
bool Snack::AutoRun()
{
	MOV_DIRECT direct;
	Point phead = { 0 };
	Point pgift = { 0 };
	phead.line = this->SnackBody->next->point.line;
	phead.col = this->SnackBody->next->point.col;
	pgift.line = this->gift.point.line;
	pgift.col = this->gift.point.col;
	if (pgift.line == phead.line)
	{
		if (pgift.col > phead.col)
			direct = MOV_RIGHT;
		else
			direct = MOV_LEFT;
	}
	else if (pgift.col == phead.col)
	{
		if (pgift.line > phead.line)
			direct = MOV_DOWN;
		else
			direct = MOV_UP;
	}
	else
		direct = this->LastDirect;

	GetNextElem(direct);
	int check = this->NextInfo.ne;
	if (check == ELEM_WALL || check == ELEM_SNACK || check == ELEM_SNACK_HEAD || check == ELEM_SNACK_TAIL)
	{
		if (direct == MOV_UP || direct == MOV_DOWN)
		{
			int left = 1, right = 1;
			while (this->Map[(phead.line)*(this->Cols) + (phead.col - left)] != ELEM_WALL\
				&&this->Map[(phead.line)*(this->Cols) + (phead.col - left)] != ELEM_SNACK\
				&&this->Map[(phead.line)*(this->Cols) + (phead.col - left)] != ELEM_SNACK_HEAD\
				&&this->Map[(phead.line)*(this->Cols) + (phead.col - left)] != ELEM_SNACK_TAIL)
				left++;
			while (this->Map[(phead.line)*(this->Cols) + (phead.col + right)] != ELEM_WALL \
				&&this->Map[(phead.line)*(this->Cols) + (phead.col + right)] != ELEM_SNACK\
				&&this->Map[(phead.line)*(this->Cols) + (phead.col + right)] != ELEM_SNACK_HEAD\
				&&this->Map[(phead.line)*(this->Cols) + (phead.col + right)] != ELEM_SNACK_TAIL)
				right++;
			if (left>right)
				direct = MOV_LEFT;
			else
				direct = MOV_RIGHT;
		}
		else
		{
			int up = 1, down = 1;
			while (this->Map[(phead.line - up)*(this->Cols) + (phead.col)] != ELEM_WALL\
				&&this->Map[(phead.line - up)*(this->Cols) + (phead.col)] != ELEM_SNACK\
				&&this->Map[(phead.line - up)*(this->Cols) + (phead.col)] != ELEM_SNACK_HEAD\
				&&this->Map[(phead.line - up)*(this->Cols) + (phead.col)] != ELEM_SNACK_TAIL)
				up++;
			while (this->Map[(phead.line + down)*(this->Cols) + (phead.col)] != ELEM_WALL \
				&&this->Map[(phead.line + down)*(this->Cols) + (phead.col)] != ELEM_SNACK\
				&&this->Map[(phead.line + down)*(this->Cols) + (phead.col)] != ELEM_SNACK_HEAD\
				&&this->Map[(phead.line + down)*(this->Cols) + (phead.col)] != ELEM_SNACK_TAIL)
				down++;
			if (down>up)
				direct = MOV_DOWN;
			else
				direct = MOV_UP;
		}
	}
	return Run(direct);
}
void Snack::GetNextElem(MOV_DIRECT direct)
{
	Point NextStep = { 0 };
	NextStep.line = this->SnackBody->next->point.line;
	NextStep.col = this->SnackBody->next->point.col;
	switch (direct)
	{
	case MOV_UP:
	{
				   NextStep.line--;
				   if (!this->HaveWall)
				   {
					   if (NextStep.line == 0)
						   NextStep.line = this->Lines - 2;
				   }
				   break;
	}
	case MOV_DOWN:
	{
					 NextStep.line++;
					 if (!this->HaveWall)
					 {
						 if (NextStep.line == this->Lines - 1)
							 NextStep.line = 1;
					 }
					 break;
	}
	case MOV_LEFT:
	{
					 NextStep.col--;
					 if (!this->HaveWall)
					 {
						 if (NextStep.col == 0)
							 NextStep.col = this->Cols - 2;
					 }
					 break;
	}
	case MOV_RIGHT:
	{
					  NextStep.col++;
					  if (!this->HaveWall)
					  {
						  if (NextStep.col == this->Cols - 1)
							  NextStep.col = 1;
					  }
					  break;
	}
	}
	this->NextInfo.ne = (this->Map[(NextStep.line)*(this->Cols) + (NextStep.col)]);
	this->NextInfo.np.line = NextStep.line;
	this->NextInfo.np.col = NextStep.col;
}
void Snack::DeleteSnack()
{
	PointNode  *fpnode = this->SnackBody, *pnode = this->SnackBody->next;
	this->SnackBody = NULL;
	while (pnode)
	{
		free(fpnode);
		fpnode = pnode;
		pnode = pnode->next;
	}
}
void Snack::Clear()
{
	CleanMap();
	this->SnackBody = CreatePointNode();
	this->SnackBody->next = CreatePointNode();
	this->SnackBody->next->point.line = this->Lines / 2;
	this->SnackBody->next->point.col = this->Cols / 2;
	this->Score = 0;
	this->SnackLen = 1;
	LastDirect = MOV_RIGHT;
	CreateGift();
	DrawToMap();
}
int Snack::GetScore()
{
	return this->Score;
}
bool Snack::Run(MOV_DIRECT direct)
{
	bool ret =Move(direct);
	DrawToMap();
	return ret;
}
bool Snack::Move(MOV_DIRECT direct)
{
	if (direct!=MOV_NULL && direct + this->LastDirect != MOV_DOWN + MOV_UP && direct + this->LastDirect != MOV_LEFT + MOV_RIGHT)
		this->LastDirect = direct;
	GetNextElem(this->LastDirect);
	int check = this->NextInfo.ne;
	if (check == ELEM_WALL || check == ELEM_SNACK||check==ELEM_SNACK_HEAD||check==ELEM_SNACK_TAIL)
		return false;
	if (check == ELEM_GIFT)
	{
		this->Score += this->gift.reward;
		this->SnackLen++;
		AddNode(this->NextInfo.np);
		CreateGift();
		return true;
	}
	if (check==ELEM_VECANT)
	{
		AddNode(this->NextInfo.np);
		DeleteTail();
		return true;
	}
	return true;
}
int Snack::GetSnackLen()
{
	return this->SnackLen;
}
void Snack::DeleteTail()
{
	PointNode * fpnode = this->SnackBody, *pnode = this->SnackBody->next;
	while (pnode)
	{
		if (pnode->next == NULL)
			break;
		pnode = pnode->next;
		fpnode = fpnode->next;
	}
	free(pnode);
	fpnode->next = NULL;
}
void Snack::AddNode(Point& np)
{
	PointNode * tp = CreatePointNode();
	tp->point.line = np.line;
	tp->point.col = np.col;
	tp->next = this->SnackBody->next;
	this->SnackBody->next = tp;
}
void Snack::DrawToMap()
{
	CleanMap();
	PointNode * tp = this->SnackBody->next;
	while (tp)
	{
		if (tp == this->SnackBody->next)
			this->Map[(tp->point.line)*(this->Cols) + (tp->point.col)] = ELEM_SNACK_HEAD;
		else if (tp->next == NULL)
			this->Map[(tp->point.line)*(this->Cols) + (tp->point.col)] = ELEM_SNACK_TAIL;
		else
			this->Map[(tp->point.line)*(this->Cols)+(tp->point.col)] = ELEM_SNACK;
		tp = tp->next;
	}
	this->Map[(this->gift.point.line)*(this->Cols) + (this->gift.point.col)] = ELEM_GIFT;
}
void Snack::CleanMap()
{
	for (int i = 0; i < this->Lines; i++)
	{
		for (int j = 0; j < this->Cols; j++)
		{
			this->Map[i*this->Cols + j] = ELEM_VECANT;
		}
	}
	for (int i = 0; i < this->Lines; i++)
	{
		this->Map[i*this->Cols + 0] = 1;
		this->Map[i*this->Cols + this->Cols - 1] = ELEM_WALL;
	}
	for (int j = 0; j < this->Cols; j++)
	{
		this->Map[0 * this->Cols + j] = 1;
		this->Map[(this->Lines - 1)*this->Cols + j] = ELEM_WALL;
	}
}
int Snack::GetLines()
{
	return this->Lines - 2;
}
int Snack::GetCols()
{
	return this->Cols - 2;
}
ELEM_TYPE & Snack::MapAt(int rindex, int cindex)
{
	if (rindex < 0 || cindex < 0 || rindex >= this->Lines - 2 || cindex >= this->Cols - 2)
	{
		ELEM_TYPE a = -1;
		return a;
	}
	return this->Map[(rindex + 1)*this->Cols + (cindex + 1)];
}
void Snack::CreateGift()
{
	while (1)
	{
		gift.point.line = rand() % (this->Lines - 2) + 1;
		gift.point.col = rand() % (this->Cols - 2) + 1;
		if (this->Map[(gift.point.line)*(this->Cols) + (gift.point.col)] == ELEM_VECANT)
			break;
	}
	gift.reward = 1;
	if (rand() % 100 < 33)
		gift.reward = rand() % 10 + 2;
}
PointNode * Snack::CreatePointNode()
{
	PointNode * p = (PointNode *)malloc(sizeof(PointNode));
	p->next = NULL;
	p->point.line = 0;
	p->point.col = 0;
	return p;
}