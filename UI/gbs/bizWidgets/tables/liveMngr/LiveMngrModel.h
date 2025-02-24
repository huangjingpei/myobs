#ifndef LiveMngrModel_H
#define LiveMngrModel_H

#include <QAbstractTableModel>
#include <QVector>
#include <QColor>
#include <QStringList>

class LiveMngrModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum ColumnIndex {
        Index = 0,
	ActviateSN,
        ActivateKey,
	Remarks,
	BizCreateTime,
	BizLeftTime,
	LiveAccount,
	LiveDeviceCount,
	RemoteAuthInfo,
	LiveOperation,
    };

public:
    LiveMngrModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // 可选： 设置单元格背景色
    QColor backgroundColor(int row, int column) const;

    // 添加： 获取一行的数据
    QVector<QVariant> getRowData(int row) const;

    // 设置单元格数据
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    //设置header
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

    //设置编辑标记
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void insertRows(int row, int count, const QModelIndex &parent = QModelIndex(),
		    const QStringList &rawData = QStringList());
    bool removeRow(int row, const QModelIndex &parent);

    void clearRows();

signals:
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());

private:
    QVector<QVector<QVariant>> m_data;
    QStringList m_headers;
    QMap<int, QStringList> m_items;

};

#endif // LiveMngrModel_H
