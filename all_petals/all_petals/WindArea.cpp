#include "WindArea.h"
#include "stdlib.h"
#include "time.h"

WindArea::WindArea(){}

Vec3i WindArea::Wind_Cross_Init()//���ɺ��Ĳ�����Ԫ������Ϊ����ߵ���ʱ��Ƕȣ����������ڡ�
{
	Vec3i WindInfo;
	srand((unsigned)time(NULL));

	int egl = rand() % 31 + 15; //�������һ��15-45��ķ����
	int force = rand() % 11 + 5; //�������һ��5-15�ķ���
	int cycle = rand() % 6 + 3; //��ĳ������ڣ������Χ3-8��;

	WindInfo[0] = egl;
	WindInfo[1] = force;
	WindInfo[2] = cycle;

	return WindInfo;
}

vector<Vec3i> WindArea::Wind_Whirl_Init() // ��������Ĳ�����vector�ֱ����£��ң��ϣ�����ķ�����ݡ�
{
	vector<Vec3i> WhirlWindInfo;
	srand((unsigned)time(NULL));

	for (int i = 0; i < 4; i++)
	{
		Vec3i WindInfo;

		int egl = rand() % 41 + 15; //�������һ��15-45��ķ����
		int force = rand() % 11 + 5; //�������һ��5-15�ķ���
		int cycle = rand() % 6 + 3; //��ĳ������ڣ������Χ3-8��;

		WindInfo[0] = egl;
		WindInfo[1] = force;
		WindInfo[2] = cycle;

		WhirlWindInfo.push_back(WindInfo);
	}

	return WhirlWindInfo;
}

Vec3f WindArea::Cross_Wind_Caculate(Vec3i WindInfo, Vec2f coord, int TimeClock) //��������Ϣ��ʱ�̡�λ����Ϣ��������λ��ʱ�̶�Ӧ�ķ�����
{
	Vec3f Result;
	//�����������յ��λ����Ϣ
	float cosA = cos((float)(180 - WindInfo[0]) / 180 * 3.1415926); //����ߵ�б��
	float sinA = sin((float)(180 - WindInfo[0]) / 180 * 3.1415926);
	float b1 = cosA*(-500) - sinA*(-500);//����ߵķ���
	float b2 = cosA*(-500) - sinA*(500);
	float b;
	if (b1 < b2)
		b = b2;
	else
		b = b1;
	float WindEndDist = max(abs(sinA * 500 - cosA * 500 + b), abs(sinA * -500 - cosA * 500 + b));
	//����λ�����꣬����˥��
	float NDist = abs(sinA * coord[0] - cosA * coord[1] + b);
	float decay = 1 - NDist / WindEndDist * 0.8;
	//����������Ϣ������ʱ���ȡ����
	float Tu = 0.3 * WindInfo[2] * CLOCKS_PER_SEC; //�������ʱ��(clock)
	float Td = 0.7 * WindInfo[2] * CLOCKS_PER_SEC; //���+ƽ�ȳ�����ʱ��(clock)

	Result[0] = cos((float)(WindInfo[0]) / 180 * 3.1415926);
	Result[1] = sin((float)(WindInfo[0]) / 180 * 3.1415926);
	int TimeClockMod = TimeClock % (WindInfo[2] * CLOCKS_PER_SEC);
	if (TimeClockMod < Tu) //�����׶���
	{
		Result[2] = (float)WindInfo[1] / Tu * TimeClockMod;
	}
	else if (TimeClockMod < Td) //��ƽ�Ƚ׶���
	{
		Result[2] = WindInfo[1] + 0.2 * WindInfo[1] * sin(double(TimeClockMod - Tu) / CLOCKS_PER_SEC);
	}
	else  //�ڷ�����Ľ׶�
	{
		Result[2] = (WindInfo[1] + 0.2 * sin(double(Td - Tu) / CLOCKS_PER_SEC))*(1 - (double)(TimeClockMod - Td) / (double)(WindInfo[2] * CLOCKS_PER_SEC - Td));
	}


	return Result;
}

Vec3f WindArea::Whirl_Wind_Caculate(vector<Vec3i> WindInfo, Vec2f coord, int clock) //�����ĸ���������Ϣ��ʱ�̡�λ����Ϣ��������λ��ʱ�̶�Ӧ�ķ�����
{
	Vec2f tforce;
	tforce[0] = 0;
	tforce[1] = 0;

	for (int i = 0; i < 4; i++) //���ĸ�����ķ�ֱ����
	{
		Vec3i twind = WindInfo[i];
		//�����жϸ�λ���Ƿ�����һ�糡��
		float k = tan((float)(twind[0]) / 180 * 3.1415926);
		float b = 500 * (k - 1);
		//��ת�����λ����Ϣ��ʹ��Ӧ�ĵױ�Ϊ�·��ıߡ�
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

		if (y < k * x + b)//����ڷ糡��Χ�ڣ���ô���뺯�����������С
		{
			Vec2f tcoord;
			tcoord[0] = x;
			tcoord[1] = y;
			Vec3f windforce = Cross_Wind_Caculate(twind,tcoord,clock);
			//����Ҫ�Ի�õķ���з������ת
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

