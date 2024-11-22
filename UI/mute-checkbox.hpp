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
			    image: url(:/Light/mute.svg);
			}

			QCheckBox::indicator:unchecked {
			    image: url(:/Light/settings/audio.svg);
			}

			QCheckBox::indicator:unchecked:hover {
			    image: url(:/Light/settings/audio.svg);
			}

			QCheckBox::indicator:unchecked:focus {
			    image: url(:/Light/settings/audio.svg);
			}

			QCheckBox::indicator:checked:hover {
			    image: url(:/Light/mute.svg);
			}

			QCheckBox::indicator:checked:focus {
			    image: url(:/Light/mute.svg);
			}

			QCheckBox::indicator:checked:disabled {
			    image: url(:/Light/mute.svg);
			}

			QCheckBox::indicator:unchecked:disabled {
			    image: url(:/Light/settings/audio.svg);
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
