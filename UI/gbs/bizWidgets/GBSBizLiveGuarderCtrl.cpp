#include "GBSBizLiveGuarderCtrl.h"
#include "ui_GBSBizLiveGuarderCtrl.h"
#include "GBSMsgDialog.h"
#include "gbs/bizWidgets/GBSAddBroker.h"
#include "gbs/bizWidgets/GBSRemoveBroker.h"

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QGridLayout>
#include <QLabel>

#include "qt-wrappers.hpp"


#include "window-basic-main.hpp"
#include "display-helpers.hpp"
#include <QMovie>
class GridButtons : public QWidget {


public:
    GridButtons(QWidget *parent = nullptr) : QWidget(parent), currentPage(0), buttonsPerPage(60), buttonsPerRow(12), buttonCount(0), pageCount(0) {
        QHBoxLayout *mainLayout = new QHBoxLayout(this);
        QVBoxLayout *vertLayout = new QVBoxLayout(this);

        // 创建翻页按钮
        lblPage = new QLabel(this);
        QPushButton* btnHelp = new QPushButton(this);
        QPushButton *btnUp = new QPushButton(this);
        QPushButton *btnDown = new QPushButton(this);
        connect(btnUp, &QPushButton::clicked, this, &GridButtons::scrollUp);
        connect(btnDown, &QPushButton::clicked, this, &GridButtons::scrollDown);
        connect(btnHelp, &QPushButton::clicked, this, &GridButtons::showHelp);
        btnUp->setStyleSheet(
            "QPushButton {"
            "   background-image: url(:gbs/images/gbs/biz/gbs-arrow-up-20x.png);"
            "   background-repeat: no-repeat;"
            "   background-position: center;"
            "   color: white;"
            "   border: none;" // 无边框
            "   border-radius: 5px;" // 圆角
            "   font-size: 16px;"
            "   padding: 0;" // 不添加内边距
            "}"
            "QPushButton:pressed {"
            "   background-color: #D1D8DD;" // 按下时背景颜色
            "   padding-left: 3px;"  // 向左移动 3px
            "   padding-top: 3px;"    // 向上移动 3px
            "   background-repeat: no-repeat;"
            "   background-position: center;"
            "}"
            );

        btnDown->setStyleSheet(
            "QPushButton {"
            "   background-image: url(:gbs/images/gbs/biz/gbs-arrow-down-20px.png);"
            "   background-repeat: no-repeat;"
            "   background-position: center;"
            "   color: white;"
            "   border: none;" // 无边框
            "   border-radius: 5px;" // 圆角
            "   font-size: 16px;"
            "   padding: 0;" // 不添加内边距
            "}"
            "QPushButton:pressed {"
            "   background-color: #D1D8DD;" // 按下时背景颜色
            "   padding-left: 3px;"  // 向左移动 3px
            "   padding-top: 3px;"    // 向上移动 3px
            "   background-repeat: no-repeat;"
            "   background-position: center;"
            "}"
            );


        btnHelp->setStyleSheet(
            "QPushButton {"
            "   background-image: url(:gbs/images/gbs/biz/gbs-tooltip.png);"
            "   background-repeat: no-repeat;"
            "   background-position: center;"
            "   color: white;"
            "   border: none;" // 无边框
            "   border-radius: 5px;" // 圆角
            "   font-size: 16px;"
            "   padding: 0;" // 不添加内边距
            "}"
            "QPushButton:pressed {"
            "   background-color: #D1D8DD;" // 按下时背景颜色
            "   padding-left: 3px;"  // 向左移动 3px
            "   padding-top: 3px;"    // 向上移动 3px
            "   background-repeat: no-repeat;"
            "   background-position: center;"
            "}"
            );


        // 创建 QStackedWidget 来存放各页按钮
        stackedWidget = new QStackedWidget(this);

        // 创建多页按钮并添加到 stackedWidget
        // addButtonPages();
        for (int i = 0; i < 116; i++) {
            addButton("text");
        }

        // 将翻页按钮放到主布局中
        mainLayout->addWidget(stackedWidget);  // 左边放按钮页面
        QHBoxLayout *horizLayout = new QHBoxLayout();
        horizLayout->addStretch(); // 添加伸缩因子，推送 btnHelp 向右
        horizLayout->addWidget(btnHelp); // 将 btnHelp 添加到右侧
        vertLayout->setSpacing(60); // 设置上下间距为 60px
        vertLayout->addLayout(horizLayout);
        vertLayout->addWidget(btnUp);
        vertLayout->addWidget(btnDown);
        vertLayout->addWidget(lblPage);
        mainLayout->addLayout(vertLayout);  // 右边放翻页按钮

        connect(btnHelp, &QPushButton::clicked, this, [this]() {
            QWidget* widget = new QWidget;
            QVBoxLayout *layout = new QVBoxLayout(widget);

            layout->setAlignment(Qt::AlignCenter); // 整体内容居中
	    QSpacerItem *spacer0 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	    layout->addItem(spacer0);

            layout->addLayout(createButtonLabelLayout("", "#C9DCFF", "浅色代表未开播未连接"));
	    layout->addLayout(createButtonLabelLayout("", "#00C566", "绿色代表正常分发"));
	    layout->addLayout(createButtonLabelLayout("", "#FFCD19", "黄色代表在线人数居多"));
	    layout->addLayout(createButtonLabelLayout("", "#EB3F5E", "红色代表直播间异常"));
	    layout->addLayout(createButtonLabelLayout("", "#2667FE", "蓝色语音输入按钮"));
	    QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	    layout->addItem(spacer);
            GBSMsgDialog *dialog = new GBSMsgDialog("颜色含义解释", layout, this);
            dialog->exec();
        });

    }

