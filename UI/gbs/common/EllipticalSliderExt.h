#ifndef ELLIPTICALSLIDEREXT_H
#define ELLIPTICALSLIDEREXT_H
#include <QSlider>
#include <QPainter>
#include <QDebug>
#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>

class EllipticalSliderExt : public QSlider {
    Q_OBJECT
public:
    explicit EllipticalSliderExt(QWidget *parent = nullptr) : EllipticalSliderExt(300, 40, parent) {
    }
    explicit EllipticalSliderExt(int w, int h, QWidget *parent = nullptr) : QSlider(Qt::Horizontal, parent), sliderHeight(h) {
        setFixedSize(w, h);
        setRange(0, 100);
    }

    void setRealSize(int w, int h) {
        setFixedSize(w, h);
		sliderHeight = h; // Update the height
		update(); // Redraw to reflect height change
    }

	void setSliderHeight(int h) {
		sliderHeight = h;
		setFixedSize(width(), h); // Update widget height as well
		update(); // Redraw slider
	}

	int sliderHeight;

    void setRealRange(int begin, int end) {
        setRange(begin, end);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
	    Q_UNUSED(event);

	    QPainter painter(this);
	    painter.setRenderHint(QPainter::Antialiasing);

	    // Calculate groove rectangle based on sliderHeight
	    int grooveHeight = sliderHeight / 4; // Make groove much narrower (1/10 of height)
	    QRect grooveRect = QRect(0, (sliderHeight - grooveHeight) / 2, width(), grooveHeight); // Centered vertically

	    // Calculate filled width
	    int filledWidth = static_cast<int>((double(this->value()) / this->maximum()) * grooveRect.width());

	    // Draw the un-filled groove (gray rounded rectangle)
	    painter.setBrush(QColor("#808080")); // Grey
	    painter.setPen(Qt::NoPen);
	    painter.drawRoundedRect(grooveRect, grooveHeight / 2, grooveHeight / 2); // Rounded corners

	    // Draw the filled groove (red rounded rectangle)
	    if (filledWidth > 0) {
		    painter.setBrush(QColor("#EB3F5E")); // Red
		    painter.drawRoundedRect(QRect(grooveRect.x(), grooveRect.y(), filledWidth, grooveRect.height()),
					    grooveHeight / 2, grooveHeight / 2);
	    }

	    // Calculate original handle radius and position (handle)
	    int handleRadius = grooveHeight / 2; // Handle radius based on grooveHeight (match groove)
	    int handleX = qMax(handleRadius, qMin(filledWidth, grooveRect.width() - handleRadius)); // Ensure it doesn't go out of bounds

	    // Calculate the original handle rectangle
	    QRect handleRect = QRect(handleX - handleRadius, sliderHeight / 2 - handleRadius, handleRadius * 2, handleRadius * 2);

	    // Draw the original handle (red dot, as a handle)
	    painter.setBrush(QColor("#EB3F5E")); // Red
	    painter.drawEllipse(handleRect);     // Draw the original handle

	    // Calculate the extra handle radius and position
	    int extraHandleRadius = sliderHeight / 4; // Make the bigger handle smaller.  Adjust as needed
	    int extraHandleX = filledWidth;           // New handle located on the right edge of the red filled part

        // Ensure the new handle does not exceed the slider's boundaries, taking into account the larger radius
        extraHandleX = qMax(extraHandleRadius, qMin(extraHandleX, grooveRect.width() - extraHandleRadius));

	    // Calculate the extra handle rectangle
	    QRect extraHandleRect = QRect(extraHandleX - extraHandleRadius, sliderHeight / 2 - extraHandleRadius,
					  extraHandleRadius * 2, extraHandleRadius * 2);

	    // Draw the extra handle (red dot, consistent with the color of the drawn part)
	    painter.setBrush(QColor("#EB3F5E"));  // Red
	    painter.drawEllipse(extraHandleRect); // Draw the new handle

	    // Draw the label text (only displayed near the original handle)
	    painter.setPen(Qt::white); // Set text color to white, contrast with grey background
	    QFont font = painter.font();
	    font.setPointSize(10); // Set font size to 10 points
	    painter.setFont(font);
	    painter.drawText(handleX + handleRadius - 20, grooveRect.y() - grooveHeight / 2 - 15, 40, 15, Qt::AlignCenter,
			     QString::number(this->value()) + "%");
    }

};

#endif //ELLIPTICALSLIDEREXT_H
