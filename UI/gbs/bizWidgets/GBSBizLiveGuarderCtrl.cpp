#include "GBSBizLiveGuarderCtrl.h"
#include "ui_GBSBizLiveGuarderCtrl.h"
#include "GBSMsgDialog.h"
#include "gbs/dialog/GBSAddMatrix.h"
#include "gbs/bizWidgets/GBSRemoveBroker.h"
#include "gbs/GBSMainCollector.h"
#include "gbs/dto/GBSLiveDevices.h"
#include "gbs/dto/GBSLiveAccountInfo.h"
#include "gbs/common/QBizLogger.h"
#include "gbs/GBSDanmaType.h"
#include "gbs/common/QIniFile.h"
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QGridLayout>
#include <QLabel>
#include "gbs/common/DanmakuWidget.h"
#include "gbs/common/SystemUtils.h"
#include "gbs/common/QIniFile.h"
#include "window-basic-main.hpp"
#include <QDesktopServices>
#include <QUrl>
#include <QCompleter>
#include <QStringList>
#include "gbs/remoteCtrl/localqueryinfo.h"
#include "gbs/dialog/GBSOperationReadME.h"
#include "gbs/dialog/GBSColorReadME.h"
#include "gbs/dialog/GBSDeleteMatrix.h"


#include "qt-wrappers.hpp"


#include "window-basic-main.hpp"
#include "display-helpers.hpp"


static int calculateDaysUntilExpiration(const QString &startTimeStr)
{
	// 1. 解析起始时间字符串 (格式: "YYYY-MM-DD HH:MM:SS")
	QDateTime startDateTime = QDateTime::fromString(startTimeStr, "yyyy-MM-dd HH:mm:ss");
	if (!startDateTime.isValid()) {
		qDebug() << "Invalid start time format. Expected format: yyyy-MM-dd HH:mm:ss";
		return -1; // 返回 -1 表示解析失败
	}

	// 2. 给起始时间增加 365 天
	QDateTime expirationDateTime = startDateTime.addDays(365);

	// 3. 获取当前时间
	QDateTime currentDateTime = QDateTime::currentDateTime();

	// 4. 计算当前时间与到期时间之间的差值（以天为单位）
	qint64 daysDifference = currentDateTime.daysTo(expirationDateTime);

	return daysDifference;
}

#include <QMovie>
class GridButtons : public QWidget {

