#include <time.h>
#include <stdlib.h>
#include "wctrlbaoshi.h"
//#include <stdio.h>
enum
{	//WC  WIN CTRL
	BSCTRL_INITSIGN,			//初始化标志
	BSCTRL_PLAYTIMES,			//游戏次数
	BSCTRL_BETOUTTAIL,		//抽水用
	BSCTRL_BASECOIN,			//净分
	BSCTRL_AIMCOIN,			//杀送分目标

	BSCTRL_NOWRANGE,          //实际输赢
	BSCTRL_BASECOIN2,			//净分备份
	BSCTRL_ST,				//控制状态
	BSCTRL_USECZGX,			//保存玩家预支的充值贡献
	BSCTRL_EVMAXBET,			//玩家一段时间内近似平均押分

	BSCTRL_BASECOIN1,			//净分备份

	BSCTRL_BONUSBASE,			//奖池值，过关抽奖根据奖池值来
	BSCTRL_USECZGXSIGN,		//使用控制净分标志
	BSCTRL_SFBASECOIN,		//充值贡献起作用时，玩家的输赢值
	BSCTRL_SFSIGN,			//充值贡献是否在送分的标志
	BSCTRL_SFINITSIGN,		//初始化标志
};


static int NIUCHOICENUM = 5;
static int NIUGAMEWINRATE = 10;
static int REDBLACKBONUSRATE = 80;
static int BSSET_MAXGAMEWAVEUP = 10000000;
static int BSSET_MAXGAMEWAVEDOWN = 30000000;


static int BSSET_UPMAXBETLV = 2000;
static int BSSET_DOWNMAXBETLV = 3000;
static int BSSET_MAXWIN = 8000000;
static int BSSET_BETOUTRATE = 20;


static int BSSET_BONUSOUTRATE = 25;

static int set_time_rand_sign = 0;
static int set_rand_numrange_sign = 0;

void BSSetGameInfo(int setarray[])
{
	BSSET_UPMAXBETLV = setarray[BSSET_UPMAXBETLV_ID];
	BSSET_DOWNMAXBETLV = setarray[BSSET_DOWNMAXBETLV_ID];
	BSSET_MAXWIN = setarray[BSSET_MAXWIN_ID];
	BSSET_BETOUTRATE = setarray[BSSET_BETOUTRATE_ID];
	BSSET_BONUSOUTRATE = setarray[BSSET_BONUSOUTRATE_ID];
	BSSET_MAXGAMEWAVEUP = setarray[BSSET_MAXGAMEWAVEUP_ID];
	BSSET_MAXGAMEWAVEDOWN = setarray[BSSET_MAXGAMEWAVEDOWN_ID];
}


static int GetRanNum(void)
{
	int tempnum = 0;
	int times = 0;

	if (set_time_rand_sign == 0)
	{
		int i;
		srand((int)time(0));
		set_time_rand_sign = 1;

		for (i = 0; i < 100; i++)
		{
			if (rand() >= 1000000)
			{
				set_rand_numrange_sign = 1;
				break;
			}
		}
	}

	tempnum = rand();

	if (set_rand_numrange_sign > 0)
	{
		tempnum = tempnum % 30000;
	}
	else
	{
		times = 0;
		while (tempnum >= 30000)
		{
			times++;
			tempnum = rand();

			if (times >= 100)
			{
				tempnum = tempnum % 30000;
				break;
			}
		}

	}

	return tempnum;
}

static void NiuSetGameinfo(int setarray[])
{
	//NIUGAMEWINRATE = setarray[BRNIU_SET_BETOUTRATE];
	//RBMAXGAMEWAVEUP = setarray[BRNIU_SET_GAMEWAVEUP];
	//RBMAXGAMEWAVEDOWN = setarray[BRNIU_SET_GAMEWAVEDOWN];
}

int BSGetPaybackRate(int type, int num)
{
	int paybackarray[] =
	{
		1, 2, 10, 50, 100, 200, 400, 600, 1000, 2000, 5000, 10000,
		2, 4, 20, 100, 200, 400, 800, 1200, 2000, 4000, 10000, 20000,
		4, 8, 40, 200, 400, 800, 1600, 2400, 4000, 8000, 20000, 40000,
		8, 16, 80, 400, 800, 1600, 3200, 4800, 8000, 16000, 40000, 80000,
		16, 32, 160, 800, 1600, 3200, 6400, 9600, 16000, 32000, 80000, 160000,
	};
	int onetypenum = 12;
	int id = 0;
	
	if (num > 15)
	{
		num = 15;
	}
	type = type - 1;
	id = type*onetypenum + num - 4;

	//if (id >= 60 || id < 0)
	//{
	//	id = id;
	//}

	return paybackarray[id];
}

static int BSGetLinkNum(int array[], int rownum, int startnum)
{
	int num = 0;
	int numleft = 0;
	int numright = 0;
	int numdown = 0;
	int numup = 0;

	if (array[startnum] > 0)
	{
		num = 1;
		array[startnum] = 0;

		if (startnum%rownum > 0 && array[startnum - 1] > 0)
		{
			numleft = BSGetLinkNum(array, rownum, startnum - 1);
		}
		if (startnum%rownum < rownum - 1 && array[startnum + 1] > 0)
		{
			numright = BSGetLinkNum(array, rownum, startnum + 1);
		}
		if (startnum / rownum < rownum - 1 && array[startnum + rownum] > 0)
		{
			numdown = BSGetLinkNum(array, rownum, startnum + rownum);
		}
		if (startnum / rownum > 0 && array[startnum - rownum] > 0)
		{
			numup = BSGetLinkNum(array, rownum, startnum - rownum);
		}



		return num + numdown + numleft + numright;
	}

	return 0;

}

