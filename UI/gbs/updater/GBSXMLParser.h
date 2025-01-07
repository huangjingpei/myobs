#pragma once
#ifndef GBSXMLPARSER_H
#define GBSXMLPARSER_H

#include <QString>
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>
#include <QException>
#include <QList>

// 自定义 XML 解析异常类
class XmlParseException : public QException {
public:
    void raise() const override { throw *this; }
    XmlParseException *clone() const override { return new XmlParseException(*this); }
};

struct Enclosure {
    QString url;
    QString version;
    QString os;
};

class GBSXMLParser {
public:
    GBSXMLParser();
    ~GBSXMLParser();

    // 解析 XML 文件的函数
    void parseXml(const QString &filePath);

    void parseXml(QByteArray bytes);

    // 获取解析后的内容
    QList<QString> getTitles() const { return titles; }
    QList<QString> getDescriptions() const { return descriptions; }
    QList<QString> getPubDates() const { return pubDates; }
    QList<Enclosure> getEnclosures() const { return enclosures; }
    QList<QString> getFeatures() const {return features;}

private:
    // 存储解析结果的成员变量
    QList<QString> titles;
    QList<QString> descriptions;
    QList<QString> pubDates;
    QList<Enclosure> enclosures;
    QList<QString> features;
    // 帮助函数：处理 XML 元素
    void processElement(QXmlStreamReader &xmlReader);
};

#endif // GBSXMLPARSER_H



////////////////////////////////////////////////////////////////////////////////////////
/*

<?xml version="1.0" encoding="UTF-8"?>
<rss version="2.0" xmlns:sparkle="http://www.andymatuschak.org/xml-namespaces/sparkle">
    <channel>
        <title>多多客</title>
        <description>新版本多多客可用</description>
        <language>zh</language>
        <item>
            <title>Version 1.0.0+4</title>
<!--            <sparkle:releaseNotesLink>-->
<!--                https://your_domain/your_path/release_notes.html-->
<!--            </sparkle:releaseNotesLink>-->

                                    <li>1、优化直播弹幕获取</li>
            <!--                        <li>2、新增抖音弹幕功能</li>-->
            <!--                        <li>3、修复bug</li>-->

            <description>
                <![CDATA[
                    <ul>
                    <li>1、欢迎使用多多客</li>
                    </ul>
                ]]>
            </description>

            <pubDate>2024.11.27 12:00</pubDate>
            <enclosure url="https://renew.guobo.shop/exe/duoduoke/ddk.exe"
                sparkle:dsaSignature="MDwCHHCSO9yYjWfES2ACwVWu2aHg7e4MfIzOMb8dP7YCHCa+LeEnuP+X5nTG8eEf1gZKF4mQrK38sl8GUk0="
                sparkle:version="1.0.0+4"
                sparkle:os="windows"
                length="0"
                type="application/octet-stream" />
        </item>
    </channel>
</rss>

*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
