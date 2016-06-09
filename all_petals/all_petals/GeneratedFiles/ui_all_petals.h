/********************************************************************************
** Form generated from reading UI file 'all_petals.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ALL_PETALS_H
#define UI_ALL_PETALS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_all_petalsClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *all_petalsClass)
    {
        if (all_petalsClass->objectName().isEmpty())
            all_petalsClass->setObjectName(QStringLiteral("all_petalsClass"));
        all_petalsClass->resize(600, 400);
        menuBar = new QMenuBar(all_petalsClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        all_petalsClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(all_petalsClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        all_petalsClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(all_petalsClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        all_petalsClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(all_petalsClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        all_petalsClass->setStatusBar(statusBar);

        retranslateUi(all_petalsClass);

        QMetaObject::connectSlotsByName(all_petalsClass);
    } // setupUi

    void retranslateUi(QMainWindow *all_petalsClass)
    {
        all_petalsClass->setWindowTitle(QApplication::translate("all_petalsClass", "all_petals", 0));
    } // retranslateUi

};

namespace Ui {
    class all_petalsClass: public Ui_all_petalsClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ALL_PETALS_H
