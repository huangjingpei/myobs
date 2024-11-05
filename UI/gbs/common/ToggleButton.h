#ifndef GSBTOGGLEBUTTON_H
#define GSBTOGGLEBUTTON_H
#include <QWidget>
#include <QPushButton>
#include <QPainter>
#include <QHBoxLayout>
#include <QPropertyAnimation>
class ToggleButton : public QWidget {
	Q_OBJECT

public:
	explicit ToggleButton(int width, int height, QWidget *parent = nullptr)
		: QWidget(parent), isChecked(false), animationProgress(0) {
		setFixedSize(width, height);  // Set the size of the toggle button

		// Animation for smooth movement of the toggle
		animation = new QPropertyAnimation(this, "animationProgress", this);
		animation->setDuration(200);
		connect(animation, &QPropertyAnimation::valueChanged, this, [this]() {
			this->update();  // Manually trigger a redraw
		});
	}

	bool getIsChecked() const {
		return isChecked;
	}

protected:
	void paintEvent(QPaintEvent *event) override {
		QPainter painter(this);
		painter.setRenderHint(QPainter::Antialiasing);

		// Draw background (rounded rectangle)
		QColor backgroundColor = isChecked ? QColor(0, 197, 102) : QColor(224, 224, 224);
		painter.setBrush(backgroundColor);
		painter.setPen(Qt::NoPen);
		qDebug()<< "width " << width() << " height " << height();
		qDebug()<< "width " << rect().width() << " height " << rect().height() << "  x " << rect().x() << " y " << rect().y();
		painter.drawRoundedRect(rect(), height() / 2, height() / 2);

		// Draw the toggle (circle)
		int toggleDiameter = height() - 4;
		int toggleX = static_cast<int>((width() - toggleDiameter * 1.4) * animationProgress);
		qDebug() << "toggleDiameter " << toggleDiameter << " toggleX " << toggleX << " animationProgress " << animationProgress;

		QRect toggleRect(toggleX, 2, toggleDiameter*1.4, toggleDiameter);
		painter.setBrush(QColor(255, 255, 255));
		painter.drawEllipse(toggleRect);
	}

	void mousePressEvent(QMouseEvent *event) override {
		// Toggle the state on mouse press
		isChecked = !isChecked;
		animateToggle();
	}

private slots:
	void animateToggle() {
		// Start the animation to smoothly move the toggle
		animation->setStartValue(animationProgress);
		animation->setEndValue(isChecked ? 1.0 : 0.0);
		animation->start();
	}

private:
	bool isChecked;  // State of the toggle button
	QPropertyAnimation *animation;
	qreal animationProgress;  // Used to interpolate between 0 and 1 for the toggle position

	// Property to allow animation control
	Q_PROPERTY(qreal animationProgress READ getAnimationProgress WRITE setAnimationProgress)

	qreal getAnimationProgress() const {
		return animationProgress;
	}

	void setAnimationProgress(qreal progress) {
		animationProgress = progress;
		update();
	}
};


#endif //GSBTOGGLEBUTTON_H
