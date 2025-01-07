#include "GBSBizSettingLiveSourceDupRM.h"
#include "ui_GBSBizSettingLiveSourceDupRM.h"
#include <QFileDialog>
#include "gbs/common/FileIOUtils.h"
#include "window-basic-main.hpp"

GBSBizSettingLiveSourceDupRM::GBSBizSettingLiveSourceDupRM(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GBSBizSettingLiveSourceDupRM)
{
    ui->setupUi(this);

    //ui->tabWidget->setStyleSheet(
    //    "QTabWidget::pane {"
    //    "    border: none;"  // 移除tab pane的边框
    //    "}"
    //    "QTabBar::tab {"
    //    "    background: none;"  // tab的背景颜色
    //    "    padding: 10px;"  // tab内容的填充
    //    "    margin-right: 10px;"  // 调整tab之间的水平间距
    //    "    border: none;"  // 移除tab的边框
    //    "    color: #01C667;"
    //    "    font-size:16px;"
    //    "}"
    //    "QTabBar::tab:first {"
    //    "    margin-left: 100px;"  // 调整第一个tab项的左外边距
    //    "}"
    //    "QTabBar::tab:selected {"
    //    "    background: #C0C0C0;"  // 选中tab的背景颜色
    //    "    border: none;"  // 选中时也不显示边框
    //    "    border-radius: 16px"
    //    "}"
    //    "QTabBar {"
    //    "    qproperty-alignment: AlignCenter;"  // 设置tab项居中对齐
    //    "}"
    //    );



    setStyleForAllSliders(this);

    iniFile = std::make_unique<IniSettings>("gbs.ini");
    bool jitterOn = iniFile->value("RemoveDuplicate", "video.jitter", false).toBool();
    ui->horizontalSlider_19->setValue(jitterOn);
    connect(ui->horizontalSlider_19, &QSlider::valueChanged, this, [this](int value) {
	    ui->horizontalSlider_19->setValue(value);
	    iniFile->setValue("RemoveDuplicate", "video.jitter", value);
	    });

    bool extractFrame = iniFile->value("RemoveDuplicate", "video.ExtractFrame", false).toBool();
    ui->horizontalSlider_20->setValue(extractFrame);
    connect(ui->horizontalSlider_20, &QSlider::valueChanged, this, [this](int value) {
	    ui->horizontalSlider_20->setValue(value);
	    iniFile->setValue("RemoveDuplicate", "video.ExtractFrame", value);
    });

    bool extractVideoLayer = iniFile->value("RemoveDuplicate", "video.ExtractVideoLayer", false).toBool();
    ui->horizontalSlider_21->setValue(extractVideoLayer);
    connect(ui->horizontalSlider_21, &QSlider::valueChanged, this, [this](int value) {
	    ui->horizontalSlider_21->setValue(value);
	    iniFile->setValue("RemoveDuplicate", "video.ExtractVideoLayer", value);
    });
    bool timeClockOverlay = iniFile->value("RemoveDuplicate", "video.TimeClockOverlay", false).toBool();
    ui->horizontalSlider_22->setValue(timeClockOverlay);
    connect(ui->horizontalSlider_22, &QSlider::valueChanged, this, [this](int value) {
	    ui->horizontalSlider_22->setValue(value);
	    iniFile->setValue("RemoveDuplicate", "video.TimeClockOverlay", value);
    });
    bool whosCommingOverlay = iniFile->value("RemoveDuplicate", "video.WhosCommingOverlay", false).toBool();
    ui->horizontalSlider_23->setValue(whosCommingOverlay);
    connect(ui->horizontalSlider_23, &QSlider::valueChanged, this, [this](int value) {
	    ui->horizontalSlider_23->setValue(value);
	    iniFile->setValue("RemoveDuplicate", "video.WhosCommingOverlay", value);
    });
    bool audioEffectOverlay = iniFile->value("RemoveDuplicate", "video.AudioEffectOverlay", false).toBool();
    ui->horizontalSlider_24->setValue(audioEffectOverlay);
    connect(ui->horizontalSlider_24, &QSlider::valueChanged, this, [this](int value) {
	    ui->horizontalSlider_24->setValue(value);
	    iniFile->setValue("RemoveDuplicate", "video.AudioEffectOverlay", value);
    });
    bool productOverlay = iniFile->value("RemoveDuplicate", "video.ProductOverlay", false).toBool();
    ui->horizontalSlider_25->setValue(productOverlay);
    connect(ui->horizontalSlider_25, &QSlider::valueChanged, this, [this](int value) {
	    ui->horizontalSlider_25->setValue(value);
	    iniFile->setValue("RemoveDuplicate", "video.ProductOverlay", value);
    });

    bool globalControl = iniFile->value("RemoveDuplicate", "video.GlobalControl", false).toBool();
    ui->horizontalSlider_30->setValue(globalControl);
    connect(ui->horizontalSlider_30, &QSlider::valueChanged, this, [this](int value) {
	    ui->horizontalSlider_30->setValue(value);
	    iniFile->setValue("RemoveDuplicate", "video.GlobalControl", value);
    });

    int localJitter = iniFile->value("RemoveDuplicate", "video.LocalJitter", 0).toBool();
    ui->hsJittterSlider->setValue(localJitter);
    connect(ui->hsJittterSlider, &QSlider::valueChanged, this, [this](int value) {
	    ui->hsJittterSlider->setValue(value);
	    OBSBasic *main = OBSBasic::Get();
	    main->changeTransform(value);
	    iniFile->setValue("RemoveDuplicate", "video.LocalJitter", value);
    });
    int localExtractFrame = iniFile->value("RemoveDuplicate", "video.LocalExtractFrame", 0).toInt();
    ui->hsExtractFrameSlider->setValue(localExtractFrame);
    connect(ui->hsExtractFrameSlider, &QSlider::valueChanged, this, [this](int value) {
	    ui->hsExtractFrameSlider->setValue(value);
	    iniFile->setValue("RemoveDuplicate", "video.LocalExtractFrame", value);
    });
    int localTransparent = iniFile->value("RemoveDuplicate", "video.LocalTransparent", 0).toInt();
    ui->hsTransparentSlider->setValue(localTransparent);
    ui->hsTransparentSlider->setMinimum(0); // 设置滑块的最小值
    ui->hsTransparentSlider->setMaximum(100); // 设置滑块的最大值
    connect(ui->hsTransparentSlider, &QSlider::valueChanged, this, [this](int value) {
	    ui->hsTransparentSlider->setValue(value);
	    iniFile->setValue("RemoveDuplicate", "video.LocalTransparent", value);
    });

    
    bool autoVaryResponse = iniFile->value("RemoveDuplicate", "audio.AutoVaryResponse", false).toBool();
    ui->horizontalSlider_26->setValue(autoVaryResponse);
    connect(ui->horizontalSlider_26, &QSlider::valueChanged, this, [this](int value) {
	    ui->horizontalSlider_26->setValue(value);
	    iniFile->setValue("RemoveDuplicate", "audio.LocalJitter", value);
    });
    bool backgroundDenoise = iniFile->value("RemoveDuplicate", "audio.BackgroundDenoise", false).toBool();
    ui->horizontalSlider_27->setValue(backgroundDenoise);
    connect(ui->horizontalSlider_27, &QSlider::valueChanged, this, [this](int value) {
	    ui->horizontalSlider_27->setValue(value);
	    iniFile->setValue("RemoveDuplicate", "audio.BackgroundDenoise", value);
    });
    bool aiSpeechResponse = iniFile->value("RemoveDuplicate", "audio.AiSpeechResponse", false).toBool();
    ui->horizontalSlider_28->setValue(aiSpeechResponse);
    connect(ui->horizontalSlider_28, &QSlider::valueChanged, this, [this](int value) {
	    ui->horizontalSlider_28->setValue(value);
	    iniFile->setValue("RemoveDuplicate", "audio.AiSpeechResponse", value);
    });

    bool aiBigModelUse = iniFile->value("RemoveDuplicate", "audio.AiBigModelUse", false).toBool();
    ui->horizontalSlider_29->setValue(aiBigModelUse);
    connect(ui->horizontalSlider_29, &QSlider::valueChanged, this, [this](int value) {
	    ui->horizontalSlider_29->setValue(value);
	    iniFile->setValue("RemoveDuplicate", "audio.AiBigModelUse", value);
    });



    QString dedupImagePath = iniFile->value("RemoveDuplicate", "audio.DedupImagePath", "").toString();
    ui->lineEdit_10->setText(dedupImagePath);
    connect(ui->lineEdit_10, &QLineEdit::textChanged, this, [this](const QString &text) {
	    ui->lineEdit_10->setText(text);
	    iniFile->setValue("RemoveDuplicate", "audio.DedupImagePath", text);
	    OBSBasic *main = OBSBasic::Get();
	    if (text == "") {
		    main->removeSlideShowSource();
	    } else {
		    QDir dir(text);
		    if (dir.exists()) {
			    QStringList filters;
			    filters << "*.png" << "*.jpeg" << "*.jpg"; // 可以加上 .jpg 作为对比
			    QStringList files = dir.entryList(filters, QDir::Files);
			    QStringList absoluteFiles;
			    for (const QString &file : files) {
				    // 使用 absoluteFilePath 获取文件的完整路径
				    QString fullPath = dir.absoluteFilePath(file);
				    absoluteFiles << fullPath;
			    }
			    main->addSlideShowSource(absoluteFiles);
		    }
		    
	    }
    });

    QString timeClockImagePath = iniFile->value("RemoveDuplicate", "audio.TimeClockImagePath", "").toString();
    ui->lineEdit_6->setText(timeClockImagePath);
    connect(ui->lineEdit_6, &QLineEdit::textChanged, this, [this](const QString &text) {
	    ui->lineEdit_6->setText(text);
	    iniFile->setValue("RemoveDuplicate", "audio.TimeClockImagePath", text);
	    OBSBasic *main = OBSBasic::Get();
	    if (text == "") {
		    main->removeTimeClockSource();
	    } else {

		    QFileInfo fileInfo(text);
		    if (fileInfo.isFile()) {
			    main->addTimeClockSource(text);
		    }

		    
	    }
    });

    QString otherPlatformMateralImagePath = iniFile->value("RemoveDuplicate", "audio.OtherPlatformMateralImagePath", "").toString();
    ui->lineEdit_9->setText(otherPlatformMateralImagePath);
    connect(ui->lineEdit_9, &QLineEdit::textChanged, this, [this](const QString &text) {
	    ui->lineEdit_9->setText(text);
	    iniFile->setValue("RemoveDuplicate", "audio.OtherPlatformMateralImagePath", text);
    });
    QString whosCommingImagePath = iniFile->value("RemoveDuplicate", "audio.WhosCommingImagePath", "").toString();
    ui->lineEdit_8->setText(whosCommingImagePath);
    connect(ui->lineEdit_8, &QLineEdit::textChanged, this, [this](const QString &text) {
	    ui->lineEdit_8->setText(text);
	    iniFile->setValue("RemoveDuplicate", "audio.WhosCommingImagePath", text);
    });

    QString audioEffectImagePath = iniFile->value("RemoveDuplicate", "audio.AudioEffectImagePath", "").toString();
    ui->lineEdit_7->setText(audioEffectImagePath);
    connect(ui->lineEdit_7, &QLineEdit::textChanged, this, [this](const QString &text) {
	    ui->lineEdit_7->setText(text);
	    iniFile->setValue("RemoveDuplicate", "audio.AudioEffectImagePath", text);
    });


    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &GBSBizSettingLiveSourceDupRM::onTabChanged);
    connect(ui->pushButton_6, &QPushButton::clicked, this, [&]() {
        QString filePath = QFileDialog::getOpenFileName(nullptr, QObject::tr("Open File"),
                                                        "", QObject::tr("All Files (*)"));
        if (!filePath.isEmpty()) {
                qDebug() << "Selected file:" << filePath;
            ui->lineEdit_7->setText(filePath);
        }

    });
    connect(ui->pushButton_7, &QPushButton::clicked, this, [&]() {
	    QString text = ui->lineEdit_10->text();
	    QDir dirPath(text);

	    if (dirPath.isEmpty()) {
		    dirPath = QDir::homePath(); // 默认路径
	    } else {
		    if (!dirPath.exists()) {
			    dirPath = QDir::homePath();
		    }
	    }
	    QString dir = FileIOUtils::getInstance()->SelectDirectory(this, "请选择有图片的目录", dirPath.absolutePath());


	    if (!dir.isEmpty()) {
		qDebug() << "Selected dir:" << dir;
		ui->lineEdit_10->setText(dir);
		ui->lineEdit_10->setProperty("changed", true);
		iniFile->setValue("RemoveDuplicate", "audio.DedupImagePath", dir);
	}

    });
    connect(ui->pushButton_8, &QPushButton::clicked, this, [this]() {
		QString directoryPath = ui->lineEdit_6->text();
	    QFileInfo fileInfo(directoryPath);


	    if (directoryPath.isEmpty()) {
			directoryPath = QDir::homePath(); // 默认路径
	    } else {
		    if (fileInfo.exists() && fileInfo.isFile()) {
			    directoryPath = fileInfo.absolutePath();
		    } else {
			    directoryPath = QDir::homePath();
		    }
	    }

	QString filePath = FileIOUtils::getInstance()->OpenFile(this, "获取时钟文件", directoryPath,
							QObject::tr("Html files (*.htm *.html"));
        
	if (!filePath.isEmpty()) {
		qDebug() << "Selected file:" << filePath;

		ui->lineEdit_6->setText(filePath);

		mTimerClockDupRm = filePath;
		
		ui->lineEdit_6->setProperty("changed", true);
	}
		

    });
    connect(ui->pushButton_9, &QPushButton::clicked, this, [&]() {
        QString filePath = QFileDialog::getOpenFileName(nullptr, QObject::tr("Open File"),
                                                        "", QObject::tr("All Files (*)"));
        if (!filePath.isEmpty()) {
            qDebug() << "Selected file:" << filePath;
            ui->lineEdit_9->setText(filePath);
	    ui->lineEdit_9->setProperty("changed", true);

        }

    });
    connect(ui->pushButton_10, &QPushButton::clicked, this, [&]() {
        QString filePath = QFileDialog::getOpenFileName(nullptr, QObject::tr("Open File"),
                                                        "", QObject::tr("All Files (*)"));
        if (!filePath.isEmpty()) {
            qDebug() << "Selected file:" << filePath;
            ui->lineEdit_8->setText(filePath);
	    ui->lineEdit_8->setProperty("changed", true);

        }

    });

    //QString sliderStyleSheet = R"(
    //        "QSlider {"
    //        "    background: transparent;"
    //        "}"
    //        "QSlider::groove:horizontal {"
    //        "    border: none;"
    //        "    height: 20px;"
    //        "    margin: 0px;"
    //        "    border-radius: 10px;"
    //        "    background: qlineargradient(spread:pad, x1:0, y1:0.5, x2:1, y2:0.5, stop:0 #212121, stop:1 #000000);"
    //        "}"
    //        "QSlider::handle:horizontal {"
    //        "    background: white;"
    //        "    border: 2px solid #000000;"
    //        "    width: 26px;"
    //        "    height: 26px;"
    //        "    margin: -3px 0;"  // 调整滑块位置以适应槽
    //        "    border-radius: 13px;"
    //        "}"
    //        "QSlider::handle:horizontal:pressed {"
    //        "    width: 8px;" /* 保持滑块宽度一致 */
    //        "    height: 4px;" /* 保持滑块高度一致 */
    //        "    margin-left: -2px;" /* 保持外边距一致 */
    //        "    margin-right: -2px;" /* 保持外边距一致 */
    //        "}"
    //        "QSlider::add-page:horizontal {"
    //        "    background: #EB3F5E;"
    //        "    border-radius: 2px;"
    //        "}"
    //        "QSlider::sub-page:horizontal {"
    //        "    background: #EB3F5E;"
    //        "    border-radius: 2px;"
    //        "}"
    //)";

    ui->hsJittterSlider->setRealSize(521, 20);
    ui->hsJittterSlider->setRealRange(0, 100);
    ui->hsExtractFrameSlider->setRealSize(521, 20);
    ui->hsExtractFrameSlider->setRealRange(0, 100);
    ui->hsTransparentSlider->setRealSize(521, 20);
    ui->hsTransparentSlider->setRealRange(0, 100);

    //ui->hsJittterSlider->setStyleSheet(sliderStyleSheet);
    //ui->hsExtractFrameSlider->setStyleSheet(sliderStyleSheet);
    //ui->hsTransparentSlider->setStyleSheet(sliderStyleSheet);

    connect(ui->hsExtractFrameSlider, &QSlider::valueChanged, this, []() {
	    
	    });


    connect(ui->btnApply_4, &QPushButton::clicked, this, &GBSBizSettingLiveSourceDupRM::onApply);
    connect(ui->btnCancel_4, &QPushButton::clicked, this, &GBSBizSettingLiveSourceDupRM::onCancel);
    connect(ui->btnConfirm_4, &QPushButton::clicked, this, &GBSBizSettingLiveSourceDupRM::onConfirm);
}


