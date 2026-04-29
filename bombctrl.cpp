#include <time.h>
#include <stdlib.h>
#include "bombctrl.h"

enum 
{
	BOMB_BASECOIN,		   //弹头净分
	BOMB_ALLBASECOIN,        //总弹头净分
	BOMB_OUTTAIL,           //抽水用
	BOMB_INITSIGN,          //标志
};

static int XMSET_MONEYBOMB_RATE_NEW = 18800;					  //新手的钱、净分比
static int XMSET_MONEYBOMB_RATE_NORMAL = 16800;                //正常的钱、净分比
static int XMSET_MONEY_NEW = 8000;							  //走新手净分比的充值阈值（单位RMB)
static int XMSET_INBOMB_RATE[3] = { 50, 100, 150 };			  //加弹头净分的百分比
static int XMSET_MONEY_BLOCK = 100;                          //用于计算随机弹头净分的钱的区块大小
static int XMSET_BOMB_BASECOIN_LIMIT[4] = { -2000000, -10000000, -20000000,-50000000};               //弹头净分的下限值
static int XMSET_BOMB_BASECOIN_BETOUTRATE = 10;                //弹头净分的抽水率（千分）
static int XMSET_BOMB_LIMIT_RATE[4] = { 60, 30, 15, 5 };

static int bomb_set_time_rand_sign = 0;
static int bomb_set_game_info_sign = 0;
static int set_rand_numrange_sign = 0;

void BombSetGameInfo(int setarray[])
{
	XMSET_MONEYBOMB_RATE_NEW = setarray[ID_XMSET_MONEYBOMB_RATE_NEW];
	XMSET_MONEYBOMB_RATE_NORMAL = setarray[ID_XMSET_MONEYBOMB_RATE_NORMAL];
	XMSET_MONEY_NEW = setarray[ID_XMSET_MONEY_NEW];

	XMSET_INBOMB_RATE[0] = setarray[ID_XMSET_INBOMB_RATE];
	XMSET_INBOMB_RATE[1] = setarray[ID_XMSET_INBOMB_RATE+1];
	XMSET_INBOMB_RATE[2] = setarray[ID_XMSET_INBOMB_RATE+2];

	XMSET_MONEY_BLOCK = setarray[ID_XMSET_MONEY_BLOCK];
	XMSET_BOMB_BASECOIN_LIMIT[0] = setarray[ID_XMSET_BOMB_BASECOIN_LIMIT];
	XMSET_BOMB_BASECOIN_LIMIT[1] = setarray[ID_XMSET_BOMB_BASECOIN_LIMIT+1];
	XMSET_BOMB_BASECOIN_LIMIT[2] = setarray[ID_XMSET_BOMB_BASECOIN_LIMIT+2];
	XMSET_BOMB_BASECOIN_LIMIT[3] = setarray[ID_XMSET_BOMB_BASECOIN_LIMIT+3];
	XMSET_BOMB_BASECOIN_BETOUTRATE = setarray[ID_XMSET_BOMB_BASECOIN_BETOUTRATE];
	XMSET_BOMB_LIMIT_RATE[0] = setarray[ID_XMSET_BOMB_LIMIT_RATE];
	XMSET_BOMB_LIMIT_RATE[1] = setarray[ID_XMSET_BOMB_LIMIT_RATE+1];
	XMSET_BOMB_LIMIT_RATE[2] = setarray[ID_XMSET_BOMB_LIMIT_RATE+2];
	XMSET_BOMB_LIMIT_RATE[3] = setarray[ID_XMSET_BOMB_LIMIT_RATE+3];

	bomb_set_game_info_sign = 36;
}

void BombOldIDInit(long long playerbombinfo[], long long bombbasecoin,long long allbombbasecoin)
{
	playerbombinfo[BOMB_BASECOIN] = bombbasecoin;
	playerbombinfo[BOMB_ALLBASECOIN] = allbombbasecoin;
	playerbombinfo[BOMB_OUTTAIL] = 0;
	playerbombinfo[BOMB_INITSIGN] = 0xcfde;
}

void BombCheckInit(long long playerbombinfo[])
{
	if (playerbombinfo[BOMB_INITSIGN] != 0xcfde)
	{
		playerbombinfo[BOMB_BASECOIN] = 0;
		playerbombinfo[BOMB_ALLBASECOIN] = 0;
		playerbombinfo[BOMB_OUTTAIL] = 0;
		playerbombinfo[BOMB_INITSIGN] = 0xcfde;
	}
}

