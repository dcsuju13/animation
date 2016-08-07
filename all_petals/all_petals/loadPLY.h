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
	vector<coor> points;//��
	vector<coor> face;//��
	vector<coor> normal;//��ķ���
	vector<int> axis;//������±�����,���洢��һ�������Ӧ��
	vector<QPoint> tex;//��������
	int petal_num;//�������
	int single_petal;//���������������

	string tex_file;//�����ļ�





};