void GBSBizSettingLiveSourceDupRM::onApply(bool checked) {

}
void GBSBizSettingLiveSourceDupRM::onCancel(bool checked) {

}
void GBSBizSettingLiveSourceDupRM::onConfirm(bool checked) {

}

void GBSBizSettingLiveSourceDupRM::onTabChanged(int index) {
    qDebug() << "当前选项卡索引为:" << index;
    //这里应该是添加或者删除一个windows 最合理
    if (index == 1) {
        ui->widget->setVisible(false);

    } else {

        ui->widget->setVisible(true);

    }
}

void GBSBizSettingLiveSourceDupRM::setStyleForAllSliders(QWidget *widget) {
    QSlider *slider = qobject_cast<QSlider *>(widget);
    if (slider) {

        slider->setStyleSheet(
            "QSlider {"
            "    background: transparent;"
            "}"
            "QSlider::groove:horizontal {"
            "    border: none;"
            "    height: 20px;"
            "    margin: 0px;"
            "    border-radius: 10px;"
            "    background: qlineargradient(spread:pad, x1:0, y1:0.5, x2:1, y2:0.5, stop:0 #212121, stop:1 #000000);"
            "}"
            "QSlider::handle:horizontal {"
            "    background: white;"
            "    border: 2px solid #000000;"
            "    width: 26px;"
            "    height: 26px;"
            "    margin: -3px 0;"  // 调整滑块位置以适应槽
            "    border-radius: 13px;"
            "}"
            "QSlider::handle:horizontal:pressed {"
            "    background: #bfbfbf;"
            "}"
            );
        slider->setRange(0, 1);
    }
    for (int i = 0; i < widget->children().size(); ++i) {
        QWidget *child = qobject_cast<QWidget*>(widget->children().at(i));
        if (child) {
            setStyleForAllSliders(child);
        }
    }
}

GBSBizSettingLiveSourceDupRM::~GBSBizSettingLiveSourceDupRM()
{
	QVariant data;
	data = ui->lineEdit_6->property("changed");
	//if (data.value<bool>() == true) {
	//	emit ui->lineEdit_6->textChanged(ui->lineEdit_6->text());
	//}
	////data = ui->lineEdit_10->property("changed");
	////if (data.value<std::string>() == "true") {
	////	emit ui->lineEdit_10->textChanged(ui->lineEdit_10->text());
	////}
	////data = ui->lineEdit_8->property("changed");
	////if (data.value<std::string>() == "true") {
	////	emit ui->lineEdit_8->textChanged(ui->lineEdit_8->text());
	////}
	////data = ui->lineEdit_9->property("changed");
	////if (data.value<std::string>() == "true") {
	////	emit ui->lineEdit_9->textChanged(ui->lineEdit_9->text());
	////}
    delete ui;
}