    QHBoxLayout* createButtonLabelLayout(const QString &buttonText, const QString &color, const QString &labelText) {
        QHBoxLayout *layout = new QHBoxLayout;

        QPushButton *button = new QPushButton(buttonText);
	QString styleSheet = "QPushButton { background-color: %1; color: white; border: none; padding: 10px; }";
	QString style = styleSheet.arg(color);
	button->setStyleSheet(style);
        button->setFixedSize(38, 28); // 设置按钮大小

        QLabel *label = new QLabel(labelText);

        layout->addWidget(button);
        layout->addWidget(label);
        layout->setAlignment(Qt::AlignLeft);

        return layout;
    }
private slots:
    // 翻页逻辑
    void scrollUp() {
        int currentIndex = stackedWidget->currentIndex();
        if (currentIndex > 0) {
            stackedWidget->setCurrentIndex(--currentIndex);
        }
        lblPage->setText(QString("%1/%2 页").arg(currentIndex+1).arg(pageCount));
    }

    void scrollDown() {
        int currentIndex = stackedWidget->currentIndex();
        if (currentIndex < pageCount -1) {
            stackedWidget->setCurrentIndex(++currentIndex);
        }
        lblPage->setText(QString("%1/%2 页").arg(currentIndex+1).arg(pageCount));

    }

    void showHelp() {

    }

    void setLiveRoomExecption(std::string name) {
        //background: #EB3F5E;

    }

    void setLiveRoomHot(std::string name) {
        //background: #FFCD19;

    }

    void setLiveRoomOffline(std::string name) {
        //background: #C9DCFF;
    }

    void setLiveRoomWorking(std::string name) {
        //background: #00C566;

    }

private:
    QLabel *lblPage;
    QStackedWidget *stackedWidget;
    int currentPage;    // 当前页数
    const int buttonsPerPage;  // 每页的按钮数
    const int buttonsPerRow;   // 每行按钮数量
    int buttonCount;
    int pageCount;

