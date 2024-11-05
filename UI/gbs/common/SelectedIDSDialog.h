#ifndef SELECTED_IDS_DIALOG_H
#define SELECTED_IDS_DIALOG_H

#include <QListWidget>
#include <QCheckBox>
#include <QRadioButton>
#include <QDialog>
#include <QPushButton>
#include <QFrame>
class SelectedIDSDialog : public QDialog {
	Q_OBJECT
public:
	SelectedIDSDialog(QWidget *parent = nullptr) : QDialog(parent)
	{
		setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
					  QSizePolicy::Expanding));
		setWindowFlags(Qt::FramelessWindowHint |
			       Qt::WindowMinimizeButtonHint |
			       Qt::WindowMaximizeButtonHint |
			       Qt::WindowCloseButtonHint);
		// 主布局
		QVBoxLayout *mainLayout = new QVBoxLayout(this);

		// 标题和右侧按钮
		QHBoxLayout *headerLayout = new QHBoxLayout();
		QLabel *titleLabel = new QLabel("选择抖音直播间");
		selectButton = new QRadioButton("全选选择");
		headerLayout->addWidget(titleLabel);
		headerLayout->addStretch();
		headerLayout->addWidget(selectButton);
		mainLayout->addLayout(headerLayout);

		// 列表区域
		QScrollArea *scrollArea = new QScrollArea(this);
		scrollArea->setWidgetResizable(true); // 自适应大小

		// 列表
		listWidget = new QListWidget(this);
		listWidget->setStyleSheet("border: none;"); // 设置无边框

		mainLayout->addWidget(listWidget);

		// 确认按钮
		QPushButton *confirmButton = new QPushButton("确认", this);
		mainLayout->addWidget(confirmButton);

		// 信号与槽
		connect(selectButton, &QRadioButton::clicked, this,
			&SelectedIDSDialog::allSelected);
		connect(confirmButton, &QPushButton::clicked, [this]() {
			QString submitData;
			for (int i = 0; i < listWidget->count(); ++i) {
				QListWidgetItem *item = listWidget->item(i);
				if (item) {
					// 获取项关联的 QWidget
					QWidget *widget =
						listWidget->itemWidget(item);
					if (widget) {
						QCheckBox *checkBox =
							widget->findChild<
								QCheckBox *>(); // 查找复选框
						if (checkBox) {
							if (checkBox->isChecked()) {
								QString str =
									checkBox->text();
							}
						}

						QLabel *label = widget->findChild<
							QLabel *>(); // 查找复选框
						if (label) {
							QString str =
								label->text();
							if (!str.isEmpty()) {
								int index = str.indexOf(
									" (");
								if (index > 0) {
									submitData += str.left(
										index);
								}
								submitData += ",";
								
							}
						}
					}
				}
			}
			submitData = this->ids.left(this->ids.length() - 1);
			emit dataSubmitted(submitData); // 发出信号
			accept();
		});
	}

	void allSelected(bool checked)
	{

		for (int i = 0; i < listWidget->count(); ++i) {
			QListWidgetItem *item = listWidget->item(i);
			if (item) {

				// 获取项关联的 QWidget
				QWidget *widget = listWidget->itemWidget(item);
				if (widget) {
					QCheckBox *checkBox = widget->findChild<
						QCheckBox *>(); // 查找复选框
					if (checkBox) {
						checkBox->setChecked(checked);
					}
				}
			}
		}
		if (checked) {
			selectButton->setText("取消全选");
		} else {
			selectButton->setText("全选选择");
		}
	}
signals:
	void dataSubmitted(const QString &data); // 定义信号
public:
	void addListItem(const QString &text, const QString &id)
	{
		// 创建一个自定义的 widget
		QWidget *itemWidget = new QWidget();
		
		QVBoxLayout *itemLayout = new QVBoxLayout(itemWidget);

		QHBoxLayout *hLayout = new QHBoxLayout();
		QLabel *label = new QLabel(text);
		QCheckBox *checkBox = new QCheckBox();
		hLayout->addWidget(label);
		hLayout->addWidget(checkBox);
		hLayout->setAlignment(Qt::AlignLeft |
				      Qt::AlignVCenter); // 左对齐
		hLayout->setSpacing(100);                // 控制间距

		// 添加下划线
		QFrame *line = new QFrame();
		line->setFrameShape(QFrame::HLine); // 设置为水平线
		//line->setFrameShadow(QFrame::Sunken); // 线的阴影效果
		line->setStyleSheet("background-color: gray;"); // 线的颜色

		itemLayout->addLayout(hLayout);
		itemLayout->addWidget(line); // 添加下划线
		itemLayout->setSpacing(0);   // 不要项之间的额外间距

		itemWidget->setLayout(itemLayout);

		// 将自定义 widget 添加到 QListWidget
		QListWidgetItem *listItem = new QListWidgetItem();
		listItem->setSizeHint(itemWidget->sizeHint()); // 设置项的大小
		listWidget->addItem(listItem);
		listWidget->setItemWidget(listItem, itemWidget);
		ids += id;
		ids += ",";
	}

public:
	QString ids;
	QPushButton *confirmButton;
	QRadioButton *selectButton;
	QListWidget *listWidget;
};
#endif //SELECTED_IDS_DIALOG_H
