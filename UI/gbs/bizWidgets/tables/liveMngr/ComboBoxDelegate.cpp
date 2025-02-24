#include "ComboBoxDelegate.h"
#include <QComboBox>
#include <QLineEdit>
#include <QStyleOptionViewItem>
#include <QPainter>
#include <QApplication>
#include <QStyle>
#include "LiveMngrModel.h"

ComboBoxDelegate::ComboBoxDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QWidget *ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QComboBox *comboBox = new QComboBox(parent);
    //if (index.column() == LiveMngrModel::Language) {
    //    comboBox->addItems(QStringList() << "英语" << "中文" << "日语"); // 语言选项
    //} else if (index.column() == LiveMngrModel::Tone) {
    //    comboBox->addItems(QStringList() << "男声" << "女声" << "童声"); // 音色选项
    //}
    return comboBox;
}

void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    QString value = index.model()->data(index, Qt::DisplayRole).toString();
    comboBox->setCurrentText(value);
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    model->setData(index, comboBox->currentText(), Qt::EditRole);
}

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}

QStringList ComboBoxDelegate::items(const QModelIndex &index) const
{
    //if (index.column() == LiveMngrModel::Language) {
    //    return (QStringList() << "英语" << "中文" << "日语");
    //} else if (index.column() == LiveMngrModel::Tone) {
    //    return (QStringList() << "男声" << "女声" << "童声");
    //}
    return QStringList();
}
