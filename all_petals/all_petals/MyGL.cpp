#include "MyGL.h"


MyGL::MyGL()
{
	rotationX = 0;
	rotationY = 0;
	rotationZ = 0;
	translationX = 0;
	translationY = 0;
	translationZ = 0;

	//加载网格
	ply = loadPLY("all.ply","texture.txt");
	ply.Analyse();
	points = ply.getPoints();
	lastPPos = points;
	faces = ply.getFaces();
	normals = ply.getNormals();
	axis = ply.getAxisIndex();
	petal_num = ply.getPetal_num();
	single_petal = ply.getPetal_single();
	tex_coor = ply.getTexture();

	//初始化lastData
	for (int i = 0; i < points.size(); i++){
		lastData.push_back({ { 0, 0, 0 }, { 0, 0, 0 } });
		lastMove.push_back({ 0, 0, 0 });
	}
	//花瓣宽度和高度
	//a,b为花瓣最宽的两个点
	coor a = points[(single_petal - 1) / 2 - 1];
	coor b = points[(single_petal - 1) / 2 + 1];
	p_width = sqrtf(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
	p_length = sqrtf(pow(points.front().x, 2) + pow(points.front().y, 2) + pow(points.front().z, 2));

	h = points.front().z + 5;//高度
	distance = sqrtf(pow(points.front().x, 2) + pow(points.front().y, 2));
	
	wind = WindArea();


	//Fw = {0,0,20};
	Fw = { 0, 0, 0 };
	lastFw = { 0, 0, 0 };

	startTimer(10);//开启计时器，单位ms
	starttime = clock();
	windinfo = wind.Wind_Whirl_Init();//旋风
	//crossinfo = wind.Wind_Cross_Init();//定向
	deltatime = 0;

}



MyGL::~MyGL()
{
}

void MyGL::loadGLTexture()
{
	QImage tex, buf;
	if (!buf.load("egg.jpg")){
		QImage dummy(128, 128, QImage::Format_RGB32);
		dummy.fill(QColor(Qt::green).rgb());
		buf = dummy;
	}

	tex = QGLWidget::convertToGLFormat(buf);

	//创建一个纹理名字
	glGenTextures(1, &texture[0]);
	//将纹理名字绑定到目标上
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	//创建纹理
	glTexImage2D(GL_TEXTURE_2D, 0, 3, tex.width(), tex.height(), 0,
		GL_RGBA, GL_UNSIGNED_BYTE, tex.bits());

	gheight = tex.height();//图片高度
	gwidth = tex.width();//图片宽度

	//设置显示图像时的放大、缩小的滤波方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


void MyGL::initializeGL()
{

	setGeometry(300, 150, 800, 600);//设置窗口初始位置和大小
	
	/*
	//载入纹理
	loadGLTexture();
	//启用纹理
	glEnable(GL_TEXTURE_2D);
	*/
	
	
	

	//qglClearColor(Qt::black);
	glClearColor(0.0, 0.0, 0.0, 0);
	//glShadeModel(GL_FLAT);
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0);//设置深度缓存
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);//设置深度测试类型
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);//进行透视校正


}

