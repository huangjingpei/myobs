#ifndef PLAYERPROGRESSBAR_H
#define PLAYERPROGRESSBAR_H
#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QTimer>
#include <QTime>
#include <QMouseEvent>
#include <QStyle>
#include <QDebug>

class CustomSlider : public QSlider {
	Q_OBJECT

public:
	explicit CustomSlider(Qt::Orientation orientation, bool seekable, QWidget *parent = nullptr)
		: QSlider(orientation, parent), seekable(seekable) {}

protected:
	// 重写鼠标按下事件
	void mousePressEvent(QMouseEvent *event) override {
		if (event->button() == Qt::LeftButton) {  // 只处理左键点击
			if (seekable) {
				int value;

				// 如果是水平滑块，计算点击的位置
				if (orientation() == Qt::Horizontal) {
					value = QStyle::sliderValueFromPosition(minimum(), maximum(), event->pos().x(), width());
				} else {
					// 如果是垂直滑块，计算点击的位置
					value = QStyle::sliderValueFromPosition(minimum(), maximum(), event->pos().y(), height());
				}

				// 更新滑块的值并发出 valueChanged 信号
				setValue(value);
				// qDebug() << "Custom click at position:" << event->pos() << " New value:" << value;
			}
			// 执行父类默认行为，确保滑块可以被拖动
			QSlider::mousePressEvent(event);
		}
	}
private:
	bool seekable;
};

class PlayerProgressBar : public QWidget {
	Q_OBJECT

public:
	explicit PlayerProgressBar(QString startHMS, int lastSecs, bool seekable = true, QWidget *parent = nullptr)
		: QWidget(parent), currentTime(0), totalTime(lastSecs), seekable(seekable) {
		QHBoxLayout *layout = new QHBoxLayout(this);
		QString endHMS = addSecondsToTime(startHMS, lastSecs);
		qDebug() << startHMS << ":" << endHMS;



		// Play/Pause Button
		playPauseButton = new QPushButton(this);
		playPauseButton->setFixedSize(40, 40);
		playPauseButton->setStyleSheet("background-image: url(:/resources/pause-icon.png); background-repeat: no-repeat; background-position: center;");
		connect(playPauseButton, &QPushButton::clicked, this, &PlayerProgressBar::togglePlayPause);
		layout->addWidget(playPauseButton);

		// Current Time Label
		currentTimeLabel = new QLabel(startHMS, this);
		layout->addWidget(currentTimeLabel);

		// Progress Bar (Slider)
		progressBar = new CustomSlider(Qt::Horizontal, seekable, this);
		progressBar->setRange(0, totalTime);  // set total duration in seconds

		// 设置样式表
		progressBar->setStyleSheet(R"(
		    QSlider::groove:horizontal {
			border: 1px solid #999999;  /* 整个进度条的边框 */
			height: 8px;               /* 进度条的高度 */
			background: #e0e0e0;       /* 进度条的背景颜色 */
			border-radius: 4px;        /* 圆角效果 */
		    }

		    QSlider::sub-page:horizontal {
			background: #EB3F5E;       /* 已播放部分的颜色 */
			border-radius: 4px;        /* 圆角效果 */
			height: 8px;               /* 和 groove 一致 */
		    }

		    QSlider::add-page:horizontal {
			background: #d3d3d3;       /* 未播放部分的颜色 */
			border-radius: 4px;        /* 圆角效果 */
		    }

		    QSlider::handle:horizontal {
			background: #EB3F5E;       /* 滑块的颜色 */
			border: 1px solid #EB3F5E; /* 滑块的边框 */
			width: 18px;               /* 滑块的宽度 */
			margin: -5px 0;            /* 调整滑块位置，使其在进度条中间 */
			border-radius: 9px;        /* 圆形滑块 */
		    }
		)");

		layout->addWidget(progressBar);
		connect(progressBar, &QSlider::sliderMoved, this, &PlayerProgressBar::seek);


		// Total Time Label
		totalTimeLabel = new QLabel(endHMS, this);
		layout->addWidget(totalTimeLabel);

		// Timer to simulate progress
		timer = new QTimer(this);
		connect(timer, &QTimer::timeout, this, &PlayerProgressBar::updateProgress);
	}

private:
	QString addSecondsToTime(const QString &timeString, int secondsToAdd) {
		// 将输入的时间字符串解析为 QTime 对象
		QTime time = QTime::fromString(timeString, "hh:mm:ss");

		// 检查时间格式是否有效
		if (!time.isValid()) {
			qWarning() << "Invalid time format!";
			return QString();
		}

		// 使用 addSecs 函数添加 N 秒
		QTime newTime = time.addSecs(secondsToAdd);

		// 返回新的时间字符串
		return newTime.toString("hh:mm:ss");
	}


public slots:
	void togglePlayPause() {
		if (timer->isActive()) {
			timer->stop();
			playPauseButton->setStyleSheet("background-image: url(:/resources/play-icon.png); background-repeat: no-repeat; background-position: center;");
		} else {
			timer->start(1000); // Update every second
			playPauseButton->setStyleSheet("background-image: url(:/resources/pause-icon.png); background-repeat: no-repeat; background-position: center;");
		}
	}

	void updateProgress() {
		// if (currentTime < totalTime) {
		// 	currentTime++;
		// 	progressBar->setValue(currentTime);
		// 	currentTimeLabel->setText(formatTime(currentTime));
		// } else {
		// 	timer->stop();
		// }
	}

	void seek(int position) {
		qDebug() << "seek position " << position;
		currentTime = position;
		progressBar->setValue(currentTime);
		currentTimeLabel->setText(formatTime(currentTime));
	}

private:
	QPushButton *playPauseButton;
	QLabel *currentTimeLabel;
	CustomSlider *progressBar;
	QLabel *totalTimeLabel;


	QTimer *timer;
	int currentTime; //当前Slider上对应的刻度时间
	int playingTime; //当前播放的事件（playingTime<=currentTime) ，playingTime 当前开始播放之后的时间，用户点击开始播放后时间进行累加，如果不可移动的情况下，seek 操作应该退回到playingTime
	int totalTime;
	bool seekable;

	QString formatTime(int timeInSeconds) {
		int hours = timeInSeconds / 3600;
		int minutes = (timeInSeconds % 3600) / 60;
		int seconds = timeInSeconds % 60;

		return QString("%1:%2:%3").arg(hours, 2, 10, QChar('0'))
			.arg(minutes, 2, 10, QChar('0'))
			.arg(seconds, 2, 10, QChar('0'));
	}
};


#endif //PLAYERPROGRESSBAR_H
