#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QList>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>

#include <QDateTime>
#include <QTime>


#include "../common/PlayerProgressBar.h"
class TestPlayerProgressBar : public QWidget
{
	Q_OBJECT

public:
	TestPlayerProgressBar(QWidget *parent = nullptr) : QWidget(parent)
	{

		QDateTime currentDateTime = QDateTime::currentDateTime();
		QString dateTimeString = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
		qDebug() << "data time " << dateTimeString;
		QTime currentTime = QTime::currentTime();
		QString currentTimeString = currentTime.toString("hh:mm:ss");
		QString endTimeString = currentTime.addSecs(12*3600).toString("hh:mm:ss");

		qDebug() << "time now " << currentTimeString;
		// 初始化布局
		QVBoxLayout *mainLayout = new QVBoxLayout(this);
		// Create and add multiple VolumeWidgets to the layout
		PlayerProgressBar *playerWidget = new PlayerProgressBar(currentTimeString, 12*3600);
		playerWidget->setFixedSize(400, 40);
		mainLayout->addWidget(playerWidget);


	}



};