void MyGL::resizeGL(int width, int height)
{
	GLfloat nrange = 400.0f;
	if (0 == height)
		height = 1;//防止一条边为0
	glViewport(0, 0, (GLint)width, (GLint)height);//重置当前视口，本身不是重置窗口的，只不过是这里被Qt给封装好了
	glMatrixMode(GL_PROJECTION);//选择投影矩阵
	glLoadIdentity();//重置选择好的投影矩阵
	//裁剪区域
	/*if (width <= height)
	glOrtho(-nrange, nrange, -nrange*height / width, nrange*height / width, -nrange, nrange);
	else
	glOrtho(-nrange*width / height, nrange*width / height, -nrange, nrange, -nrange, nrange);
	*/

	gluPerspective(70.0f, (GLfloat)width / (GLfloat)height, 0, 2 * nrange);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void MyGL::paintGL()
{

	drawMesh();

	//updateGL();
	glFlush();
}



void MyGL::drawMesh()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0, 0, -600);
	glTranslatef(translationX, translationY, translationZ);

	glRotatef(rotationX, 1.0, 0.0, 0.0);
	glRotatef(rotationY, 0.0, 1.0, 0.0);
	glRotatef(rotationZ, 0.0, 0.0, 1.0);


	//绘制坐标轴
	glLineWidth(3);
	glColor3f(1.0f, 0.0f, 0.0f);//x
	glBegin(GL_LINES);
	glVertex3f((GLfloat)0, (GLfloat)0, (GLfloat)0);
	glVertex3f((GLfloat)100, (GLfloat)0, (GLfloat)0);
	glEnd();

	glColor3f(0.0f, 1.0f, 0.0f);//y
	glBegin(GL_LINES);
	glVertex3f((GLfloat)0, (GLfloat)0, (GLfloat)0);
	glVertex3f((GLfloat)0, (GLfloat)100, (GLfloat)0);
	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);//z
	glBegin(GL_LINES);
	glVertex3f((GLfloat)0, (GLfloat)0, (GLfloat)0);
	glVertex3f((GLfloat)0, (GLfloat)0, (GLfloat)100);
	glEnd();

	glLineWidth(1);
	glPointSize(3);


	//画点
	glColor3f(1.0f, 1.0f, 1.0f);


	for (int i = 0; i < faces.size(); i++)
	{
		coor a = lastPPos[faces[i].x];
		coor b = lastPPos[faces[i].y];
		coor c = lastPPos[faces[i].z];

		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_LINES);
		glVertex3f((GLfloat)a.x, (GLfloat)a.y, (GLfloat)a.z);
		glVertex3f((GLfloat)b.x, (GLfloat)b.y, (GLfloat)b.z);

		glVertex3f((GLfloat)a.x, (GLfloat)a.y, (GLfloat)a.z);
		glVertex3f((GLfloat)c.x, (GLfloat)c.y, (GLfloat)c.z);

		glVertex3f((GLfloat)b.x, (GLfloat)b.y, (GLfloat)b.z);
		glVertex3f((GLfloat)c.x, (GLfloat)c.y, (GLfloat)c.z);
		glEnd();

		/*
		 设置纹理坐标和物体几何坐标 
		glBegin(GL_TRIANGLES);
		double t[2] = { tex_coor[((int)faces[i].x) % tex_coor.size()].y() / (gwidth*1.0), (gheight - tex_coor[((int)faces[i].x) % tex_coor.size()].x()) / (gheight*1.0) };
		glTexCoord2f(tex_coor[((int)faces[i].x) % tex_coor.size()].y() / (gwidth*1.0), (gheight - tex_coor[((int)faces[i].x) % tex_coor.size()].x()) / (gheight*1.0));
		glVertex3f((GLfloat)a.x, (GLfloat)a.y, (GLfloat)a.z);

		double t1[2] = { tex_coor[((int)faces[i].y) % tex_coor.size()].y() / (gwidth*1.0), (gheight - tex_coor[((int)faces[i].y) % tex_coor.size()].x()) / (gheight*1.0) };
		glTexCoord2f(tex_coor[((int)faces[i].y) % tex_coor.size()].y() / (gwidth*1.0), (gheight - tex_coor[((int)faces[i].y) % tex_coor.size()].x()) / (gheight*1.0));
		glVertex3f((GLfloat)b.x, (GLfloat)b.y, (GLfloat)b.z);

		double t2[2] = { tex_coor[((int)faces[i].z) % tex_coor.size()].y() / (gwidth*1.0), (gheight - tex_coor[((int)faces[i].z) % tex_coor.size()].x()) / (gheight*1.0) };
		glTexCoord2f(tex_coor[((int)faces[i].z) % tex_coor.size()].y() / (gwidth*1.0), (gheight - tex_coor[((int)faces[i].z) % tex_coor.size()].x()) / (gheight*1.0));
		glVertex3f((GLfloat)c.x, (GLfloat)c.y, (GLfloat)c.z);


		glEnd();
		*/
		
		



		//rotationZ += 0.2;
	}


	/*
	t++;

	lastFw = Fw;
	Fw = {  5 * sin(0.1 * t ), 0, 0 };
//	Fw = { 0, 0,  -2 * sin(0.1*t + 1)};
//	Fw = { 0, -2 * sin(0.1*t ), 0 };

	*/

	nowtime = clock();
	lastdeltatime = deltatime;
	deltatime = (double)(nowtime - starttime); //需要获取风力时，先利用clock获得此时的时间

	for (int j = 0; j < petal_num; j++)
	{
		int offset = j*single_petal;//偏移量
		Vec2f coord;
		coord[0] = points[offset].x;
		coord[1] = points[offset].y;
		

		//代入函数求解风力，这里代入的是计算旋风场的风力，返回值（x方向，y方向，风力）其中x与y正交
		Vec3f windforce = wind.Whirl_Wind_Caculate(windinfo, coord, (int)deltatime);
		Vec3f windforcelast = wind.Whirl_Wind_Caculate(windinfo, coord, (int)lastdeltatime);
		//Vec3f windforce = wind.Cross_Wind_Caculate(crossinfo, coord, (int)deltatime);
		//Vec3f windforcelast = wind.Cross_Wind_Caculate(crossinfo, coord, (int)lastdeltatime);

		
		if (j == 0)
		{
			testclock[count] = deltatime;
			testforce[count] = windforce[2];
			count++;
		}
		if (count == 1000)
		{
			count = 1;
		}

		Fw = { windforce[0] * windforce[2], windforce[1] * windforce[2], 0 };
		lastFw = { windforcelast[0] * windforcelast[2], windforcelast[1] * windforcelast[2], 0 };

		double Fw1 = sqrtf(pow(Fw.x, 2) + pow(Fw.y, 2) + pow(Fw.z, 2));
		double lastFw1 = sqrtf(pow(lastFw.x, 2) + pow(lastFw.y, 2) + pow(lastFw.z, 2));

		//如果风力减为0或者反向了，所有复位
		if ((dot(Fw, lastFw) < 0) || (fabs(Fw1) < 0.15))
		{
			lastPPos = points;
			for (int i = 0; i < single_petal; i++)
			{
				lastData[i+offset][0] = { 0, 0, 0 };
				lastData[i+offset][1] = { 0, 0, 0 };
			}
		}
		else if (Fw1 >= lastFw1)
		{
			flag_resume = false;
			if (checkThreshold(offset))
			{
				//求解下一帧

				for (int i = axis.size() - 2; i > 0; i--)
				{
					if (checkThreshold(offset))
					{
						//求解下一帧
						caculateAxisMove(axis[i] + offset, offset);
					}
				}

			}
		}
		else//力减小，开始恢复
		{
			flag_resume = true;
			resume(offset);
		
		}


	}




	

	/*
	//力增大
	//如果风力减为0或者反向了，所有复位
	if ((dot(Fw, lastFw) < 0) || (fabs(Fw1) < 0.15))
	{
		lastPPos = points;
		for (int i = 0; i < points.size(); i++){
			lastData[i][0]={ 0, 0, 0 };
			lastData[i][1] = { 0, 0, 0 };
		}

	}
	//else if (Fw1 == lastFw1)
	//lastPPos = lastPPos;
	else if (Fw1 >= lastFw1)
	{
		flag_resume = false;
		for (int j = 0; j < petal_num; j++)//每个花瓣
		{
			int offset = j*single_petal;//偏移量
			if (checkThreshold(offset))
			{
				//求解下一帧
			
					for (int i = axis.size() - 2; i > 0; i--)
					{
						if (checkThreshold(offset))
						{
							//求解下一帧
							caculateAxisMove(axis[i]+offset,offset);
						}
					}
			
			}
		}

	}
	else//力减小，开始恢复
	{
		flag_resume = true;//标记为恢复
		//求解下一帧，恢复
		for (int j = 0; j < petal_num; j++)//每个花瓣
		{
			int offset = j*single_petal;//偏移量
			
			resume(offset);
					
		}
		

	}

	*/


	glPointSize(1);

	//使得能够旋转
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.5f, 0.0f, 0.0f);
	glEnd();


	return;
}

