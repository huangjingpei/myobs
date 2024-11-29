#include <QWidget>
#include <QLayout>
#include <QComboBox>
#include <QMessageBox>
#include <QDataStream>
#include <QKeyEvent>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QLabel>
#include <QPushButton>
#include <QToolBar>
#include <iostream>
#include <chrono>

#include "FileIOUtils.h"
#include <QDebug>
#include "gbs/common/QBizLogger.h"




std::shared_ptr<FileIOUtils> FileIOUtils::getInstance()
{
	static std::shared_ptr<FileIOUtils> instance(
		new FileIOUtils());
	return instance;
}


QString FileIOUtils::SelectDirectory(QWidget *parent, QString title, QString path)
{
	QString dir = QFileDialog::getExistingDirectory(parent, title, path,
							QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	return dir;
}

QString FileIOUtils::SaveFile(QWidget *parent, QString title, QString path, QString extensions)
{
	QString file = QFileDialog::getSaveFileName(parent, title, path, extensions);

	return file;
}

QString FileIOUtils::OpenFile(QWidget *parent, QString title, QString path, QString extensions)
{
	QString file = QFileDialog::getOpenFileName(parent, title, path, extensions);

	return file;
}

QStringList FileIOUtils::OpenFiles(QWidget *parent, QString title, QString path, QString extensions)
{
	QStringList files = QFileDialog::getOpenFileNames(parent, title, path, extensions);

	return files;
}