    void addButton(QString text) {
        // 如果当前页面已经满了，创建新页面
        if (buttonCount % buttonsPerPage == 0) {
            addButtonPage();  // 创建新页面
            stackedWidget->setCurrentIndex(pageCount);  // 切换到新创建的页面
            pageCount++;  // 增加页面计数
        }

        // 获取当前页面
        QWidget* currentPage = stackedWidget->currentWidget();
        qDebug() << "page count " << pageCount << " button count " << buttonCount << " currentPage:" << currentPage;

        if (currentPage) {
            // 添加新的按钮到当前页面
            QPushButton* button = new QPushButton(text, this);
	    QLabel *iconLabel = new QLabel(button);

            int row = (buttonCount % buttonsPerPage) / buttonsPerRow;
            int col = (buttonCount % buttonsPerPage) % buttonsPerRow;
            button->setFixedSize(78,60);
	    QMovie *movie = new QMovie(":gbs/images/gbs/biz/gbs-sound-wave.gif", QByteArray(), iconLabel);
	    if (!movie->isValid()) {
		    qDebug() << "Failed to load GIF file.";
	    }
	    movie->setScaledSize(QSize{48, 48});

	    iconLabel->setText("TTTT");
	    iconLabel->setFixedSize(78, 60); // 设置 GIF 的显示区域大小

            //iconLabel->setPixmap(QPixmap(":gbs/images/gbs/biz/gbs-wave.png").scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	    iconLabel->setMovie(movie);
            //iconLabel->setGeometry(button->width() - 25, 0, 20, 20); // 设置位置
	    iconLabel->setAlignment(Qt::AlignCenter); // 设置 GIF 居中显示

            iconLabel->hide(); // 默认隐藏
            // 当按钮被按下时
            connect(button, &QPushButton::pressed, [=]() {
                iconLabel->show(); // 显示小图片

                button->setStyleSheet("QPushButton {"
				      "   background-color: #2667FE;"
				      "   border: none;"       // 无边框
				      "   border-radius: 3px;" // 圆角
				      "   font-size: 20px;"
				      "   text-align: center;"
				      "}");
		iconLabel->update(); // 强制重绘
		movie->start();

            });

            // 当按钮释放时，隐藏小图片
            connect(button, &QPushButton::released, this, [=]() {
                iconLabel->hide();
                button->setStyleSheet(
                    "QPushButton {"
                    "   background-color: #E4ECFF;"
                    "   border: none;" // 无边框
                    "   border-radius: 3px;" // 圆角
                    "   font-size: 20px;"
                    "   text-align: center;"
                    "}"
                    );
		    movie->stop();
            });


            button->setStyleSheet(
                "QPushButton {"
                "   background-color: #E4ECFF;"
                "   border: none;" // 无边框
                "   border-radius: 3px;" // 圆角
                "   font-size: 20px;"
                "   text-align: center;"
                "}"
                );

            QGridLayout *layout = qobject_cast<QGridLayout*>(currentPage->layout());
            if (layout) {
                layout->addWidget(button, row, col);  // 添加按钮到布局
            }

        }

        buttonCount++;  // 计数加一
    }

    void addButtonPage() {
        QWidget *pageWidget = new QWidget(this);  // 创建一个新的页面
        QGridLayout *gridLayout = new QGridLayout(pageWidget);  // 设置网格布局

        // 设置按钮间距和布局边距
        gridLayout->setHorizontalSpacing(10);
        gridLayout->setVerticalSpacing(10);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout->setSpacing(10);

        // 将新页面添加到 QStackedWidget 中
        pageWidget->setLayout(gridLayout);

        // 添加 QSpacerItem 来确保按钮排列在上方，空白区域填充在下面
        QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(spacer, buttonsPerPage / buttonsPerRow, 0, 1, buttonsPerRow);  // 在最后一行添加 SpacerItem

        stackedWidget->addWidget(pageWidget);
    }

    // 创建按钮页面并添加到 QStackedWidget
    void addButtonPages() {
        int totalButtons = 100;  // 假设总共有 100 个按钮
        int pages = (totalButtons + buttonsPerPage - 1) / buttonsPerPage;  // 计算页数

        for (int page = 0; page < pages; ++page) {
            QWidget *pageWidget = new QWidget(this);  // 创建一个页面
            QGridLayout *gridLayout = new QGridLayout(pageWidget);  // 网格布局
            // 设置统一的按钮间距
            gridLayout->setHorizontalSpacing(10);  // 设置列间距为 10
            gridLayout->setVerticalSpacing(10);    // 设置行间距为 10
            gridLayout->setContentsMargins(0, 0, 0, 0);  // 设置布局的边距为 0
            gridLayout->setSpacing(10);  // 设置统一的组件间距
            // 填充该页面的按钮
            for (int i = 0; i < buttonsPerPage; ++i) {
                int buttonIndex = page * buttonsPerPage + i;
                if (buttonIndex >= totalButtons) {
                    break;  // 如果按钮数量不足，退出循环
                }
                QPushButton *btn = new QPushButton(QString("D%1").arg(buttonIndex + 1), this);
                int row = i / buttonsPerRow;
                int col = i % buttonsPerRow;
                gridLayout->addWidget(btn, row, col);
            }

            stackedWidget->addWidget(pageWidget);  // 将该页面添加到 QStackedWidget 中
        }
    }

};

