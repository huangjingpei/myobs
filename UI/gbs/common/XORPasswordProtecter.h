#include <QFile>
#include <QTextStream>
#include <QByteArray>
#include <QCryptographicHash>

class XORPasswordProtecter {
public:
    XORPasswordProtecter(const QByteArray& key) : encryptionKey(key) {}

    // 加密密码
    QByteArray encrypt(const QString& password) {
        QByteArray data = password.toUtf8();
        for (int i = 0; i < data.size(); ++i) {
            data[i] = data[i] ^ encryptionKey[i % encryptionKey.size()];
        }
        return data.toBase64(); // 转成 Base64 保存到文件
    }

    // 解密密码
    QString decrypt(const QByteArray& encryptedData) {
        QByteArray data = QByteArray::fromBase64(encryptedData);
        for (int i = 0; i < data.size(); ++i) {
            data[i] = data[i] ^ encryptionKey[i % encryptionKey.size()];
        }
        return QString::fromUtf8(data);
    }

    // 保存密码到文件
    void savePassword(const QString& password, const QString& filePath) {
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << encrypt(password);
            file.close();
        }
    }

    // 从文件读取并解密密码
    QString loadPassword(const QString& filePath) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QByteArray encryptedData = in.readAll().toUtf8();
            file.close();
            return decrypt(encryptedData);
        }
        return QString();
    }

private:
    QByteArray encryptionKey;  // 密钥
};

// // 使用示例
// int main() {
//     QByteArray key = "simplekey";  // 设置一个简单的密钥
//     XORPasswordProtecter manager(key);

//     QString password = "MySecretPassword";
//     QString filePath = "password.txt";

//     // 保存加密密码到文件
//     manager.savePassword(password, filePath);

//     // 从文件读取并解密
//     QString decryptedPassword = manager.loadPassword(filePath);

//     qDebug() << "原始密码:" << password;
//     qDebug() << "解密后的密码:" << decryptedPassword;

//     return 0;
// }