static void BSDelOnekind(int array[], int rownum,int bssign,int startnum)
{
	int num = 0;
	int numleft = 0;
	int numright = 0;
	int numdown = 0;
	int numup = 0;

	if (array[startnum] == bssign)
	{
		array[startnum] = 0;

		if (startnum%rownum > 0 && array[startnum - 1] == bssign)
		{
			BSDelOnekind(array, rownum, bssign, startnum-1);
		}
		if (startnum%rownum < rownum - 1 && array[startnum + 1] == bssign)
		{
			BSDelOnekind(array, rownum, bssign, startnum + 1);
		}
		if (startnum / rownum < rownum - 1 && array[startnum + rownum] == bssign)
		{
			BSDelOnekind(array, rownum, bssign, startnum + rownum );
		}
		if (startnum / rownum > 0 && array[startnum - rownum] == bssign)
		{
			BSDelOnekind(array, rownum, bssign, startnum - rownum);
		}
	}
}
//bsresult[] 0表示组数，后面跟着一对对的结果
void BSGetOneresult(int card[], int rownum, int bsresult[])
{
	int i,j;
	int cardsign[36];
	int numsign = 0;
	int aimpos = 0;
	int aimnum = 0;
	int cardnum = rownum*rownum;


	for (i = 0; i < 5;i++)
	{
		for (j = 0; j < cardnum;j++)
		{
			if (card[j] == i+1)
				cardsign[j] = 1;
			else
				cardsign[j] = 0;
		}

		do 
		{
			numsign = 0;
			for (j = 0; j < cardnum; j++)
			{
				if (cardsign[j] == 1)
				{
					numsign = 1;
					aimpos = j;
					break;
				}
			}

			if (numsign > 0)
			{
				aimnum = BSGetLinkNum(cardsign, rownum, aimpos);
				if (aimnum > 3)
				{
					bsresult[0]++;
					bsresult[3 * bsresult[0] - 2] = i+1;           //宝石类型
					bsresult[3 * bsresult[0]-1] = aimnum;        //数量
					bsresult[3 * bsresult[0]] = aimpos;          //起始位置
				}
			}
		} 
		while (numsign > 0);

	}
}

int BSGetOneIcon(long long winflg,int level)
{
	int iconrate[3][3][5] = {
		110, 110, 200, 265, 200,
		105, 115, 205, 295, 200,
		110, 110, 200, 265, 200,

		160, 175, 225, 220, 210,		
		160, 160, 230, 220, 220,
		160, 175, 225, 220, 210,		
		
		200, 200, 200, 200, 170,
		180, 200, 200, 200, 190,
		200, 200, 200, 200, 170,
	};
	int usingrate[5];
	int i;
	int randnum;

	//if (winflg>2 || level>2 || level < 0 || winflg < 0)
	//{
	//	randnum = 0;
	//}

	if (winflg > 2 || level > 2)
	{
		winflg = 2;
		level = 2;
	}

	for (i = 0; i < 5; i++)
		usingrate[i] = iconrate[level][winflg][i];

	for (i = 1; i < 5;i++)
	{
		usingrate[i] = usingrate[i] + usingrate[i - 1];
	}
	randnum = GetRanNum() % usingrate[4];
	for (i = 0; i < 5;i++)
	{
		if (randnum < usingrate[i])
		{
			return i + 1;
		}
	}

	return 1;
}

void BSDelDiamond(int card[],int rownum, int bsresult[])
{
	int i;

	for (i = 0; i < bsresult[0];i++)
	{
		BSDelOnekind(card, rownum, bsresult[3 * i + 1], bsresult[3 * i + 3]);
	}
}

void BSInitArray(int array[], int num)
{
	int i;
	for (i = 0; i < num;i++)
	{
		array[i] = 0;
	}
}

void BSFallDown(int bsresult[],int rownum)
{
	int i,j;


	for (i = 0; i < rownum*rownum;i++)
	{
		if (bsresult[i] == 0)
		{
			for (j = 1; i + j*rownum < rownum*(rownum+1); j++)
			{
				if (bsresult[i + j*rownum] > 0)
				{
					bsresult[i] = bsresult[i + j*rownum];
					bsresult[i + j*rownum] = 0;
					break;
				}
			}
		}
	}
}

void BSGetFisrtResult(long long winsign, int level,int bsresult[])
{
	int randnum = GetRanNum() % 3;
	int i;
	int rownum = level + 4;
	int j;
	int gameresult[50];
	int allpayback = 0;
	int winnumchoice[3][3] = {
		16,16,16,
		16,19,16,
		16,16,16,
	};

	int limitwinnum;
	int controlsign = 0;

	if (winsign == 3)
	{
		winsign = 2;
		controlsign = 1;
		if (GetRanNum() % 4 < 3)
		{
			randnum = 1;
		}
		else
			randnum = 2;
	}
		


	if (winsign == 10)
	{
		if (GetRanNum() % 2 < 1)
		{
			for (j = 0; j < 100; j++)
			{
				for (i = 0; i < rownum*(rownum + 1); i++)
				{
					bsresult[i] = BSGetOneIcon(1, level);
				}
				gameresult[0] = 0;
				BSGetOneresult(bsresult, rownum, gameresult);
				if (gameresult[0] == 0)
				{
					break;
				}
			}
		}
		else
		{
			for (j = 0; j < 100; j++)
			{
				for (i = 0; i < rownum*(rownum + 1); i++)
				{
					bsresult[i] = BSGetOneIcon(1, level);
				}
				gameresult[0] = 0;
				BSGetOneresult(bsresult, rownum, gameresult);
				allpayback = 0;
				for (i = 0; i < gameresult[0]; i++)
					allpayback += BSGetPaybackRate(gameresult[i * 3 + 1], gameresult[i * 3 + 2]);
				if (allpayback > 99 && allpayback < 400)
				{
					break;
				}
			}
		}
		return;
	}
	if (winsign > 2 || level > 2)
	{
		winsign = 2;
		level = 2;
	}
	limitwinnum = winnumchoice[level][winsign];


	if (randnum < 2)
	{
		for (j = 0; j < 100;j++)
		{
			for (i = 0; i < rownum*(rownum + 1); i++)
			{
				bsresult[i] = BSGetOneIcon(winsign, level);
			}
			gameresult[0] = 0;
			BSGetOneresult(bsresult, rownum, gameresult);
			if (gameresult[0] == 0)
			{
				break;
			}
		}
	}
	else
	{
		if (GetRanNum()%3==1)
		{
			for (j = 0; j < 200; j++)
			{
				for (i = 0; i < rownum*(rownum + 1); i++)
				{
					bsresult[i] = BSGetOneIcon(winsign, level);
				}
				gameresult[0] = 0;
				BSGetOneresult(bsresult, rownum, gameresult);
				allpayback = 0;
				for (i = 0; i < gameresult[0]; i++)
					allpayback += BSGetPaybackRate(gameresult[i * 3 + 1], gameresult[i * 3 + 2]);
				if (gameresult[0] > 0 && allpayback < 32500)
				{
					if (winsign > 1 && allpayback > 4000)
					{
					}
					else if (winsign > 1 && allpayback > 1000 && GetRanNum() % 10 < 7)
					{
					}
					else if (winsign == 0 && allpayback > 6000)
					{
					}
					else if (winsign == 0 && allpayback > 1000 && GetRanNum() % 5 < 3)
					{
					}
					else if (controlsign > 0 && allpayback > 190)
					{
					}
					else
						break;
				}
			}
		}
		else
		{
			for (j = 0; j < 200; j++)
			{
				for (i = 0; i < rownum*(rownum + 1); i++)
				{
					bsresult[i] = BSGetOneIcon(winsign, level);
				}
				gameresult[0] = 0;
				allpayback = 0;
				BSGetOneresult(bsresult, rownum, gameresult);
				for (i = 0; i < gameresult[0]; i++)
					allpayback += BSGetPaybackRate(gameresult[i * 3 + 1], gameresult[i * 3 + 2]);
				if (allpayback > limitwinnum && allpayback < 32500)
				{
					if (winsign > 1 && allpayback > 4000)
					{
					}
					else if (winsign > 1 && allpayback > 1000 && GetRanNum() % 10 <7)
					{
					}
					else if (winsign == 0 && allpayback > 6000)
					{
					}
					else if (winsign == 0 && allpayback > 1000 && GetRanNum() % 5 < 3)
					{
					}
					else if (controlsign > 0 && allpayback > 190)
					{
					}
					else
						break;
				}
			}
		}
	}
}