#include <QTableWidget>
#include <QHeaderView>
#include <QLineEdit>
#include <QResizeEvent>

class MyTableWidget : public QTableWidget {
public:
	explicit MyTableWidget(QWidget *parent = nullptr) : QTableWidget(parent)
	{
		setStyleSheet("QTableWidget {"
			      "    border: 1px solid #1B2846;"           // 表格外边框
			      "    border-radius: 4px;"                  // 边框圆角
			      "    background: #C9DCFF;"                 // 表格背景色
			      "    gridline-color: #1B2846;"             // 网格线颜色
			      "    color: #FFFFFF;"                      // 字体颜色
			      "    font-size: 14px;"                     // 字体大小
			      "    selection-background-color: #007BFF;" // 选中行背景色
			      "    selection-color: #FFFFFF;"            // 选中行文字颜色
			      "}"
			      );
		setColumnCount(10);
		setHorizontalHeaderLabels({"序号", "激活编号", "备注编号", "激活备注客户名称", "开播时间", "刷新时间",
					   "直播账号", "直播数量（总）", "直播时长", "操作"});

		horizontalHeader()->setStretchLastSection(true);                // 最后一列填满
		horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽均匀分布
		//setEditTriggers(QAbstractItemView::NoEditTriggers);             // 禁止编辑
		setSelectionMode(QAbstractItemView::SingleSelection);           // 单选
		setSelectionBehavior(QAbstractItemView::SelectRows);            // 按行选择

		setShowGrid(true); // 显示网格线
		connect(this, &QTableWidget::cellPressed, this, &MyTableWidget::onCellClicked);

	}
	void addRow(const QStringList &rowData)
	{
		++index;
		if (rowData.size() != columnCount()) {
			qWarning("Row data does not match column count.");
			return;
		}

		int row = rowCount();
		insertRow(row);
		

		for (int col = 0; col < rowData.size() - 1; ++col) {
			QTableWidgetItem *item = new QTableWidgetItem(rowData[col]);
			item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			setItem(row, col, item);
		}
	}
	void removeRow(int row)
	{
		if (row >= 0 && row < rowCount()) {
			QTableWidget::removeRow(row);
		} else {
			qWarning("Invalid row index.");
		}
	}

signals:
	void onRowClick(int row, int column);


public slots:
	void onCellClicked(int row, int column) {
		selRow = row;
		selCol = column;
		qDebug() << "setCurrentCell " << row << " col " << column;
		//setCurrentCell(row, column);
		
	}


private:
	int index = 0;
	int selRow = 0;
	int selCol = 0;
};

class CustomSearchLineEdit : public QLineEdit {
public:
	CustomSearchLineEdit(QWidget *parent = nullptr) : QLineEdit(parent)
	{
		setPlaceholderText("模糊搜索");
		setFixedSize(192, 27);
		// 创建并设置放大镜图标
		iconLabel = new QLabel(this);
		iconLabel->setPixmap(QPixmap(":gbs/images/gbs/biz/gbs-search-24px.png")); // 放大镜图标路径
		iconLabel->setFixedSize(24, 24);
		iconLabel->move(10, (height() - iconLabel->height()) / 2); // 图标左边距10
		iconLabel->setVisible(true);

		// 创建并设置 QComboBox
		QPushButton *button = new QPushButton(this);
		button->setStyleSheet("QPushButton {"
				      "   background-image: url(:/Resources/gbs-arrow-down-24px.png);"
				      "   background-repeat: no-repeat;"
				      "   background-position: center;"
				      "   color: white;"
				      "   border: none;"       // 无边框
				      "   border-radius: 5px;" // 圆角
				      "   font-size: 16px;"
				      "   padding: 0;" // 不添加内边距
				      "}");
		button->setFixedSize(24, 24);
		button->move(width() - button->width(), (height() - button->height()) / 2); // 右边距10

		// // 样式表：圆角边框、填充、内边距
		// setStyleSheet("QLineEdit {"
		//               "border: 1px solid #ccc;"
		//               "border-radius: 15px;"
		//               "padding-left: 40px;"  // 给左边图标留空间
		//               "padding-right: 70px;" // 给右边下拉框留空间
		//               "}");
		connect(this, &QLineEdit::textChanged, this, [this](const QString &text) {
			qDebug() << "text changed "
				 << "text" << text;
			if (!text.isEmpty()) {
				setText(text);
				iconLabel->setVisible(false);
			} else {
				iconLabel->setVisible(true);
			}
		});
	}

protected:
	void resizeEvent(QResizeEvent *event) override
	{
		QLineEdit::resizeEvent(event);

		// 确保图标和 button 在 QLineEdit 中的正确位置
		QWidget *iconLabel = findChild<QLabel *>();
		if (iconLabel)
			iconLabel->move(10, (height() - iconLabel->height()) / 2);

		QPushButton *button = findChild<QPushButton *>();
		if (button)
			button->move(width() - button->width() - 10, (height() - button->height()) / 2);
	}

private:
	QLabel *iconLabel;
};

