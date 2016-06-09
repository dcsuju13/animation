#include "all_petals.h"
#include <QtWidgets/QApplication>
#include <MyGL.h>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//all_petals w;
	//w.show();

	MyGL gl;
	gl.show();
	return a.exec();
}