static void BSGetOneResultCZGX(int bsresult[], int level)
{
	int i,j;
	int rownum = level + 4;
	int gameresult[60];
	int limitwinnum = 600;
	int maxwinnum = 5000;
	int allpayback = 0;

	for (j = 0; j < 1000;j++)
	{
		for (i = 0; i < rownum*(rownum + 1); i++)
		{
			bsresult[i] = BSGetOneIcon(1, level);
		}
		gameresult[0] = 0;
		allpayback = 0;
		BSGetOneresult(bsresult, rownum, gameresult);
		for (i = 0; i < gameresult[0]; i++)
			allpayback += BSGetPaybackRate(gameresult[i * 3 + 1], gameresult[i * 3 + 2]);
		if (allpayback > limitwinnum && allpayback < maxwinnum)
		{
			break;
		}
	}

}

static void BSGetOneResultCZGX2(int bsresult[], int level)
{
	int i, j;
	int rownum = level + 4;
	int gameresult[60];
	int limitwinnum = 600;
	int maxwinnum = 1200;
	int allpayback = 0;

	for (j = 0; j < 1000; j++)
	{
		for (i = 0; i < rownum*(rownum + 1); i++)
		{
			bsresult[i] = BSGetOneIcon(1, level);
		}
		gameresult[0] = 0;
		allpayback = 0;
		BSGetOneresult(bsresult, rownum, gameresult);
		for (i = 0; i < gameresult[0]; i++)
			allpayback += BSGetPaybackRate(gameresult[i * 3 + 1], gameresult[i * 3 + 2]);
		if (allpayback > limitwinnum && allpayback < maxwinnum)
		{
			break;
		}
	}

}

void BSGetOneResultCZGX100(int bsresult[], int level)
{
	int i, j;
	int rownum = level + 4;
	int gameresult[60];
	int limitwinnum = 3000;
	int maxwinnum = 10000;
	int allpayback = 0;

	for (j = 0; j < 1000; j++)
	{
		for (i = 0; i < rownum*(rownum + 1); i++)
		{
			bsresult[i] = BSGetOneIcon(1, level);
		}
		gameresult[0] = 0;
		allpayback = 0;
		BSGetOneresult(bsresult, rownum, gameresult);
		for (i = 0; i < gameresult[0]; i++)
			allpayback += BSGetPaybackRate(gameresult[i * 3 + 1], gameresult[i * 3 + 2]);
		if (allpayback > limitwinnum && allpayback < maxwinnum)
		{
			//return allpayback;
			break;
		}
	}

}

void BSGetOneResultCZGX300(int bsresult[], int level)
{
	int i, j;
	int rownum = level + 4;
	int gameresult[60];
	int limitwinnum = 20000;
	int maxwinnum = 30000;
	int allpayback = 0;

	for (j = 0; j < 1000; j++)
	{
		for (i = 0; i < rownum*(rownum + 1); i++)
		{
			bsresult[i] = BSGetOneIcon(1, level);
		}
		gameresult[0] = 0;
		allpayback = 0;
		BSGetOneresult(bsresult, rownum, gameresult);
		for (i = 0; i < gameresult[0]; i++)
			allpayback += BSGetPaybackRate(gameresult[i * 3 + 1], gameresult[i * 3 + 2]);
		if (allpayback > limitwinnum && allpayback < maxwinnum)
		{
			break;
		}
	}

}

