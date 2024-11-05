#ifndef TestToggleButton_H
#define TestToggleButton_H
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QList>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>


#include "../common/ToggleButton.h"
class TestToggleButton : public QWidget
{
	Q_OBJECT

public:
	TestToggleButton(QWidget *parent = nullptr) : QWidget(parent)
	{
		// 初始化布局
		QVBoxLayout *mainLayout = new QVBoxLayout(this);
		ToggleButton *toggleButton = new ToggleButton(100, 30, this);
		toggleButton->setGeometry(100, 100, 60, 30);  // Set position and size

		ToggleButton *toggleButton2 = new ToggleButton(60, 15,this);
		toggleButton2->setGeometry(100, 100, 60, 30);  // Set position and size
		mainLayout->addWidget(toggleButton);
		mainLayout->addWidget(toggleButton2);


	}



};

#endif //TestToggleButton
