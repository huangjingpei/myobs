#ifndef GBSBIZSETTINGLIVEEDITOR_H
#define GBSBIZSETTINGLIVEEDITOR_H

#include <QWidget>

namespace Ui {
class GBSBizSettingLiveEditor;
}

class GBSBizSettingLiveEditor : public QWidget
{
    Q_OBJECT

public:
    explicit GBSBizSettingLiveEditor(QWidget *parent = nullptr);
    ~GBSBizSettingLiveEditor();

private:
    Ui::GBSBizSettingLiveEditor *ui;
};

#endif // GBSBIZSETTINGLIVEEDITOR_H