int BSGetOneGameResult2(long long playerinfo[], int bet, int level)
{
	int i;
	int bsresult[50];
	int gameresult[20];
	int allpayback = 0;
	int rownum = 4+level;

	BSInitArray(bsresult, 50);
	BSInitArray(gameresult, 20);

	for (i = 0; i < rownum*(rownum+1);i++)
	{
		bsresult[i] = BSGetOneIcon(playerinfo[BSCTRL_ST], level);
	}
	//printf("%d %d %d %d %d %d\n", bsresult[36], bsresult[37], bsresult[38], bsresult[39], bsresult[40], bsresult[41]);
	//printf("%d %d %d %d %d %d\n", bsresult[30], bsresult[31], bsresult[32], bsresult[33], bsresult[34], bsresult[35]);
	//printf("%d %d %d %d %d %d\n",  bsresult[24],bsresult[25], bsresult[26], bsresult[27], bsresult[28], bsresult[29]);
	//printf("%d %d %d %d %d %d\n", bsresult[18], bsresult[19],bsresult[20], bsresult[21], bsresult[22], bsresult[23]);
	//printf("%d %d %d %d %d %d\n",bsresult[12], bsresult[13], bsresult[14],bsresult[15], bsresult[16], bsresult[17]);
	//printf("%d %d %d %d %d %d\n", bsresult[6], bsresult[7], bsresult[8], bsresult[9], bsresult[10], bsresult[11]);
	//printf("%d %d %d %d %d %d\n", bsresult[0], bsresult[1], bsresult[2], bsresult[3], bsresult[4],bsresult[5]);
	//getchar();

	BSGetOneresult(bsresult, rownum, gameresult);
	for (i = 0; i < gameresult[0]; i++)
		allpayback += BSGetPaybackRate(gameresult[i * 3 + 1], gameresult[i * 3 + 2]);
	if (gameresult[0] > 0)
	{
		if ((playerinfo[BSCTRL_ST] == 0 && level == 2 && GetRanNum() % 2 == 1) || (playerinfo[BSCTRL_ST] == 2 && level == 2 && GetRanNum() % 5 < 4))
		{
			allpayback = 0;
			BSInitArray(gameresult, 20);
			for (i = 0; i < rownum*(rownum + 1); i++)
			{
				bsresult[i] = BSGetOneIcon(playerinfo[BSCTRL_ST], level);
			}
			BSGetOneresult(bsresult, rownum, gameresult);
			for (i = 0; i < gameresult[0]; i++)
				allpayback += BSGetPaybackRate(gameresult[i * 3 + 1], gameresult[i * 3 + 2]);
		}
		//printf("\n\n");
		if (gameresult[0] > 0)
			BSDelDiamond(bsresult, rownum, gameresult);
		//for (i = 0; i < 3 * gameresult[0] + 1;i++)
		//{
		//	printf("%d \t", gameresult[i]);
		//}
		//printf("\n");
		//printf("%d\n", allpayback);
		//printf("\n\n");
	}
	//printf("%d %d %d %d %d %d\n", bsresult[36], bsresult[37], bsresult[38], bsresult[39], bsresult[40], bsresult[41]);
	//printf("%d %d %d %d %d %d\n", bsresult[30], bsresult[31], bsresult[32], bsresult[33], bsresult[34], bsresult[35]);
	//printf("%d %d %d %d %d %d\n", bsresult[24], bsresult[25], bsresult[26], bsresult[27], bsresult[28], bsresult[29]);
	//printf("%d %d %d %d %d %d\n", bsresult[18], bsresult[19], bsresult[20], bsresult[21], bsresult[22], bsresult[23]);
	//printf("%d %d %d %d %d %d\n", bsresult[12], bsresult[13], bsresult[14], bsresult[15], bsresult[16], bsresult[17]);
	//printf("%d %d %d %d %d %d\n", bsresult[6], bsresult[7], bsresult[8], bsresult[9], bsresult[10], bsresult[11]);
	//printf("%d %d %d %d %d %d\n", bsresult[0], bsresult[1], bsresult[2], bsresult[3], bsresult[4], bsresult[5]);
	//getchar();
	while (gameresult[0] > 0)
	{
		
		BSFallDown(bsresult, rownum);
		//printf("%d %d %d %d %d %d\n", bsresult[36], bsresult[37], bsresult[38], bsresult[39], bsresult[40], bsresult[41]);
		//printf("%d %d %d %d %d %d\n", bsresult[30], bsresult[31], bsresult[32], bsresult[33], bsresult[34], bsresult[35]);
		//printf("%d %d %d %d %d %d\n", bsresult[24], bsresult[25], bsresult[26], bsresult[27], bsresult[28], bsresult[29]);
		//printf("%d %d %d %d %d %d\n", bsresult[18], bsresult[19], bsresult[20], bsresult[21], bsresult[22], bsresult[23]);
		//printf("%d %d %d %d %d %d\n", bsresult[12], bsresult[13], bsresult[14], bsresult[15], bsresult[16], bsresult[17]);
		//printf("%d %d %d %d %d %d\n", bsresult[6], bsresult[7], bsresult[8], bsresult[9], bsresult[10], bsresult[11]);
		//printf("%d %d %d %d %d %d\n", bsresult[0], bsresult[1], bsresult[2], bsresult[3], bsresult[4], bsresult[5]);
		//getchar();
		BSInitArray(gameresult, 20);
		for (i = 0; i < rownum*(rownum + 1);i++)
		{
			if (bsresult[i] == 0)
			{
				bsresult[i] = BSGetOneIcon(playerinfo[BSCTRL_ST], level);
			}
		}
		//printf("%d %d %d %d %d %d\n", bsresult[36], bsresult[37], bsresult[38], bsresult[39], bsresult[40], bsresult[41]);
		//printf("%d %d %d %d %d %d\n", bsresult[30], bsresult[31], bsresult[32], bsresult[33], bsresult[34], bsresult[35]);
		//printf("%d %d %d %d %d %d\n", bsresult[24], bsresult[25], bsresult[26], bsresult[27], bsresult[28], bsresult[29]);
		//printf("%d %d %d %d %d %d\n", bsresult[18], bsresult[19], bsresult[20], bsresult[21], bsresult[22], bsresult[23]);
		//printf("%d %d %d %d %d %d\n", bsresult[12], bsresult[13], bsresult[14], bsresult[15], bsresult[16], bsresult[17]);
		//printf("%d %d %d %d %d %d\n", bsresult[6], bsresult[7], bsresult[8], bsresult[9], bsresult[10], bsresult[11]);
		//printf("%d %d %d %d %d %d\n", bsresult[0], bsresult[1], bsresult[2], bsresult[3], bsresult[4], bsresult[5]);
		//getchar();
		BSGetOneresult(bsresult, rownum, gameresult);
		for (i = 0; i < gameresult[0]; i++)
			allpayback += BSGetPaybackRate(gameresult[i * 3 + 1], gameresult[i * 3 + 2]);
		if (gameresult[0] > 0)
		{
			//printf("\n\n");
			BSDelDiamond(bsresult, rownum, gameresult);
			//for (i = 0; i < 3 * gameresult[0] + 1; i++)
			//{
			//	printf("%d \t", gameresult[i]);
			//}
			//printf("\n");
			//printf("%d\n", allpayback);
			//printf("\n\n");
		}
		//printf("%d %d %d %d %d %d\n", bsresult[36], bsresult[37], bsresult[38], bsresult[39], bsresult[40], bsresult[41]);
		//printf("%d %d %d %d %d %d\n", bsresult[30], bsresult[31], bsresult[32], bsresult[33], bsresult[34], bsresult[35]);
		//printf("%d %d %d %d %d %d\n", bsresult[24], bsresult[25], bsresult[26], bsresult[27], bsresult[28], bsresult[29]);
		//printf("%d %d %d %d %d %d\n", bsresult[18], bsresult[19], bsresult[20], bsresult[21], bsresult[22], bsresult[23]);
		//printf("%d %d %d %d %d %d\n", bsresult[12], bsresult[13], bsresult[14], bsresult[15], bsresult[16], bsresult[17]);
		//printf("%d %d %d %d %d %d\n", bsresult[6], bsresult[7], bsresult[8], bsresult[9], bsresult[10], bsresult[11]);
		//printf("%d %d %d %d %d %d\n", bsresult[0], bsresult[1], bsresult[2], bsresult[3], bsresult[4], bsresult[5]);
		//getchar();
	} 

	//playerinfo[NIUCTRL_BASECOIN] -= bet;
	//playerinfo[NIUCTRL_BASECOIN] += bet*allpayback / 100;

	//playerinfo[NIUCTRL_BETOUTTAIL] += bet*allpayback / 100; 
	//playerinfo[NIUCTRL_BASECOIN] += playerinfo[NIUCTRL_BETOUTTAIL] / 1000 * NIUGAMEWINRATE;
	//playerinfo[NIUCTRL_BETOUTTAIL] = playerinfo[NIUCTRL_BETOUTTAIL] % 1000;
	return allpayback;
}

