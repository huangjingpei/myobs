#include "LiveMngrModel.h"
#include <QBrush>
#include <QTextDocument>

LiveMngrModel::LiveMngrModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    // 初始化数据 (可以根据实际情况修改)
	m_headers << "序号" << "客户号" << "激活秘钥" << "备注" << "开通时间" << "剩余时长" << "直播账号"
		  << "直播数量" << "远程信息" << "操作";

    //QVector<QVariant> row1;
    //row1 << "0001" << "54564568556" << "KS/CZQ/0101" << "55656565656"
    //     << "2022-12-12 16:15:00" << "2022-12-12 16:15:00" << "456555533"
    //     << "英语" << "妹妹" << "1" ;

    //for(int i = 0; i < 25;i++)
    //m_data.append(row1);
}

int LiveMngrModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data.count();
}

int LiveMngrModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_headers.count();
}

QVariant LiveMngrModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    int row = index.row();
    int col = index.column();

    if (row < 0 || row >= m_data.size() || col < 0 || col >= m_headers.size()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        return m_data[row][col];
    } else if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter; // 单元格内容居中对齐
    } else if (role == Qt::BackgroundRole) {
        return backgroundColor(row, col); // 设置背景颜色
    }
    return QVariant();
}

QVariant LiveMngrModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        return m_headers[section];
    }
    return QVariant();
}

QColor LiveMngrModel::backgroundColor(int row, int column) const
{
    return QColor("#f2f5ff"); // 默认浅蓝色
}

QVector<QVariant> LiveMngrModel::getRowData(int row) const
{
    if (row < 0 || row >= m_data.size()) {
        return QVector<QVariant>(); // 返回空向量
    }
    return m_data[row];
}

// 设置单元格数据
bool LiveMngrModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        int row = index.row();
        int col = index.column();

        m_data[row][col] = value;
        emit dataChanged(index, index, {Qt::DisplayRole});
        return true;
    }
    return false;
}

//设置header
bool LiveMngrModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role){
    if (role == Qt::EditRole && orientation == Qt::Horizontal) {
        if(section < 0 || section >= m_headers.size()){
            return false;
        }
        m_headers[section] = value.toString();
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}


//设置编辑标记
Qt::ItemFlags LiveMngrModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags flag = QAbstractTableModel::flags(index);
    if (index.column() == RemoteAuthInfo || index.column() == LiveOperation) {
        flag |= Qt::ItemIsEditable;
    }
    return flag;
}

void LiveMngrModel::insertRows(int row, int count, const QModelIndex &parent, const QStringList &rawData)
{
	beginInsertRows(parent, row, row + count - 1);
	if (rawData.size() != m_headers.size()) {
		qDebug() << "数据长度错误";
	}
	for (int i = 0; i < count; ++i) {
		QVector<QVariant> newRow;

		for (int j = 0; j < m_headers.size(); ++j) {
			if (rawData.size() >= m_headers.size())
				newRow.append(rawData[j]);
		}

		m_data.insert(row, newRow);
	}
	endInsertRows();
}

bool LiveMngrModel::removeRow(int row, const QModelIndex &parent)
{
	if (row < 0 || row >= rowCount(parent)) {
		return false;
	}

	beginRemoveRows(parent, row, row);
	// 假设数据存储在 m_data 中（QList 或 QVector）
	m_data.removeAt(row); // 移除对应行数据
	endRemoveRows();

	return true;
}


void LiveMngrModel::clearRows() {
	// 检查是否有数据需要清空
	if (m_data.isEmpty()) {
		qDebug() << "操作：无行可清空";
		return;
	}

	// 通知视图所有数据将重置
	beginResetModel();
	m_data.clear(); // 清空数据容器
	endResetModel();

	qDebug() << "操作：成功清空所有行";
}
