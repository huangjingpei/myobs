#pragma once

#include <QCheckBox>

class MuteCheckBox : public QCheckBox {
	Q_OBJECT

public:
	MuteCheckBox(QWidget *parent = nullptr) : QCheckBox(parent)
	{
		setTristate(true);
		//setProperty("class", "indicator-mute");
		QString styleSheet = R"(
			QCheckBox::indicator:checked {
			    image: url(:/res/images/mute.svg);
			}

			QCheckBox::indicator:unchecked {
			    image: url(:/settings/images/settings/audio.svg);
			}
			)";

		// 应用到 QCheckBox
		setStyleSheet(styleSheet);

	}

protected:
	/* While we need it to be tristate internally, we don't want a user being
	 * able to manually get into the partial state. */
	void nextCheckState() override
	{
		if (checkState() != Qt::Checked)
			setCheckState(Qt::Checked);
		else
			setCheckState(Qt::Unchecked);
	}
};