void MyGL::timerEvent(QTimerEvent *)
{
	updateGL();
}


void MyGL::mousePressEvent(QMouseEvent *event)
{

	lastPos = event->pos();
}


void MyGL::mouseMoveEvent(QMouseEvent *event)
{
	GLfloat dx = GLfloat(event->x() - lastPos.x()) / width();
	GLfloat dy = GLfloat(event->y() - lastPos.y()) / height();
	if (event->buttons() & Qt::LeftButton){
		rotationX += 180 * dy;
		rotationY += 180 * dx;
		updateGL();
	}
	else if (event->buttons() & Qt::RightButton){
		rotationX += 180 * dy;
		rotationZ += 180 * dx;
		updateGL();
	}
	lastPos = event->pos();

}

void MyGL::wheelEvent(QWheelEvent *e)
{
	if (e->orientation() == Qt::Horizontal)
	{

	}
	else{
		GLfloat zValue = e->delta() / 8;
		translationZ -= zValue * 5;
		if (translationZ>1.0)
		{
			translationZ -= zValue * 5;

		}

	}
	updateGL();
	return;
}

void MyGL::caculateAxisMove(int index,int offset)//计算该帧位移
{
	coor info1 = lastData[index][0];//方向1的上一帧a,w,x
	coor info2 = lastData[index][1];//方向2的上一帧a,w,x


	//两个旋转方向的方向向量
	vector<int>::iterator it = std::find(axis.begin(), axis.end(), index-offset);//获得该中轴点在axis中的位置
	if (it == axis.end()) return;
	int frontindex = *(++it)+offset;//获取前一个中轴点的下标(高度低于当前中轴点的)
	int ffrontindex;

	if (frontindex == offset+single_petal - 1)//前一中轴点已经为花心坐标，即端点
		ffrontindex = frontindex;
	else
		ffrontindex = frontindex >= offset+single_petal - 3 ? offset+single_petal - 1 : frontindex + 3;
	coor thisp = lastPPos[index];//要计算位移的中轴点
	coor lastp = lastPPos[frontindex];//上一个中轴点
	coor llastp = lastPPos[ffrontindex];//再上一个中轴点
	coor besidep;
	if (frontindex>0 && frontindex<axis.size() - 1)
		besidep = lastPPos[frontindex + 1];//要计算中轴点的一个对应轮廓点，用于计算方向2
	else
		besidep = { lastPPos[index + 1].x, lastPPos[index + 1].y, lastPPos[frontindex].z };//要计算中轴点的一个对应轮廓点，用于计算方向2

	coor v1;
	if (ffrontindex != frontindex)
		v1 = { lastp.x - llastp.x, lastp.y - llastp.y, lastp.z - llastp.z };//方向一：沿着上一段的中轴的方向
	else
		v1 = { 0.1, 0.1, 0.1 };


	coor t = normals[frontindex];

	//转轴向量
	coor r = { thisp.x - lastp.x, thisp.y - lastp.y, thisp.z - lastp.z };
	double l = sqrtf(powf(r.x, 2) + powf(r.y, 2) + powf(r.z, 2));
	//-------------------------test  v2
	coor v2 = cross(r, t);//方向二：沿着平行方向

	//单位化
	double norm = sqrtf(powf(v1.x, 2) + powf(v1.y, 2) + powf(v1.z, 2));
	v1.x /= norm; v1.y /= norm; v1.z /= norm;
	norm = sqrtf(powf(v2.x, 2) + powf(v2.y, 2) + powf(v2.z, 2));
	v2.x /= norm; v2.y /= norm; v2.z /= norm;


	//两个方向的等式右边的值
	t = cross(r, Fw);
	double T1 = dot(cross(r, Fw), v1);
	double T2 = dot(cross(r, Fw), v2);

	//质量 刚度 阻尼
	double m = (index + 3) * 150;//质量
	double K = (index + 2)*(index + 4) * 50 / l;//刚度
	double D = 1.2*K + (p_length*p_width) / 5000;//阻尼

	//转动惯量
	double I = m*(powf(r.x, 2) + powf(r.y, 2) + powf(r.z, 2));

	//角加速度
	double a1 = (T1 - 3 * D*info1.y - K*info1.z) / I;//侧面转动幅度小
	double a2 = (T2 - D*info2.y - K*info2.z) / I;



	if (fabs(info1.y) < 1e-7&&a1 < 0)
		info1.y = -0.001;
	else if (fabs(info1.y) < 1e-7&&a1 > 0)
		info1.y = 0.001;

	if (fabs(info2.y) < 1e-7&&a2 < 0)
		info2.y = -0.001;
	else if (fabs(info2.y) < 1e-7&&a2 > 0)
		info2.y = 0.001;


	//角速度
	double delta = 0.5;
	double w1 = info1.y + a1*delta;
	double w2 = info2.y + a2*delta;


	//从原始点位置到现在角位移（绕上一个中轴点）
	double x1 = info1.z + 2 * (w1 + info1.y)*delta / 2;
	double x2 = info2.z + 2 * (w2 + info2.y)*delta / 2;


	//旋转该点
	//coor rotated = this->PointRotate(v1,lastPPos[index],x1);//沿着方向1旋转
	//rotated = this->PointRotate(v2,rotated,-x2);//沿着方向2旋转

	//-----------------------------test
	double angle1 = 2 * (w1 + info1.y)*delta / 2;//方向v1位移应比较小
	double angle2 = 2 * (w2 + info2.y)*delta / 2;

	/*
	//区分是否为恢复
	if (flag_resume)//恢复
	{
	angle1 = -5 * info1.y*delta;
	angle2 = -5 * info2.y*delta;
	}
	*/

	coor rotated = PointRotate(v1, lastPPos[index], angle1);//沿着方向1旋转
	rotated = PointRotate(v2, rotated, angle2);//沿着方向2旋转


	if (checkThreshold(offset) && (((info1.y*w1 >= 1e-7) && (info2.y*w2 >= 1e-7)) || (fabs(info1.z)<1e-7)))
	{

		//保长处理
		coor dist1 = { points[index].x - points[frontindex].x, points[index].y - points[frontindex].y, points[index].z - points[frontindex].z };
		coor dist2 = { rotated.x - lastp.x, rotated.y - lastp.y, rotated.z - lastp.z };
		double scale = sqrtf(pow(dist1.x, 2) + pow(dist1.y, 2) + pow(dist1.z, 2)) / sqrtf(pow(dist2.x, 2) + pow(dist2.y, 2) + pow(dist2.z, 2));

		lastPPos[index] = { lastp.x + dist2.x*scale, lastp.y + dist2.y*scale, lastp.z + dist2.z*scale };


		//该点之后的中轴点也要随之旋转同样的角度
		for (int i = index - 3; i > 0+offset; i -= 3)
		{
			coor rotated = PointRotate(v1, lastPPos[i], angle1);//沿着方向1旋转
			rotated = PointRotate(v2, rotated, angle2);//沿着方向2旋转

			if (checkThreshold(offset))
			{
				//保长处理
				lastp = lastPPos[i + 3];
				coor dist1 = { points[i].x - points[i + 3].x, points[i].y - points[i + 3].y, points[i].z - points[i + 3].z };
				coor dist2 = { rotated.x - lastp.x, rotated.y - lastp.y, rotated.z - lastp.z };
				double scale = sqrtf(pow(dist1.x, 2) + pow(dist1.y, 2) + pow(dist1.z, 2)) / sqrtf(pow(dist2.x, 2) + pow(dist2.y, 2) + pow(dist2.z, 2));

				lastPPos[i] = { lastp.x + dist2.x*scale, lastp.y + dist2.y*scale, lastp.z + dist2.z*scale };


				//该轴对应两边的点为相同位移
				if (i<offset+single_petal - 2 && i>1+offset)
				{
					coor t = { points[i - 1].x - points[i].x, points[i - 1].y - points[i].y, points[i - 1].z - points[i].z };

					if (std::find(axis.begin(), axis.end(), i - 1) == axis.end())
						lastPPos[i - 1] = { lastPPos[i].x + t.x, lastPPos[i].y + t.y, lastPPos[i].z + t.z };

					t = { points[i + 1].x - points[i].x, points[i + 1].y - points[i].y, points[i + 1].z - points[i].z };
					if (std::find(axis.begin(), axis.end(), i + 1) == axis.end())
						lastPPos[i + 1] = { lastPPos[i].x + t.x, lastPPos[i].y + t.y, lastPPos[i].z + t.z };
				}


			}



		}
		//顶点
		rotated = PointRotate(v1, lastPPos[offset], angle1);//沿着方向1旋转
		rotated = PointRotate(v2, rotated, angle2);//沿着方向2旋转

		//保长处理
		lastp = lastPPos[2+offset];
		dist1 = { points[offset].x - points[2+offset].x, points[offset].y - points[2+offset].y, points[offset].z - points[2+offset].z };
		dist2 = { rotated.x - lastp.x, rotated.y - lastp.y, rotated.z - lastp.z };
		scale = sqrtf(pow(dist1.x, 2) + pow(dist1.y, 2) + pow(dist1.z, 2)) / sqrtf(pow(dist2.x, 2) + pow(dist2.y, 2) + pow(dist2.z, 2));

		lastPPos[offset] = { lastp.x + dist2.x*scale, lastp.y + dist2.y*scale, lastp.z + dist2.z*scale };



		//该点之前的三个之内中轴点也旋转
		int count = 2;
		for (int i = index + 3; i < single_petal+offset && count<7; i += 3)
		{
			coor rotated = PointRotate(v1, lastPPos[i], angle1 / (count*0.8));//沿着方向1旋转
			rotated = PointRotate(v2, rotated, angle2 / (count*0.8));//沿着方向2旋转

			if (checkThreshold(offset) && (i + 3<points.size()))
				//if (i + 3<points.size())
			{
				//保长处理
				lastp = lastPPos[i + 3];
				coor dist1 = { points[i].x - points[i + 3].x, points[i].y - points[i + 3].y, points[i].z - points[i + 3].z };
				coor dist2 = { rotated.x - lastp.x, rotated.y - lastp.y, rotated.z - lastp.z };
				double scale = sqrtf(pow(dist1.x, 2) + pow(dist1.y, 2) + pow(dist1.z, 2)) / sqrtf(pow(dist2.x, 2) + pow(dist2.y, 2) + pow(dist2.z, 2));

				lastPPos[i] = { lastp.x + dist2.x*scale, lastp.y + dist2.y*scale, lastp.z + dist2.z*scale };



				//该轴对应两边的点为相同位移
				if (i<offset+single_petal - 2 && i>1+offset)
				{
					coor t = { points[i - 1].x - points[i].x, points[i - 1].y - points[i].y, points[i - 1].z - points[i].z };
					if (std::find(axis.begin(), axis.end(), i - 1) == axis.end())
						lastPPos[i - 1] = { lastPPos[i].x + t.x, lastPPos[i].y + t.y, lastPPos[i].z + t.z };

					t = { points[i + 1].x - points[i].x, points[i + 1].y - points[i].y, points[i + 1].z - points[i].z };
					if (std::find(axis.begin(), axis.end(), i + 1) == axis.end())
						lastPPos[i + 1] = { lastPPos[i].x + t.x, lastPPos[i].y + t.y, lastPPos[i].z + t.z };
				}



			}
			count++;
		}



		//该轴对应两边的点为相同位移
		if ((index<offset+single_petal- 2) && (index>1+offset))
		{
			coor t = { points[index - 1].x - points[index].x, points[index - 1].y - points[index].y, points[index - 1].z - points[index].z };
			if (std::find(axis.begin(), axis.end(), index - 1) == axis.end())
				lastPPos[index - 1] = { lastPPos[index].x + t.x, lastPPos[index].y + t.y, lastPPos[index].z + t.z };

			t = { points[index + 1].x - points[index].x, points[index + 1].y - points[index].y, points[index + 1].z - points[index].z };
			if (std::find(axis.begin(), axis.end(), index + 1) == axis.end())
				lastPPos[index + 1] = { lastPPos[index].x + t.x, lastPPos[index].y + t.y, lastPPos[index].z + t.z };
		}



	}


	//当已经到达平衡位置，不再改变；未到达时继续形变
	//if (fabs(info1.y)>1e-7)
	if ((info1.y*w1 >= 1e-7))
		lastData[index][0] = { a1, w1, x1 };
	else
		lastData[index][0].y = 0.001*info1.y / fabs(info1.y);
	//if (fabs(info2.y)>1e-7)
	if ((info2.y*w2 >= 1e-7))
		lastData[index][1] = { a2, w2, x2 };
	else
		lastData[index][1].y = 0.001*info2.y / fabs(info2.y);




	return;

}

