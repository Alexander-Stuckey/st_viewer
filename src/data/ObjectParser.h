#ifndef OBJECTPARSER_H
#define OBJECTPARSER_H

#include <QObject>
#include <QVariantMap>

#include <QDebug>

// The object parser is used to initialize or construct QObjects using their
// meta data properties as targets and a variant data structure as the source.
// The variant source, which can be either a QVariant or a QVariantMap, is
// mapped to the properties of the QObject trying to convert the variant data
// to the data type of the mapped object member variable.
// Currently the parsing only supports flat structures.

namespace data
{

// parse an object from variant map using intermediary DTO type
// the attributes name/type must match in the QVariant and the DTO
bool parseObject(const QVariant &source, QObject *target);
}

#endif // OBJECTPARSER_H //
