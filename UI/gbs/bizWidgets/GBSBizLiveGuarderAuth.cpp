#include "GBSBizLiveGuarderAuth.h"
#include "ui_GBSBizLiveGuarderAuth.h"
#include "GBSBizLiveGuarder.h"
#include "gbs/common/GBSHttpClient.h"
#include "gbs/GBSMainCollector.h"
#include "gbs/dto/GBSLiveAccountInfo.h"
#include "gbs/common/QBizLogger.h"
#include <QList>
#include<QLineEdit>
#define AUTH_CODE_NUM 6

static QLineEdit *currentLineEdit = nullptr; // 保存当前鼠标悬停的QLineEdit
class MyEventFilter : public QObject {
	Q_OBJECT

public:
	MyEventFilter(QObject *parent = nullptr) : QObject(parent) {}

protected:
	bool eventFilter(QObject *obj, QEvent *event) override
	{
		if (event->type() == QEvent::FocusIn) {
			qDebug() << obj->objectName() << "获得了焦点 (事件过滤器)";
			//  可以在这里修改 obj （例如，QPushButton）的外观
			QLineEdit *lineEdit = qobject_cast<QLineEdit *>(obj);
			if (lineEdit) {
				//button->setStyleSheet("background-color: lightblue;");
				lineEdit->setText("");
				currentLineEdit = lineEdit;
				emit ClearLineEditText(lineEdit);

			}
			return false; // 让事件继续传递
		} else if (event->type() == QEvent::FocusOut) {
			qDebug() << obj->objectName() << "失去了焦点 (事件过滤器)";
			QLineEdit *lineEdit = qobject_cast<QLineEdit *>(obj);
			if (lineEdit) {
				//lineEdit->setStyleSheet(""); // 移除样式
	
				currentLineEdit = nullptr; // 鼠标移出该QLineEdit

			}
			return false; // 让事件继续传递
		} else if (event->type() == QEvent::KeyPress) {
			QLineEdit *lineEdit = qobject_cast<QLineEdit *>(obj);
			QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
			if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
				emit EnterKeyPressed();
			} else if ((keyEvent->key() == Qt::Key_Backspace) || (keyEvent->key() == Qt::Key_Delete) ){
				emit DelKeyPressed();
			} else {
				QString text = keyEvent->text();
				QChar ch = text[0];
				if (ch.isPrint()) {
					lineEdit->setText("*");
					lineEdit->setProperty("value", ch);
				}
				
			}
		}
		//  标准的事件处理
		return QObject::eventFilter(obj, event);
	}

	public:
public:
signals:
    void EnterKeyPressed();
    void DelKeyPressed();
    void ClearLineEditText(QLineEdit* editText);

};