coor MyGL::PointRotate(coor RotateAxis, coor RotateP, double angle)
{
	coor res;

	double norm = sqrtf(powf(RotateAxis.x, 2) + powf(RotateAxis.y, 2) + powf(RotateAxis.z, 2));
	float x = RotateAxis.x / norm, y = RotateAxis.y / norm, z = RotateAxis.z / norm;

	Mat A(3, 3, CV_32FC1, Scalar(0));
	A.at<float>(0, 0) = x*x;  A.at<float>(1, 0) = x*y;   A.at<float>(2, 0) = x*z;
	A.at<float>(0, 1) = y*x;  A.at<float>(1, 1) = y*y;   A.at<float>(2, 1) = y*z;
	A.at<float>(0, 2) = z*x;  A.at<float>(1, 2) = z*y;   A.at<float>(2, 2) = z*z;
	//cout << A<<endl;

	Mat B(3, 3, CV_32FC1, Scalar(0));
	B.at<float>(0, 0) = 0;  B.at<float>(0, 1) = -z;   B.at<float>(0, 2) = y;
	B.at<float>(1, 0) = z;  B.at<float>(1, 1) = 0;   B.at<float>(1, 2) = -x;
	B.at<float>(2, 0) = -y;  B.at<float>(2, 1) = x;   B.at<float>(2, 2) = 0;
	//cout << B << endl;

	Mat I(3, 3, CV_32FC1, Scalar(0));
	I.at<float>(0, 0) = 1; I.at<float>(1, 1) = 1; I.at<float>(2, 2) = 1;

	Mat P(1, 3, CV_32FC1, Scalar(0));
	P.at<float>(0, 0) = RotateP.x; P.at<float>(0, 1) = RotateP.y;  P.at<float>(0, 2) = RotateP.z;

	Mat M = A + cos(angle)*(I - A) + sin(angle)*B;
	Mat RotatedM = P * M.t();

	res = { RotatedM.at<float>(0, 0), RotatedM.at<float>(0, 1), RotatedM.at<float>(0, 2) };

	return res;

}

