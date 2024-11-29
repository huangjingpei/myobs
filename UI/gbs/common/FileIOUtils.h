#pragma once
#include <string>
#include <memory>
#include <list>
#include <mutex>

class FileIOUtils {
public:
	static std::shared_ptr<FileIOUtils> getInstance();

	QString SelectDirectory(QWidget *parent, QString title, QString path);
	QString SaveFile(QWidget *parent, QString title, QString path, QString extensions);
	QString OpenFile(QWidget *parent, QString title, QString path, QString extensions);
	QStringList OpenFiles(QWidget *parent, QString title, QString path, QString extensions);
};
