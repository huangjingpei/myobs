#pragma once
#include <QSettings>
#include <QString>
#include <QVariant>
#include <QStringList>
#include <QCoreApplication>
#include <QAnyStringView>

class IniSettings {
public:
	IniSettings(const QString &fileName)
		: settings(QCoreApplication::applicationDirPath() + "/" +
				   fileName,
			   QSettings::IniFormat)
	{
	}

	// 保存设置到指定的 section
	void setValue(QAnyStringView section, QAnyStringView key,
		      const QVariant &value)
	{
		settings.beginGroup(section);  // 进入指定的 section
		settings.setValue(key, value); // 设置值
		settings.endGroup();           // 退出 section
	}

	// 读取指定 section 中的设置
	QVariant value(QAnyStringView section, QAnyStringView key,
		       const QVariant &defaultValue = QVariant()) const
	{
		settings.beginGroup(section); // 进入指定的 section
		QVariant result = settings.value(key, defaultValue); // 读取值
		settings.endGroup(); // 退出 section
		return result;
	}

	// 删除指定 section 中的设置
	void remove(QAnyStringView section, QAnyStringView key)
	{
		settings.beginGroup(section); // 进入指定的 section
		settings.remove(key);         // 删除值
		settings.endGroup();          // 退出 section
	}

	// 获取指定 section 的所有键
	QStringList allKeys(QAnyStringView section) const
	{
		settings.beginGroup(section); // 进入指定的 section
		QStringList keys = settings.allKeys(); // 获取所有键
		settings.endGroup();                   // 退出 section
		return keys;
	}

private:
	mutable QSettings settings; // 将 QSettings 声明为 mutable
};


//test 
#include <QCoreApplication>
#include <QDebug>

//int main(int argc, char *argv[])
//{
//	QCoreApplication a(argc, argv);
//
//	IniSettings iniSettings("config.ini");
//
//	// 保存设置到指定的 section
//	iniSettings.setValue("UserSettings", "username", "exampleUser");
//	iniSettings.setValue("UserSettings", "password", "examplePassword");
//	iniSettings.setValue("WindowSettings", "width", 800);
//	iniSettings.setValue("WindowSettings", "height", 600);
//
//	// 读取设置
//	QString username =
//		iniSettings.value("UserSettings", "username", "defaultUser")
//			.toString();
//	QString password =
//		iniSettings.value("UserSettings", "password", "defaultPassword")
//			.toString();
//	int width = iniSettings.value("WindowSettings", "width", 1024).toInt();
//	int height = iniSettings.value("WindowSettings", "height", 768).toInt();
//
//	qDebug() << "Username:" << username;
//	qDebug() << "Password:" << password;
//	qDebug() << "Window Size:" << width << "x" << height;
//
//	// 获取所有设置的键
//	QStringList userKeys = iniSettings.allKeys("UserSettings");
//	qDebug() << "UserSettings keys:";
//	for (const QString &key : userKeys) {
//		qDebug() << key << ":"
//			 << iniSettings.value("UserSettings", key).toString();
//	}
//
//	return a.exec();
//}