static long long BSGetBasecoin(long long basecoin1, long long basecoin2, long long basecoin3)
{
	if (basecoin1 == basecoin2 || basecoin1 == basecoin3)
	{
		return basecoin1;
	}

	if (basecoin2 == basecoin3)
	{
		return basecoin3;
	}

	return 0;
}

void BSSetBomb(int bsresult[], int rownum,int id)
{
	//int rowid = id%rownum;
	//int columnid = id / rownum;
	int i;


	for (i = rownum*rownum + (id%rownum); i > id;)
	{
		bsresult[i] = bsresult[i - rownum];
		i = i - rownum;
	}
	bsresult[id] = 6;
}

void BSSetOnegameResult(long long sign, int level, int bsresult[])
{
	int rownum = level + 4;
	int bombsign = 0;

	BSGetFisrtResult(sign, level, bsresult);
	if (level == 0)
	{
		if (GetRanNum()%3 == 1)
		{
			bombsign = 1;
		}
	}
	else if (level == 1)
	{
		if (GetRanNum() % 5 == 1)
		{
			bombsign = 1;
		}
	}
	else
	{
		if (GetRanNum() % 8 == 1)
		{
			bombsign = 1;
		}
	}

	if (bombsign == 1)
	{
		bombsign = GetRanNum() % (rownum*rownum);
		BSSetBomb(bsresult, rownum, bombsign);
		bsresult[bombsign] = 6;
	}
}

