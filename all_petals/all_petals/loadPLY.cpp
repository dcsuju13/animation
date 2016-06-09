#include "loadPLY.h"


loadPLY::loadPLY()
{
}

loadPLY::loadPLY(string filename){
	bool res = OpenMesh::IO::read_mesh(mesh, filename);
	if (!res)  return;

}

loadPLY::~loadPLY()
{
}


void loadPLY::Analyse(){
	points.clear();


	//��
	for (auto it = mesh.vertices_begin(); it != mesh.vertices_end(); ++it){
		auto point = mesh.point(it.handle());
		coor t = { point.data()[0], point.data()[1], point.data()[2] };

		points.push_back(t);

	}

	//��
	face.clear();

	for (auto it = mesh.faces_begin(); it != mesh.faces_end(); ++it){
		OpenMesh::FaceHandle fh = it.handle();

		coor facet;//��ǰ������
		int count = 0;//��ǵڼ�����
		for (auto it1 = mesh.fv_begin(fh); it1 != mesh.fv_end(fh); ++it1){
			auto point = mesh.point(it1.handle());
			t = { point.data()[0], point.data()[1], point.data()[2] };

			//find
			vector<coor>::iterator itv;//��¼�õ��ӦPoints�е�λ��
			itv = find_if(points.begin(), points.end(), [](coor const& obj){
				return (fabs(obj.x - t.x) < 0.01) && (fabs(obj.y - t.y) < 0.01) && (fabs(obj.z - t.z) < 0.01);
			});

			if (itv != points.end())//д����facet
			{
				if (count == 0){
					facet.x = itv - points.begin();
				}
				else if (count == 1){
					facet.y = itv - points.begin();
				}
				else
					facet.z = itv - points.begin();
			}
			else
				break;
			count++;


		}
		face.push_back(facet);

	}

	//��ķ�����
	normal.clear();

	mesh.request_face_normals();
	mesh.request_vertex_normals();
	mesh.update_normals();
	for (auto it = mesh.vertices_begin(); it != mesh.vertices_end(); ++it){
		auto vertex = it.handle();
		MyMesh::Normal normalt = mesh.normal(vertex);
		t = { normalt.data()[0], normalt.data()[1], normalt.data()[2] };
		normal.push_back(t);
	}

	//��⻨����Ŀ
	petal_num = 0;
	single_petal = 0;//���������������
	for (int i = 0; i < points.size(); i++)
	{
		single_petal++;
		if (points[i+1].z - points[i ].z>10)
			break;
	}
	petal_num = points.size() / single_petal;

	//�����±�����
	axis.clear();

	axis.push_back(0);
	for (int i = 2; i < single_petal - 2; i += 3)
		axis.push_back(i);
	axis.push_back(single_petal - 1);

	//int a=0;
	return;

}

vector<coor> loadPLY::getPoints(){
	return points;
}

vector<coor> loadPLY::getFaces(){
	return face;

}

vector<coor> loadPLY::getNormals(){
	return normal;
}

vector<int> loadPLY::getAxisIndex(){
	return axis;

}

int loadPLY::getPetal_num(){
	return petal_num;
}

int loadPLY::getPetal_single(){
	return single_petal;
}