class LiveManageWidget : public QWidget {

public:
	explicit LiveManageWidget(QWidget *parent = nullptr) : QWidget(parent)
	{

		QVBoxLayout *mainLayout = new QVBoxLayout;
		QHBoxLayout *naviLayout = new QHBoxLayout;
		QLabel *label = new QLabel("开播管理");
		label->setStyleSheet("QLabel {"
				     "   font-size: 16px;"
				     "}");
		QPushButton *btnAdd = new QPushButton();
		btnAdd->setFixedSize(24, 24);
		btnAdd->setStyleSheet("QPushButton {"
				      "   background-image: url(:gbs/images/gbs/biz/gbs-add-24px.png);"
				      "   background-repeat: no-repeat;"
				      "   background-position: center;"
				      "   color: white;"
				      "   border: none;"       // 无边框
				      "   border-radius: 5px;" // 圆角
				      "   font-size: 16px;"
				      "   padding: 0;" // 不添加内边距
				      "}");
		CustomSearchLineEdit *lineEdit = new CustomSearchLineEdit;
		naviLayout->addWidget(label);
		naviLayout->addWidget(lineEdit);
		naviLayout->addWidget(btnAdd);
		mainLayout->addLayout(naviLayout);
		MyTableWidget *table = new MyTableWidget();

		QPushButton *delButton = new QPushButton();
		delButton->setStyleSheet("QPushButton {"
					 "   background-image: url(:gbs/images/gbs/biz/gbs-trash-20px.png);"
					 "   background-repeat: no-repeat;"
					 "   background-position: center;"
					 "   color: white;"
					 "   border: none;"       // 无边框
					 "   border-radius: 5px;" // 圆角
					 "   font-size: 16px;"
					 "   padding: 0;" // 不添加内边距
					 "}");
		QPushButton *disableVideoButton = new QPushButton();
		disableVideoButton->setStyleSheet("QPushButton {"
						  "   background-image: url(:gbs/images/gbs/biz/gbs-disable-video.png);"
						  "   background-repeat: no-repeat;"
						  "   background-position: center;"
						  "   color: white;"
						  "   border: none;"       // 无边框
						  "   border-radius: 5px;" // 圆角
						  "   font-size: 16px;"
						  "   padding: 0;" // 不添加内边距
						  "}");
		QPushButton *shareButton = new QPushButton();
		shareButton->setStyleSheet("QPushButton {"
					   "   background-image: url(:gbs/images/gbs/biz/gbs-remote-share.png);"
					   "   background-repeat: no-repeat;"
					   "   background-position: center;"
					   "   color: white;"
					   "   border: none;"       // 无边框
					   "   border-radius: 5px;" // 圆角
					   "   font-size: 16px;"
					   "   padding: 0;" // 不添加内边距
					   "}");

		QStringList rowData = {QString("%1").arg(index++),
				       "54564568556",
				       "KS/CZQ/0101",
				       "王涛（广东省珠海市）",
				       "2022-12-12 16:15:00",
				       "2022-12-12 16:15:00",
				       "456555533",
				       "23台",
				       "50天 17:45:59",
				       ""};
		operationWidget = new QWidget;
		operationWidget->setStyleSheet("QWidget { alignment: center; }");
		QHBoxLayout *operation = new QHBoxLayout(operationWidget); // 在添加控件之前创建布局

		delButton->setFixedSize(20, 20);
		disableVideoButton->setFixedSize(20, 20);
		shareButton->setFixedSize(20, 20);

		operation->setSpacing(20); // 只设置一次间距
		operation->addWidget(delButton);
		operation->addWidget(disableVideoButton);
		operation->addWidget(shareButton);

		operationWidget->setLayout(operation); // 将布局设置给控件

		// 设置边距为0
		operation->setContentsMargins(QMargins(0, 0, 0, 0));
		operation->setAlignment(Qt::AlignCenter); // 设置对齐方式为居中

		table->addRow(rowData);
		table->setCellWidget(table->rowCount() - 1, rowData.size() - 1, operationWidget);
		mainLayout->addWidget(table);

		connect(delButton, &QPushButton::clicked, this, [table, this]() {
			QList<QTableWidgetItem *> selectedRows = table->selectedItems();
			if (!selectedRows.isEmpty()) {
				// 获取第一行的索引
				int row = selectedRows.first()->row();
				table->removeRow(row);
				
			} else {
				QHBoxLayout *layout = new QHBoxLayout;
				QLabel *label = new QLabel("请选中行后才能进行删除.");
				
				layout->addWidget(label);
				GBSMsgDialog *dialog = new GBSMsgDialog("错误提示", layout, this);
				dialog->exec();
			}

			int currentRow = table->currentRow();
			qDebug() << "currentRow " << currentRow;
			table->removeRow(currentRow);
			});


		QObject::connect(btnAdd, &QPushButton::clicked, this, [table, this]() {
			GBSAddBroker *broker = new GBSAddBroker(this);
			broker->show();
			#if 0
			QPushButton *delButton = new QPushButton();
			delButton->setStyleSheet("QPushButton {"
						 "   background-image: url(:gbs/images/gbs/biz/gbs-trash-20px.png);"
						 "   background-repeat: no-repeat;"
						 "   background-position: center;"
						 "   color: white;"
						 "   border: none;"       // 无边框
						 "   border-radius: 5px;" // 圆角
						 "   font-size: 16px;"
						 "   padding: 0;" // 不添加内边距
						 "}");
			QPushButton *disableVideoButton = new QPushButton();
			disableVideoButton->setStyleSheet("QPushButton {"
							  "   background-image: url(:gbs/images/gbs/biz/gbs-disable-video.png);"
							  "   background-repeat: no-repeat;"
							  "   background-position: center;"
							  "   color: white;"
							  "   border: none;"       // 无边框
							  "   border-radius: 5px;" // 圆角
							  "   font-size: 16px;"
							  "   padding: 0;" // 不添加内边距
							  "}");
			QPushButton *shareButton = new QPushButton();
			shareButton->setStyleSheet("QPushButton {"
						   "   background-image: url(:gbs/images/gbs/biz/gbs-remote-share.png);"
						   "   background-repeat: no-repeat;"
						   "   background-position: center;"
						   "   color: white;"
						   "   border: none;"       // 无边框
						   "   border-radius: 5px;" // 圆角
						   "   font-size: 16px;"
						   "   padding: 0;" // 不添加内边距
						   "}");

			QStringList rowData = {QString("%1").arg(index++),
					       "54564568556",
					       "KS/CZQ/0101",
					       "王涛（广东省珠海市）",
					       "2022-12-12 16:15:00",
					       "2022-12-12 16:15:00",
					       "456555533",
					       "23台",
					       "50天 17:45:59",
					       ""};
			QWidget *operationWidget = new QWidget;
			operationWidget->setStyleSheet("QWidget { alignment: center; }");
			QHBoxLayout *operation = new QHBoxLayout(operationWidget); // 在添加控件之前创建布局

			delButton->setFixedSize(20, 20);
			disableVideoButton->setFixedSize(20, 20);
			shareButton->setFixedSize(20, 20);

			operation->setSpacing(20); // 只设置一次间距
			operation->addWidget(delButton);
			operation->addWidget(disableVideoButton);
			operation->addWidget(shareButton);

			operationWidget->setLayout(operation); // 将布局设置给控件
			connect(delButton, &QPushButton::clicked, this, [table, this]() {
				int currentRow = table->currentRow();
				qDebug() << "currentRow " << currentRow;
				table->removeRow(currentRow);
			});

			// 设置边距为0
			operation->setContentsMargins(QMargins(0, 0, 0, 0));
			operation->setAlignment(Qt::AlignCenter); // 设置对齐方式为居中

			table->addRow(rowData);
			table->setCellWidget(table->rowCount() - 1, rowData.size() - 1, operationWidget);
			#endif
		});
		setLayout(mainLayout);

	}

private:
	QWidget *operationWidget;
	int index = 0;
	
};