	Q_OBJECT

public:
    GridButtons(QWidget *parent = nullptr) : QWidget(parent), currentPage(0), buttonsPerPage(60), buttonsPerRow(12), buttonCount(0), pageCount(0) {
        QHBoxLayout *mainLayout = new QHBoxLayout(this);
        QVBoxLayout *vertLayout = new QVBoxLayout(this);

        // 创建翻页按钮
        lblPage = new QLabel(this);
        QPushButton* btnHelp = new QPushButton(this);
        QPushButton *btnUp = new QPushButton(this);
        QPushButton *btnDown = new QPushButton(this);
	btnHelp->setFixedSize(24, 24);
	btnUp->setFixedSize(24, 24);
	btnDown->setFixedSize(24, 24);
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
        //// addButtonPages();
        //for (int i = 0; i < 116; i++) {
        //    addButton("text");
        //}

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
	mainLayout->setStretch(0, 19);
	mainLayout->setStretch(0, 1);

        connect(btnHelp, &QPushButton::clicked, this, [this]() {
		//QWidget* widget = new QWidget;
		//QVBoxLayout *layout = new QVBoxLayout(widget);

		//layout->setAlignment(Qt::AlignHCenter); // 整体内容居中
		//QSpacerItem *spacer0 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Minimum);
		//layout->addSpacerItem(spacer0);

		//layout->addLayout(createButtonLabelLayout("", "#C9DCFF", "浅色代表未开播未连接"));
		//layout->addLayout(createButtonLabelLayout("", "#00C566", "绿色代表正常分发"));
		//layout->addLayout(createButtonLabelLayout("", "#FFCD19", "黄色代表在线人数居多"));
		//layout->addLayout(createButtonLabelLayout("", "#EB3F5E", "红色代表直播间异常"));
		//layout->addLayout(createButtonLabelLayout("", "#2667FE", "蓝色语音输入按钮"));
		//QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
		//layout->addSpacerItem(spacer);
		//GBSMsgDialog *dialog = new GBSMsgDialog("颜色含义解释", layout, this);
		//dialog->exec();
		GBSColorReadME *readME = new GBSColorReadME();
		readME->exec();
	
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
    int pageCount = 0;

    public:
    void addButton(QString text, int id)
    {
	mId = id;
	lblPage->setText(QString("%1/%2 页").arg(stackedWidget->currentIndex() + 1).arg(pageCount));

        // 如果当前页面已经满了，创建新页面
        if (buttonCount % buttonsPerPage == 0) {
            addButtonPage();  // 创建新页面
            stackedWidget->setCurrentIndex(pageCount);  // 切换到新创建的页面
            pageCount++;  // 增加页面计数


        }

        // 获取当前页面
        QWidget* currentPage = stackedWidget->currentWidget();
        //qDebug() << "page count " << pageCount << " button count " << buttonCount << " currentPage:" << currentPage;

        if (currentPage) {
            // 添加新的按钮到当前页面
		QStringList parts = text.split("/");
		QString buttonName = "unknown";
		if (!parts.isEmpty() && parts.size() > 0) {
		    buttonName = parts.at(0);
		}
		QPushButton *button = new QPushButton(buttonName, this);
	    
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
			OBSBasic *main = OBSBasic::Get();
			main->beginTalk(id);



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
		    OBSBasic *main = OBSBasic::Get();
		    main->endTalk(id);

		    emit notifyDanmukuChanged(id);
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
    signals:
    void notifyDanmukuChanged(int val);
    private:
    int mId{0};
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

        // 添加 QSpacerItem 来确保按钮排列在上方，���白区域填充在下面
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

    public:
	    // 清除所有按钮
	    void clearButtons()
	    {
		    // 遍历所有页面，删除所有按钮
		    for (int i = 0; i < stackedWidget->count(); ++i) {
			    QWidget *pageWidget = stackedWidget->widget(i);
			    if (pageWidget) {
				    QLayout *layout = pageWidget->layout();
				    if (layout) {
					    // 遍历布局中的所有项
					    QLayoutItem *child;
					    while ((child = layout->takeAt(0)) != nullptr) {
						    QWidget *widget = child->widget();
						    if (widget) {
							    widget->deleteLater(); // 删除控件
						    }
						    delete child; // 删除布局项
					    }
				    }
			    }
		    }

		    // 清空 stackedWidget 并重置状态
		    while (stackedWidget->count() > 0) {
			    stackedWidget->removeWidget(stackedWidget->widget(0));
		    }

		    buttonCount = 0;
		    pageCount = 0;
		    currentPage = 0;
		    lblPage->setText(QString("%1/%2 页").arg(0).arg(0)); // 应该显示 0/0
	    }

};

#include <QTableWidget>
#include <QHeaderView>
#include <QLineEdit>
#include <QResizeEvent>
#include "gbs/bizWidgets/tables/liveMngr/LiveMngrWidget.h"
//
//class MyTableWidget : public QTableWidget {
//	Q_OBJECT
//public:
//	explicit MyTableWidget(QWidget *parent = nullptr) : QTableWidget(parent)
//	{
//		setStyleSheet("QTableWidget {"
//			      "    border: 1px solid #1B2846;"           // 表格外边框
//			      "    border-radius: 4px;"                  // 边框圆角
//			      "    background: #C9DCFF;"                 // 表格背景色
//			      "    gridline-color: #1B2846;"             // 网格线颜色
//			      "    color: #FFFFFF;"                      // 字体颜色
//			      "    font-size: 14px;"                     // 字体大小
//			      "    selection-background-color: #007BFF;" // 选中行背景色
//			      "    selection-color: #FFFFFF;"            // 选中行文字颜色
//			      "}"
//			      "QTableWidget::pane { border: 0; }"
//			      "QTableWidget::tab { border: 0; }"
//			      "QTableWidget::tab-bar { border: 0; }"
//			      );
//		setColumnCount(12);
//		setHorizontalHeaderLabels({"序号", "激活编号", "备注编号", "客户号", "开播时间", "剩余时长", "直播账号",
//				"直播数量（总）", "直播时长", "ToDesk账号",
//				"ToDesk密码","操作"});
//
//		horizontalHeader()->setStretchLastSection(true);                // 最后一列填满
//		horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 列宽均匀分布
//		//setEditTriggers(QAbstractItemView::NoEditTriggers);             // 禁止编辑
//		setSelectionMode(QAbstractItemView::SingleSelection);           // 单选
//		setSelectionBehavior(QAbstractItemView::SelectRows);            // 按行选择
//
//		setShowGrid(false); // 显示网格线
//		connect(this, &QTableWidget::cellPressed, this, &MyTableWidget::onCellClicked);
//
//	}
//	void addRow(const QStringList &rowData)
//	{
//		++index;
//		if (rowData.size() != columnCount()) {
//			qWarning("Row data does not match column count.");
//			return;
//		}
//
//		int row = rowCount();
//		insertRow(row);
//		
//
//		for (int col = 0; col < rowData.size() - 1; ++col) {
//			QTableWidgetItem *item = new QTableWidgetItem(rowData[col]);
//			item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
//			setItem(row, col, item);
//		}
//	}
//	void removeRow(int row)
//	{
//		if (row >= 0 && row < rowCount()) {
//			QTableWidget::removeRow(row);
//		} else {
//			qWarning("Invalid row index.");
//		}
//	}
//
//signals:
//	void onRowClick(int row, int column);
//
//
//public slots:
//	void onCellClicked(int row, int column) {
//		selRow = row;
//		selCol = column;
//		qDebug() << "setCurrentCell " << row << " col " << column;
//		//setCurrentCell(row, column);
//		
//	}
//
//
//private:
//	int index = 0;
//	int selRow = 0;
//	int selCol = 0;
//};

class CustomSearchLineEdit : public QLineEdit {
	Q_OBJECT
public:
	CustomSearchLineEdit(QWidget *parent = nullptr) : QLineEdit(parent)
	{
		setPlaceholderText("模糊搜索");
		setFixedSize(192, 27);
		// 创建并设置放大镜图标
		iconLabel = new QLabel(this);
		QPixmap searchPixmap(":gbs/images/gbs/biz/gbs-search-24px.png"); // 相对路径
		iconLabel->setPixmap(searchPixmap);                              // 放大镜图标路径
		iconLabel->setFixedSize(24, 24);
		iconLabel->move(10, (height() - iconLabel->height()) / 2); // 图标左边距10
		iconLabel->setVisible(true);

		QHBoxLayout *searchLayout = new QHBoxLayout(this);
		searchLayout->setContentsMargins(5, 0, 5, 0); // 设置左右边距
		searchLayout->setSpacing(0);                  // 移除组件之间的间距

		    // 添加组件到布局
		searchLayout->addWidget(iconLabel);
		searchLayout->addStretch(); // 将图标推到左侧
					    // 设置文本框的文本边距，为图标和下拉框留出空间
		setTextMargins(30, 0, 85, 0); // 左边距25px，右边距85px

		QStringList words;
		words << "apple" << "banana" << "orange" << "grape" << "watermelon";
		QCompleter *completer = new QCompleter(words, this);
		completer->setCaseSensitivity(Qt::CaseInsensitive);
		setCompleter(completer);

		setStyleSheet("QLineEdit {"
					"    width: 193px;"
					"    height: 27px;"
					"    background-color: rgba(143, 146, 161, 5%);"
					"    border-radius: 5px;"
					"    border: 1px solid #D8D8D8;"
					"}");
		connect(this, &QLineEdit::textChanged, this, [this](const QString &text) {
			qDebug() << "text changed "
				 << "text" << text;
			setText(text);
		});
	}

signals:
	void returnPressed(QString text);

protected:
	// 重写 keyPressEvent 方法
	void keyPressEvent(QKeyEvent *event) override
	{
		if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
			emit returnPressed(text()); 
			return;               
		}
		QLineEdit::keyPressEvent(event);
	}

private:
	QLabel *iconLabel;
};

class LiveManageWidget : public QWidget {
	Q_OBJECT;

public:
	explicit LiveManageWidget(QWidget *parent = nullptr) : QWidget(parent)
	{

		QVBoxLayout *mainLayout = new QVBoxLayout;
		
		QHBoxLayout *naviLayout = new QHBoxLayout;
		QLabel *label = new QLabel("  矩阵直播终端信息列表");
		label->setStyleSheet("QLabel {"
				     "   font-size: 16px;"
				     "}");
		QPushButton *btnHelp = new QPushButton(this);
		btnHelp->setFixedSize(24, 24);
		btnHelp->setStyleSheet("QPushButton {"
				       "   background-image: url(:gbs/images/gbs/biz/gbs-tooltip.png);"
				       "   background-repeat: no-repeat;"
				       "   background-position: center;"
				       "   color: white;"
				       "   border: none;"       // 无边框
				       "   border-radius: 5px;" // 圆角
				       "   font-size: 16px;"
				       "   padding: 0;" // 不添加内边距
				       "}"
				       "QPushButton:pressed {"
				       "   background-color: #D1D8DD;" // 按下时背景颜色
				       "   padding-left: 3px;"         // 向左移动 3px
				       "   padding-top: 3px;"          // 向上移动 3px
				       "   background-repeat: no-repeat;"
				       "   background-position: center;"
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
				      "}"
				      "QPushButton:pressed {"
				      "   background-color: #D1D8DD;" // 按下时背景颜色
				      "   padding-left: 3px;"         // 向左移动 3px
				      "   padding-top: 3px;"          // 向上移动 3px
				      "   background-repeat: no-repeat;"
				      "   background-position: center;"
				      "}"
		);
		CustomSearchLineEdit *lineEdit = new CustomSearchLineEdit;
		
		naviLayout->addWidget(label);
		naviLayout->addWidget(lineEdit);
		naviLayout->addWidget(btnAdd);
		naviLayout->addWidget(btnHelp);

		QObject::connect(btnAdd, &QPushButton::clicked, this, [this]() {
			GBSAddMatrix *matrix = new GBSAddMatrix(this);
			matrix->show();
		});
		mainLayout->addLayout(naviLayout);
		table = new LiveMngrWidget();
		connect(table, &LiveMngrWidget::deleteLiveClient, this,
			[this](int row, QString id) {
				//LiveMngrWidget *sender = qobject_cast<LiveMngrWidget *>(sender());
				GBSDeleteMatrix *delMatrix = new GBSDeleteMatrix();
				delMatrix->setLiveId(id.toInt());
				delMatrix->setLiveWidget(table);
				delMatrix->setRow(row);
				delMatrix->exec();
			});
		connect(table, &LiveMngrWidget::disconnectLiveClient, this, [](QString id) {

			});
		connect(table, &LiveMngrWidget::connectRemoteMachine, this, [](QString username, QString password) {
			QTimer::singleShot(0, [username, password]() {
				connect_remote_machine(username, password);
			});
			});
		mainLayout->addWidget(table);
		setLayout(mainLayout);
		connect(lineEdit, &CustomSearchLineEdit::returnPressed, this, [this](QString text) {
			emit fuzzyMatching(text);
			});

		connect(btnHelp, &QPushButton::clicked, this,
			[]() { GBSOperationReadME *readME = new GBSOperationReadME();
			readME->exec();
			});


	}

public:

	void addRow(QStringList data) { table->addRow(data);

	}

	// 添加一个清除表格的函数
	void clearTable() { table->clearRows();
	}

signals:
	void fuzzyMatching(QString text);

private:
	LiveMngrWidget *table;
	int index = 0;
	
};


GBSBizLiveGuarderCtrl::GBSBizLiveGuarderCtrl(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GBSBizLiveGuarderCtrl)
{
    ui->setupUi(this);

  
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
    //ui->tabWidget->setStyleSheet("QTabWidget::pane {"
				// "    border: none;" // 移除tab pane的边框
				// "}"); // 清空 QTabWidget 的样式表

    //ui->tabWidget->setStyleSheet("QTabBar::tab {"
				//"    color: #78828A; "              // 默认字体颜色
				//"    background: none; "       // 默认背景颜色
				// "    padding: 40px; "      // 内边距
				//"    padding: 4px; "             // 内边距
				//"    border-radius: 5px; "        // 圆角
				//"    font-size: 14px;"         // 设置字体大小为16像素

				//"}"
				//"QTabBar::tab:selected {"
				//"    color: #00C566; "              // 选中字体颜色
				//"    background: none; "       // 选中背景颜色
				//"    padding: 4px; "              // 内边距
				//"}"
				//"QTabBar::tab:first {"
				//"    margin-left: 40px;" // 调整第一个tab项的左外边距
				//"}"
				// "QTabBar {"
				// "    qproperty-alignment: AlignCenter;" // 设置tab项居中对齐
				// "    qproperty-drawBase: 0; /* 重要：移除底部基线 */"
				// "}"
	
				//"QTabWidget::pane { border: 0; }"
				//"QTabBar::tab { border: none; }"
				//"QTabWidget::tab-bar { border: none; }"
    //);

	std::unique_ptr<IniSettings> iniFile = std::make_unique<IniSettings>("gbs.ini");
	 QString pullRtmpUrl = iniFile->value("LiveBroker", "url", "unknown").toString();
    

	 //connect(ui->tabWidget, &QTabWidget::currentChanged, this, &GBSBizLiveGuarderCtrl::onTabChanged);
    

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

	ui->wgtPreview->SetLocked(false);
	ui->wgtPreview->Init();
	connect(ui->wgtPreview, &OBSQTDisplay::DisplayCreated, addDisplay);

	OBSBasic *main = reinterpret_cast<OBSBasic *>(App()->GetMainWindow());

	main->addGuarderCtrlScene();
	GBSHttpClient::getInstance()->registerHandler(this);
	
	GBSLiveAccountInfo account = GBSMainCollector::getInstance()->getAccountInfo();
	GBSHttpClient::getInstance()->countZlmLiveDeviceInfo(account.getId());
	GBSHttpClient::getInstance()->pageSrsLiveDeviceV2(account.getId(), 0);

	mWssKeepaliveId = std::to_string(account.getId()) + "_" + GetMachineIdFromRegistry() +
			  GetWindowsProductIDFromRegistery() + GBSMainCollector::getInstance()->getSystemUniqueNo();

	int userId = account.getUserId();
	mWebSocketClient = WebSocketClient::Create();
	mWebSocketClient->setName("DamakuReciver");
	if (!mWebSocketClient->IsRunnig()) {
		QLogD("Start Weboscket userid %d", userId);
		std::string url = GBSMainCollector::getInstance()->getBaseWebSocketV2();
		std::string danmuReceiveId = std::to_string(userId) + "_" + GetMachineIdFromRegistry() + GetWindowsProductIDFromRegistery() + GBSMainCollector::getInstance()->getSystemUniqueNo();
		std::string wssUrl = url + "/sendBarrageToAdmin/" + danmuReceiveId;
		mWebSocketClient->Start(wssUrl);
	}
	mWebSocketClient->RegisterHandler(this);
	connect(this, &GBSBizLiveGuarderCtrl::signalDanmakuReceived, this, &GBSBizLiveGuarderCtrl::addNewWidget, Qt::QueuedConnection);

	//QWidget *currentWidget = ui->tabWidget->currentWidget();
	//
	//
	//
	danmaKuAreaLayout = new QVBoxLayout();
	danmaKuAreaLayout->setSpacing(6);                 // 设置widget之间的固定间距为6像素
	danmaKuAreaLayout->setAlignment(Qt::AlignBottom); // 保证最新widget在最底部

	QWidget *containerWidget = new QWidget();         // 用于容纳所有widgets
	containerWidget->setObjectName("ContainerWidget");
	containerWidget->setStyleSheet("#ContainerWidget {"
	                               "   border: 2px solid #F9F9F9;"
	                               "   border-radius: 5px;"
	                               "}");
	containerWidget->setLayout(danmaKuAreaLayout);

	danmakuscrollArea = new QScrollArea();
	danmakuscrollArea->setWidget(containerWidget);
	danmakuscrollArea->setWidgetResizable(true);
	danmakuscrollArea->setStyleSheet("QTextEdit {"
		"   background-color: #F9F9F9;" // 文本框背景色
		"}"
		"QScrollBar:vertical {" // 垂直滚动条
		"   border: none;"
		"   background: #DEDEDE;"   // 滚动条背景色
		"   width: 14px;"           // 滚动条宽度 (包含箭头)
		"   margin: 14px 0 14px 0;" // 上下箭头区域高度 (正方形边长)
		"}"
		"QScrollBar::handle:vertical {" // 垂直滚动条滑块
		"   background: #00C566;"       // 滑块颜色（绿色）
		"   min-height: 20px;"          // 滑块最小高度
		"}"
		"QScrollBar::add-line:vertical {" // 垂直滚动条下箭头 (隐藏)
		"   border: none;"
		"   background: none;"            // 隐藏
		"   height: 14px;"                // 箭头高度
		"   subcontrol-position: bottom;" // 箭头位于底部
		"   subcontrol-origin: margin;"
		"}"
		"QScrollBar::sub-line:vertical {" // 垂直滚动条上箭头 (隐藏)
		"   border: none;"
		"   background: none;"         // 隐藏
		"   height: 14px;"             // 箭头高度
		"   subcontrol-position: top;" // 箭头位于顶部
		"   subcontrol-origin: margin;"
		"}"
		"QScrollBar::up-arrow:vertical {" // 上端绿色正方形和白色三角形
		"   border: none;"
		"   background: #00C566;" // 绿色正方形
		"   width: 14px;"         // 正方形边长
		"   height: 14px;"        // 正方形边长
		"   subcontrol-position: top;"
		"   subcontrol-origin: content;"
		"   image: url(:/gbs/images/gbs/biz/gbs-green-scrollbar-uparrow.png);" // 设置图片
		"}"
		"QScrollBar::down-arrow:vertical {" // 下端绿色正方形和白色三角形
		"   border: none;"
		"   background: #00C566;" // 绿色正方形
		"   width: 14px;"         // 正方形边长
		"   height: 14px;"        // 正方形边长
		"   subcontrol-position: bottom;"
		"   subcontrol-origin: content;"
		"    image: url(:/gbs/images/gbs/biz/gbs-green-scrollbar-downarrow.png);" // 设置图片
		"}"
		"QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {" // 垂直滚动条页
		"   background: none;" // 上下翻页时区域的颜色，这里设置成透明
		"}");

		// 限制ScrollArea尺寸，防止几何超出问题
	//danmakuscrollArea->setMinimumSize(520, 460);
	danmakuscrollArea->setMaximumHeight(3000);

	//// Set the scroll area as the layout for the current widget
	//QVBoxLayout *mainLayout = new QVBoxLayout(currentWidget);
	//mainLayout->addWidget(danmakuscrollArea);

	ui->verticalLayout_2->addWidget(danmakuscrollArea);


	//设置直播间管理，开播管理

	mWssTimer = new QTimer(this);
	connect(mWssTimer, &QTimer::timeout, this, &GBSBizLiveGuarderCtrl::onWssKeepAlive);
	if (!mWssTimer->isActive()) {
		mWssTimer->start();
		mWssTimer->setInterval(10000);
	}
	ui->widget_2->setStyleSheet(R"(
		QWidget#widget_2{
			background: #F9F9F9;
			border-radius: 16px 16px 16px 16px;
			border: none;
		})");
	ui->tabWidget_2->setStyleSheet("QTabWidget::pane {"
				     "    border: none;" // 移除tab pane的边框
				     "}");               // 清空 QTabWidget 的样式表

	ui->tabWidget_2->setStyleSheet("QTabBar::tab {"
				     "    color: #78828A; "     // 默认字体颜色
				     "    background: none; "   // 默认背景颜色
				     "    padding: 40px; "      // 内边距
				     "    padding: 4px; "       // 内边距
				     "    border-radius: 5px; " // 圆角
				     "    font-size: 14px;"     // 设置字体大小为16像素

				     "}"
				     "QTabBar::tab:selected {"
				     "    color: #00C566; "   // 选中字体颜色
				     "    background: none; " // 选中背景颜色
				     "    padding: 4px; "     // 内边距
				     "}"
				     "QTabBar::tab:first {"
				     "    margin-left: 40px;" // 调整第一个tab项的左外边距
				     "}"
				     "QTabWidget::pane { border: 0; }"
				     "QTabBar::tab { border: none; }"
				     "QTabWidget::tab-bar { border: none; }");
	connect(ui->tabWidget_2, &QTabWidget::currentChanged, this, &GBSBizLiveGuarderCtrl::onTabChanged2);

	mDanmakuType = DANITEM_TYPE_ALL;
	gridButtons = new GridButtons(this);
	ui->tabWidget_2->addTab(gridButtons, "直播间");
	ui->lineEdit_2->setAlignment(Qt::AlignCenter);
	ui->lineEdit_3->setAlignment(Qt::AlignCenter);
	ui->lineEdit->setAlignment(Qt::AlignCenter);

	ui->pushButton->setCheckable(true); // 允许按钮被选中
	ui->pushButton_2->setCheckable(true); // 允许按钮被选中
	ui->pushButton_3->setCheckable(true); // 允许按钮被选中
	ui->pushButton_4->setCheckable(true); // 允许按钮被选中
	ui->pushButton_5->setCheckable(true); // 允许按钮被选中
	btnDanmaLists.append(ui->pushButton);
	btnDanmaLists.append(ui->pushButton_2);
	btnDanmaLists.append(ui->pushButton_3);
	btnDanmaLists.append(ui->pushButton_4);
	btnDanmaLists.append(ui->pushButton_5);

	connect(ui->pushButton, &QPushButton::toggled, this, &GBSBizLiveGuarderCtrl::updateStyle);
	connect(ui->pushButton_2, &QPushButton::toggled, this, &GBSBizLiveGuarderCtrl::updateStyle);
	connect(ui->pushButton_3, &QPushButton::toggled, this, &GBSBizLiveGuarderCtrl::updateStyle);
	connect(ui->pushButton_4, &QPushButton::toggled, this, &GBSBizLiveGuarderCtrl::updateStyle);
	connect(ui->pushButton_5, &QPushButton::toggled, this, &GBSBizLiveGuarderCtrl::updateStyle);


	for (QPushButton *item : btnDanmaLists) {
		if (item == ui->pushButton) {
			item->setStyleSheet("QPushButton {"
					    "   border-radius: 5px;" // 圆角
					    "   color: #00c566;"
					    "   font-size: 14px;"
					    "   padding:10px;"
					    "}"

					    "QPushButton:pressed {"
					    "   background-color: #D1D8DD;" // 按下时背景颜色
					    "   padding-left: 3px;"         // 向左移动 3px
					    "   padding-top: 3px;"          // 向上移动 3px
					    "   background-repeat: no-repeat;"
					    "   background-position: center;"
					    "}");
		} else {
			item->setStyleSheet("QPushButton {"
					    "   border-radius: 5px;" // 圆角
					    "   color: #78828A;"
					    "   border: black;" // 无边框
					    "   font-size: 12px;"
					    "   padding:10px;"
					    "}"

					    "QPushButton:pressed {"
					    "   background-color: #D1D8DD;" // 按下时背景颜色
					    "   padding-left: 3px;"         // 向左移动 3px
					    "   padding-top: 3px;"          // 向上移动 3px
					    "   background-repeat: no-repeat;"
					    "   background-position: center;"
					    "}");
		}
		
	}


	connect(gridButtons, &GridButtons::notifyDanmukuChanged, this, &GBSBizLiveGuarderCtrl::onDanmukuChanged);


	connect(ui->pushButton, &QPushButton::clicked, this, [this](){ userDanmakuType = DANITEM_TYPE_ALL;
		});
	connect(ui->pushButton_2, &QPushButton::clicked, this, [this]() { userDanmakuType = DANITEM_TYPE_ALL;
		});
	connect(ui->pushButton_3, &QPushButton::clicked, this, [this]() { userDanmakuType = DANITEM_TYPE_WHOIS; });
	connect(ui->pushButton_4, &QPushButton::clicked, this, [this]() { userDanmakuType = DANITEM_TYPE_GIFT; });
	connect(ui->pushButton_5, &QPushButton::clicked, this, [this]() { userDanmakuType = DANITEM_TYPE_CHAT; });

	liveManageWidget = new LiveManageWidget(this);
	
	connect(liveManageWidget, &LiveManageWidget::fuzzyMatching, this, [this](QString text) {
			//模糊匹配. //TODO
		});
	ui->tabWidget_2->addTab(liveManageWidget, "	开播管理");

	GBSHttpClient::getInstance()->getPullStreamUrlV2();


}


void GBSBizLiveGuarderCtrl::updateStyle(bool checked)
{
	QPushButton *button = qobject_cast<QPushButton *>(sender());
	int i = 0;
	for (QPushButton *item : btnDanmaLists) {
		++i;
		if ((button == item)) {
			if (button == ui->pushButton) {
				qDebug() << "index " << i << "selected";
				item->setStyleSheet("QPushButton {"
						    "   border-radius: 5px;" // 圆角
						    "   color: #00c566;"
						    "   font-size: 14px;"
						    "   padding:10px;"
						    "}"

						    "QPushButton:pressed {"
						    "   background-color: #D1D8DD;" // 按下时背景颜色
						    "   padding-left: 3px;"         // 向左移动 3px
						    "   padding-top: 3px;"          // 向上移动 3px
						    "   background-repeat: no-repeat;"
						    "   background-position: center;"
						    "}");
			} else {
				qDebug() << "index " << i << "selected";
				item->setStyleSheet("QPushButton {"
						    "   border-radius: 5px;" // 圆角
						    "   color: #00c566;"
						    "   font-size: 12px;"
						    "   padding:10px;"
						    "}"

						    "QPushButton:pressed {"
						    "   background-color: #D1D8DD;" // 按下时背景颜色
						    "   padding-left: 3px;"         // 向左移动 3px
						    "   padding-top: 3px;"          // 向上移动 3px
						    "   background-repeat: no-repeat;"
						    "   background-position: center;"
						    "}");
			}
			
		} else {
			if (button == ui->pushButton) {
				item->setStyleSheet("QPushButton {"
						    "   border-radius: 5px;" // 圆角
						    "   color: #78828A;"
						    "   font-size: 14px;"
						    "   padding:10px;"
						    "}"

						    "QPushButton:pressed {"
						    "   background-color: #D1D8DD;" // 按下时背景颜色
						    "   padding-left: 3px;"         // 向左移动 3px
						    "   padding-top: 3px;"          // 向上移动 3px
						    "   background-repeat: no-repeat;"
						    "   background-position: center;"
						    "}");
			} else {
				qDebug() << "index " << i << "un-selected";
				item->setStyleSheet("QPushButton {"
						    "   border-radius: 5px;" // 圆角
						    "   color: #78828A;"
						    "   font-size: 12px;"
						    "   padding:10px;"
						    "}"

						    "QPushButton:pressed {"
						    "   background-color: #D1D8DD;" // 按下时背景颜色
						    "   padding-left: 3px;"         // 向左移动 3px
						    "   padding-top: 3px;"          // 向上移动 3px
						    "   background-repeat: no-repeat;"
						    "   background-position: center;"
						    "}");
			}
			
		}
	}
}

void GBSBizLiveGuarderCtrl::onPullRtmpUrl(const std::string url)
{
	qDebug() << "onPullRtmpUrl " << url;

	if (!url.empty()) {
		QString newUrl = QString::fromStdString(url);
		if (newUrl.startsWith("rtmp://", Qt::CaseInsensitive) == 0) {
			OBSBasic*main = OBSBasic::Get();
			QMetaObject::invokeMethod(main, "stopPullStream");
			return;
		}
		if (pullRtmpUrl != newUrl) {
			pullRtmpUrl = newUrl;
			OBSBasic *main = OBSBasic::Get();
			QMetaObject::invokeMethod(main, "startPullStream", Q_ARG(QString, pullRtmpUrl));
		}
		obs_source_t *source = obs_get_source_by_name("RTMP 矩阵地址");
		if (source) {
			obs_source_set_enabled(source, true);
			obs_set_output_source(0, source);
			return;
		}
	}
}

void GBSBizLiveGuarderCtrl::onWssKeepAlive() {
	if (mWebSocketClient && mWssRunning) {
		std::string target = "ping_" + mWssKeepaliveId;
		mWebSocketClient->Send(target);
	}
}

void GBSBizLiveGuarderCtrl::onTabChanged(int index)
{

	if (validWidget != nullptr) {

		qDebug() << "GBSBizLiveGuarderCtrl " << index;
		ui->horizontalLayout->removeWidget(validWidget);
		delete validWidget;
		validWidget = nullptr;
		update();
	}

}

void GBSBizLiveGuarderCtrl::onTabChanged2(int index) {
	//if (index == 0) {
	//	ui->verticalLayout->removeItem(ui->horizontalLayout_2);
	//} else {
	//	ui->verticalLayout->addItem(ui->horizontalLayout_2);
	//}

	GBSLiveAccountInfo account = GBSMainCollector::getInstance()->getAccountInfo();
	GBSHttpClient::getInstance()->pageSrsLiveDeviceV2(account.getId(), 0);
}

GBSBizLiveGuarderCtrl::~GBSBizLiveGuarderCtrl()
{
	mWssTimer->stop();
	mWebSocketClient->UnRegisterHandler(this);
	mWebSocketClient->Stop();
	GBSHttpClient::getInstance()->unRegisterHandler(this);

	obs_display_remove_draw_callback(ui->wgtPreview->GetDisplay(), GBSBizLiveGuarderCtrl::RenderMain, this);
	OBSBasic *main = reinterpret_cast<OBSBasic *>(App()->GetMainWindow());
	main->removeGuarderCtrlScene();
    delete ui;
}

void GBSBizLiveGuarderCtrl::onListDevices(std::list<GBSLiveDevices> devices, int pageNum)
{
	currentPageNum = pageNum;
	//currentliveDevices = std::move(devices);
	QMetaObject::invokeMethod(this, [devices,this]() {
		currentliveDevices = std::move(devices);
		liveManageWidget->clearTable();
		gridButtons->clearButtons();
		int index = ui->tabWidget_2->currentIndex();
		if (index == 0) {
			for (std::list<GBSLiveDevices>::iterator it = currentliveDevices.begin();
			     it != currentliveDevices.end(); ++it) {
				gridButtons->addButton(QString::fromStdString
				((*it).getNotes()), (*it).getId());
			}

		} else if (index == 1) {

			int count = (int)currentliveDevices.size();
			if (count > 0) {
				int i = 0;
				for (std::list<GBSLiveDevices>::iterator it = currentliveDevices.begin();
				     it != currentliveDevices.end(); ++it) {
					QStringList rawData;
					QString No = QString("%1").arg(i + 1, 3, 10, QChar('0'));
					QString activateCode = QString::fromStdString((*it).getActivationCode());
					QStringList parts = activateCode.split("-");
					if (!parts.isEmpty() && parts.size() > 1) {
						activateCode = parts.at(1);
					}
					QString notes = QString::fromStdString((*it).getNotes());
					QString deviceCode = QString::fromStdString((*it).getDeviceCode());//客户号
					QString createTime = QString::fromStdString((*it).getCreatedTime());
					int leftDays = calculateDaysUntilExpiration(createTime);
					QString liveAccountId = QString::number((*it).getId());
					QString liveDeviceCount =
						QString("%1台/(%2台)").arg(count).arg(deviceTotalCount);
					QString liveTimeLength = QString("50天 17:45:59 ");
					ui->lineEdit->setText(QString("%1 台").arg(count));
					ui->lineEdit_3->setText(QString("%1 台").arg(deviceTotalCount));
					QString toDeskAccount = QString::fromStdString((*it).getToDeskAccount());
					QString toDeskPassword = QString::fromStdString((*it).getToDeskPassword());

					rawData << No << deviceCode << activateCode << notes << createTime
						<< QString("%1 (天)").arg(leftDays)
						<< liveAccountId << liveDeviceCount  << toDeskAccount + "/" +toDeskPassword
						<< "";
					liveManageWidget->addRow(rawData);
					i++;
				}
			}
		}

	},
	Qt::QueuedConnection);

	
}

void GBSBizLiveGuarderCtrl::onDeviceCount(int onLineCount, int totalCount) {
	deviceOnLineCount = onLineCount;
	deviceTotalCount = totalCount;
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



void GBSBizLiveGuarderCtrl::onMessage(std::string msg)
{
	try {
		QString qMsg = QString::fromLocal8Bit(msg);
		QFile file("network_messages.txt");
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
			// 处理文件打开失败的情况
			return;
		}
		QTextStream out(&file);
		out << (qMsg) << "\n";
		file.close();

		auto jsonObject = nlohmann::json::parse(msg);
		if (jsonObject.is_object()) {
			processDanmaItem(jsonObject);
		} else if (jsonObject.is_array()) {
			int size = (int)jsonObject.size();
			for (int i = 0; i < size; i++) {
				processDanmaItem(jsonObject[i]);
			}
		}
	} catch (const nlohmann::json::parse_error &e) {
		qDebug() << "JSON 解析错误: " << msg << " Execption: " << e.what();
	} catch (const std::exception &e) {
		qDebug() << "标准异常: " << msg << " Execption: " << e.what();
	}
}

void GBSBizLiveGuarderCtrl::onOpen()
{
	mWssRunning = true;
	qDebug() << "onOpen";
}

void GBSBizLiveGuarderCtrl::onFail()
{
	mWssRunning = false;
	qDebug() << "onFail";

	Sleep(5 * 1000);
	QMetaObject::invokeMethod(this, [this]() {
		mWebSocketClient->UnRegisterHandler(this);
		mWebSocketClient->Stop();
		GBSLiveAccountInfo account = GBSMainCollector::getInstance()->getAccountInfo();
		int userId = account.getUserId();
		mWebSocketClient = WebSocketClient::Create();
		mWebSocketClient->setName("DamakuReciver");
		if (!mWebSocketClient->IsRunnig()) {
			QLogD("Start Weboscket userid %d", userId);
			std::string url = GBSMainCollector::getInstance()->getBaseWebSocketV2();
			std::string danmuReceiveId = std::to_string(userId) + "_" + GetMachineIdFromRegistry() + GetWindowsProductIDFromRegistery() + GBSMainCollector::getInstance()->getSystemUniqueNo();
			std::string wssUrl = url + "/sendBarrageToAdmin/" + danmuReceiveId;
			mWebSocketClient->Start(wssUrl);
		}
		mWebSocketClient->RegisterHandler(this);
	},
	Qt::QueuedConnection);
}

void GBSBizLiveGuarderCtrl::onClose()
{
	mWssRunning = false;
	qDebug() << "onClose";
}

void GBSBizLiveGuarderCtrl::processDanmaItem(const nlohmann::json jsonObject)
{
	std::string jsonString = jsonObject.dump();
	std::string platform = jsonObject["platform"].is_null() ? "" : jsonObject["platform"].get<std::string>();
	std::string liveId = jsonObject["liveId"].is_null() ? "" : jsonObject["liveId"].get<std::string>();
	std::string liveDeviceId = jsonObject["liveDeviceId"].is_null() ? "" : jsonObject["liveDeviceId"].get<std::string>();
	std::string deviceName = jsonObject["deviceName"].is_null() ? "" : jsonObject["deviceName"].get<std::string>();
	std::string platformAcct = jsonObject["platformAcct"].is_null() ? "" : jsonObject["platformAcct"].get<std::string>();

	
	//if (mDanmakuType != DANITEM_TYPE_ALL) {
	//	if (std::stoi(liveDeviceId) != mDanmakuValue) {
	//		return;
	//	}
	//}
	std::string uniqueName = deviceName;
	if (uniqueName.empty()) {
		return;
	}
	QString qUniqueName = QString::fromStdString(uniqueName);
	qUniqueName += " [";
	if (platformAcct == "") platformAcct = "null";
	qUniqueName += platformAcct;
	qUniqueName += "]";
	if (!platform.empty()) {
		if (platform == "DY") {
			danmaPlatIconString = ":gbs/images/gbs/biz/gbs-logo-douyin.png";
		} else if (platform == "KS") {
			danmaPlatIconString = ":gbs/images/gbs/biz/gbs-logo-kuai.png";
		} else if (platform == "SPH") {
			danmaPlatIconString = ":gbs/images/gbs/biz/gbs-logo-wechat.png";
		} else if (platform == "TK") {
			danmaPlatIconString = ":gbs/images/gbs/biz/gbs-logo-tiktok.png";
		} else if (platform == "BILI") {
			danmaPlatIconString = ":gbs/images/gbs/biz/gbs-logo-bilibili.png";
		} else if (platform == "PDD") {
			danmaPlatIconString = ":gbs/images/gbs/biz/gbs-logo-pdd.png";
		} else if (platform == "FB") {
			danmaPlatIconString = ":gbs/images/gbs/biz/gbs-logo-facebook.png";
		} 
	}
	if (platform.empty()) {
		QLogE("No platform about danmaku, ProcessDanmaItem failed.");
		return;
	}

	std::string type = jsonObject["type"].get<std::string>();
	qDebug() << "damaku type " << type;
	if (type == "MemberMessage") {
		auto danma = std::make_shared<DammaMemberMSG>();
		danma->type = "MemberMessage";
		danma->name = jsonObject["name"].get<std::string>();
		danma->head_image = jsonObject["head_image"].get<std::string>();

		danma->content = jsonObject["content"].get<std::string>();
		danma->msgType = 1;

		QString danmaText = QString::fromStdString(danma->name) + ":" + QString::fromStdString(danma->content);
		
		emit signalDanmakuReceived(qUniqueName, danmaPlatIconString, danmaText, QString::fromStdString(type),
					   QString::fromStdString(liveDeviceId));

	} else if (type == "ChatMessage") {
		auto danma = std::make_shared<DanmaChatMessage>();
		danma->type = "ChatMessage";
		danma->name = jsonObject["name"].get<std::string>();
		danma->head_image = jsonObject["head_image"].get<std::string>();
		danma->content = jsonObject["content"].get<std::string>();
		danma->msgType = 2;
		QString danmaText = QString::fromStdString(danma->name) + ":" + QString::fromStdString(danma->content);
		emit signalDanmakuReceived(qUniqueName, danmaPlatIconString, danmaText, QString::fromStdString(type),
					   QString::fromStdString(liveDeviceId));

	} else if (type == "GiftMessage") {
		auto danma = std::make_shared<DanmaGiftMessage>();
		danma->type = "GiftMessage";
		danma->name = jsonObject["name"].get<std::string>();
		danma->head_image = jsonObject["head_image"].get<std::string>();
		danma->content = jsonObject["content"].get<std::string>();
		danma->gift_name = jsonObject["gift_name"].get<std::string>();
		danma->gift_count = jsonObject["gift_count"].get<std::string>();
		danma->msgType = 3;
		QString danmaText = QString::fromStdString(danma->name) + ":" + QString::fromStdString(danma->content);
		emit signalDanmakuReceived(qUniqueName, danmaPlatIconString, danmaText, QString::fromStdString(type),
					   QString::fromStdString(liveDeviceId));

	} else if (type == "SocialMessage") {
		auto danma = std::make_shared<DanmaSocialMessage>();
		danma->type = "SocialMessage";
		danma->name = jsonObject["name"].get<std::string>();
		danma->head_image = jsonObject["head_image"].get<std::string>();
		danma->content = jsonObject["content"].get<std::string>();
		danma->msgType = 4;
		QString danmaText = QString::fromStdString(danma->name) + ":" + QString::fromStdString(danma->content);
		emit signalDanmakuReceived(qUniqueName, danmaPlatIconString, danmaText, QString::fromStdString(type),
			QString::fromStdString(liveDeviceId));
	}
 else if (type == "LikeMessage") {
	 auto danma = std::make_shared<DanmaLikeMessage>();
	 danma->type = "LikeMessage";
	 danma->name = jsonObject["name"].get<std::string>();
	 danma->head_image = jsonObject["head_image"].get<std::string>();
	 danma->content = jsonObject["content"].get<std::string>();
	 danma->count = jsonObject["count"].get<std::string>();
	 danma->msgType = 5;
	 QString danmaText = QString::fromStdString(danma->name) + ":" + QString::fromStdString(danma->content);
	 emit signalDanmakuReceived(qUniqueName, danmaPlatIconString, danmaText, QString::fromStdString(type),
		 QString::fromStdString(liveDeviceId));
	}
 else if (type == "RoomMessage") {

	 auto danma = std::make_shared<DanmaRoomMessage>();
	 danma->content = jsonObject["content"].get<std::string>();
	 danma->count = jsonObject["count"].get<std::string>();
	 danma->msgType = 6;
	 QString danmaText = QString::fromStdString(uniqueName) + ":" + QString::fromStdString(danma->content);
	 emit signalDanmakuReceived(qUniqueName, danmaPlatIconString, danmaText, QString::fromStdString(type),
		 QString::fromStdString(liveDeviceId));



	}
}


void GBSBizLiveGuarderCtrl::addNewWidget(const QString& atext, const QString& aimagePath, const QString& atext2,
	const QString& atype, const QString& liveId)
{
	QString text = atext;
	QString imagePath = aimagePath;
	QString text2 = atext2;
	QString type = atype;
	DanmaItem item{ QTime::currentTime(), text, imagePath, text2, type, liveId };
	int danmakuType = DANITEM_TYPE_ALL;
	//QList<DanmaItem> danmItemWhoisList = thlWhoIsDanmukus.localData();
	//QList<DanmaItem> danmItemAllList = thlAllDanmakus.localData();
	//QList<DanmaItem> danmItemGiftList = thlGiftDanmakus.localData();
	//QList<DanmaItem> danmItemLikeList = thlLikeDanmakus.localData();
	//QList<DanmaItem> danmItemChatList = thlChatDanmakus.localData();
	if (type == "MemberMessage") {
		whoIsDanmukus.push_back(item);
		if (whoIsDanmukus.size() > 300) {
			whoIsDanmukus.pop_front();
		}
		danmakuType = DANITEM_TYPE_WHOIS;
	}
	else if (type == "ChatMessage") {
		chatDanmakus.push_back(item);
		if (chatDanmakus.size() > 300) {
			chatDanmakus.pop_front();
		}
		danmakuType = DANITEM_TYPE_CHAT;
	}
	else if (type == "GiftMessage") {
		giftDanmakus.push_back(item);
		if (giftDanmakus.size() > 300) {
			giftDanmakus.pop_front();
		}
		danmakuType = DANITEM_TYPE_GIFT;
	}
	else if (type == "SocialMessage") {
	}
	else if (type == "LikeMessage") {
		likeDanmakus.push_back(item);
		if (likeDanmakus.size() > 300) {
			likeDanmakus.pop_front();
		}
		danmakuType = DANITEM_TYPE_LIKE;
	}
	if (allDanmakus.size() > 300) {
		allDanmakus.pop_front();
	}
	allDanmakus.push_back(item);

	//两种情况清除现有弹幕，并重新加载弹幕
	//1. 如果用户且了单个直播间,要从所有按照用户id过滤消息并显示
	//2. 如果用户过滤弹幕类型，如用户留言，礼物打赏等等

	if (userDanmakuType != userLastDanmakuType) {
		userLastDanmakuType = userDanmakuType;
		//清除掉原有弹幕

		while (QLayoutItem* item = danmaKuAreaLayout->takeAt(0)) {
			if (item->widget()) {
				item->widget()->deleteLater();
			}
			delete item;
		}
		QList<DanmaItem> currentDanmakus;
		//添加新的弹幕
		if (userDanmakuType == DANITEM_TYPE_ALL) {
			currentDanmakus = allDanmakus;
		}
		else if (userDanmakuType == DANITEM_TYPE_GIFT) {
			currentDanmakus = giftDanmakus;
		}
		else if (userDanmakuType == DANITEM_TYPE_CHAT) {
			currentDanmakus = chatDanmakus;
		}
		else if (userDanmakuType == DANITEM_TYPE_WHOIS) {
			currentDanmakus = whoIsDanmukus;
		} else if (userDanmakuType == DANITEM_TYPE_SINGLE) {
			for (DanmaItem item : allDanmakus) {
				int iLiveId = -1;
				if (item.liveId.length() > 1) {
					iLiveId = item.liveId.left(1).toInt();
				}
				qDebug() << "iLiveId " << iLiveId << " mDanmakuValue " << mDanmakuValue;
				if (iLiveId == mDanmakuValue) {
					currentDanmakus.push_back(item);
				}
			}


		}

		if (currentDanmakus.size() > 0) {
			for (DanmaItem item : currentDanmakus) {
				// 创建新 widget
				DanmakuWidget *newWidget = new DanmakuWidget();
				newWidget->setFirstRowContent(item.deviceName, item.iamgePath);
				newWidget->setSecondRowContent(item.danmaku);

				// 添加到布局
				widgetList.append(newWidget);

				// 插入到布局的最底部
				danmaKuAreaLayout->insertWidget(danmaKuAreaLayout->count(),
								newWidget); // 最新的 widget 添加到布局的最后
				qDebug() << "danmaKuAreaLayout count " << danmaKuAreaLayout->count();

				// 滚动到最底部显示最新添加的widget
				QScrollBar *vScrollBar = danmakuscrollArea->verticalScrollBar();
				vScrollBar->setValue(vScrollBar->maximum());

				// 检查是否超过 300 个 widget
				if (danmaKuAreaLayout->count() > 300) {
					// 移除最早的 widget
					QLayoutItem *oldestItem = danmaKuAreaLayout->takeAt(0);
					if (oldestItem->widget()) {
						delete oldestItem->widget();
					}
					delete oldestItem;
				}
			}
		}
	}
	if ((danmakuType != userDanmakuType) &&
	    ((userDanmakuType != DANITEM_TYPE_ALL) && (userDanmakuType != DANITEM_TYPE_SINGLE))) {
		return;
	}
	if (userDanmakuType == DANITEM_TYPE_SINGLE) {
		int iLiveId = -1;
		if (item.liveId.length() > 1) {
			iLiveId = item.liveId.toInt();
		}
		if (iLiveId != mDanmakuValue) {
			return;
		}
	}



	// 创建新 widget
	DanmakuWidget *newWidget = new DanmakuWidget();
	newWidget->setFirstRowContent(text, imagePath);
	newWidget->setSecondRowContent(text2);


	// 添加到布局
	widgetList.append(newWidget);

	// 插入到布局的最底部
	danmaKuAreaLayout->insertWidget(danmaKuAreaLayout->count(), newWidget); // 最新的 widget 添加到布局的最后
	qDebug() << "danmaKuAreaLayout count " << danmaKuAreaLayout->count();

	// 滚动到最底部显示最新添加的widget
	QScrollBar *vScrollBar = danmakuscrollArea->verticalScrollBar();
	vScrollBar->setValue(vScrollBar->maximum());

	// 检查是否超过 300 个 widget
	if (danmaKuAreaLayout->count() > 300) {
		// 移除最早的 widget
		QLayoutItem *oldestItem = danmaKuAreaLayout->takeAt(0);
		if (oldestItem->widget()) {
			delete oldestItem->widget();
		}
		delete oldestItem;
	}
}
void GBSBizLiveGuarderCtrl::onDanmukuChanged(int value) {
	userDanmakuType = DANITEM_TYPE_SINGLE;
	mDanmakuValue = value;
}
#include "GBSBizLiveGuarderCtrl.moc"
