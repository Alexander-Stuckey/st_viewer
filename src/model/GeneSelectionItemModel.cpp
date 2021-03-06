#include "GeneSelectionItemModel.h"
#include <QDebug>
#include <QColor>

static const int COLUMN_NUMBER = 2;

GeneSelectionItemModel::GeneSelectionItemModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

GeneSelectionItemModel::~GeneSelectionItemModel()
{
}

QVariant GeneSelectionItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || m_geneselection.empty()) {
        return QVariant(QVariant::Invalid);
    }

    const auto item = m_geneselection.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case Name:
            return item.first;
        case Count:
            return item.second;
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    if (role == Qt::ForegroundRole && index.column() == Name) {
        return QColor(0, 155, 60);
    }

    if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
        case Count:
            return Qt::AlignRight;
        case Name:
            return Qt::AlignLeft;
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    return QVariant(QVariant::Invalid);
}

QVariant GeneSelectionItemModel::headerData(int section,
                                            Qt::Orientation orientation,
                                            int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case Name:
            return tr("Gene");
        case Count:
            return tr("Total count");
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    if (orientation == Qt::Horizontal && role == Qt::ToolTipRole) {
        switch (section) {
        case Name:
            return tr("The name of the gene");
        case Count:
            return tr("The aggregated number of reads");
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    if (role == Qt::TextAlignmentRole) {
        switch (section) {
        case Count:
            return Qt::AlignLeft;
        case Name:
            return Qt::AlignLeft;
        default:
            return QVariant(QVariant::Invalid);
        }
    }

    // return invalid value
    return QVariant(QVariant::Invalid);
}

Qt::ItemFlags GeneSelectionItemModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);
    return defaultFlags;
}

bool GeneSelectionItemModel::geneName(const QModelIndex &index, QString *genename) const
{
    if (!index.isValid() || m_geneselection.empty()) {
        return false;
    }

    const auto item = m_geneselection.at(index.row());

    if (index.column() == Name) {
        // the name is the first in the pair
        *genename = item.first;
        return true;
    }

    return false;
}

int GeneSelectionItemModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_geneselection.size();
}

int GeneSelectionItemModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : COLUMN_NUMBER;
}

void GeneSelectionItemModel::loadSelectedGenes(
    const UserSelection::geneTotalCountsVector &geneSelectionList)
{
    beginResetModel();
    m_geneselection = geneSelectionList;
    endResetModel();
}

void GeneSelectionItemModel::clear()
{
    beginResetModel();
    m_geneselection.clear();
    endResetModel();
}