GBSBizLiveGuarderCtrl::GBSBizLiveGuarderCtrl(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GBSBizLiveGuarderCtrl)
{
    ui->setupUi(this);

    GridButtons *buttons = new GridButtons(this);
    ui->horizontalLayout->addWidget(buttons);
    validWidget = buttons;

    //ui->tabWidget->setStyleSheet("QTabWidget::pane {"
				// "    border: none;" // 移除tab pane的边框
				// "}"
				// "QTabBar::tab {"
				// "    background: none;"   // tab的背景颜色
				// "    padding: 10px;"      // tab内容的填充
				// "    margin-right: 10px;" // 调整tab之间的水平间距
				// "    border: none;"       // 移除tab的边框
				// "    color: #FF0000;"
				// "    font-size:16px;"
				// "}"
				// "QTabBar::tab:first {"
				// "    margin-left: 100px;" // 调整第一个tab项的左外边距
				// "}"
				// "QTabBar::tab:selected {"
				// "    background: green;" // 选中tab的背景颜色
				// "    border: none;"        // 选中时也不显示边框
				// "    border-radius: 16px"
				// "    font-color:#00C566;"
				// "    border: 1px solid #00FF00; "
				// "}"
				// "QTabBar {"
				// "    qproperty-alignment: AlignCenter;" // 设置tab项居中对齐
				// "}"
				// "QTabBar::tab:hover {"
				// "    background: #E0E0E0; "
				// "    color: #0000FF; "
				// "}"
    //);
    ui->tabWidget->setStyleSheet("QTabWidget::pane {"
				 "    border: none;" // 移除tab pane的边框
				 "}"); // 清空 QTabWidget 的样式表

    ui->tabWidget->tabBar()->setStyleSheet("QTabBar::tab {"
				"    color: #78828A; "              // 默认字体颜色
				"    background: none; "       // 默认背景颜色
				 "    padding: 40px; "      // 内边距
				"    padding: 4px; "             // 内边距
				"    border-radius: 5px; "        // 圆角
				"    font-size: 14px;"         // 设置字体大小为16像素

				"}"
				"QTabBar::tab:selected {"
				"    color: #00C566; "              // 选中字体颜色
				"    background: none; "       // 选中背景颜色
				"    padding: 4px; "              // 内边距
				"}"
				"QTabBar::tab:first {"
				"    margin-left: 100px;" // 调整第一个tab项的左外边距
				"}"
    );

	 connect(ui->tabWidget, &QTabWidget::currentChanged, this, &GBSBizLiveGuarderCtrl::onTabChanged);
	   

	auto displayResize = [this]() {
		 struct obs_video_info ovi;

		 if (obs_get_video_info(&ovi))
			 ResizePreview(ovi.base_width, ovi.base_height);
	 };
	 connect(ui->wgtPreview, &OBSQTDisplay::DisplayResized, displayResize);
	auto addDisplay = [this](OBSQTDisplay *window) {
		OBSBasic *main = reinterpret_cast<OBSBasic *>(App()->GetMainWindow());
		 obs_display_add_draw_callback(window->GetDisplay(), GBSBizLiveGuarderCtrl::RenderMain, this);

		struct obs_video_info ovi;
		if (obs_get_video_info(&ovi))
			ResizePreview(ovi.base_width, ovi.base_height);
	};

	ui->wgtPreview->SetLocked(true);
	ui->wgtPreview->Init();
	connect(ui->wgtPreview, &OBSQTDisplay::DisplayCreated, addDisplay);

	OBSBasic *main = reinterpret_cast<OBSBasic *>(App()->GetMainWindow());
	main->addGuarderCtrlScene();
}

