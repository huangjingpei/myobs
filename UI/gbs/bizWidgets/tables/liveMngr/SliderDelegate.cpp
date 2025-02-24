#include "SliderDelegate.h"
#include <QSlider>
#include <QApplication>
#include <QStyle>
#include <QPainter>
#include <QDebug>

SliderDelegate::SliderDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QWidget *SliderDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSlider *slider = new QSlider(Qt::Horizontal, parent);
    slider->setMinimum(0);
    slider->setMaximum(1);
    slider->setSingleStep(1);
    slider->setPageStep(1);
    slider->setTickInterval(1);
    slider->setTickPosition(QSlider::NoTicks); // 设置刻度线位置

    return slider;
}

void SliderDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QSlider *slider = static_cast<QSlider*>(editor);
    int value = index.model()->data(index, Qt::EditRole).toInt(); // 或者 Qt::DisplayRole，取决于模型如何存储数据
    slider->setValue(value);
}

void SliderDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QSlider *slider = static_cast<QSlider*>(editor);
    model->setData(index, slider->value(), Qt::EditRole);
}

void SliderDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}


void SliderDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionSlider sliderOpt;
    //if (option.widget)
    //{
    sliderOpt.initFrom(option.widget);
    //}

    sliderOpt.rect = option.rect;
    sliderOpt.minimum = 0;
    sliderOpt.maximum = 1;
    sliderOpt.sliderPosition = index.model()->data(index, Qt::DisplayRole).toInt();
    sliderOpt.orientation = Qt::Horizontal;
    sliderOpt.tickPosition = QSlider::NoTicks; // 不显示刻度线
    sliderOpt.state = option.state | QStyle::State_Enabled;

    QApplication::style()->drawComplexControl(QStyle::CC_Slider, &sliderOpt, painter);
}
