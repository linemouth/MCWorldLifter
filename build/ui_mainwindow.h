/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QPushButton *startButton;
    QLabel *bottomLabel;
    QLabel *regionFolderLabel;
    QSpinBox *bottomSpinBox;
    QSpinBox *amountSpinBox;
    QProgressBar *progressBar;
    QLabel *amountLabel;
    QLineEdit *dirEdit;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(560, 139);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        startButton = new QPushButton(centralWidget);
        startButton->setObjectName(QStringLiteral("startButton"));
        startButton->setEnabled(false);

        gridLayout->addWidget(startButton, 3, 0, 1, 1);

        bottomLabel = new QLabel(centralWidget);
        bottomLabel->setObjectName(QStringLiteral("bottomLabel"));

        gridLayout->addWidget(bottomLabel, 1, 0, 1, 1);

        regionFolderLabel = new QLabel(centralWidget);
        regionFolderLabel->setObjectName(QStringLiteral("regionFolderLabel"));

        gridLayout->addWidget(regionFolderLabel, 0, 0, 1, 1);

        bottomSpinBox = new QSpinBox(centralWidget);
        bottomSpinBox->setObjectName(QStringLiteral("bottomSpinBox"));
        bottomSpinBox->setMaximum(254);
        bottomSpinBox->setValue(5);

        gridLayout->addWidget(bottomSpinBox, 1, 1, 1, 1);

        amountSpinBox = new QSpinBox(centralWidget);
        amountSpinBox->setObjectName(QStringLiteral("amountSpinBox"));
        amountSpinBox->setMinimum(1);
        amountSpinBox->setMaximum(254);
        amountSpinBox->setValue(128);

        gridLayout->addWidget(amountSpinBox, 2, 1, 1, 1);

        progressBar = new QProgressBar(centralWidget);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setEnabled(true);
        progressBar->setMaximum(1);
        progressBar->setValue(0);
        progressBar->setTextVisible(false);

        gridLayout->addWidget(progressBar, 3, 1, 1, 1);

        amountLabel = new QLabel(centralWidget);
        amountLabel->setObjectName(QStringLiteral("amountLabel"));

        gridLayout->addWidget(amountLabel, 2, 0, 1, 1);

        dirEdit = new QLineEdit(centralWidget);
        dirEdit->setObjectName(QStringLiteral("dirEdit"));

        gridLayout->addWidget(dirEdit, 0, 1, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        startButton->setText(QApplication::translate("MainWindow", "Lift it!", 0));
        bottomLabel->setText(QApplication::translate("MainWindow", "Chop Level", 0));
        regionFolderLabel->setText(QApplication::translate("MainWindow", "Region Folder", 0));
        progressBar->setFormat(QApplication::translate("MainWindow", "%v/%m Chunks", 0));
        amountLabel->setText(QApplication::translate("MainWindow", "Lift Amount", 0));
        dirEdit->setText(QApplication::translate("MainWindow", "C:\\Users\\Collin\\AppData\\Roaming\\.minecraft\\saves\\ChunkLifterTest\\region", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