void BSGameResult(long long bsplayerinfo[], long long czgongxian[], int bet, int level, int bsresult[])
{
	int rannum = GetRanNum();
	long long *playerinfo = bsplayerinfo;
	int usingczgxsign = 0;

	//初始化
	if (playerinfo[BSCTRL_INITSIGN] != 0xcfde)
	{
		playerinfo[BSCTRL_INITSIGN] = 0xcfde;
		playerinfo[BSCTRL_PLAYTIMES] = 0;
		playerinfo[BSCTRL_BETOUTTAIL] = 0;
		playerinfo[BSCTRL_BASECOIN] = 0;
		playerinfo[BSCTRL_AIMCOIN] = 0;

		playerinfo[BSCTRL_NOWRANGE] = 0;
		playerinfo[BSCTRL_BASECOIN2] = 0;
		playerinfo[BSCTRL_ST] = 0;
		playerinfo[BSCTRL_USECZGX] = 0;
		playerinfo[BSCTRL_EVMAXBET] = 1000;

		playerinfo[BSCTRL_BASECOIN1] = 0;
		playerinfo[BSCTRL_BONUSBASE] = 0;
		playerinfo[BSCTRL_USECZGXSIGN] = 0;
	}

	//保证充值贡献度释放时，控制净分的变化比较小的模块的初始化
	if (playerinfo[BSCTRL_SFINITSIGN] != 0xcfde)
	{
		playerinfo[BSCTRL_SFINITSIGN] = 0xcfde;
		playerinfo[BSCTRL_SFSIGN] = 0;
		playerinfo[BSCTRL_SFBASECOIN] = 0;
	}

	playerinfo[BSCTRL_BASECOIN] = BSGetBasecoin(playerinfo[BSCTRL_BASECOIN], playerinfo[BSCTRL_BASECOIN1], playerinfo[BSCTRL_BASECOIN2]);

	playerinfo[BSCTRL_EVMAXBET] = (playerinfo[BSCTRL_EVMAXBET] * 9 + bet) / 10;

	//普通状态
	if (playerinfo[BSCTRL_ST] == 0)
	{
		//玩家的控制净分比较高，则跳转到杀分状态
		if (playerinfo[BSCTRL_BASECOIN] > BSSET_MAXGAMEWAVEUP || playerinfo[BSCTRL_BASECOIN] > playerinfo[BSCTRL_EVMAXBET] * BSSET_UPMAXBETLV || playerinfo[BSCTRL_NOWRANGE] + 100000 > BSSET_MAXWIN)
		{
			playerinfo[BSCTRL_ST] = 2;
			playerinfo[BSCTRL_AIMCOIN] = playerinfo[BSCTRL_EVMAXBET] * BSSET_UPMAXBETLV - (playerinfo[BSCTRL_EVMAXBET] * BSSET_DOWNMAXBETLV) / 4 * (GetRanNum() % 7 + 1);
			if (playerinfo[BSCTRL_AIMCOIN] + playerinfo[BSCTRL_EVMAXBET] * BSSET_DOWNMAXBETLV < 0)
			{
				playerinfo[BSCTRL_AIMCOIN] = 0 - playerinfo[BSCTRL_EVMAXBET] * BSSET_DOWNMAXBETLV;
			}
		}
		//玩家的控制净分比较低，则跳转到送分状态
		else if (playerinfo[BSCTRL_BASECOIN] + playerinfo[BSCTRL_EVMAXBET] * BSSET_DOWNMAXBETLV < 0 || playerinfo[BSCTRL_BASECOIN] < BSSET_MAXGAMEWAVEDOWN)
		{
			playerinfo[BSCTRL_ST] = 1;
			playerinfo[BSCTRL_AIMCOIN] = (playerinfo[BSCTRL_EVMAXBET] * BSSET_DOWNMAXBETLV) / 4 * (1 + GetRanNum() % 5) - playerinfo[BSCTRL_EVMAXBET] * BSSET_DOWNMAXBETLV;
			if (playerinfo[BSCTRL_AIMCOIN] > playerinfo[BSCTRL_EVMAXBET] * BSSET_UPMAXBETLV)
			{
				playerinfo[BSCTRL_AIMCOIN] = playerinfo[BSCTRL_EVMAXBET] * BSSET_UPMAXBETLV;
			}
		}
		else
		{
			//普通状态下有一定的概率随机跳转到杀送分状态
			if (rannum % 3880 < 10)
			{
				if (rannum % 2 == 0)
				{
					playerinfo[BSCTRL_ST] = 2;
					playerinfo[BSCTRL_AIMCOIN] = playerinfo[BSCTRL_BASECOIN] - (playerinfo[BSCTRL_EVMAXBET] * BSSET_DOWNMAXBETLV) / 6 * (GetRanNum() % 8 + 1);
					if (playerinfo[BSCTRL_AIMCOIN] + playerinfo[BSCTRL_EVMAXBET] * BSSET_DOWNMAXBETLV < 0)
					{
						playerinfo[BSCTRL_AIMCOIN] = 0 - playerinfo[BSCTRL_EVMAXBET] * BSSET_DOWNMAXBETLV;
					}
				}
				else
				{ 
					playerinfo[BSCTRL_ST] = 1;
					playerinfo[BSCTRL_AIMCOIN] = playerinfo[BSCTRL_BASECOIN] + (playerinfo[BSCTRL_EVMAXBET] * BSSET_UPMAXBETLV) / 6 * (GetRanNum() % 6 + 1);
					if (playerinfo[BSCTRL_AIMCOIN] > playerinfo[BSCTRL_EVMAXBET] * BSSET_UPMAXBETLV)
					{
						playerinfo[BSCTRL_AIMCOIN] = playerinfo[BSCTRL_EVMAXBET] * BSSET_UPMAXBETLV;
					}
				}
			}
		}
	}
	//送分状态
	else if (playerinfo[BSCTRL_ST] == 1)
	{
		//送分完成则跳转到普通状态
		if (playerinfo[BSCTRL_BASECOIN] > BSSET_MAXGAMEWAVEUP || playerinfo[BSCTRL_BASECOIN] > playerinfo[BSCTRL_AIMCOIN] || playerinfo[BSCTRL_BASECOIN] > playerinfo[BSCTRL_EVMAXBET] * BSSET_UPMAXBETLV || playerinfo[BSCTRL_NOWRANGE] + 100000 > BSSET_MAXWIN)
		{
			playerinfo[BSCTRL_ST] = 0;
		}
	}
	//杀分状态
	else
	{
		//达到杀分目标则跳转到普通状态
		if (playerinfo[BSCTRL_BASECOIN] < playerinfo[BSCTRL_AIMCOIN] || playerinfo[BSCTRL_BASECOIN] + playerinfo[BSCTRL_EVMAXBET] * BSSET_DOWNMAXBETLV < 0 || playerinfo[BSCTRL_BASECOIN] < BSSET_MAXGAMEWAVEDOWN)
		{
			playerinfo[BSCTRL_ST] = 0;
		}
	}

	//使用的充值贡献度余额小于0，则继续从充值贡献度里面扣
	if (playerinfo[BSCTRL_USECZGX] < 0)
	{
		if (playerinfo[BSCTRL_USECZGX] + czgongxian[0] >= 0)
		{
			czgongxian[0] = playerinfo[BSCTRL_USECZGX] + czgongxian[0];
			playerinfo[BSCTRL_USECZGX] = 0;
		}
		else if (czgongxian[0] > 0)
		{
			playerinfo[BSCTRL_USECZGX] = playerinfo[BSCTRL_USECZGX] + czgongxian[0];
			czgongxian[0] = 0;
		}
	}

	if (playerinfo[BSCTRL_SFSIGN] == 0 && czgongxian[0] >= 10 * bet)
	{
		playerinfo[BSCTRL_SFSIGN] = 1;
		playerinfo[BSCTRL_SFBASECOIN] = 0;
	}

	if (playerinfo[BSCTRL_SFSIGN] >0 && czgongxian[0] < 10 * bet && playerinfo[BSCTRL_USECZGX] < 10 * bet)
	{
		playerinfo[BSCTRL_SFSIGN] = 0;
	}

	if (playerinfo[BSCTRL_USECZGX] >= 10 * bet && GetRanNum() % 2 == 1)
	{
		usingczgxsign = 1;
	}

	if (czgongxian[0] >= 10 * bet && GetRanNum() % 6 == 1)
	//if (czgongxian[0] > 100 * bet )
	{
		playerinfo[BSCTRL_USECZGX] += 10 * bet;
		czgongxian[0] -= 10 * bet;
		usingczgxsign = 1;
	}

	if (czgongxian[0] >= 100 * bet && GetRanNum() % 28 == 1)
	{
		playerinfo[BSCTRL_USECZGX] += 100 * bet;
		czgongxian[0] -= 100 * bet;
		usingczgxsign = 2;
	}

	//if (czgongxian[0] >= 300 * bet && GetRanNum() % 200 == 1)
	//{
	//	playerinfo[BSCTRL_USECZGX] += 300 * bet;
	//	czgongxian[0] -= 300 * bet;
	//	usingczgxsign = 3;
	//}

	playerinfo[BSCTRL_USECZGXSIGN] = 0;

	//充值贡献小送分
	if (usingczgxsign == 1)
	{
		BSGetOneResultCZGX(bsresult, level);
		playerinfo[BSCTRL_USECZGXSIGN] = 1;
		playerinfo[BSCTRL_PLAYTIMES]++;
		playerinfo[BSCTRL_BASECOIN] -= bet;
		playerinfo[BSCTRL_SFBASECOIN] -= bet;
		playerinfo[BSCTRL_NOWRANGE] -= bet;
		playerinfo[BSCTRL_BETOUTTAIL] += bet;
		playerinfo[BSCTRL_BASECOIN] += playerinfo[BSCTRL_BETOUTTAIL] / 1000 * (BSSET_BETOUTRATE + BSSET_BONUSOUTRATE);
		playerinfo[BSCTRL_BONUSBASE] += playerinfo[BSCTRL_BETOUTTAIL] / 1000 * BSSET_BONUSOUTRATE;
		playerinfo[BSCTRL_BETOUTTAIL] = playerinfo[BSCTRL_BETOUTTAIL] % 1000;
	}
	//充值贡献大送分
	else if (usingczgxsign == 2)
	{
		BSGetOneResultCZGX100(bsresult, level);
		playerinfo[BSCTRL_USECZGXSIGN] = 1;
		playerinfo[BSCTRL_PLAYTIMES]++;
		playerinfo[BSCTRL_BASECOIN] -= bet;
		playerinfo[BSCTRL_SFBASECOIN] -= bet;
		playerinfo[BSCTRL_NOWRANGE] -= bet;
		playerinfo[BSCTRL_BETOUTTAIL] += bet;
		playerinfo[BSCTRL_BASECOIN] += playerinfo[BSCTRL_BETOUTTAIL] / 1000 * (BSSET_BETOUTRATE + BSSET_BONUSOUTRATE);
		playerinfo[BSCTRL_BONUSBASE] += playerinfo[BSCTRL_BETOUTTAIL] / 1000 * BSSET_BONUSOUTRATE;
		playerinfo[BSCTRL_BETOUTTAIL] = playerinfo[BSCTRL_BETOUTTAIL] % 1000;
	}
	//else if (usingczgxsign == 3)
	//{
	//	BSGetOneResultCZGX300(bsresult, level);
	//	playerinfo[BSCTRL_USECZGXSIGN] = 1;
	//	playerinfo[BSCTRL_PLAYTIMES]++;
	//	playerinfo[BSCTRL_BASECOIN] -= bet;
	//	playerinfo[BSCTRL_NOWRANGE] -= bet;
	//	playerinfo[BSCTRL_BETOUTTAIL] += bet;
	//	playerinfo[BSCTRL_BASECOIN] += playerinfo[BSCTRL_BETOUTTAIL] / 1000 * (BSSET_BETOUTRATE + BSSET_BONUSOUTRATE);
	//	playerinfo[BSCTRL_BONUSBASE] += playerinfo[BSCTRL_BETOUTTAIL] / 1000 * BSSET_BONUSOUTRATE;
	//	playerinfo[BSCTRL_BETOUTTAIL] = playerinfo[BSCTRL_BETOUTTAIL] % 1000;
	//}
	else
	{
		playerinfo[BSCTRL_PLAYTIMES]++;
		playerinfo[BSCTRL_BASECOIN] -= bet;
		playerinfo[BSCTRL_SFBASECOIN] -= bet;
		playerinfo[BSCTRL_NOWRANGE] -= bet;
		if (playerinfo[BSCTRL_PLAYTIMES] < 50 && playerinfo[BSCTRL_BASECOIN] < 200 * playerinfo[BSCTRL_EVMAXBET])
		{
			if (playerinfo[BSCTRL_PLAYTIMES] < 11)
			{
				BSSetOnegameResult(10, level, bsresult);
			}
			else
				BSSetOnegameResult(1, level, bsresult);
		}
		else
		{
			if (playerinfo[BSCTRL_SFSIGN] > 0 && (playerinfo[BSCTRL_SFBASECOIN] >= 10 * bet || playerinfo[BSCTRL_SFBASECOIN] <= 0 - 4 * bet))
			{
				if (playerinfo[BSCTRL_SFBASECOIN] >= 10 * bet)
				{
					BSSetOnegameResult(3, level, bsresult);
				}
				else if (playerinfo[BSCTRL_SFBASECOIN] <= 0 - 4 * bet)
				{
					BSGetOneResultCZGX2(bsresult, level);
				}
				else
					BSSetOnegameResult(playerinfo[BSCTRL_ST], level, bsresult);
			}
			else
			{
				if (playerinfo[BSCTRL_SFSIGN] > 0)
				{
					BSSetOnegameResult(3, level, bsresult);
				}
				else
					BSSetOnegameResult(playerinfo[BSCTRL_ST], level, bsresult);
			}
				
		}

		playerinfo[BSCTRL_BETOUTTAIL] += bet;
		playerinfo[BSCTRL_BASECOIN] += playerinfo[BSCTRL_BETOUTTAIL] / 1000 * (BSSET_BETOUTRATE + BSSET_BONUSOUTRATE);
		playerinfo[BSCTRL_BONUSBASE] += playerinfo[BSCTRL_BETOUTTAIL] / 1000 * BSSET_BONUSOUTRATE;
		playerinfo[BSCTRL_BETOUTTAIL] = playerinfo[BSCTRL_BETOUTTAIL] % 1000;
	}


	playerinfo[BSCTRL_BASECOIN1] = playerinfo[BSCTRL_BASECOIN];
	playerinfo[BSCTRL_BASECOIN2] = playerinfo[BSCTRL_BASECOIN];
}

