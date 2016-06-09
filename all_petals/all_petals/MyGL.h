#pragma once
#include <QGLWidget> 
#include <QVector>
#include <QPoint>


#include <iostream>
#include <string>
#include <vector>
#include "MyGL.h"
#include "loadPLY.h"
#include "global.h"
#include <QMouseEvent>
#include<QWheelEvent>  
#include <QColorDialog>
#include <GL/GLU.h>
#include <GL/glut.h>
#include <sstream>
#include <fstream>
#include <cmath>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <time.h>

using namespace cv;




// -------------------- OpenMesh
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
// ----------------------------------------------------------------------------
typedef OpenMesh::TriMesh_ArrayKernelT<>  MyMesh;


using namespace std;


class MyGL :
	public QGLWidget
{
public:
	MyGL();
	~MyGL();

private:
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();
	void timerEvent(QTimerEvent *);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *e);

	void drawMesh();//绘制花瓣网格

	void caculateAxisMove(int index,int offset);//计算中轴点该帧角位移
	coor PointRotate(coor RotateAxis, coor RotateP, double angle);//旋转点
	coor cross(coor a, coor b);//求解叉乘
	double dot(coor a, coor b);//求解点乘
	bool checkThreshold(int offset);//检测是否到达阈值
	void resume(int offset);//恢复


	loadPLY ply;

	GLfloat rotationX;
	GLfloat rotationY;
	GLfloat rotationZ;
	GLfloat translationX;
	GLfloat translationY;
	GLfloat translationZ;
	QPoint lastPos;


	vector<coor> points;//点
	vector<coor> lastPPos;//点的上一帧位置
	vector<coor> faces;//面
	vector<coor> normals;//法向量
	vector<int> axis;//中轴索引
	int petal_num;//花瓣个数
	int single_petal;//单个花瓣采样点

	vector<vector<coor>> lastData;//记录每个点上一帧的两个方向额角加速度a,角速度w，角位移x
	vector<coor> lastMove;//记录每个点上一帧的位移向量

	coor Fw;//风力
	coor lastFw;//上一帧的风力
	double h;
	double distance;

	double p_width = 0;
	double p_length = 0;

	bool start = true;//标记是否初始状态，即w=0原因是静止
	bool flag_resume = false;//标记是否为恢复


	int t = 0;


};

