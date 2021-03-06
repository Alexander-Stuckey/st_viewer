#ifndef DATASETITEMMODEL_H
#define DATASETITEMMODEL_H

#include <QAbstractItemModel>
#include "data/DataProxy.h"

class QModelIndex;
class QStandardItemModel;
class QItemSelection;

// Wrapper model class for datasets in the Datasets table. Provides easy
// means of enumerating all the data sets connected to a single user.
// TODO highlight openned dataset
class DatasetItemModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_ENUMS(Column)

public:
    enum Column {
        Name = 0,
        Tissue = 1,
        Species = 2,
        Created = 3,
        LastModified = 4
    };

    explicit DatasetItemModel(QObject *parent = 0);
    virtual ~DatasetItemModel();

    // Necessary to override
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // Updates the data model with the data given as input
    void loadDatasets(const DataProxy::DatasetList &datasetList);

    // Returns a list of datasets for the given items selection
    DataProxy::DatasetList getDatasets(const QItemSelection &selection);

    // Clear the current model
    void clear();

private:
    DataProxy::DatasetList m_datasets_reference;

    Q_DISABLE_COPY(DatasetItemModel)
};

#endif // DATASETITEMMODEL_H
