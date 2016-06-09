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

	void drawMesh();//���ƻ�������

	void caculateAxisMove(int index,int offset);//����������֡��λ��
	coor PointRotate(coor RotateAxis, coor RotateP, double angle);//��ת��
	coor cross(coor a, coor b);//�����
	double dot(coor a, coor b);//�����
	bool checkThreshold(int offset);//����Ƿ񵽴���ֵ
	void resume(int offset);//�ָ�


	loadPLY ply;

	GLfloat rotationX;
	GLfloat rotationY;
	GLfloat rotationZ;
	GLfloat translationX;
	GLfloat translationY;
	GLfloat translationZ;
	QPoint lastPos;


	vector<coor> points;//��
	vector<coor> lastPPos;//�����һ֡λ��
	vector<coor> faces;//��
	vector<coor> normals;//������
	vector<int> axis;//��������
	int petal_num;//�������
	int single_petal;//�������������

	vector<vector<coor>> lastData;//��¼ÿ������һ֡�����������Ǽ��ٶ�a,���ٶ�w����λ��x
	vector<coor> lastMove;//��¼ÿ������һ֡��λ������

	coor Fw;//����
	coor lastFw;//��һ֡�ķ���
	double h;
	double distance;

	double p_width = 0;
	double p_length = 0;

	bool start = true;//����Ƿ��ʼ״̬����w=0ԭ���Ǿ�ֹ
	bool flag_resume = false;//����Ƿ�Ϊ�ָ�


	int t = 0;


};

