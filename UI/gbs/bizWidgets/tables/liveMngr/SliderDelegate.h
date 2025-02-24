#ifndef SLIDERDELEGATE_H
#define SLIDERDELEGATE_H

#include <QStyledItemDelegate>
#include <QSlider>
#include <QStyleOptionViewItem>
#include <QModelIndex>

class SliderDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    SliderDelegate(QObject *parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // SLIDERDELEGATE_H