coor MyGL::cross(coor a, coor b)
{
	coor res;

	res.x = a.y*b.z - a.z*b.y;
	res.y = b.x*a.z - a.x*b.z;
	res.z = a.x*b.y - b.x*a.y;

	return res;
}

double MyGL::dot(coor a, coor b)
{
	double res = a.x*b.x + a.y*b.y + a.z*b.z;
	return res;
}

bool MyGL::checkThreshold(int offset)
{
	//调节风
	if (fabs(Fw.y / Fw.x) > 3)
	{
		Fw.x = 0;
	}


	bool res = true;

	/*
	coor dir1 = { lastPPos[0].x - lastPPos[2].x, lastPPos[0].y - lastPPos[2].y, lastPPos[0].z - lastPPos[2].z };
	coor dir2 = normals[0];

	//允许原点到顶点向量比原始向量的最大旋转幅度
	//阈值和花瓣面积和宽度和风在旋转方向的分量有关
	double threshold2 = (p_length*p_width)*0.05 / 10000 + p_width*0.07 / 100 + fabs(dot(Fw, dir2))*0.06 + fabs(dot(Fw, dir1))*0.02;


	//旋转后的新的位置和原始位置比，移动的夹角
	coor a = lastPPos[0]	;//新位置
	coor b = points[0];//原始位置
	double da = sqrtf(pow(a.x, 2) + pow(a.y, 2) + pow(a.z, 2));
	double db = sqrtf(pow(b.x, 2) + pow(b.y, 2) + pow(b.z, 2));
	double t  = dot(a, b) / (da*db);
	double d_angle;
	if (fabs(t) <= 1)
	d_angle = acos(dot(a, b) / (da*db));
	else
	d_angle = 0;

	if (fabs(d_angle) < fabs(threshold2))
	res = true;
	else
	{
	res = false;

	//减小风力
	Fw.x=0;
	}

	*/

	//引入虚拟弹簧求解平衡位置
	coor v_s = { points[offset].x - lastPPos[offset].x, points[offset].y - lastPPos[offset].y, points[offset].z - lastPPos[offset].z };//原始位置-当前位置
	double s = sqrtf(powf(v_s.x, 2) + powf(v_s.y, 2) + powf(v_s.z, 2));
	//if (fabs(s) < 0.001) s = 0.1;

	double K = 0.15;
	double Ft = K*s / 5.442;//恢复力

	coor t = { v_s.x / s, v_s.y / s, v_s.z / s };
	double Fw1 = -dot(Fw, t);//风力在恢复力方向的反向分量大小

	if (!flag_resume)//不是恢复
	{
		if ((Ft < Fw1 || (Ft*Fw1<0) || fabs(s) < 0.001) && (s <= 1.3*p_length))//风力大，继续旋转
			res = true;
		else
		{
			res = false;
		}

	}
	/*
	else//恢复
	{
	if (Ft < Fw1 || fabs(s) < 0.001)//已经恢复到合适位置,不应再旋转
	res = false;
	else
	res = true;
	}
	*/

	return res;
}

