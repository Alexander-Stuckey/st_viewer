/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include "ExperimentsItemModel.h"

#include <QDebug>
#include <QItemSelection>
#include <QDateTime>

#include <set>

#include "dataModel/GeneSelection.h"

static const int COLUMN_NUMBER = 6;

ExperimentsItemModel::ExperimentsItemModel(QObject* parent)
    : QAbstractTableModel(parent)
{

}

ExperimentsItemModel::~ExperimentsItemModel()
{

}

QVariant ExperimentsItemModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || m_geneselectionList.empty()) {
        return QVariant(QVariant::Invalid);
    }

    if (role == Qt::DisplayRole) {
        const DataProxy::GeneSelectionPtr item = m_geneselectionList.at(index.row());
        Q_ASSERT(!item.isNull());
        switch (index.column()) {
        case Name: return item->name();
        case Dataset: return item->datasetName();
        case Comment: return item->comment();
        case NGenes: return QString::number(item->selectedItems().size());
        case Created:
            return QDateTime::fromMSecsSinceEpoch(item->created().toLongLong());
        case LastModified:
            return QDateTime::fromMSecsSinceEpoch(item->lastModified().toLongLong());
        default: return QVariant(QVariant::Invalid);
        }
    }

    if (role == Qt::ForegroundRole && index.column() == Name) {
        return QColor(0, 155, 60);
    }

    if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
        case NGenes:
        case Created:
        case LastModified: return Qt::AlignRight;
        default: return QVariant(QVariant::Invalid);
        }
    }

    return QVariant(QVariant::Invalid);
}

QVariant ExperimentsItemModel::headerData(int section,
                                            Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case Name: return tr("Name");
        case Dataset: return tr("Dataset");
        case Comment: return tr("Comment");
        case NGenes: return tr("Number Genes");
        case Created: return tr("Created");
        case LastModified: return tr("Last Modified");
        default: return QVariant(QVariant::Invalid);
        }
    }

    if (orientation == Qt::Horizontal && role == Qt::ToolTipRole) {
        switch (section) {
        case Name: return tr("The name of the selection");
        case Dataset: return tr("The dataset name where the selection was made");
        case Comment: return tr("The comments made on the selection");
        case NGenes: return tr("The number of unique genes present in the selection");
        case Created: return tr("Created at this date");
        case LastModified: return tr("Last Modified at this date");
        default: return QVariant(QVariant::Invalid);
        }
    }

    if (role == Qt::TextAlignmentRole) {
        switch (section) {
        case Name:
        case Dataset:
        case Comment:
        case NGenes:
        case Created:
        case LastModified: return Qt::AlignLeft;
        default: return QVariant(QVariant::Invalid);
        }
    }

    // return invalid value
    return QVariant(QVariant::Invalid);
}

Qt::ItemFlags ExperimentsItemModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);
    return defaultFlags;
}

void ExperimentsItemModel::sort(int column, Qt::SortOrder order)
{
    QAbstractItemModel::sort(column, order);
}

int ExperimentsItemModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_geneselectionList.count();
}

int ExperimentsItemModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : COLUMN_NUMBER;
}

void ExperimentsItemModel::reset()
{
    beginResetModel();
    m_geneselectionList.clear();
    endResetModel();
}

void ExperimentsItemModel::loadSelectedGenes(const DataProxy::GeneSelectionList selectionList)
{
    beginResetModel();
    m_geneselectionList.clear();
    m_geneselectionList = selectionList;
    endResetModel();
}

DataProxy::GeneSelectionList
ExperimentsItemModel::getSelections(const QItemSelection &selection)
{
    std::set<int> rows;
    for (const auto &index : selection.indexes()) {
        rows.insert(index.row());
    }

    DataProxy::GeneSelectionList selectionList;
    for (const auto &row : rows) {
        auto selection = m_geneselectionList.at(row);
        selectionList.push_back(selection);
    }

    return selectionList;
}