GBSBizLiveGuarderAuth::GBSBizLiveGuarderAuth(QWidget *parent)
	: QWidget(parent),
      ui(new Ui::GBSBizLiveGuarderAuth)
{
	ui->setupUi(this);

    MyEventFilter *filter = new MyEventFilter(this);
    qlists.push_back(ui->lePassword0);
    qlists.push_back(ui->lePassword1);
    qlists.push_back(ui->lePassword2);
    qlists.push_back(ui->lePassword3);
    qlists.push_back(ui->lePassword4);
    qlists.push_back(ui->lePassword5);

	ui->lePassword0->installEventFilter(filter);
	ui->lePassword1->installEventFilter(filter);
	ui->lePassword2->installEventFilter(filter);
	ui->lePassword3->installEventFilter(filter);
	ui->lePassword4->installEventFilter(filter);
	ui->lePassword5->installEventFilter(filter);

	connect(filter, &MyEventFilter::EnterKeyPressed,this, &GBSBizLiveGuarderAuth::EnterKeyPressed);
	connect(filter, &MyEventFilter::DelKeyPressed, this, & GBSBizLiveGuarderAuth::DelKeyPressed);
	connect(filter, &MyEventFilter::ClearLineEditText, this, &GBSBizLiveGuarderAuth::ClearLineEditText);

    for (int i = 0; i < AUTH_CODE_NUM; i++) {
        qlists[i]->setMaxLength(1);
        qlists[i]->setMaxLength(1); // 限制每个输入框最多输入1个字符
        qlists[i]->setAlignment(Qt::AlignCenter); // 居中显示
        // 设置样式表，使其看起来像一个圆形框
        qlists[i]->setStyleSheet(R"(
                QLineEdit {
                    border: 1px solid #E9EBED;  /* 虚线边框 */
                    border-radius: 25px;           /* 圆角，半径等于宽高一半 */
                    background-color: #F9F9F9;     /* 背景颜色 */
                    font-size: 24px;               /* 字体大小 */
                    size:24px;
                    color: gray;                   /* 字体颜色 */
            center:true;
                }
                QLineEdit:focus {
                    border: 2px solid green;       /* 焦点状态边框 */
                }
            )");

        // 连接 textChanged 信号，每次输入后自动跳转到下一个框
        connect(qlists[i], &QLineEdit::textChanged, this, [=]() {
            if ((qlists[i]->text().length() == 1) && (i < 5)){
                focusNextInput(i);
            }
        });
    }
    qlists[0]->setFocus();


    QString resetPassword = R"(
        <p style="font-size: 18px;">
            <span style="color: black;">忘记了？</span>
             <a href="constrains" style="color: #00C566; text-decoration: none;">重置</a>
        </p>)";
    ui->lblResetPassword->setText(resetPassword);
    ui->lblResetPassword->setTextFormat(Qt::RichText);
    ui->lblResetPassword->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->lblResetPassword->setOpenExternalLinks(false);

    connect(this, &GBSBizLiveGuarderAuth::enterGuarderCtrl, reinterpret_cast<GBSBizLiveGuarder *>(parent), &GBSBizLiveGuarder::enterGuarderCtrl);

    connect(ui->pushButton, &QPushButton::clicked, this, &GBSBizLiveGuarderAuth::sendEnterCtrl);
    GBSHttpClient::getInstance()->registerHandler(this);
}

void GBSBizLiveGuarderAuth::sendEnterCtrl() {
	GBSLiveAccountInfo account = GBSMainCollector::getInstance()->getAccountInfo();
	GBSHttpClient::getInstance()->enterControlV2(getInputPassword().toUtf8().constData(), account.getId(), account.getLiveDeviceId());
}

GBSBizLiveGuarderAuth::~GBSBizLiveGuarderAuth()
{
	GBSHttpClient::getInstance()->unRegisterHandler(this);
	qlists.clear();
	delete ui;
}

void GBSBizLiveGuarderAuth::focusNextInput(int currentIndex) {
    qlists[(currentIndex+1) % 6]->setFocus(); // 跳到下一个输入框
	if (currentIndex == 5) {
	    ui->pushButton->setFocus();
	}
}

void GBSBizLiveGuarderAuth::EnterKeyPressed() {
	bool valid = true;
	for (auto le : qlists) {
		if (le->text().isEmpty()) {
			valid = false;
			break;
		}
	}
	if (valid) {
		sendEnterCtrl();
	}
}
void GBSBizLiveGuarderAuth::DelKeyPressed() {
	int i = 0;
	for (auto le : qlists) {
		if (currentLineEdit == le) {
			currentLineEdit->setText("");
			if (i > 0) {
				qlists[i - 1]->setFocus();
			}
		}
		++i;
	}
}

void GBSBizLiveGuarderAuth::ClearLineEditText(QLineEdit *editText) {
	editText->setText("");
	editText->setProperty("value", "");
}


QString GBSBizLiveGuarderAuth::getPresetPassword() {
    return presetPassword;
}
QString GBSBizLiveGuarderAuth::getInputPassword() {
	inputPassword.clear();
	for (int i = 0; i < qlists.count(); i++) {
		inputPassword += qlists.at(i)->property("value").toChar();
	}
	qDebug() << "inputPassword " << inputPassword;

    return inputPassword;
}
void GBSBizLiveGuarderAuth::setPresetPassword(QString password) {
    presetPassword = password;
}

void GBSBizLiveGuarderAuth::onEnterGuardCtrl(int result) {
	if (result == 1) {
		emit enterGuarderCtrl();
	} else {
		QLogE("Enter guard ctrl error. input password: %s", getInputPassword().toUtf8().constData());
	}
}

#include "GBSBizLiveGuarderAuth.moc"
