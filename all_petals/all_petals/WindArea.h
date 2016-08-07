#pragma once
#include <QPoint>
#include <vector>
#include <cmath>
#include <cv.h>
#include "global.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

class WindArea
{
public:
	WindArea();
	~WindArea();

	Vec3i Wind_Cross_Init();//���ɺ��Ĳ�����Ԫ������Ϊ������x������y�����������ڡ�
	vector<Vec3i> Wind_Whirl_Init(); // ��������Ĳ�����vector�ֱ����£��ң��ϣ�����ķ�����ݡ�
	Vec3f Cross_Wind_Caculate(Vec3i WindInfo,Vec2f coord,int clock); //��������Ϣ��ʱ�̡�λ����Ϣ��������λ��ʱ�̶�Ӧ�ķ�����
	Vec3f Whirl_Wind_Caculate(vector<Vec3i> WindInfo,Vec2f coord, int clock); //��������Ϣ��ʱ�̡�λ����Ϣ��������λ��ʱ�̶�Ӧ�ķ�����

private:
};