void MyGL::resume(int offset)
{
	vector<coor> newPos = lastPPos;
	//引入虚拟弹簧求解平衡位置
	coor v_s = { points[offset].x - lastPPos[offset].x, points[offset].y - lastPPos[offset].y, points[offset].z - lastPPos[offset].z };//原始位置-当前位置
	double s = sqrtf(powf(v_s.x, 2) + powf(v_s.y, 2) + powf(v_s.z, 2));
	if (fabs(s) < 0.001) s = 0.1;

	coor t = { v_s.x / s, v_s.y / s, v_s.z / s };
	double Fw1 = -dot(Fw, t);//风力在恢复力方向的反向分量大小
	if (fabs(Fw1) < 1e-5)//无风
	{
		lastPPos = points;
		return;
	}


	double K = 0.15 / 5.442;
	double new_s = Fw1 / K;//新的偏移量
	new_s = (new_s >= 1.3*p_length) ? 1.3*p_length : new_s;

	double ratio = new_s / s;
	newPos[offset] = { points[offset].x - ratio*v_s.x, points[offset].y - ratio*v_s.y, points[offset].z - ratio*v_s.z };//顶点的新位置
	for (int i = 2+offset; i < offset+single_petal - 2; i += 3)
	{
		v_s = { points[i].x - lastPPos[i].x, points[i].y - lastPPos[i].y, points[i].z - lastPPos[i].z };
		newPos[i] = { points[i].x - ratio*v_s.x, points[i].y - ratio*v_s.y, points[i].z - ratio*v_s.z };//顶点的新位置
	}
	//长度处理，从花心向上
	for (int i = offset+single_petal - 3; i >offset; i -= 3)
	{
		int front = (i == offset + single_petal - 3) ? offset + single_petal - 1 : i + 3;
		v_s = { newPos[i].x - newPos[front].x, newPos[i].y - newPos[front].y, newPos[i].z - newPos[front].z };//新的方向
		s = sqrtf(powf(v_s.x, 2) + powf(v_s.y, 2) + powf(v_s.z, 2));
		t = { v_s.x / s, v_s.y / s, v_s.z / s };//新的方向单位向量

		coor v_s0 = { points[i].x - points[front].x, points[i].z - points[front].z, points[i].z - points[front].z };
		double s0 = sqrtf(powf(v_s0.x, 2) + powf(v_s0.y, 2) + powf(v_s0.z, 2));//应有的长度

		//.....
		if (s0>s)
			lastPPos[i] = { lastPPos[front].x + s0*t.x, lastPPos[front].y + s0*t.y, lastPPos[front].z + s0*t.z };
		else
			lastPPos[i] = { lastPPos[front].x + s*t.x, lastPPos[front].y + s*t.y, lastPPos[front].z + s*t.z };//..........

		//该轴对应两边的点为相同位移
		if (i<offset+single_petal - 2 && i>1+offset)
		{
			coor t = { points[i - 1].x - points[i].x, points[i - 1].y - points[i].y, points[i - 1].z - points[i].z };
			if (std::find(axis.begin(), axis.end(), i - 1) == axis.end())
				lastPPos[i - 1] = { lastPPos[i].x + t.x, lastPPos[i].y + t.y, lastPPos[i].z + t.z };

			t = { points[i + 1].x - points[i].x, points[i + 1].y - points[i].y, points[i + 1].z - points[i].z };
			if (std::find(axis.begin(), axis.end(), i + 1) == axis.end())
				lastPPos[i + 1] = { lastPPos[i].x + t.x, lastPPos[i].y + t.y, lastPPos[i].z + t.z };
		}

	}
	//顶点
	int front = 2+offset;
	v_s = { newPos[offset].x - newPos[front].x, newPos[offset].y - newPos[front].y, newPos[offset].z - newPos[front].z };//新的方向
	s = sqrtf(powf(v_s.x, 2) + powf(v_s.y, 2) + powf(v_s.z, 2));
	t = { v_s.x / s, v_s.y / s, v_s.z / s };//新的方向单位向量
	coor v_s0 = { points[offset].x - points[front].x, points[offset].z - points[front].z, points[offset].z - points[front].z };
	double s0 = sqrtf(powf(v_s0.x, 2) + powf(v_s0.y, 2) + powf(v_s0.z, 2));//应有的长度
	lastPPos[offset] = { lastPPos[front].x + s0*t.x, lastPPos[front].y + s0*t.y, lastPPos[front].z + s0*t.z };

	return;
}



