#ifndef GBSSTREAMSELECTOR_H
#define GBSSTREAMSELECTOR_H
#include <QMouseEvent>
#include <QDialog>
#include <QPoint>
#include <QLabel>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QScrollArea>
#include <QList>
namespace Ui {
class GBSStreamSelector;
}
class CompositeButton : public QWidget {
	  Q_OBJECT

public:
	// 构造函数：传入标签文本和是否勾选的状态
	CompositeButton(const QString &labelText, bool isChecked, QWidget *parent = nullptr) : QWidget(parent)
	{
		m_checkBox = new QCheckBox(this);
		m_label = new QLabel(this);
		m_label->setText(labelText);

		// 设置 QCheckBox 的勾选状态
		m_checkBox->setChecked(isChecked);

		// 创建 QHBoxLayout 将两个控件水平排列
		QHBoxLayout *layout = new QHBoxLayout(this);
		layout->addWidget(m_checkBox);
		layout->addWidget(m_label);
		layout->setStretchFactor(m_checkBox, 1);
		layout->setStretchFactor(m_label, 2);

		// 设置大小为 64x64
		setFixedSize(64, 64);

		// 设置布局
		setLayout(layout);

		setStyleSheet("QWidget {"
			      "   background-color: #656565;"
			      "   border: none;"       // 无边框
			      "   border-radius: 3px;" // 圆角
			      "   font-size: 10px;"
			      "   text-align: left;"
			      "}");
	}

public:
	bool isChecked() { return m_checkBox->isChecked(); }
	int getLiveId() { return liveAccountId; }
	std::string getUinqueId() { return uuid; }

	void setConstraint(int liveId, std::string dname)
	{
		liveAccountId = liveId;
		uuid = dname;
	}

protected:
	// 重载 mousePressEvent 和 mouseReleaseEvent 来模拟点击行为
	void mousePressEvent(QMouseEvent *event) override
	{
		// 判断鼠标点击是否在当前 QWidget 范围内（即整个 CompositeButton 区域）
		if (rect().contains(event->pos())) {
			// 模拟点击 QCheckBox
			m_checkBox->setChecked(!m_checkBox->isChecked());
		}
		// 调用父类的处理方式
		QWidget::mousePressEvent(event);
	}

	void mouseReleaseEvent(QMouseEvent *event) override
	{
		// 如果需要处理点击释放后的行为，可以在这里进行
		QWidget::mouseReleaseEvent(event);
	}

private:
	QCheckBox *m_checkBox;
	QLabel *m_label;
	int liveAccountId{-1};
	std::string uuid{""};
};

class GBSStreamSelector : public QDialog {
	Q_OBJECT

public:
	explicit GBSStreamSelector(QWidget *parent = nullptr);
	~GBSStreamSelector();

public:
	void addStreamButton(CompositeButton *button);

private:
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;

private:
	Ui::GBSStreamSelector *ui;
	QPoint dragPosition;
	int rowIndex{0};
	int heightIndex{0};
	int rawMax{6};
	int heightMax{10};
	QScrollArea *scrollArea;
	QList<CompositeButton *> buttons;
};

#endif // GBSSTREAMSELECTOR_H
