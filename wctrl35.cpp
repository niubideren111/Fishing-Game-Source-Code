#include <time.h>
#include <stdlib.h>
#include "wctrl35.h"

enum
{	//WC  WIN CTRL
	WC35_INITSIGN,			//初始化标志
	WC35_PLAYTIMES,		//游戏次数
	WC35_BETOUTTAIL,
	WC35_BASECOIN,		   //净分
	WC35_AIMCOIN,

	WC35_NOWRANGE,          //实际输赢
	WC35_BASECOIN2,
	WC35_ST,
	WC35_USECZGX,
	WC35_EVMAXBET,

	WC35_BASECOIN1,

	WC35_BONUSBASE,
	WC35_USECZGXSIGN,
};


int UNIVERSAL_ICON = 9;

static int BSSET_UPMAXBETLV = 400;
static int BSSET_DOWNMAXBETLV = 600;
static int BSSET_MAXWIN = 8000000;
static int BSSET_BETOUTRATE = 20;
static int BSSET_BONUSOUTRATE = 0;

static int set_time_rand_sign = 0;
static int set_rand_numrange_sign = 0;

//void WC35SetGameInfo(int setarray[])
//{
//	BSSET_UPMAXBETLV = setarray[WC35SET_UPMAXBETLV_ID];
//	BSSET_DOWNMAXBETLV = setarray[WC35SET_DOWNMAXBETLV_ID];
//	BSSET_MAXWIN = setarray[WC35SET_MAXWIN_ID];
//	BSSET_BETOUTRATE = setarray[WC35SET_BETOUTRATE_ID];
//	BSSET_BONUSOUTRATE = setarray[WC35SET_BONUSOUTRATE_ID];
//}


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

static long long WC35GetBasecoin(long long basecoin1, long long basecoin2, long long basecoin3)
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

static void WC35GetOneLineResult(int result[], int sourceline[],int menbernum, int lineresult[])
{
	int iconid = 0;
	int num = 0;
	int i;
	int line[20];

	for (i = 0; i < menbernum;i++)
	{
		line[i] = sourceline[i] - 1;
	}

	for (i = 0; i < menbernum; i++)
	{
		if (result[line[i]] < UNIVERSAL_ICON)
		{
			iconid = result[line[i]];
			break;
		}
	}

	if (iconid == 0)
	{
		lineresult[0] = UNIVERSAL_ICON;
		lineresult[1] = menbernum;
		return;
	}

	for (i = 0; i < menbernum; i++)
	{
		if (result[line[i]] == UNIVERSAL_ICON || result[line[i]] == iconid)
		{
			num++;
		}
		else
			break;
	}
	lineresult[0] = iconid;
	lineresult[1] = num;
}

