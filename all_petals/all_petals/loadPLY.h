#pragma once

#include <QPoint>
#include <vector>
#include "global.h"

using namespace std;


// -------------------- OpenMesh
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
// ----------------------------------------------------------------------------
typedef OpenMesh::TriMesh_ArrayKernelT<>  MyMesh;


class loadPLY
{
public:
	loadPLY();
	loadPLY(string filename,string tex);
	~loadPLY();

	void Analyse();
	vector<coor> getPoints();
	vector<coor> getFaces();
	vector<coor> getNormals();
	vector<int> getAxisIndex();
	vector<QPoint> getTexture();
	int getPetal_num();
	int getPetal_single();
private:
	MyMesh mesh;
	vector<coor> points;//点
	vector<coor> face;//面
	vector<coor> normal;//点的法向
	vector<int> axis;//中轴的下标数组,仅存储第一个花瓣对应的
	vector<QPoint> tex;//纹理坐标
	int petal_num;//花瓣个数
	int single_petal;//单个花瓣采样点数

	string tex_file;//纹理文件





};

