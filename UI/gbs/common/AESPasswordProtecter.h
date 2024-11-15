#ifndef __AES_PASSWORD_PROTECTER_H__
#define __AES_PASSWORD_PROTECTER_H__
#include "./Qt-AES/qaesencryption.h"  // 包含 QAESEncryption 头文件
#include <QFile>
#include <QTextStream>
#include <QCryptographicHash>
#include <QDebug>

class AESPasswordProtecter {
public:
    AESPasswordProtecter(const QByteArray& key) : encryptionKey(key) {}

    QByteArray encrypt(const QString &text) {
        QByteArray data = text.toUtf8();
        data = pkcs7Padding(data, 16);  // AES 块大小为 16 字节

        QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);
        QByteArray hashKey = QCryptographicHash::hash(encryptionKey, QCryptographicHash::Sha256);
        QByteArray iv(16, 0);  // 初始化向量
        QByteArray encryptedData = encryption.encode(data, hashKey, iv).toBase64();
        return encryptedData;
    }

    QString decrypt(const QByteArray &encryptedData) {
        QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);
        QByteArray hashKey = QCryptographicHash::hash(encryptionKey, QCryptographicHash::Sha256);
        QByteArray iv(16, 0);
        QByteArray decodedData = QByteArray::fromBase64(encryptedData);
        QByteArray decryptedData = encryption.decode(decodedData, hashKey, iv);

        decryptedData = removePkcs7Padding(decryptedData);
        return QString::fromUtf8(decryptedData);
    }


    // 保存加密密码到文件
    void savePassword(const QString& password, const QString& filePath) {
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << encrypt(password);  // 保存加密后的 Base64 数据
            file.close();
        } else {
            qDebug() << "无法打开文件进行写入";
        }
    }

    // 从文件读取并解密密码
    QString loadPassword(const QString& filePath) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QByteArray encryptedData = in.readAll().toUtf8();
            file.close();
            return decrypt(encryptedData);  // 返回解密后的密码
        } else {
            qDebug() << "无法打开文件进行读取";
            return QString();
        }
    }

private:
    QByteArray pkcs7Padding(const QByteArray &data, int blockSize) {
        int paddingSize = blockSize - (data.size() % blockSize);
        QByteArray paddedData = data;
        paddedData.append(QByteArray(paddingSize, static_cast<char>(paddingSize)));
        return paddedData;
    }
    QByteArray removePkcs7Padding(const QByteArray &data) {
        if (data.isEmpty()) {
            return data;
        }
        int paddingSize = static_cast<int>(data[data.size() - 1]);
        return data.left(data.size() - paddingSize);
    }


private:
    QByteArray encryptionKey;  // 密钥
};

/*


int main() {
    QByteArray key = "my_secret_key";  // 设置一个密钥（建议使用一个足够复杂的密钥）
    AESPasswordProtecter manager(key);

    QString password = "MySecurePassword123";  // 要加密的密码
    QString filePath = "encrypted_password.txt";

    // 将加密的密码保存到文件
    manager.savePassword(password, filePath);
    qDebug() << "加密后的密码已保存到文件。";

    // 从文件读取并解密密码
    QString decryptedPassword = manager.loadPassword(filePath);
    qDebug() << "原始密码:" << password;
    qDebug() << "解密后的密码:" << decryptedPassword;

    return 0;
}

AESPasswordProtecter 类使用 AES-256 加密模式并结合 CBC（Cipher Block Chaining）模式进行加密。
encryptionKey 是加密和解密所使用的密钥。为了增加安全性，生成 SHA-256 哈希值作为实际的加密密钥。
iv 是初始化向量（Initialization Vector），在这里使用零填充的 16 字节数组。如果需要更安全，可以在实际应用中考虑随机生成 iv 并保存到文件中。
使用 PKCS7 填充后，解密出的数据将与原始数据一致，并且不会有多余的填充字符。



*/

#endif //__AES_PASSWORD_PROTECTER_H__