int BombGetRanNum(void)
{
	int tempnum = 0;
	int times = 0;

	if (bomb_set_time_rand_sign == 0)
	{
		int i;
		srand((int)time(0));
		bomb_set_time_rand_sign = 1;

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

//使用弹头,每使用一次调用一次；玩家在背包用弹头换金币，也每换一个弹头调用一次
int BYSSBombUseBomb(long long playerbombinfo[], int bombcoin)
{
	int tempbombcoin = bombcoin * XMSET_INBOMB_RATE[BombGetRanNum() % 3] / 100;

	if (bomb_set_game_info_sign != 36)
	{
		return -1;
	}

	BombCheckInit(playerbombinfo);

	playerbombinfo[BOMB_BASECOIN] += tempbombcoin;
	playerbombinfo[BOMB_ALLBASECOIN] += tempbombcoin;

	return 1;
}

int BYSSBombUseBombOnfish(long long playerbombinfo[], int bombcoin)
{
	int tempbombcoin = bombcoin;

	if (bomb_set_game_info_sign != 36)
	{
		return -1;
	}

	BombCheckInit(playerbombinfo);

	playerbombinfo[BOMB_BASECOIN] += tempbombcoin;
	playerbombinfo[BOMB_ALLBASECOIN] += tempbombcoin;

	return 1;
}

//付费
int BYSSBombBuyCoin(long long playerbombinfo[], int money)
{
	int tempbombcoin = 0;
	int times = money / XMSET_MONEY_BLOCK;
	int i;
	int moneybombrate = 0;

	if (bomb_set_game_info_sign != 36)
	{
		return -1;
	}

	BombCheckInit(playerbombinfo);

	if (playerbombinfo[BOMB_ALLBASECOIN] < XMSET_MONEY_NEW * XMSET_MONEYBOMB_RATE_NEW)
		moneybombrate = XMSET_MONEYBOMB_RATE_NEW;
	else
		moneybombrate = XMSET_MONEYBOMB_RATE_NORMAL;

	for (i = 0; i < times; i++)
	{
		tempbombcoin += XMSET_MONEY_BLOCK * moneybombrate * XMSET_INBOMB_RATE[BombGetRanNum() % 3] / 100;
	}

	tempbombcoin += (money % XMSET_MONEY_BLOCK) * moneybombrate * XMSET_INBOMB_RATE[BombGetRanNum() % 3] / 100;


	playerbombinfo[BOMB_BASECOIN] += tempbombcoin;
	playerbombinfo[BOMB_ALLBASECOIN] += tempbombcoin;

	return 1;
}

//获得弹头
int BYSSBombGetBomb(long long playerbombinfo[], int bombcoin)
{
	if (bomb_set_game_info_sign != 36)
	{
		return -1;
	}
	BombCheckInit(playerbombinfo);
	playerbombinfo[BOMB_BASECOIN] -= bombcoin;

	return 1;
}


int BYSSBombCannotGetBomb(long long playerbombinfo[], int bombcoin)
{
	if (bomb_set_game_info_sign != 36)
	{
		return -1;
	}
	BombCheckInit(playerbombinfo);
	if (playerbombinfo[BOMB_BASECOIN] - bombcoin <= XMSET_BOMB_BASECOIN_LIMIT[3])
	{
		if (BombGetRanNum() % 100 < XMSET_BOMB_LIMIT_RATE[3])
			return 0;
		else
			return 1;
	}

	if (playerbombinfo[BOMB_BASECOIN] - bombcoin <= XMSET_BOMB_BASECOIN_LIMIT[2])
	{
		if (BombGetRanNum() % 100 < XMSET_BOMB_LIMIT_RATE[2])
			return 0;
		else
			return 1;
	}

	if (playerbombinfo[BOMB_BASECOIN] - bombcoin <= XMSET_BOMB_BASECOIN_LIMIT[1])
	{
		if (BombGetRanNum() % 100 < XMSET_BOMB_LIMIT_RATE[1])
			return 0;
		else
			return 1;
	}

	if (playerbombinfo[BOMB_BASECOIN] - bombcoin <= XMSET_BOMB_BASECOIN_LIMIT[0])
	{
		if (BombGetRanNum() % 100 < XMSET_BOMB_LIMIT_RATE[0])
			return 0;
		else
			return 1;
	}

	return 0;
}

int BYSSBombBetout(long long playerbombinfo[], int costcoin)
{
	if (bomb_set_game_info_sign != 36)
	{
		return -1;
	}
	BombCheckInit(playerbombinfo);
	playerbombinfo[BOMB_OUTTAIL] += costcoin;
	playerbombinfo[BOMB_BASECOIN] -= (playerbombinfo[BOMB_OUTTAIL] / 1000)*XMSET_BOMB_BASECOIN_BETOUTRATE;
	playerbombinfo[BOMB_OUTTAIL] = playerbombinfo[BOMB_OUTTAIL] % 1000;

	return 1;
}

int BYSSBombAddBombbasecoin(long long playerbombinfo[], int addbasecoin)
{
	if (bomb_set_game_info_sign != 36)
	{
		return -1;
	}
	BombCheckInit(playerbombinfo);
	playerbombinfo[BOMB_BASECOIN] += addbasecoin;
	

	return 1;
}

int BYSSBombSmallgameBet(long long playerbombinfo[], int betcoin)
{
	if (bomb_set_game_info_sign != 36)
	{
		return -1;
	}
	BombCheckInit(playerbombinfo);
	playerbombinfo[BOMB_BASECOIN] -= betcoin;


	return 1;
}


int BYSSBombSmallgameWin(long long playerbombinfo[], int wincoin)
{
	if (bomb_set_game_info_sign != 36)
	{
		return -1;
	}
	BombCheckInit(playerbombinfo);
	playerbombinfo[BOMB_BASECOIN] += wincoin;


	return 1;
}