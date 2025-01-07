#include "GBSXMLParser.h"

GBSXMLParser::GBSXMLParser() {
    // 构造函数，可以进行一些初始化（如果需要）
}

GBSXMLParser::~GBSXMLParser() {
    // 析构函数
}

void GBSXMLParser::parseXml(const QString &filePath) {
    QFile file(filePath);
    
    // 打开文件并处理打开失败的异常
    if (!file.exists()) {
        qCritical() << "File does not exist:" << filePath;
        return;
    }
    
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "Failed to open file:" << file.errorString();
        return;
    }

    QXmlStreamReader xmlReader(&file);
    
    try {
        // 逐行读取 XML 内容
        while (!xmlReader.atEnd()) {
            xmlReader.readNext();
            
            if (xmlReader.hasError()) {
                throw XmlParseException();
            }

            if (xmlReader.isStartElement()) {
                // 处理每个元素
                processElement(xmlReader);
            } else if (xmlReader.isEndElement()) {
                // 可选择处理结束标签
            }
        }
        
        if (xmlReader.hasError()) {
            throw XmlParseException();
        }
    } catch (const XmlParseException &e) {
        qCritical() << "XML parsing error:" << xmlReader.errorString() << e.what();
    } catch (const std::exception &e) {
        qCritical() << "Unexpected error:" << e.what();
    } catch (...) {
        qCritical() << "Unknown error occurred while parsing XML.";
    }
    
    file.close();
}

void GBSXMLParser::parseXml(QByteArray bytes) {

	QXmlStreamReader xmlReader(bytes);

	try {
		// 逐行读取 XML 内容
		while (!xmlReader.atEnd()) {
			xmlReader.readNext();

			if (xmlReader.hasError()) {
				throw XmlParseException();
			}

			if (xmlReader.isStartElement()) {
				// 处理每个元素
				processElement(xmlReader);
			} else if (xmlReader.isEndElement()) {
				// 可选择处理结束标签
			}
		}

		if (xmlReader.hasError()) {
			throw XmlParseException();
		}
	} catch (const XmlParseException &e) {
		qCritical() << "XML parsing error:" << xmlReader.errorString() << e.what();
	} catch (const std::exception &e) {
		qCritical() << "Unexpected error:" << e.what();
	} catch (...) {
		qCritical() << "Unknown error occurred while parsing XML.";
	}
}

void GBSXMLParser::processElement(QXmlStreamReader &xmlReader)
{
    qDebug() << "Start Element:" << xmlReader.name().toString();

    // 处理每个元素的值并保存到成员变量中
    if (xmlReader.name() == QString("title")) {
        titles.append(xmlReader.readElementText());  // 这里仍然可以直接读取文本
    } else if (xmlReader.name() == QString("description")) {
        descriptions.append(xmlReader.readElementText());
    } else if (xmlReader.name() == QString("pubDate")) {
        pubDates.append(xmlReader.readElementText());
    } else if (xmlReader.name() == QString("li")) {
        features.append(xmlReader.readElementText());
    } else if (xmlReader.name() == QString("enclosure")) {
        Enclosure enclosure;
        enclosure.url = xmlReader.attributes().value("url").toString();
        enclosure.version = xmlReader.attributes().value("sparkle:version").toString();
        enclosure.os = xmlReader.attributes().value("sparkle:os").toString();
        enclosures.append(enclosure);
    } else if (xmlReader.name() == QString("sparkle:releaseNotesLink")) {
        qDebug() << "Release Notes Link:" << xmlReader.readElementText();
    } else {
        qDebug() << "Unknown element:" << xmlReader.name().toString();
    }
}
