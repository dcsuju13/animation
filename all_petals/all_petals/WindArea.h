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

	Vec3i Wind_Cross_Init();//生成横风的参数，元素内容为：风向x，风向y，风力，周期。
	vector<Vec3i> Wind_Whirl_Init(); // 生成旋风的参数，vector分别存放下，右，上，左方向的风的数据。
	Vec3f Cross_Wind_Caculate(Vec3i WindInfo,Vec2f coord,int clock); //输入风的信息和时刻、位置信息，获得这个位置时刻对应的风力。
	Vec3f Whirl_Wind_Caculate(vector<Vec3i> WindInfo,Vec2f coord, int clock); //输入风的信息和时刻、位置信息，获得这个位置时刻对应的风力。

private:
};
