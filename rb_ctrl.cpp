#include <time.h>
#include <stdlib.h>
#include "rb_ctrl.h"

enum{
	RBCTRL_BASECOIN,
	RBCTRL_NOWBASECOIN,
	RBCTRL_ST,
	RBCTRL_BACKBASECOIN1,
	RBCTRL_RBBASEBACK1,

	RBCTRL_MAINAIMRANGE,  
	RBCTRL_NOWAIMRANGE,
	RBCTRL_OUTTAIL,
	RBCTRL_INITSIGN,
	RBCTRL_EVMAXBET,     //最大押分的10盘均值

	RBCTRL_BONUSBASE,
	RBCTRL_BONUSGET,
	RBCTRL_RBBASECOIN,
	RBCTRL_REDBLUEMAXBET,
	RBCTRL_ALLBETEV,

	RBCTRL_BACKBASECOIN2,
	RBCTRL_RBBASEBACK2,

};

static int REDBLACKCHOICENUM = 3;
static int REDBLACKGAMEWINRATE = 25;
static int REDBLACKBONUSRATE = 0;
static int RBMAXGAMEWAVEUP = 100000000;
static int RBMAXGAMEWAVEDOWN = 300000000;
static int RB_SHA_PAYBACK = 85;            //杀分时的回报率
static int RB_SONG_PAYBACK = 115;			 //送分时的回报率


static int paybackrate[] = {20,20,2,3,4,10,15};
static int set_time_randnum_sign = 0;
static int set_rand_numrange_sign = 0;

