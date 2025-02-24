#ifndef LiveMngrRemoteAuthDelegate_H
#define LiveMngrRemoteAuthDelegate_H

#include <QStyledItemDelegate>
#include <QPushButton>
#include <QTextEdit>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QHBoxLayout>
#include <QWidget>

class LiveMngrRemoteAuthDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    LiveMngrRemoteAuthDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    mutable QMap<QModelIndex, QRect> buttonRects; // 存储每个单元格的按钮矩形
};

#endif // LiveMngrRemoteAuthDelegate_H
