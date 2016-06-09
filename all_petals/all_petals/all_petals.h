#ifndef ALL_PETALS_H
#define ALL_PETALS_H

#include <QtWidgets/QMainWindow>
#include "ui_all_petals.h"

class all_petals : public QMainWindow
{
	Q_OBJECT

public:
	all_petals(QWidget *parent = 0);
	~all_petals();

private:
	Ui::all_petalsClass ui;
};

#endif // ALL_PETALS_H