static int GetRanNum(void)
{
	int tempnum = 0;
	int times = 0;

	if (set_time_randnum_sign == 0)
	{
		int i;
		srand((int)time(0));
		set_time_randnum_sign = 1;

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



void RedBlueGameBonusOut(int redbluectrlinfo[], int bonusout)
{
	redbluectrlinfo[RBCTRL_BONUSGET] += bonusout;
}

void DisOrderTool(int arr[], int num)
{
	int i;
	int temp;
	int ran;

	for (i = 0; i < num; i++)
	{
		ran = GetRanNum() % num;
		temp = arr[i];
		arr[i] = arr[ran];
		arr[ran] = temp;
	}
}

void OrderArray(int array[], int num)
{
	int i, j;
	int temp;

	if (num < 2)
		return;

	for (i = 0; i < num - 1; i++)
	{
		for (j = 0; j < num - i - 1; j++)
		{
			if (array[j] > array[j + 1])
			{
				temp = array[j];
				array[j] = array[j + 1];
				array[j + 1] = temp;
			}
		}
	}
}



//类型说明 6 三条，5 同花顺，4 同花；3 顺子；2大对；1 其他牌型
//int cardtype[4],类型，关键值1，关键值2，关键值3
void RedBlueGetCardtype(int card[], int cardtype[])
{
	int temp[3];
	temp[0] = (card[0] % 16) * 16 + card[0] / 16;
	temp[1] = (card[1] % 16) * 16 + card[1] / 16;
	temp[2] = (card[2] % 16) * 16 + card[2] / 16;
	OrderArray(temp, 3);

	if (card[0] % 16 == card[1] % 16 && card[1] % 16 == card[2] % 16)
	{
		cardtype[0] = 6;
		if (card[0]%16 == 1)
		{
			cardtype[1] = 14;
			cardtype[2] = cardtype[1];
			cardtype[3] = cardtype[1];
		}
		else
		{
			cardtype[1] = card[0] % 16;
			cardtype[2] = cardtype[1];
			cardtype[3] = cardtype[1];
		}



		return;
	}

	if (card[0] / 16 == card[1] / 16 && card[1] / 16 == card[2] / 16)
	{
		if ((temp[1]/16 == temp[0]/16 + 1 && temp[2]/16 == temp[1]/16 + 1) || (temp[0]/16 == 1 && temp[1]/16 == 12 && temp[2]/16 == 13))
		{
			cardtype[0] = 5;
			if (temp[0] / 16 == 1 && temp[1] / 16 == 12 && temp[2] / 16 == 13)
			{
				cardtype[1] = temp[0]+13*16;
				cardtype[2] = temp[2];
				cardtype[3] = temp[1];
			}
			else
			{
				cardtype[1] = temp[2];
				cardtype[2] = temp[1];
				cardtype[3] = temp[0];
			}

			return;
		}

		cardtype[0] = 4;
		if (temp[0]/16 == 1)
		{
			cardtype[1] = temp[0] + 13 * 16;
			cardtype[2] = temp[2];
			cardtype[3] = temp[1];
		}
		else
		{
			cardtype[1] = temp[2];
			cardtype[2] = temp[1];
			cardtype[3] = temp[0];
		}

		return;
	}

	if ((temp[1]/16 == temp[0]/16 + 1 && temp[2]/16 == temp[1]/16 + 1) || (temp[0]/16 == 1 && temp[1]/16 == 12 && temp[2]/16 == 13))
	{
		cardtype[0] = 3;
		if (temp[0]/16 == 1 && temp[1] / 16 == 12 && temp[2] / 16 == 13)
		{
			cardtype[1] = temp[0] + 13 * 16;
			cardtype[2] = temp[2];
			cardtype[3] = temp[1];
		}
		else
		{
			cardtype[1] = temp[2];
			cardtype[2] = temp[1];
			cardtype[3] = temp[0];
		}

		return;
	}

	if (temp[0]/16 == temp[1]/16 )
	{
		if (temp[1] / 16 > 8 || temp[1] / 16 == 1)
		{
			cardtype[0] = 2;
			if (temp[1]/16 == 1)
				cardtype[1] = temp[1]+13*16;
			else
				cardtype[1] = temp[1];

			cardtype[2] = temp[2];
			cardtype[3] = temp[2];
		}
		else
		{
			cardtype[0] = 1;
			cardtype[1] = temp[1];
			cardtype[2] = temp[2];
			cardtype[3] = temp[2];
		}


		return;
	}

	if (temp[2]/16 == temp[1]/16)
	{
		if (temp[1] / 16 > 8 || temp[1] / 16 == 1)
		{
			cardtype[0] = 2;
			if (temp[1]/16 == 1)
				cardtype[1] = temp[2]+13*16;
			else
				cardtype[1] = temp[2];

			cardtype[2] = temp[0];
			cardtype[3] = temp[0];
		}
		else
		{
			cardtype[0] = 1;
			cardtype[1] = temp[2];
			cardtype[2] = temp[0];
			cardtype[3] = temp[0];
		}

		return;
	}

	cardtype[0] = 0;
	if (temp[0]/16 == 1)
	{
		cardtype[1] = temp[0]+13*16;
		cardtype[2] = temp[2];
		cardtype[3] = temp[1];
	}
	else
	{
		cardtype[3] = temp[0];

		cardtype[2] = temp[1];
		cardtype[1] = temp[2];
	}

}

int CompareCardtype(int cardtype1[], int cardtype2[])
{
	if (cardtype1[0] > cardtype2[0])
	{
		return 1;
	}
	else if (cardtype1[0] == cardtype2[0])
	{
		if (cardtype1[1]/16 > cardtype2[1]/16)
		{
			return 1;
		}
		else if (cardtype1[1]/16 == cardtype2[1]/16)
		{
			if (cardtype1[2]/16 > cardtype2[2]/16)
			{
				return 1;
			}
			else if (cardtype1[2]/16 == cardtype2[2]/16)
			{
				if (cardtype1[3]/16 > cardtype2[3]/16)
				{
					return 1;
				}
				else if (cardtype1[3]/16 == cardtype2[3]/16)
				{
					if (cardtype1[1] > cardtype2[1])
					{
						return 1; 
					}

					return 0;
				}
				else
				{
					return 0;
				}
			}
			else
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

int RBSetCardtype(int gameresult[], int cardtype1[], int cardtype2[], int wincardtype[])
{
	int i;


	RedBlueGetCardtype(gameresult, cardtype1);
	RedBlueGetCardtype(&gameresult[3], cardtype2);

	if (CompareCardtype(cardtype1, cardtype2) > 0)
	{
		for (i = 0; i < 4; i++)
			wincardtype[i] = cardtype1[i];

		return 1;
	}
	else
	{
		for (i = 0; i < 4; i++)
			wincardtype[i] = cardtype2[i];

		return 0;
	}
}



void RBSongPai(int card[], int gameresult[])
{
	int color;
	int cardvalue;
	int i, j;
	int ran = GetRanNum() % 10000;

	if (ran < 1000)
	{
		ran *= 20;
		
		if (ran > 2000)
		{
			color = GetRanNum() % 4;

			for (j = 0; j < 3; j++)
			{
				for (i = 0; i < 52 - j; i++)
				{
					if (card[i] / 16 == color)
					{
						gameresult[j] = card[i];
						card[i] = card[51 - j];
						card[51 - j] = gameresult[j];
						break;
					}

				}
			}
			for (i = 0; i < 3; i++)
			{
				gameresult[3 + i] = card[i];
			}
		}
		else if (ran > 1000)
		{
			color = GetRanNum() % 4;
			cardvalue = 3 + (GetRanNum() % 12);

			if (cardvalue == 14)
			{
				gameresult[0] = 16 * color + 1;
				gameresult[1] = 16 * color + 13;
				gameresult[2] = 16 * color + 12;
			}
			else
			{
				gameresult[0] = 16 * color + cardvalue;
				gameresult[1] = 16 * color + cardvalue - 1;
				gameresult[2] = 16 * color + cardvalue - 2;
			}


			color = 0;
			for (i = 0; i < 51; i++)
			{
				if (card[i] != gameresult[0] && card[i] != gameresult[1] && card[i] != gameresult[2])
				{
					gameresult[3 + color] = card[i];
					color++;
					if (color >= 3)
					{
						break;
					}
				}
			}

		}
		else
		{
			cardvalue = 1 + GetRanNum() % 13;

			for (j = 0; j < 3; j++)
			{
				for (i = 0; i < 51 - j; i++)
				{
					if (card[i] % 16 == cardvalue)
					{
						gameresult[j] = card[i];
						card[i] = card[51 - j];
						card[51 - j] = gameresult[j];
						break;
					}

				}
			}

			for (i = 0; i < 3; i++)
			{
				gameresult[3 + i] = card[i];
			}
		}
	}
}

void RBSongCaijin(int card[], int gameresult[])
{
	int ran;
	int color;
	int cardvalue;
	int i, j;


	//DisOrderTool(card,52);

	ran = GetRanNum() % 2;
	if (ran < 1)
	{
		color = GetRanNum() % 4;
		cardvalue = 3 + (GetRanNum() % 12);

		if (cardvalue == 14)
		{
			gameresult[0] = 16 * color + 1;
			gameresult[1] = 16 * color + 13;
			gameresult[2] = 16 * color + 12;
		}
		else
		{
			gameresult[0] = 16 * color + cardvalue;
			gameresult[1] = 16 * color + cardvalue - 1;
			gameresult[2] = 16 * color + cardvalue - 2;
		}

		color = 0;
		for (i = 0; i < 51; i++)
		{
			if (card[i] != gameresult[0] && card[i] != gameresult[1] && card[i] != gameresult[2])
			{
				gameresult[3 + color] = card[i];
				color++;
				if (color >= 3)
				{
					break;
				}
			}
		}

	}
	else
	{
		cardvalue = 1 + GetRanNum() % 13;

		for (j = 0; j < 3; j++)
		{
			for (i = 0; i < 51 - j; i++)
			{
				if (card[i] % 16 == cardvalue)
				{
					gameresult[j] = card[i];
					card[i] = card[51 - j];
					card[51 - j] = gameresult[j];
					break;
				}

			}
		}

		for (i = 0; i < 3; i++)
		{
			gameresult[3 + i] = card[i];
		}
	}

}

long long RBGetBasecoin(long long basecoin1, long long basecoin2, long long basecoin3)
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

int RedBlueGameResult(int bet[], long long playerinfo[], int gameresult[],int win_result[])
{
	int maxbet;
	int maxbet2;
	const int gamerange[8] = { 50, 20, 30, 100, 80, 40, 50, 60 };
	//const int subgamerange[8] = {240,80,160,480,320,160,240,180};
	int i;
	int winflg = 0;
	int temprand = 0;
	int realwinflg = 0;
	int courseresult = 0;
	//int *playerinfo = redbluectrlinfo;
	int card[] = {
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,
		0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
		0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d,
		0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d,

		//方块A...
		//梅花A...
		//红桃A...
		//黑桃A...
	};
	int cardtype1[4], cardtype2[4], wincardtype[4];
	int winredblue;
	int winflg2;

	//计算平均最大押分
	maxbet = bet[2];

	if (bet[0] > bet[1])
	{
		maxbet2 = bet[0];
	}
	else
		maxbet2 = bet[1];


	//初始化
	if (playerinfo[RBCTRL_INITSIGN] != 0xcfde)
	{
		playerinfo[RBCTRL_INITSIGN] = 0xcfde;
		srand((int)time(0));
		playerinfo[RBCTRL_BASECOIN] = 0;
		playerinfo[RBCTRL_BACKBASECOIN1] = 0;
		playerinfo[RBCTRL_BACKBASECOIN2] = 0;
		playerinfo[RBCTRL_ST] = 0;
		playerinfo[RBCTRL_MAINAIMRANGE] = gamerange[GetRanNum() % 8] / 4 ;
		playerinfo[RBCTRL_NOWAIMRANGE] = 20 + 10 * (1 + GetRanNum() % 4);
		playerinfo[RBCTRL_NOWBASECOIN] = 0;
		playerinfo[RBCTRL_OUTTAIL] = 0;
		playerinfo[RBCTRL_EVMAXBET] = maxbet;
		playerinfo[RBCTRL_BONUSBASE] = (4 + GetRanNum() % 4) * 10000000;
		playerinfo[RBCTRL_BONUSGET] = 0;

		playerinfo[RBCTRL_RBBASECOIN] = 0;
		playerinfo[RBCTRL_RBBASEBACK1] = 0;
		playerinfo[RBCTRL_RBBASEBACK2] = 0;
		playerinfo[RBCTRL_REDBLUEMAXBET] = maxbet2;
		playerinfo[RBCTRL_ALLBETEV] = bet[0] + bet[1] + bet[2];
	}

	playerinfo[RBCTRL_RBBASECOIN] = RBGetBasecoin(playerinfo[RBCTRL_RBBASECOIN], playerinfo[RBCTRL_RBBASEBACK1], playerinfo[RBCTRL_RBBASEBACK2]);
	playerinfo[RBCTRL_BASECOIN] = RBGetBasecoin(playerinfo[RBCTRL_BASECOIN], playerinfo[RBCTRL_BACKBASECOIN1], playerinfo[RBCTRL_BACKBASECOIN2]);
	playerinfo[RBCTRL_EVMAXBET] = (playerinfo[RBCTRL_EVMAXBET] * 9 + maxbet) / 10;

	if (playerinfo[RBCTRL_EVMAXBET] < 500000)
	{
		playerinfo[RBCTRL_EVMAXBET] = 500000;
	}

	playerinfo[RBCTRL_REDBLUEMAXBET] = (playerinfo[RBCTRL_REDBLUEMAXBET] * 9 + maxbet2) / 10;
	if (playerinfo[RBCTRL_REDBLUEMAXBET] < 1000000)
	{
		playerinfo[RBCTRL_REDBLUEMAXBET] = 1000000;
	}

	playerinfo[RBCTRL_ALLBETEV] = (playerinfo[RBCTRL_ALLBETEV] * 9 + bet[0] + bet[1] + bet[2]) / 10;
	if (playerinfo[RBCTRL_ALLBETEV] < 1000000)
	{
		playerinfo[RBCTRL_ALLBETEV] = 1000000;
	}

	if (playerinfo[RBCTRL_BONUSBASE] > 1000000000 && playerinfo[RBCTRL_BONUSGET] > 1000000000)
	{
		playerinfo[RBCTRL_BONUSBASE] -= 100000000;
		playerinfo[RBCTRL_BONUSGET] -= 100000000;
	}



	if (playerinfo[RBCTRL_ST] == 0)
	{
		//去送分
		if (playerinfo[RBCTRL_BASECOIN] + RBMAXGAMEWAVEDOWN < 0 || playerinfo[RBCTRL_BASECOIN] + playerinfo[RBCTRL_MAINAIMRANGE] * playerinfo[RBCTRL_EVMAXBET] < 0)
		{
			playerinfo[RBCTRL_ST] = 1;
			playerinfo[RBCTRL_NOWBASECOIN] = 0;
			playerinfo[RBCTRL_NOWAIMRANGE] = playerinfo[RBCTRL_MAINAIMRANGE] / 3 * (1 + GetRanNum() % 4);
			winflg = 1;

		}
		//去杀分
		else if (playerinfo[RBCTRL_BASECOIN] > RBMAXGAMEWAVEUP || playerinfo[RBCTRL_BASECOIN] > playerinfo[RBCTRL_MAINAIMRANGE] * playerinfo[RBCTRL_EVMAXBET] / 2)
		{
			playerinfo[RBCTRL_ST] = 2;
			playerinfo[RBCTRL_NOWBASECOIN] = 0;
			playerinfo[RBCTRL_NOWAIMRANGE] = playerinfo[RBCTRL_MAINAIMRANGE] / 3 * (1 + GetRanNum() % 6);
			winflg = 2;
		}
		else
		{
			temprand = GetRanNum();
			if (temprand % 800 < 10)
			{
				if (temprand % 800 < 5)
				{
					playerinfo[RBCTRL_ST] = 2;
					playerinfo[RBCTRL_NOWBASECOIN] = 0;
					playerinfo[RBCTRL_NOWAIMRANGE] = playerinfo[RBCTRL_MAINAIMRANGE] / 6 * (1 + GetRanNum() % 6);
					winflg = 2;
				}
				else
				{
					playerinfo[RBCTRL_ST] = 1;
					playerinfo[RBCTRL_NOWBASECOIN] = 0;
					playerinfo[RBCTRL_NOWAIMRANGE] = playerinfo[RBCTRL_MAINAIMRANGE] / 6 * (1 + GetRanNum() % 4);
					winflg = 1;
				}
			}
			else
			{
				winflg = 0;
			}
		}
	}
	else if (playerinfo[RBCTRL_ST] == 1)
	{
		if (playerinfo[RBCTRL_BASECOIN] > RBMAXGAMEWAVEUP || (playerinfo[RBCTRL_NOWBASECOIN] > playerinfo[RBCTRL_NOWAIMRANGE] * playerinfo[RBCTRL_EVMAXBET]) || (playerinfo[RBCTRL_BASECOIN] >  playerinfo[RBCTRL_MAINAIMRANGE] * playerinfo[RBCTRL_EVMAXBET] / 2) || (playerinfo[RBCTRL_BASECOIN] > 0 && GetRanNum() % 36 == 1))
		{
			playerinfo[RBCTRL_ST] = 0;
			playerinfo[RBCTRL_NOWBASECOIN] = 0;
			playerinfo[RBCTRL_MAINAIMRANGE] = gamerange[GetRanNum() % 8];
			winflg = 0;
		}
		else
		{
			winflg = 1;
		}
	}
	else
	{
		if (playerinfo[RBCTRL_BASECOIN] + RBMAXGAMEWAVEDOWN < 0 || (playerinfo[RBCTRL_NOWBASECOIN] + playerinfo[RBCTRL_NOWAIMRANGE] * playerinfo[RBCTRL_EVMAXBET] < 0) || (playerinfo[RBCTRL_BASECOIN] + playerinfo[RBCTRL_MAINAIMRANGE] * playerinfo[RBCTRL_EVMAXBET] * 2 / 3 < 0))
		{
			playerinfo[RBCTRL_ST] = 0;
			playerinfo[RBCTRL_NOWBASECOIN] = 0;
			playerinfo[RBCTRL_MAINAIMRANGE] = gamerange[GetRanNum() % 8];
			winflg = 0;
		}
		else
		{
			winflg = 2;
		}
	}

	//处理IN OUT
	playerinfo[RBCTRL_NOWBASECOIN] -= bet[2];
	playerinfo[RBCTRL_BASECOIN] -= bet[2];

	playerinfo[RBCTRL_RBBASECOIN] -= bet[0];
	playerinfo[RBCTRL_RBBASECOIN] -= bet[1];

	DisOrderTool(card, 52);
	for (i = 0; i < 6; i++)
	{
		gameresult[i] = card[i];
	}
	winredblue = RBSetCardtype(gameresult, cardtype1, cardtype2, wincardtype);

	if (winflg == 2)
	{
		if (wincardtype[0] > 3)
		{
			if (GetRanNum() % 8 == 4)
			{
				DisOrderTool(card, 52);
				for (i = 0; i < 6; i++)
				{
					gameresult[i] = card[i];
				}
				winredblue = RBSetCardtype(gameresult, cardtype1, cardtype2, wincardtype);
			}
		}
	}
	else if (winflg == 1)
	{
		RBSongPai(card, gameresult);
		winredblue = RBSetCardtype(gameresult, cardtype1, cardtype2, wincardtype);
	}

	if (wincardtype[0] > 1 && playerinfo[RBCTRL_BASECOIN] + paybackrate[wincardtype[0]] * bet[2] > RBMAXGAMEWAVEUP + 10000)
	{
		DisOrderTool(card, 52);
		for (i = 0; i < 6; i++)
		{
			gameresult[i] = card[i];
		}
		winredblue = RBSetCardtype(gameresult, cardtype1, cardtype2, wincardtype);
	}

	if (wincardtype[0] < 2 && playerinfo[RBCTRL_BASECOIN] + RBMAXGAMEWAVEDOWN < 0)
	{
		DisOrderTool(card, 52);
		for (i = 0; i < 6; i++)
		{
			gameresult[i] = card[i];
		}
		RBSongPai(card, gameresult);
		winredblue = RBSetCardtype(gameresult, cardtype1, cardtype2, wincardtype);
	}



	if (REDBLACKBONUSRATE > 0)
	{
		if (playerinfo[RBCTRL_BONUSBASE] > playerinfo[RBCTRL_BONUSGET] + 160000000 && winflg < 2)
		{
			if (GetRanNum() % 20 == 8)
			{
				RBSongCaijin(card, gameresult);
				winredblue = RBSetCardtype(gameresult, cardtype1, cardtype2, wincardtype);
			}
		}
		else if (playerinfo[RBCTRL_BONUSBASE] < playerinfo[RBCTRL_BONUSGET] + 50000000)
		{
			if (wincardtype[0] > 4 || (wincardtype[0] == 4 && wincardtype[1] == 14))
			{
				for (i = 0; i < 6; i++)
				{
					gameresult[i] = card[6 + i];
				}
				winredblue = RBSetCardtype(gameresult, cardtype1, cardtype2, wincardtype);
			}
		}

	}

	if ((wincardtype[0] == 2 || wincardtype[0] == 1) && cardtype1[1] / 16 == cardtype2[1] / 16)
	{
		DisOrderTool(card, 52);
		for (i = 0; i < 6; i++)
		{
			gameresult[i] = card[i];
		}
		winredblue = RBSetCardtype(gameresult, cardtype1, cardtype2, wincardtype);

		if ((wincardtype[0] == 2 || wincardtype[0] == 1) && cardtype1[1] / 16 == cardtype2[1] / 16)
		{
			DisOrderTool(card, 52);
			for (i = 0; i < 6; i++)
			{
				gameresult[i] = card[i];
			}
			winredblue = RBSetCardtype(gameresult, cardtype1, cardtype2, wincardtype);
		}
	}


	//有一半的机会将前面的结果互换
	if (GetRanNum() % 2 == 1)
	{
		int temp;
		for (i = 0; i < 3; i++)
		{
			temp = gameresult[i];
			gameresult[i] = gameresult[3 + i];
			gameresult[3 + i] = temp;
		}
		winredblue = (winredblue + 1) % 2;
	}

	winflg2 = 0;
	if (playerinfo[RBCTRL_RBBASECOIN] + 30 * playerinfo[RBCTRL_REDBLUEMAXBET] < 0 || playerinfo[RBCTRL_RBBASECOIN] + RBMAXGAMEWAVEDOWN*3/4 < 0)
	{
		if (GetRanNum() % 6 == 1)
		{
			winflg2 = 1;
		}
	}
	else if (playerinfo[RBCTRL_RBBASECOIN] >  30 * playerinfo[RBCTRL_REDBLUEMAXBET] || playerinfo[RBCTRL_RBBASECOIN] > RBMAXGAMEWAVEUP*2/3)
	{
		if (GetRanNum() % 6 == 1)
		{
			winflg2 = 2;
		}
	}


	if ((winredblue == 1 && playerinfo[RBCTRL_RBBASECOIN] + bet[0] * paybackrate[0]/10 > RBMAXGAMEWAVEUP + 10000) || (winredblue == 0 && playerinfo[RBCTRL_RBBASECOIN] + bet[1] * paybackrate[1]/10 > RBMAXGAMEWAVEUP+10000))
	{
		winflg2 = 2;
	}

	if ((winredblue == 1 && playerinfo[RBCTRL_RBBASECOIN] + bet[0] * paybackrate[0]/10 + RBMAXGAMEWAVEDOWN <  10000) || (winredblue == 0 && playerinfo[RBCTRL_RBBASECOIN] + bet[1] * paybackrate[1]/10 + RBMAXGAMEWAVEDOWN <  10000))
	{
		winflg2 = 1;
	}


	if (winflg2 == 2)
	{
		if ((bet[0] * paybackrate[0] > bet[1] * paybackrate[1] && winredblue == 1) || (bet[0] * paybackrate[0] < bet[1] * paybackrate[1] && winredblue == 0))
		{
			int temp;
			for (i = 0; i < 3; i++)
			{
				temp = gameresult[i];
				gameresult[i] = gameresult[3 + i];
				gameresult[3 + i] = temp;
			}
			winredblue = (winredblue + 1) % 2;
		}
	}
	else if (winflg2 == 1)
	{
		if ((bet[0] * paybackrate[0] < bet[1] * paybackrate[1] && winredblue == 1) || (bet[0] * paybackrate[0] > bet[1] * paybackrate[1] && winredblue == 0))
		{
			int temp;
			for (i = 0; i < 3; i++)
			{
				temp = gameresult[i];
				gameresult[i] = gameresult[3 + i];
				gameresult[3 + i] = temp;
			}
			winredblue = (winredblue + 1) % 2;
		}
	}





	//特殊牌型betout
	if (wincardtype[0] > 1)
	{
		long long allwinnum = paybackrate[wincardtype[0]] * bet[2];
		long long bonusout = 0;

		playerinfo[RBCTRL_NOWBASECOIN] += allwinnum;
		playerinfo[RBCTRL_BASECOIN] += allwinnum;
		playerinfo[RBCTRL_OUTTAIL] += allwinnum;


		playerinfo[RBCTRL_BASECOIN] += playerinfo[RBCTRL_OUTTAIL] / 1000 * REDBLACKGAMEWINRATE;
		bonusout = playerinfo[RBCTRL_OUTTAIL] / 1000 * REDBLACKBONUSRATE;

		playerinfo[RBCTRL_BONUSBASE] += bonusout;
		playerinfo[RBCTRL_BASECOIN] += bonusout;

		playerinfo[RBCTRL_OUTTAIL] = playerinfo[RBCTRL_OUTTAIL] % 1000;
	}

	//红蓝betout
	{
		long long allwinnum = 0;
		long long bonusout = 0;

		if (winredblue > 0)
		{
			allwinnum = bet[0] * paybackrate[0] / 10;
		}
		else
			allwinnum = bet[1] * paybackrate[1] / 10;

		playerinfo[RBCTRL_RBBASECOIN] += allwinnum;
		playerinfo[RBCTRL_OUTTAIL] += allwinnum;


		playerinfo[RBCTRL_RBBASECOIN] += playerinfo[RBCTRL_OUTTAIL] / 1000 * REDBLACKGAMEWINRATE;
		bonusout = playerinfo[RBCTRL_OUTTAIL] / 1000 * REDBLACKBONUSRATE;

		playerinfo[RBCTRL_BONUSBASE] += bonusout;
		playerinfo[RBCTRL_RBBASECOIN] += bonusout;

		playerinfo[RBCTRL_OUTTAIL] = playerinfo[RBCTRL_OUTTAIL] % 1000;
	}

	winredblue = RBSetCardtype(gameresult, cardtype1, cardtype2, wincardtype);
	win_result[0] = winredblue;
	win_result[1] = cardtype1[0];
	win_result[2] = cardtype2[0];
	win_result[3] = wincardtype[0];

	//备份
	playerinfo[RBCTRL_RBBASEBACK1] = playerinfo[RBCTRL_RBBASECOIN];
	playerinfo[RBCTRL_RBBASEBACK2] = playerinfo[RBCTRL_RBBASECOIN];

	playerinfo[RBCTRL_BACKBASECOIN1] = playerinfo[RBCTRL_BASECOIN];
	playerinfo[RBCTRL_BACKBASECOIN2] = playerinfo[RBCTRL_BASECOIN];


	return 1;
}

int RedblueGetBonusnum(int redbluectrlinfo[])
{
	int bonus;
	if (redbluectrlinfo[RBCTRL_INITSIGN] != 0x3643)
	{
		bonus = (9000 + GetRanNum() % 2000) * 10000;
	}
	else
		bonus = ((int)redbluectrlinfo[RBCTRL_ALLBETEV]) * 68 * 10 * REDBLACKBONUSRATE / 1000 + (GetRanNum() % 100) * 10000;

	return (int)bonus;
}

