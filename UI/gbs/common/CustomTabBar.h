#ifndef CUSTOMTABBAR_H
#define CUSTOMTABBAR_H
#include <QPainter>
#include <QTabBar>
#include <QStyleOptionTab>
#include <QStyle>

class CustomTabBar : public QTabBar {
    Q_OBJECT

public:
    CustomTabBar(QWidget *parent = nullptr) : QTabBar(parent) {}

protected:
    void paintEvent(QPaintEvent *event) override ;
};
#endif // CUSTOMTABBAR_H