void BSGetMoreResult(long long playerinfo[], int level, int moreresult[], int num)
{
	int i;
	for (i = 0; i < num;i++)
	{
		moreresult[i] = BSGetOneIcon(playerinfo[BSCTRL_ST], level);
	}
}

void BSAddBetout(long long playerinfo[], int outcoin)
{
	if (outcoin < 0)
	{
		return;
	}

	playerinfo[BSCTRL_BASECOIN] = BSGetBasecoin(playerinfo[BSCTRL_BASECOIN], playerinfo[BSCTRL_BASECOIN1], playerinfo[BSCTRL_BASECOIN2]);
	if (playerinfo[BSCTRL_USECZGXSIGN] > 0)
	{
		playerinfo[BSCTRL_USECZGX] -= outcoin;
	}
	else
	{
		playerinfo[BSCTRL_BASECOIN] += outcoin;
		playerinfo[BSCTRL_SFBASECOIN] += outcoin;
		playerinfo[BSCTRL_NOWRANGE] += outcoin;
		playerinfo[BSCTRL_BASECOIN1] = playerinfo[BSCTRL_BASECOIN];
		playerinfo[BSCTRL_BASECOIN2] = playerinfo[BSCTRL_BASECOIN];
	}
}

int BSGetRandResult(int choice[], int choicenum, int aimnum)
{
	int i;
	int smallsign = 0, bigsign = 0;
	int smallall = 0, bigall = 0;
	int bigrate = 0,smallrate = 0;
	int choicerate[100];
	int smallleft,bigleft;
	int randnum;

	if (choicenum > 100)
	{
		return -1;
	}

	for (i = 0; i < choicenum;i++)
	{
		if (choice[i] < aimnum)
		{
			smallsign = 1;
		}
		if (choice[i]>aimnum)
		{
			bigsign = 1;
		}
	}

	if (smallsign == 0 && bigsign == 0)
		return 0;
	if (smallsign != bigsign)
	{
		if (smallsign == 0)
		{
			return 0;
		}
		if (bigsign == 0)
		{
			return 2;
		}
		return -1;
	}

	smallsign = 0;
	bigsign = 0;
	for (i = 0; i < choicenum;i++)
	{
		if (choice[i] < aimnum)
		{
			smallsign++;
			smallall += choice[i];
		}
		else
		{
			bigsign++;
			bigall += choice[i];
		}
	}
	smallall = smallall / smallsign;
	bigall = bigall / bigsign;

	bigrate = 1000 * (aimnum - smallall) / (bigall - smallall);
	smallrate = 1000 - bigrate;
	bigleft = bigrate - bigrate / bigsign*bigsign;
	smallleft = smallrate - smallrate / smallsign*smallsign;

	for (i = 0; i < choicenum;i++)
	{
		if (choice[i] < aimnum)
		{
			choicerate[i] = smallrate / smallsign;
			if (smallleft>0)
			{
				smallleft--;
				choicerate[i]++;
			}
		}
		else
		{
			choicerate[i] = bigrate / bigsign;
			if (bigleft > 0)
			{
				bigleft--;
				choicerate[i]++;
			}
		}
	}

	for (i = 1; i < choicenum;i++)
	{
		choicerate[i] = choicerate[i] + choicerate[i - 1];
	}
	randnum = GetRanNum() % choicerate[choicenum - 1];

	for (i = 0; i < choicenum;i++)
	{
		if (randnum < choicerate[i])
		{
			return i;
		}
	}

	return 0;
}