int WC35GetGameWin(int result[], int bet)
{
	int lines[9][5] = {
		{ 6, 7, 8, 9, 10 },
		{ 1, 2, 3, 4, 5 },
		{ 11, 12, 13, 14, 15 },

		{ 1, 7, 13, 9, 5 },
		{ 11, 7, 3, 9, 15 },

		{ 1, 2, 8, 4, 5 },
		{ 11, 12, 8, 14, 15 },

		{ 6, 2, 3, 4, 10 },
		{ 6, 12, 13, 14, 10 },
	};
	int linesnum = 9;
	int onebet = bet / linesnum;
	int paybackrate[9][6] = {
		{ 0, 0, 2, 5, 20, 50 },
		{ 0, 0, 3, 10, 40, 100 },
		{ 0, 0, 5, 15, 60, 150 },
		{ 0, 0, 7, 20, 100, 250 },
		{ 0, 0, 10, 30, 160, 400 },

		{ 0, 0, 15, 40, 200, 500 },
		{ 0, 0, 20, 80, 400, 1000 },
		{ 0, 0, 50, 200, 1000, 2500 },
		{ 0, 0, 0, 0, 2000, 5000 },
	};
	int i;
	int allwin = 0;
	int allline[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
	int allwin2 = 0;
	int onelineresult[2];
	int templine[5];
	int j;

	WC35GetOneLineResult(result, allline, 15, onelineresult);
	if (onelineresult[1] >= 15)
	{
		return paybackrate[onelineresult[0] - 1][5] * bet;
	}

	for (i = 0; i < linesnum;i++)
	{
		WC35GetOneLineResult(result, lines[i], 5, onelineresult);
		if (onelineresult[1] >2)
		{
			allwin += paybackrate[onelineresult[0] - 1][onelineresult[1]-1] * onebet;
		}

		if (onelineresult[1] < 5)
		{
			for (j = 0; j < 5;j++)
			{
				templine[j] = lines[i][4 - j];
			}
			WC35GetOneLineResult(result, templine, 5, onelineresult);
			if (onelineresult[1] > 2)
			{
				allwin += paybackrate[onelineresult[0] - 1][onelineresult[1]-1] * onebet;
			}
		}
	}

	return allwin;
}

//图标的值从1到9
static int WC35GetOneIcon(int iconrate[], int allnum,int num)
{
	int i;
	int rannum = GetRanNum() % allnum;

	for (i = 0; i < num;i++)
	{
		if (rannum <iconrate[i])
		{
			return i + 1;
		}
	}
	return 1;
}

static void WC35GetGameResult(int iconrate[],int result[])
{
	int i;
	int tempiconrate[9];

	tempiconrate[0] = iconrate[0];
	for (i = 1; i < UNIVERSAL_ICON; i++)
	{
		tempiconrate[i] = iconrate[i];
		tempiconrate[i] = tempiconrate[i] + tempiconrate[i - 1];
	}
	for (i = 0; i < 15;i++)
	{
		result[i] = WC35GetOneIcon(tempiconrate, tempiconrate[UNIVERSAL_ICON - 1], UNIVERSAL_ICON);
	}
}

static void WC35SetGameResult(long long playerinfo[], int gameresult[])
{
	int winrate = 40;
	int rannum = GetRanNum() % 100;
	int i;
	int winlimit[] = { 
		5, 3500,
		5, 4500,
		5, 2500,
	};
	int iconrate[] = { 230, 160, 140, 110, 90, 80, 60, 40, 73 };   //98.40%
	int iconrate1[] = { 230, 160, 140, 110, 90, 80, 60, 40, 83 };   //108.23%
	int iconrate2[] = { 230, 160, 140, 110, 90, 80, 60, 40, 65 };   //91.68%
	int iconrate3[] = { 230, 160, 140, 110, 90, 80, 60, 40, 150 };   
	int iconrate4[] = { 160, 230, 140, 110, 90, 80, 60, 40, 200 };   
	int *piconrate;
	int tempwinnum;

	//playerinfo[WC35_ST] = 0;
	//playerinfo[WC35_PLAYTIMES] = 30;
	//WC35GetGameResult(iconrate2, gameresult);
	//return;

	if (playerinfo[WC35_USECZGXSIGN] > 0)
	{
		piconrate = iconrate3;
		if (playerinfo[WC35_USECZGXSIGN] == 1)
		{
			int temprand = GetRanNum() % 666;
			winlimit[0] = 60;
			winlimit[1] = 500;

			if (temprand == 8)
			{
				for (i = 0; i < 15; i++)
				{
					gameresult[i] = 1;
				}

				return;
			}

		}
		else if (playerinfo[WC35_USECZGXSIGN] == 2)
		{
			int temprand = GetRanNum() % 666;
			winlimit[0] = 600;
			winlimit[1] = 2000;
			piconrate = iconrate4;

			if (temprand < 5)
			{
				for (i = 0; i < 15;i++)
				{
					gameresult[i] = 1;
				}
				
				return;
			}
			else if (temprand == 66 || temprand == 6)
			{
				for (i = 0; i < 15; i++)
				{
					gameresult[i] = 2;
				}
				return;
			}
			else if (temprand == 88)
			{
				for (i = 0; i < 15; i++)
				{
					gameresult[i] = 3;
				}
				return;
			}
		}

		for (i = 0; i < 1000; i++)
		{
			WC35GetGameResult(piconrate, gameresult);
			tempwinnum = WC35GetGameWin(gameresult, 9);
			if (tempwinnum >= winlimit[0] && tempwinnum <= winlimit[1])
			{
				break;
			}
		}
		//extern int outnumall;
		//extern int outnumall2;
		//extern long long gxallwinnum;
		//outnumall2++;
		//if (i >= 1000)
		//{
		//	outnumall++;
		//}

		//gxallwinnum += tempwinnum;
		return;
	}
	


	if (rannum < winrate)
	{
		if (playerinfo[WC35_ST] == 0)
		{
			if (playerinfo[WC35_PLAYTIMES] < 28 && GetRanNum() % 4 == 1)
			{
				winlimit[0] = 10;
				winlimit[1] = 200;
			}
			for (i = 0; i < 100; i++)
			{
				WC35GetGameResult(iconrate, gameresult);
				tempwinnum = WC35GetGameWin(gameresult, 9);
				if (tempwinnum >= winlimit[0] && tempwinnum <= winlimit[1])
				{
					break;
				}
			}
		}
		else if (playerinfo[WC35_ST] == 1)
		{
			int temprand = GetRanNum() % 1688;
			if (temprand < 3)
			{
				for (i = 0; i < 15; i++)
				{
					gameresult[i] = 1;
				}

				return;
			}
			else if (temprand == 66 )
			{
				for (i = 0; i < 15; i++)
				{
					gameresult[i] = 2;
				}
				return;
			}

			for (i = 0; i < 100; i++)
			{
				WC35GetGameResult(iconrate1, gameresult);
				tempwinnum = WC35GetGameWin(gameresult, 9);
				if (tempwinnum >= winlimit[2] && tempwinnum <= winlimit[3])
				{
					break;
				}
			}
		}
		else
		{ 
			for (i = 0; i < 100; i++)
			{
				WC35GetGameResult(iconrate2, gameresult);
				tempwinnum = WC35GetGameWin(gameresult, 9);
				if (tempwinnum >= winlimit[4] && tempwinnum <= winlimit[5])
				{
					break;
				}
			}
		}
	}
	else
	{
		for (i = 0; i < 100;i++)
		{
			WC35GetGameResult(iconrate,gameresult);
			if (WC35GetGameWin(gameresult,9) == 0)
			{
				break;
			}
		}
	}
}

void WC35GameResult(long long wc35playerinfo[], long long czgongxian[], int bet, int game35result[], int setinfo[])
{
	int rannum = GetRanNum();
	long long *playerinfo = wc35playerinfo;
	int usingczgxsign = 0;

	if (playerinfo[WC35_INITSIGN] != 0xcfde)
	{
		playerinfo[WC35_INITSIGN] = 0xcfde;
		playerinfo[WC35_PLAYTIMES] = 0;
		playerinfo[WC35_BETOUTTAIL] = 0;
		playerinfo[WC35_BASECOIN] = 0;
		playerinfo[WC35_AIMCOIN] = 0;

		playerinfo[WC35_NOWRANGE] = 0;
		playerinfo[WC35_BASECOIN2] = 0;
		playerinfo[WC35_ST] = 0;
		playerinfo[WC35_USECZGX] = 0;
		playerinfo[WC35_EVMAXBET] = 1000;

		playerinfo[WC35_BASECOIN1] = 0;
		playerinfo[WC35_BONUSBASE] = 0;
		playerinfo[WC35_USECZGXSIGN] = 0;
	}

	playerinfo[WC35_BASECOIN] = WC35GetBasecoin(playerinfo[WC35_BASECOIN], playerinfo[WC35_BASECOIN1], playerinfo[WC35_BASECOIN2]);

	playerinfo[WC35_EVMAXBET] = (playerinfo[WC35_EVMAXBET] * 9 + bet) / 10;


	if (playerinfo[WC35_ST] == 0)
	{
		if (playerinfo[WC35_BASECOIN] > playerinfo[WC35_EVMAXBET] * setinfo[ID_35SET_UPMAXBETLV] || playerinfo[WC35_NOWRANGE] + 100000 > setinfo[ID_35SET_MAXWIN])
		{
			playerinfo[WC35_ST] = 2;
			playerinfo[WC35_AIMCOIN] = playerinfo[WC35_EVMAXBET] * setinfo[ID_35SET_UPMAXBETLV] - (playerinfo[WC35_EVMAXBET] * setinfo[ID_35SET_DOWNMAXBETLV]) / 4 * (GetRanNum() % 7 + 1);
			if (playerinfo[WC35_AIMCOIN] + playerinfo[WC35_EVMAXBET] * setinfo[ID_35SET_DOWNMAXBETLV] < 0)
			{
				playerinfo[WC35_AIMCOIN] = 0 - playerinfo[WC35_EVMAXBET] * setinfo[ID_35SET_DOWNMAXBETLV];
			}
		}
		else if (playerinfo[WC35_BASECOIN] + playerinfo[WC35_EVMAXBET] * setinfo[ID_35SET_DOWNMAXBETLV] < 0)
		{
			playerinfo[WC35_ST] = 1;
			playerinfo[WC35_AIMCOIN] = (playerinfo[WC35_EVMAXBET] * setinfo[ID_35SET_DOWNMAXBETLV]) / 4 * (1 + GetRanNum() % 5) - playerinfo[WC35_EVMAXBET] * setinfo[ID_35SET_DOWNMAXBETLV];
			if (playerinfo[WC35_AIMCOIN] > playerinfo[WC35_EVMAXBET] * setinfo[ID_35SET_UPMAXBETLV])
			{
				playerinfo[WC35_AIMCOIN] = playerinfo[WC35_EVMAXBET] * setinfo[ID_35SET_UPMAXBETLV];
			}
		}
		else
		{
			if (rannum % 3880 < 10)
			{
				if (rannum % 2 == 0)
				{
					playerinfo[WC35_ST] = 2;
					playerinfo[WC35_AIMCOIN] = playerinfo[WC35_BASECOIN] - (playerinfo[WC35_EVMAXBET] * setinfo[ID_35SET_DOWNMAXBETLV]) / 6 * (GetRanNum() % 8 + 1);
					if (playerinfo[WC35_AIMCOIN] + playerinfo[WC35_EVMAXBET] * setinfo[ID_35SET_DOWNMAXBETLV] < 0)
					{
						playerinfo[WC35_AIMCOIN] = 0 - playerinfo[WC35_EVMAXBET] * setinfo[ID_35SET_DOWNMAXBETLV];
					}
				}
				else
				{
					playerinfo[WC35_ST] = 1;
					playerinfo[WC35_AIMCOIN] = playerinfo[WC35_BASECOIN] + (playerinfo[WC35_EVMAXBET] * setinfo[ID_35SET_UPMAXBETLV]) / 6 * (GetRanNum() % 6 + 1);
					if (playerinfo[WC35_AIMCOIN] > playerinfo[WC35_EVMAXBET] * setinfo[ID_35SET_UPMAXBETLV])
					{
						playerinfo[WC35_AIMCOIN] = playerinfo[WC35_EVMAXBET] * setinfo[ID_35SET_UPMAXBETLV];
					}
				}
			}
		}
	}
	else if (playerinfo[WC35_ST] == 1)
	{
		if (playerinfo[WC35_BASECOIN] > playerinfo[WC35_AIMCOIN] || playerinfo[WC35_BASECOIN] > playerinfo[WC35_EVMAXBET] * setinfo[ID_35SET_UPMAXBETLV] || playerinfo[WC35_NOWRANGE] + 100000 > setinfo[ID_35SET_MAXWIN])
		{
			playerinfo[WC35_ST] = 0;
		}
	}
	else
	{
		if (playerinfo[WC35_BASECOIN] < playerinfo[WC35_AIMCOIN] || playerinfo[WC35_BASECOIN] + playerinfo[WC35_EVMAXBET] * setinfo[ID_35SET_DOWNMAXBETLV] < 0)
		{
			playerinfo[WC35_ST] = 0;
		}
	}


	if (playerinfo[WC35_USECZGX] >= 10 * bet && GetRanNum()%5 < 3)
	{
		usingczgxsign = 1;
	}

	if (czgongxian[0] > 10 * bet && GetRanNum() % 5 == 1)
	{
		playerinfo[WC35_USECZGX] += 10 * bet;
		czgongxian[0] -= 10 * bet;
		usingczgxsign = 1;
	}

	if (czgongxian[0] >= 100 * bet && GetRanNum() % 28 == 1 && playerinfo[WC35_USECZGX] < 100*bet)
	{
		playerinfo[WC35_USECZGX] += 100 * bet;
		czgongxian[0] -= 100 * bet;
		usingczgxsign = 2;
	}

	playerinfo[WC35_USECZGXSIGN] = usingczgxsign;
	
	playerinfo[WC35_PLAYTIMES]++;
	playerinfo[WC35_BASECOIN] -= bet;
	playerinfo[WC35_NOWRANGE] -= bet;

	WC35SetGameResult(playerinfo, game35result);

	playerinfo[WC35_BETOUTTAIL] += bet;
	playerinfo[WC35_BASECOIN] += playerinfo[WC35_BETOUTTAIL] / 1000 * (setinfo[ID_35SET_BETOUTRATE] + BSSET_BONUSOUTRATE);
	playerinfo[WC35_BONUSBASE] += playerinfo[WC35_BETOUTTAIL] / 1000 * BSSET_BONUSOUTRATE;
	playerinfo[WC35_BETOUTTAIL] = playerinfo[WC35_BETOUTTAIL] % 1000;


	//获奖结算
	{
		long long gamewinnum = 0;
		gamewinnum = WC35GetGameWin(game35result, bet);

		if (playerinfo[WC35_USECZGXSIGN] > 0)
		{
			//extern long long gzwinnum;
			//gzwinnum += gamewinnum;
			playerinfo[WC35_USECZGXSIGN] = 0;
			playerinfo[WC35_USECZGX] -= gamewinnum;

			if (playerinfo[WC35_USECZGX] < 0)
			{
				if (playerinfo[WC35_USECZGX] + czgongxian[0] >= 0)
				{
					czgongxian[0] = playerinfo[WC35_USECZGX] + czgongxian[0];
					playerinfo[WC35_USECZGX] = 0;
				}
				else if (czgongxian[0] > 0)
				{
					playerinfo[WC35_USECZGX] = playerinfo[WC35_USECZGX] + czgongxian[0];
					czgongxian[0] = 0;
				}
			}
		}
		else
		{
			playerinfo[WC35_BASECOIN] += gamewinnum;
			playerinfo[WC35_NOWRANGE] += gamewinnum;
		}

	}

	playerinfo[WC35_BASECOIN1] = playerinfo[WC35_BASECOIN];
	playerinfo[WC35_BASECOIN2] = playerinfo[WC35_BASECOIN];
}
