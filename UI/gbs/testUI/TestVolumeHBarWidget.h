#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QList>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>


#include "../common/VolumeHBarWidget.h"
class TestVolumeHBarWidget : public QWidget
{
	Q_OBJECT

public:
	TestVolumeHBarWidget(QWidget *parent = nullptr) : QWidget(parent)
	{
		// 初始化布局
		QVBoxLayout *mainLayout = new QVBoxLayout(this);
		// Create and add multiple VolumeWidgets to the layout
		VolumeHBarWidget *microphoneWidget = new VolumeHBarWidget("Microphone", ":Resources/gbs-microphon-open.png", this);
		VolumeHBarWidget *speakerWidget = new VolumeHBarWidget("Speaker", ":Resources/gbs-speaker-close.png", this);
		mainLayout->addWidget(microphoneWidget);
		mainLayout->addWidget(speakerWidget);


	}



};