//返回结果下标
int BSGetFinalBonus(long long playerinfo[], int bonuschoice[])
{
	int paybackchoice[5]=
	{
		3000,  5000,  20000,  5000,   10000,
	};

	int bonus = (int)(playerinfo[BSCTRL_BONUSBASE]);
	int i;
	int paybackbase = 0;
	int temp = 0;
	int resultid = 0;

	paybackbase = 240000 * BSSET_BONUSOUTRATE / 1000;
	temp = bonus / paybackbase;

	if (temp < 1)
	{
		temp = 1;
	}

	for (i = 0; i < 5;i++)
	{
		bonuschoice[i] = paybackchoice[i] * temp;
	}
	
	resultid = BSGetRandResult(bonuschoice, 5, bonus);


	playerinfo[BSCTRL_BONUSBASE] = 0;

	return resultid;
}


int BSGetOneGameResult(long long playerinfo[],long long czgongxian[], int bet, int level,int bombsign[])
{
	int i,j;
	int bsresult[50];
	int gameresult[20];
	int allpayback = 0;
	int rownum = 4 + level;

	BSInitArray(bsresult, 50);
	BSInitArray(gameresult, 20);

	//BSGetFisrtResult(2, level, bsresult);

	BSGameResult(playerinfo, czgongxian, bet, level, bsresult);
	for (j = 0; j < rownum*rownum;j++)
	{
		if (bsresult[j] == 6)
		{
			bombsign[0] = 1;
			bsresult[j] = 0;
			BSFallDown(bsresult, rownum);
			for (i = 0; i < rownum*(rownum + 1); i++)
			{
				if (bsresult[i] == 0)
				{
					bsresult[i] = BSGetOneIcon(playerinfo[BSCTRL_ST], level);
				}
			}
			break;
		}
	}

	BSGetOneresult(bsresult, rownum, gameresult);
	for (i = 0; i < gameresult[0]; i++)
		allpayback += BSGetPaybackRate(gameresult[i * 3 + 1], gameresult[i * 3 + 2]);
	if (gameresult[0] > 0)
	{
		BSDelDiamond(bsresult, rownum, gameresult);
	}
	while (gameresult[0] > 0)
	{
		BSFallDown(bsresult, rownum);
		BSInitArray(gameresult, 20);
		for (i = 0; i < rownum*(rownum + 1); i++)
		{
			if (bsresult[i] == 0)
			{
				bsresult[i] = BSGetOneIcon(playerinfo[BSCTRL_ST], level);
			}
		}
		BSGetOneresult(bsresult, rownum, gameresult);
		for (i = 0; i < gameresult[0]; i++)
			allpayback += BSGetPaybackRate(gameresult[i * 3 + 1], gameresult[i * 3 + 2]);
		if (gameresult[0] > 0)
		{
			BSDelDiamond(bsresult, rownum, gameresult);
		}
	}
	return allpayback;
}