void GBSBizLiveGuarderCtrl::onTabChanged(int index) {


	if (validWidget != nullptr) {

		qDebug() << "GBSBizLiveGuarderCtrl " << index;
		ui->horizontalLayout->removeWidget(validWidget);
		delete validWidget;
		validWidget = nullptr;
		update();
	}
	
	if (index == 0 && index <=3) {
		GridButtons *buttons = new GridButtons(this);
		ui->horizontalLayout->addWidget(buttons);
		validWidget = buttons;

	} else if (index == 4) {
		LiveManageWidget *widget = new LiveManageWidget(this);
		ui->horizontalLayout->addWidget(widget);
		validWidget = widget;
	}
} 

GBSBizLiveGuarderCtrl::~GBSBizLiveGuarderCtrl()
{
	obs_display_remove_draw_callback(ui->wgtPreview->GetDisplay(), GBSBizLiveGuarderCtrl::RenderMain, this);
	OBSBasic *main = reinterpret_cast<OBSBasic *>(App()->GetMainWindow());
	main->removeGuarderCtrlScene();
    delete ui;
}



void GBSBizLiveGuarderCtrl::ResizePreview(uint32_t cx, uint32_t cy)
{
	QSize targetSize;
	bool isFixedScaling;
	obs_video_info ovi;

	/* resize preview panel to fix to the top section of the window */
	targetSize = GetPixelSize(ui->wgtPreview);

	isFixedScaling = ui->wgtPreview->IsFixedScaling();
	obs_get_video_info(&ovi);

	if (isFixedScaling) {
		previewScale = ui->wgtPreview->GetScalingAmount();

		ui->wgtPreview->ClampScrollingOffsets();

		GetCenterPosFromFixedScale(int(cx), int(cy), targetSize.width() - PREVIEW_EDGE_SIZE * 2,
					   targetSize.height() - PREVIEW_EDGE_SIZE * 2, previewX, previewY,
					   previewScale);
		previewX += ui->wgtPreview->GetScrollX();
		previewY += ui->wgtPreview->GetScrollY();

	} else {
		GetScaleAndCenterPos(int(cx), int(cy), targetSize.width() - PREVIEW_EDGE_SIZE * 2,
				     targetSize.height() - PREVIEW_EDGE_SIZE * 2, previewX, previewY, previewScale);
	}

	ui->wgtPreview->SetScalingAmount(previewScale);

	previewX += float(PREVIEW_EDGE_SIZE);
	previewY += float(PREVIEW_EDGE_SIZE);
}

