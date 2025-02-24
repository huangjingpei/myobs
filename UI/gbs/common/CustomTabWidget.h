#ifndef CUSTOMTABWIDGET_H
#define CUSTOMTABWIDGET_H

#include <QTabWidget>
#include <QTabBar>
#include "CustomTabBar.h" // 包含你之前定义的 CustomTabBar 头文件

class CustomTabWidget : public QTabWidget {
public:
    CustomTabWidget(QWidget *parent = nullptr) : QTabWidget(parent) {
        // 创建自定义的 TabBar
        CustomTabBar *customTabBar = new CustomTabBar(this);
        setTabBar(customTabBar); // 在子类中可以访问 protected 方法
    }
};
#endif // CUSTOMTABWIDGET_H
