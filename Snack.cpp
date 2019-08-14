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
	//snack will die when can't find a route and will become a circle
	Point phead = { 0 };
	Point pgift = { 0 };
	phead.line = this->SnackBody->next->point.line;
	phead.col = this->SnackBody->next->point.col;
	pgift.line = this->gift.point.line;
	pgift.col = this->gift.point.col;
	//自动寻路
	typedef struct
	{
		Point space;
		int findex;
	}FPoint;
	FPoint *MBuffer = (FPoint*)malloc(sizeof(FPoint)*this->Cols*this->Lines);
	int * MFlag = (int*)malloc(sizeof(int)*this->Cols*this->Lines);
	for (int i = 0; i < this->Cols*this->Lines; i++)
	{
		MFlag[i] = 0;
	}
	int MDirect[4][2] = { { -1, 0 }, { 0, 1 }, { 1, 0 }, { 0, -1 } }; // line col
	MOV_DIRECT MDMapping[4] = { MOV_UP, MOV_RIGHT, MOV_DOWN, MOV_LEFT };
	MBuffer[0].space.line = phead.line;
	MBuffer[0].space.col = phead.col;
	MBuffer[0].findex = -1;
	MFlag[0] = true;
	int MBufferLen = 1;
	int ProIndex = 0;
	bool FindGift = false;
	while (!FindGift)
	{
		for (int i = 0; i < 4; i++)
		{
			int rindex = MBuffer[ProIndex].space.line + MDirect[i][0];
			int cindex = MBuffer[ProIndex].space.col + MDirect[i][1];
			if (MFlag[(rindex)*(this->Cols) + (cindex)] == 0 && \
				(this->Map[(rindex)*(this->Cols) + (cindex)] == ELEM_VECANT || \
				this->Map[(rindex)*(this->Cols) + (cindex)] == ELEM_GIFT))
			{
				MBuffer[MBufferLen].space.line = rindex;
				MBuffer[MBufferLen].space.col = cindex;
				MBuffer[MBufferLen].findex = ProIndex;
				MFlag[(rindex)*(this->Cols) + (cindex)] = 1;
				if (MBuffer[MBufferLen].space.line == pgift.line && MBuffer[MBufferLen].space.col == pgift.col)
				{
					FindGift = true;
					break;
				}
				MBufferLen++;
			}
		}
		ProIndex++;
		if (ProIndex >= MBufferLen)
			break;
	}
	free(MFlag);
	MOV_DIRECT direct = MOV_NULL;
	if (FindGift==true)	//自动寻路成功开始映射方向
	{
		int PFindIndex = MBufferLen;
		while (MBuffer[PFindIndex].findex != 0)
		{
			PFindIndex = MBuffer[PFindIndex].findex;
		}
		int DC = MBuffer[PFindIndex].space.col - phead.col;
		int DL = MBuffer[PFindIndex].space.line - phead.line;
		for (int i = 0; i < 4; i++)
		{
			if (MDirect[i][0] == DL && MDirect[i][1] == DC)
			{
				direct = MDMapping[i];
				break;
			}
		}
	}
	else //解决自动寻路不成功后迂回政策
	{
		direct = this->LastDirect;

		GetNextElem(direct);
		int check = this->NextInfo.ne;
		if (check == ELEM_WALL || check == ELEM_SNACK || check == ELEM_SNACK_HEAD || check == ELEM_SNACK_TAIL)//当会死的时候，计算方向的空白数量，走向大的一方
		{
			if (direct == MOV_UP || direct == MOV_DOWN)
			{
				{
					int* BLFlag = (int *)malloc(sizeof(int)*this->Lines*this->Cols);
					int* BRFlag = (int *)malloc(sizeof(int)*this->Lines*this->Cols);
					Point * LFindQue = (Point*)malloc(sizeof(Point)*this->Lines*this->Cols);
					Point * RFindQue = (Point*)malloc(sizeof(Point)*this->Lines*this->Cols);
					for (int i = 0; i<this->Lines*this->Cols; i++)
					{
						BLFlag[i] = 0;
						BRFlag[i] = 0;
					}
					int BLLen = 2;
					int BRLen = 2;
					LFindQue[0].line = phead.line;
					LFindQue[0].col = phead.col;
					LFindQue[1].line = phead.line;
					LFindQue[1].col = phead.col - 1;
					BLFlag[(phead.line)*(this->Cols) + (phead.col)] = 1;
					RFindQue[0].line = phead.line;
					RFindQue[0].col = phead.col;
					RFindQue[1].line = phead.line;
					RFindQue[1].col = phead.col + 1;
					BRFlag[(phead.line)*(this->Cols) + (phead.col)] = 1;
					int TPLindex = 1;
					int TPRindex = 1;
					if ((this->Map[(LFindQue[1].line)*this->Cols + (LFindQue[1].col)] == ELEM_GIFT || this->Map[(LFindQue[1].line)*this->Cols + (LFindQue[1].col)] == ELEM_VECANT) &&\
						(this->Map[(RFindQue[1].line)*this->Cols + (RFindQue[1].col)] == ELEM_GIFT || this->Map[(RFindQue[1].line)*this->Cols + (RFindQue[1].col)] == ELEM_VECANT))
					{
						bool EndL = false;
						bool EndR = false;
						while (1)
						{
							for (int i = 0; i < 4; i++)
							{
								if (EndL == false)
								{
									int rlindex = LFindQue[TPLindex].line + MDirect[i][0];
									int clindex = LFindQue[TPLindex].col + MDirect[i][1];
									if (BLFlag[(rlindex)*(this->Cols) + (clindex)] == 0 && \
										(this->Map[(rlindex)*(this->Cols) + (clindex)] == ELEM_VECANT || \
										this->Map[(rlindex)*(this->Cols) + (clindex)] == ELEM_GIFT))
									{
										LFindQue[BLLen].line = rlindex;
										LFindQue[BLLen].col = clindex;
										BLFlag[(rlindex)*(this->Cols) + (clindex)] = 1;
										BLLen++;
									}
								}

								if (EndR == false)
								{
									int rrindex = RFindQue[TPRindex].line + MDirect[i][0];
									int crindex = RFindQue[TPRindex].col + MDirect[i][1];
									if (BRFlag[(rrindex)*(this->Cols) + (crindex)] == 0 && \
										(this->Map[(rrindex)*(this->Cols) + (crindex)] == ELEM_VECANT || \
										this->Map[(rrindex)*(this->Cols) + (crindex)] == ELEM_GIFT))
									{
										RFindQue[BRLen].line = rrindex;
										RFindQue[BRLen].col = crindex;
										BRFlag[(rrindex)*(this->Cols) + (crindex)] = 1;
										BRLen++;
									}
								}
							}
							TPLindex++;
							TPRindex++;
							if (TPLindex == BLLen)
								EndL = true;
							if (TPRindex == BRLen)
								EndR = true;
							if (EndL&&EndR == true)
								break;
						}
					}
					else
					{
						if (this->Map[(LFindQue[1].line)*this->Cols + (LFindQue[1].col)] == ELEM_SNACK || \
							this->Map[(LFindQue[1].line)*this->Cols + (LFindQue[1].col)] == ELEM_SNACK_HEAD || \
							this->Map[(LFindQue[1].line)*this->Cols + (LFindQue[1].col)] == ELEM_SNACK_TAIL || \
							this->Map[(LFindQue[1].line)*this->Cols + (LFindQue[1].col)] == ELEM_WALL)
							BLLen = 1;
						if (this->Map[(RFindQue[1].line)*this->Cols + (RFindQue[1].col)] == ELEM_SNACK || \
							this->Map[(RFindQue[1].line)*this->Cols + (RFindQue[1].col)] == ELEM_SNACK_HEAD || \
							this->Map[(RFindQue[1].line)*this->Cols + (RFindQue[1].col)] == ELEM_SNACK_TAIL || \
							this->Map[(RFindQue[1].line)*this->Cols + (RFindQue[1].col)] == ELEM_WALL)
							BRLen = 1;
					}
					free(BLFlag);
					free(BRFlag);
					free(LFindQue);
					free(RFindQue);
					if (BLLen>BRLen)
						direct = MOV_LEFT;
					else
						direct = MOV_RIGHT;
					

				}
				

			}
			else
			{

				{
					int* BUFlag = (int *)malloc(sizeof(int)*this->Lines*this->Cols);
					int* BDFlag = (int *)malloc(sizeof(int)*this->Lines*this->Cols);
					Point * UFindQue = (Point*)malloc(sizeof(Point)*this->Lines*this->Cols);
					Point * DFindQue = (Point*)malloc(sizeof(Point)*this->Lines*this->Cols);
					for (int i = 0; i<this->Lines*this->Cols; i++)
					{
						BUFlag[i] = 0;
						BDFlag[i] = 0;
					}
					int BULen = 2;
					int BDLen = 2;
					UFindQue[0].line = phead.line;
					UFindQue[0].col = phead.col;
					UFindQue[1].line = phead.line - 1;
					UFindQue[1].col = phead.col;
					BUFlag[(phead.line+1)*(this->Cols) + (phead.col)] = 1;
					DFindQue[0].line = phead.line;
					DFindQue[0].col = phead.col;
					DFindQue[1].line = phead.line + 1;
					DFindQue[1].col = phead.col;
					BDFlag[(phead.line-1)*(this->Cols) + (phead.col)] = 1;
					int TPUindex = 1;
					int TPDindex = 1;
					if ((this->Map[(UFindQue[1].line)*this->Cols + (UFindQue[1].col)] == ELEM_GIFT || this->Map[(UFindQue[1].line)*this->Cols + (UFindQue[1].col)] == ELEM_VECANT) && \
						(this->Map[(DFindQue[1].line)*this->Cols + (DFindQue[1].col)] == ELEM_GIFT || this->Map[(DFindQue[1].line)*this->Cols + (DFindQue[1].col)] == ELEM_VECANT))
					{
						bool EndU = false;
						bool EndD = false;
						while (1)
						{
							for (int i = 0; i < 4; i++)
							{
								if (EndU == false)
								{
									int ruindex = UFindQue[TPUindex].line + MDirect[i][0];
									int cuindex = UFindQue[TPUindex].col + MDirect[i][1];
									if (BUFlag[(ruindex)*(this->Cols) + (cuindex)] == 0 && \
										(this->Map[(ruindex)*(this->Cols) + (cuindex)] == ELEM_VECANT || \
										this->Map[(ruindex)*(this->Cols) + (cuindex)] == ELEM_GIFT))
									{
										UFindQue[BULen].line = ruindex;
										UFindQue[BULen].col = cuindex;
										BUFlag[(ruindex)*(this->Cols) + (cuindex)] = 1;
										BULen++;
									}
								}

								if (EndD == false)
								{
									int rdindex = DFindQue[TPDindex].line + MDirect[i][0];
									int cdindex = DFindQue[TPDindex].col + MDirect[i][1];
									if (BDFlag[(rdindex)*(this->Cols) + (cdindex)] == 0 && \
										(this->Map[(rdindex)*(this->Cols) + (cdindex)] == ELEM_VECANT || \
										this->Map[(rdindex)*(this->Cols) + (cdindex)] == ELEM_GIFT))
									{
										DFindQue[BDLen].line = rdindex;
										DFindQue[BDLen].col = cdindex;
										BDFlag[(rdindex)*(this->Cols) + (cdindex)] = 1;
										BDLen++;
									}
								}
							}
							TPUindex++;
							TPDindex++;
							if (TPUindex == BULen)
								EndU = true;
							if (TPDindex == BDLen)
								EndD = true;
							if (EndU&&EndD == true)
								break;
						}
					}
					else
					{
						if (this->Map[(UFindQue[1].line)*this->Cols + (UFindQue[1].col)] == ELEM_SNACK || \
							this->Map[(UFindQue[1].line)*this->Cols + (UFindQue[1].col)] == ELEM_SNACK_HEAD || \
							this->Map[(UFindQue[1].line)*this->Cols + (UFindQue[1].col)] == ELEM_SNACK_TAIL || \
							this->Map[(UFindQue[1].line)*this->Cols + (UFindQue[1].col)] == ELEM_WALL)
							BULen = 1;
						if (this->Map[(DFindQue[1].line)*this->Cols + (DFindQue[1].col)] == ELEM_SNACK || \
							this->Map[(DFindQue[1].line)*this->Cols + (DFindQue[1].col)] == ELEM_SNACK_HEAD || \
							this->Map[(DFindQue[1].line)*this->Cols + (DFindQue[1].col)] == ELEM_SNACK_TAIL || \
							this->Map[(DFindQue[1].line)*this->Cols + (DFindQue[1].col)] == ELEM_WALL)
							BDLen = 1;
					}
					free(BUFlag);
					free(BDFlag);
					free(UFindQue);
					free(DFindQue);
					if (BULen>BDLen)
						direct = MOV_UP;
					else
						direct = MOV_DOWN;
				}
				
			}
			
		}
		else	//解决下一步进入可避免的死胡同
		{
			int NextNerVcant = 0;
			for (int i = 0; i < 4; i++)
			{
				if (this->Map[(this->NextInfo.np.line + MDirect[i][0])*this->Cols + (this->NextInfo.np.col + MDirect[i][1])] == ELEM_VECANT)
					NextNerVcant++;
			}
			if (NextNerVcant >= 2)
			{
				
				int ups = 1,downs=1,lefts=1,rights=1;
				if (direct == MOV_UP)
				{
					while (this->Map[(phead.line)*this->Cols + (phead.col - lefts)] == ELEM_VECANT || \
						this->Map[(phead.line)*this->Cols + (phead.col - lefts)] == ELEM_GIFT)
						lefts++;
					while (this->Map[(phead.line)*this->Cols + (phead.col + rights)] == ELEM_VECANT || \
						this->Map[(phead.line)*this->Cols + (phead.col + rights)] == ELEM_GIFT)
						rights++;
					while (this->Map[(phead.line-ups)*this->Cols + (phead.col)] == ELEM_VECANT || \
						this->Map[(phead.line-ups)*this->Cols + (phead.col)] == ELEM_GIFT)
						ups++;
					if (ups > lefts)
					{
						if (ups > rights)
							direct = this->LastDirect;
						else
							direct = MOV_RIGHT;
					}
					else
					{
						if (lefts > rights)
							direct = MOV_LEFT;
						else
							direct = MOV_RIGHT;
					}
				}
				else if (direct == MOV_DOWN)
				{
					while (this->Map[(phead.line)*this->Cols + (phead.col - lefts)] == ELEM_VECANT || \
						this->Map[(phead.line)*this->Cols + (phead.col - lefts)] == ELEM_GIFT)
						lefts++;
					while (this->Map[(phead.line)*this->Cols + (phead.col + rights)] == ELEM_VECANT || \
						this->Map[(phead.line)*this->Cols + (phead.col + rights)] == ELEM_GIFT)
						rights++;
					while (this->Map[(phead.line + downs)*this->Cols + (phead.col)] == ELEM_VECANT || \
						this->Map[(phead.line + downs)*this->Cols + (phead.col)] == ELEM_GIFT)
						downs++;
					if (downs > lefts)
					{
						if (downs > rights)
							direct = this->LastDirect;
						else
							direct = MOV_RIGHT;
					}
					else
					{
						if (lefts > rights)
							direct = MOV_LEFT;
						else
							direct = MOV_RIGHT;
					}
				}
				else if (direct == MOV_LEFT)
				{
					while (this->Map[(phead.line)*this->Cols + (phead.col - lefts)] == ELEM_VECANT || \
						this->Map[(phead.line)*this->Cols + (phead.col - lefts)] == ELEM_GIFT)
						lefts++;
					while (this->Map[(phead.line-ups)*this->Cols + (phead.col )] == ELEM_VECANT || \
						this->Map[(phead.line-ups)*this->Cols + (phead.col )] == ELEM_GIFT)
						ups++;
					while (this->Map[(phead.line + downs)*this->Cols + (phead.col)] == ELEM_VECANT || \
						this->Map[(phead.line + downs)*this->Cols + (phead.col)] == ELEM_GIFT)
						downs++;
					if (downs > lefts)
					{
						if (downs > ups)
							direct = MOV_DOWN;
						else
							direct = MOV_UP;
					}
					else
					{
						if (lefts > ups)
							direct = this->LastDirect;
						else
							direct = MOV_UP;
					}
				}
				else if (direct == MOV_RIGHT)
				{
					while (this->Map[(phead.line)*this->Cols + (phead.col + rights)] == ELEM_VECANT || \
						this->Map[(phead.line)*this->Cols + (phead.col + rights)] == ELEM_GIFT)
						rights++;
					while (this->Map[(phead.line - ups)*this->Cols + (phead.col)] == ELEM_VECANT || \
						this->Map[(phead.line - ups)*this->Cols + (phead.col)] == ELEM_GIFT)
						ups++;
					while (this->Map[(phead.line + downs)*this->Cols + (phead.col)] == ELEM_VECANT || \
						this->Map[(phead.line + downs)*this->Cols + (phead.col)] == ELEM_GIFT)
						downs++;
					if (downs > rights)
					{
						if (downs > ups)
							direct = MOV_DOWN;
						else
							direct = MOV_UP;
					}
					else
					{
						if (rights > ups)
							direct = this->LastDirect;
						else
							direct = MOV_UP;
					}
				}
			}
			else
			{
				if (direct == MOV_UP || direct == MOV_DOWN)
				{
					if (this->Map[(this->NextInfo.np.line)*(this->Cols) + (this->NextInfo.np.col - 1)] != ELEM_GIFT&&\
						this->Map[(this->NextInfo.np.line)*(this->Cols) + (this->NextInfo.np.col - 1)] != ELEM_VECANT&&\
						this->Map[(this->NextInfo.np.line)*(this->Cols) + (this->NextInfo.np.col + 1)] != ELEM_GIFT&&\
						this->Map[(this->NextInfo.np.line)*(this->Cols) + (this->NextInfo.np.col + 1)] != ELEM_VECANT)
					{
						int left = 1, right = 1;
						while (this->Map[(phead.line)*this->Cols + (phead.col - left)] == ELEM_VECANT || \
							this->Map[(phead.line)*this->Cols + (phead.col - left)] == ELEM_GIFT)
							left++;
						while (this->Map[(phead.line)*this->Cols + (phead.col + right)] == ELEM_VECANT || \
							this->Map[(phead.line)*this->Cols + (phead.col + right)] == ELEM_GIFT)
							right++;

						if (left > right)
							direct = MOV_LEFT;
						else if (left < right)
							direct = MOV_RIGHT;
					}
				}
				else
				{
					if (this->Map[(this->NextInfo.np.line - 1)*(this->Cols) + (this->NextInfo.np.col)] != ELEM_GIFT&&\
						this->Map[(this->NextInfo.np.line - 1)*(this->Cols) + (this->NextInfo.np.col)] != ELEM_VECANT&&\
						this->Map[(this->NextInfo.np.line + 1)*(this->Cols) + (this->NextInfo.np.col)] != ELEM_GIFT&&\
						this->Map[(this->NextInfo.np.line + 1)*(this->Cols) + (this->NextInfo.np.col)] != ELEM_VECANT)
					{
						int up = 1, down = 1;
						while (this->Map[(phead.line - up)*this->Cols + (phead.col)] == ELEM_VECANT || \
							this->Map[(phead.line - up)*this->Cols + (phead.col)] == ELEM_GIFT)
							up++;
						while (this->Map[(phead.line + down)*this->Cols + (phead.col)] == ELEM_VECANT || \
							this->Map[(phead.line + down)*this->Cols + (phead.col)] == ELEM_GIFT)
							down++;
						if (up > down)
							direct = MOV_UP;
						else if (up < down)
							direct = MOV_DOWN;
					}
				}
			}
			
		}
	}
	free(MBuffer);
	
	//解决计算出的方向和现在运行方向冲突
	if (direct + this->LastDirect == MOV_DOWN + MOV_UP)
	{
		int left = 1, right = 1;
		while (this->Map[(phead.line)*(this->Cols) + (phead.col - left)] == ELEM_VECANT\
			||this->Map[(phead.line)*(this->Cols) + (phead.col - left)] == ELEM_GIFT)
			left++;
		while (this->Map[(phead.line)*(this->Cols) + (phead.col + right)] == ELEM_VECANT \
			||this->Map[(phead.line)*(this->Cols) + (phead.col + right)] == ELEM_GIFT)
			right++;
		if (left > right)
			direct = MOV_LEFT;
		else
			direct = MOV_RIGHT;
	}
	else if (direct + this->LastDirect == MOV_LEFT+MOV_RIGHT)
	{
		int up = 1, down = 1;
		while (this->Map[(phead.line - up)*(this->Cols) + (phead.col)] == ELEM_VECANT\
			||this->Map[(phead.line - up)*(this->Cols) + (phead.col)] == ELEM_GIFT)
			up++;
		while (this->Map[(phead.line + down)*(this->Cols) + (phead.col)] == ELEM_VECANT \
			||this->Map[(phead.line + down)*(this->Cols) + (phead.col)] == ELEM_GIFT)
			down++;
		if (up > down)
			direct = MOV_UP;
		else
			direct = MOV_DOWN;
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
	if (direct == MOV_UP || direct == MOV_DOWN || direct == MOV_LEFT || direct == MOV_RIGHT)
	{
		if (direct + this->LastDirect != MOV_DOWN + MOV_UP && direct + this->LastDirect != MOV_LEFT + MOV_RIGHT)
			this->LastDirect = direct;
	}
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