void GBSBizLiveGuarderCtrl::RenderMain(void *data, uint32_t, uint32_t)
{
	GS_DEBUG_MARKER_BEGIN(GS_DEBUG_COLOR_DEFAULT, "RenderMain");

	GBSBizLiveGuarderCtrl *window = static_cast<GBSBizLiveGuarderCtrl *>(data);
	obs_video_info ovi;

	obs_get_video_info(&ovi);

	window->previewCX = int(window->previewScale * float(ovi.base_width));
	window->previewCY = int(window->previewScale * float(ovi.base_height));

	gs_viewport_push();
	gs_projection_push();

	obs_display_t *display = window->ui->wgtPreview->GetDisplay();
	uint32_t width, height;
	obs_display_size(display, &width, &height);
	float right = float(width) - window->previewX;
	float bottom = float(height) - window->previewY;

	gs_ortho(-window->previewX, right, -window->previewY, bottom, -100.0f, 100.0f);

	window->ui->wgtPreview->DrawOverflow();

	/* --------------------------------------- */

	gs_ortho(0.0f, float(ovi.base_width), 0.0f, float(ovi.base_height), -100.0f, 100.0f);
	gs_set_viewport(window->previewX, window->previewY, window->previewCX, window->previewCY);

	{
		obs_render_main_texture_src_color_only();
	}
	gs_load_vertexbuffer(nullptr);

	/* --------------------------------------- */

	gs_ortho(-window->previewX, right, -window->previewY, bottom, -100.0f, 100.0f);
	gs_reset_viewport();

	uint32_t targetCX = window->previewCX;
	uint32_t targetCY = window->previewCY;

	window->ui->wgtPreview->DrawSceneEditing();

	/* --------------------------------------- */

	gs_projection_pop();
	gs_viewport_pop();

	GS_DEBUG_MARKER_END();
}
