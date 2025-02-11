#include <QClipboard>
#include "GBSStreamSelector.h"
#include "ui_GBSStreamSelector.h"
#include "gbs/common/GBSHttpClient.h"
#include "gbs/GBSMainCollector.h"

GBSStreamSelector::GBSStreamSelector(QWidget *parent) : QDialog(parent), ui(new Ui::GBSStreamSelector)
{
	ui->setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
	//setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

	ui->btnClose->setStyleSheet("QPushButton {"
				    "   background-image: url(:gbs/images/gbs/biz/gbs-close-window.png);"
				    "  border-radius: 5px;" // 圆角
				    "}"
				    "QPushButton:hover {"
				    "  border: none;"
				    "   background-color: #F9F9F9;"
				    "}"
				    "QPushButton:pressed {"
				    "   background-color: #D1D8DD;" // 按下时背景颜色
				    "   padding-left: 3px;"         // 向左移动 3px
				    "   padding-top: 3px;"          // 向上移动 3px
				    "   background-repeat: no-repeat;"
				    "   background-position: center;"
				    "}");

	ui->widget_2->setStyleSheet("QWidget { "
				    "background: qlineargradient("
				    "spread:pad, "
				    "x1:0, y1:0, x2:1, y2:1, "
				    "stop:0.1543 rgba(237, 255, 246, 255), "
				    "stop:0.822 rgba(255, 255, 255, 255)"
				    "); "
				    "border: none; "
				    "}"
				    "#myButton {"
				    "background-color:#00C566;"
				    "border: 1px solid white;"
				    "border-radius:10px;"
				    "font-size:20px;"
				    "}"
				    "#myButton:pressed {"
				    "   padding-left: 3px;" // 向左移动 3px
				    "   padding-top: 3px;"  // 向上移动 3px
				    "   background-repeat: no-repeat;"
				    "   background-position: center;"
				    "}");
	ui->widget->setStyleSheet("QWidget {"
				  "background-color:#0xFFFFFF;"
				  "background:#0xFFFFFF;"
				  "border:none;"
				  "}");

	connect(ui->btnClose, &QPushButton::clicked, this, [this]() { close(); });

	connect(ui->btnCancel, &QPushButton::clicked, this, [this]() { reject(); });
	connect(ui->btnOK, &QPushButton::clicked, this, [this]() {

		std::list<std::string> devices;
		for (auto button : buttons) {
			if (button->isChecked()) {
				std::string device = button->getUinqueId();
				devices.push_back(device);
			}
		}
		if (devices.size() > 0) {
			GBSPushStreamInfo info = GBSMainCollector::getInstance()->getPushStreamInfo();
			GBSHttpClient::getInstance()->startTranscribeLiveV2(info.getLiveAccountId(), devices,
									    info.getId());
		}
		

		accept();
	});

	// 创建一个 QScrollArea 用于添加滚动条
	scrollArea = new QScrollArea(ui->widget_2);
	scrollArea->setWidgetResizable(true);       // 让内容自适应区域大小
	scrollArea->setFrameShape(QFrame::NoFrame); // 隐藏滚动区域的边框

	// 创建一个 QWidget 用于作为滚动区域的内容容器
	QWidget *scrollContentWidget = new QWidget();
	QGridLayout *gridLayout = new QGridLayout(scrollContentWidget); // 使用 QGridLayout
	scrollContentWidget->setLayout(gridLayout);

	// 将 QScrollArea 的内容设置为 scrollContentWidget
	scrollArea->setWidget(scrollContentWidget);

	// 将 scrollArea 设置为主布局的一部分
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	ui->gridLayout_2->addWidget(scrollArea);
	setLayout(mainLayout);

	// 初始化行列计数
	rowIndex = 0;
	heightIndex = 0;
}

GBSStreamSelector::~GBSStreamSelector()
{
	delete ui;
}

void GBSStreamSelector::addStreamButton(CompositeButton *button)
{
	buttons.push_back(button);
	// 获取滚动区域的布局
	QGridLayout *gridLayout = qobject_cast<QGridLayout *>(scrollArea->widget()->layout());
	if (gridLayout) {
		// 计算行列位置
		int row = rowIndex;
		int col = heightIndex;

		qDebug() << "row " << row << " col " << col;
		// 将新按钮添加到 gridLayout
		gridLayout->addWidget(button, row, col);

		// 更新行列索引
		if (++heightIndex >= heightMax) {
			heightIndex = 0;
			rowIndex++;
		}

		// 如果超出最大高度，启用滚动
		if (rowIndex >= rawMax) {
			scrollArea->setWidgetResizable(true);
		}
	}
}


void GBSStreamSelector::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
		event->accept();
	}
	QDialog::mousePressEvent(event);
}

void GBSStreamSelector::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton) {
		move(event->globalPosition().toPoint() - dragPosition);
		event->accept();
	}
	QDialog::mouseMoveEvent(event);
}
