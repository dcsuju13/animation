#include "WindArea.h"
#include "stdlib.h"
#include "time.h"

WindArea::WindArea(){}

Vec3i WindArea::Wind_Cross_Init()//生成横风的参数，元素内容为：与边的逆时针角度，风力，周期。
{
	Vec3i WindInfo;
	srand((unsigned)time(NULL));

	int egl = rand() % 31 + 15; //随机生成一个15-45°的方向角
	int force = rand() % 11 + 5; //随机生成一个5-15的风力
	int cycle = rand() % 6 + 3; //风的持续周期，随机范围3-8秒;

	WindInfo[0] = egl;
	WindInfo[1] = force;
	WindInfo[2] = cycle;

	return WindInfo;
}

vector<Vec3i> WindArea::Wind_Whirl_Init() // 生成旋风的参数，vector分别存放下，右，上，左方向的风的数据。
{
	vector<Vec3i> WhirlWindInfo;
	srand((unsigned)time(NULL));

	for (int i = 0; i < 4; i++)
	{
		Vec3i WindInfo;

		int egl = rand() % 41 + 15; //随机生成一个15-45°的方向角
		int force = rand() % 11 + 5; //随机生成一个5-15的风力
		int cycle = rand() % 6 + 3; //风的持续周期，随机范围3-8秒;

		WindInfo[0] = egl;
		WindInfo[1] = force;
		WindInfo[2] = cycle;

		WhirlWindInfo.push_back(WindInfo);
	}

	return WhirlWindInfo;
}

Vec3f WindArea::Cross_Wind_Caculate(Vec3i WindInfo, Vec2f coord, int TimeClock) //输入风的信息和时刻、位置信息，获得这个位置时刻对应的风力。
{
	Vec3f Result;
	//计算起风点与终点的位置信息
	float cosA = cos((float)(180 - WindInfo[0]) / 180 * 3.1415926); //起风线的斜率
	float sinA = sin((float)(180 - WindInfo[0]) / 180 * 3.1415926);
	float b1 = cosA*(-500) - sinA*(-500);//起风线的方程
	float b2 = cosA*(-500) - sinA*(500);
	float b;
	if (b1 < b2)
		b = b2;
	else
		b = b1;
	float WindEndDist = max(abs(sinA * 500 - cosA * 500 + b), abs(sinA * -500 - cosA * 500 + b));
	//代入位置坐标，计算衰减
	float NDist = abs(sinA * coord[0] - cosA * coord[1] + b);
	float decay = 1 - NDist / WindEndDist * 0.8;
	//根据周期信息，代入时间获取风力
	float Tu = 0.3 * WindInfo[2] * CLOCKS_PER_SEC; //起风所需时间(clock)
	float Td = 0.7 * WindInfo[2] * CLOCKS_PER_SEC; //起风+平稳持续的时间(clock)

	Result[0] = cos((float)(WindInfo[0]) / 180 * 3.1415926);
	Result[1] = sin((float)(WindInfo[0]) / 180 * 3.1415926);
	int TimeClockMod = TimeClock % (WindInfo[2] * CLOCKS_PER_SEC);
	if (TimeClockMod < Tu) //在起风阶段内
	{
		Result[2] = (float)WindInfo[1] / Tu * TimeClockMod;
	}
	else if (TimeClockMod < Td) //在平稳阶段内
	{
		Result[2] = WindInfo[1] + 0.2 * WindInfo[1] * sin(double(TimeClockMod - Tu) / CLOCKS_PER_SEC);
	}
	else  //在风结束的阶段
	{
		Result[2] = (WindInfo[1] + 0.2 * sin(double(Td - Tu) / CLOCKS_PER_SEC))*(1 - (double)(TimeClockMod - Td) / (double)(WindInfo[2] * CLOCKS_PER_SEC - Td));
	}


	return Result;
}

Vec3f WindArea::Whirl_Wind_Caculate(vector<Vec3i> WindInfo, Vec2f coord, int clock) //输入四个方向风的信息和时刻、位置信息，获得这个位置时刻对应的风力。
{
	Vec2f tforce;
	tforce[0] = 0;
	tforce[1] = 0;

	for (int i = 0; i < 4; i++) //对四个方向的风分别求解
	{
		Vec3i twind = WindInfo[i];
		//首先判断该位置是否在这一风场内
		float k = tan((float)(twind[0]) / 180 * 3.1415926);
		float b = 500 * (k - 1);
		//旋转输入的位置信息，使对应的底边为下方的边。
		float x, y;

		switch (i)
		{
		case 0:
			x = coord[0];y = coord[1];break;
		case 1:
			x = -coord[1];y = coord[0];break;
		case 2:
			x = -coord[0];y = -coord[1];break;
		case 3:
			x = coord[1]; y = -coord[0]; break;
		default:
			break;
		}

		if (y < k * x + b)//如果在风场范围内，那么带入函数计算风力大小
		{
			Vec2f tcoord;
			tcoord[0] = x;
			tcoord[1] = y;
			Vec3f windforce = Cross_Wind_Caculate(twind,tcoord,clock);
			//还需要对获得的风进行方向的旋转
			Vec2f winddirect;
			switch (i)
			{
			case 0:
				winddirect[0] = windforce[0]*windforce[2]; winddirect[1] = windforce[1]*windforce[2]; break;
			case 1:
				winddirect[0] = -windforce[1] * windforce[2]; winddirect[1] = windforce[0] * windforce[2]; break;
			case 2:
				winddirect[0] = -windforce[0] * windforce[2]; winddirect[1] = -windforce[1] * windforce[2]; break;
			case 3:
				winddirect[0] = windforce[1] * windforce[2]; winddirect[1] = -windforce[0] * windforce[2]; break;
			default:
				break;
			}
			tforce[0] = tforce[0] + winddirect[0];
			tforce[1] = tforce[1] + winddirect[1];
		}
	}

	Vec3f Result;
	Result[2] = sqrt(pow(tforce[0], 2) + pow(tforce[1], 2));
	if (Result[2] != 0)
	{
		Result[0] = tforce[0] / Result[2];
		Result[1] = tforce[1] / Result[2];
	}
	else
	{
		Result[0] = 0;
		Result[1] = 0;
	}

	return Result;
}

WindArea::~WindArea(){}

