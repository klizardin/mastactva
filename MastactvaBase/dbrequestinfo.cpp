#include "dbrequestinfo.h"
#include "../MastactvaBase/utils.h"


//#define TRACE_DB_CREATION
//#define TRACE_DB_DATA_BINDINGS


QString DBRequestInfo::JsonFieldInfo::getSqlType() const
{
    if(idField) { return QString(g_sqlInt); }

    switch (type) {
    case layout::JsonTypesEn::jt_bool:
        return QString(g_sqlInt);
        break;
    case layout::JsonTypesEn::jt_int:
        return QString(g_sqlInt);
        break;
    case layout::JsonTypesEn::jt_double:
        return QString(g_sqlDouble);
        break;
    case layout::JsonTypesEn::jt_string:
    case layout::JsonTypesEn::jt_array:
    case layout::JsonTypesEn::jt_object:
    case layout::JsonTypesEn::jt_null:
    case layout::JsonTypesEn::jt_undefined:
        return QString(g_sqlText);
        break;
    }
    return QString(g_sqlText);
}

QString DBRequestInfo::JsonFieldInfo::toBindName(const QString &sqlName_)
{
    return QString(":") + sqlName_;
}

QString DBRequestInfo::JsonFieldInfo::getBindName() const
{
    return toBindName(sqlName);
}

void DBRequestInfo::JsonFieldInfo::bind(QSqlQuery &query_, const QJsonValue &jv_) const
{
    if(jv_.isBool())
    {
        const int v = jv_.toBool() ? 1 : 0;
        if(layout::JsonTypesEn::jt_bool == type)
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << v;
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(v));
        }
        else if(layout::JsonTypesEn::jt_int == type)
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << int(v);
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(v));
        }
        else if(layout::JsonTypesEn::jt_double == type)
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << double(v);
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(double(v)));
        }
        else
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << QString::number(v?1:0);
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(QString::number(v?1:0)));
        }
    }
    else if(jv_.isDouble())
    {
        const double v = jv_.toDouble();
        if(idField)
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << int(v);
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(int(v)));
        }
        else if(layout::JsonTypesEn::jt_int == type)
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << int(v);
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(int(v)));
        }
        else if(layout::JsonTypesEn::jt_double == type)
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << v;
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(v));
        }
        else
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << QString::number(v);
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(QString::number(v)));
        }
    }
    else if(jv_.isString())
    {
        if(idField || layout::JsonTypesEn::jt_int == type)
        {
            const int v = QVariant::fromValue(jv_.toString()).toInt();
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << v;
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(v));
        }
        else if(layout::JsonTypesEn::jt_double == type)
        {
            const double v = QVariant::fromValue(jv_.toString()).toDouble();
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << v;
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(v));
        }
        else
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << jv_.toString();
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(jv_.toString()));
        }
    }
    else if(jv_.isNull())
    {
        if(idField || layout::JsonTypesEn::jt_int == type)
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << (int)0;
#endif
            query_.bindValue(getBindName(), QVariant::fromValue((int)0));
        }
        else if(layout::JsonTypesEn::jt_double == type)
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << double(0);
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(double(0)));
        }
        else
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << QString();
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(QString()));
        }
    }
    else
    {
        if(idField || layout::JsonTypesEn::jt_int == type)
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << (int)0;
#endif
            query_.bindValue(getBindName(), QVariant::fromValue((int)0));
        }
        else if(layout::JsonTypesEn::jt_double == type)
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << double(0);
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(double(0)));
        }
        else
        {
#if defined(TRACE_DB_DATA_BINDINGS)
            qDebug() << "bind" << getBindName() << QString();
#endif
            query_.bindValue(getBindName(), QVariant::fromValue(QString()));
        }
    }
}

QJsonValue DBRequestInfo::JsonFieldInfo::jsonValue(const QVariant &val_) const
{
    switch (type) {
    case layout::JsonTypesEn::jt_bool:
        return QJsonValue(val_.toInt() != 0);
        break;
    case layout::JsonTypesEn::jt_int:
        return QJsonValue(val_.toInt());
        break;
    case layout::JsonTypesEn::jt_double:
        if(idField) { return QJsonValue(val_.toInt()); }
        else { return QJsonValue(val_.toDouble()); }
        break;
    case layout::JsonTypesEn::jt_string:
        return QJsonValue(val_.toString());
        break;
    case layout::JsonTypesEn::jt_array:
    case layout::JsonTypesEn::jt_object:
    case layout::JsonTypesEn::jt_null:
    case layout::JsonTypesEn::jt_undefined:
        return QJsonValue(val_.toString());
        break;
    }
    return QJsonValue(val_.toString());
}

QString DBRequestInfo::JsonFieldInfo::toString(const QJsonValue &jv_)
{
    if(jv_.isBool())
    {
        bool v = jv_.toBool();
        return QString::number(v?1:0);
    }
    else if(jv_.isDouble())
    {
        double v = jv_.toDouble();
        return QString::number(v);
    }
    else if(jv_.isString())
    {
        return jv_.toString();
    }
    else if(jv_.isNull())
    {
        return QString();
    }
    else
    {
        return QString();
    }
}

int DBRequestInfo::JsonFieldInfo::toInt(const QJsonValue &jv_)
{
    if(jv_.isBool())
    {
        bool v = jv_.toBool();
        return v?1:0;
    }
    else if(jv_.isDouble())
    {
        double v = jv_.toDouble();
        return (int)v;
    }
    else if(jv_.isString())
    {
        return QVariant::fromValue(jv_.toString()).toInt();
    }
    else if(jv_.isNull())
    {
        return 0;
    }
    else
    {
        return 0;
    }
}


const QString &DBRequestInfo::getTableName() const
{
    return m_tableName;
}

void DBRequestInfo::setTableName(const QString &tableName_)
{
    m_tableName = tableName_;
}

const QList<DBRequestInfo::JsonFieldInfo> &DBRequestInfo::getTableFieldsInfo() const
{
    return m_tableFieldsInfo;
}

void DBRequestInfo::setTableFieldsInfo(const QList<DBRequestInfo::JsonFieldInfo> &jsonFieldInfo_)
{
    m_tableFieldsInfo = jsonFieldInfo_;
}

const QStringList &DBRequestInfo::getRefs() const
{
    return m_refs;
}

void DBRequestInfo::setRefs(const QStringList &refs_)
{
    m_refs = refs_;
}

const QString &DBRequestInfo::getCurrentRef() const
{
    return m_currentRef;
}

void DBRequestInfo::setCurrentRef(const QString &currentRef_)
{
    m_currentRef = currentRef_;
}

const QVariant &DBRequestInfo::getIdField() const
{
    return m_idField;
}

void DBRequestInfo::setIdField(const QVariant &idField_)
{
    m_idField = idField_;
}

bool DBRequestInfo::getReadonly() const
{
    return m_readonly;
}

void DBRequestInfo::setReadonly(bool readonly_)
{
    m_readonly = readonly_;
}

const QHash<QString, QVariant> &DBRequestInfo::getExtraFields() const
{
    return m_extraFields;
}

void DBRequestInfo::setExtraFields(const QHash<QString, QVariant> &extraFields_)
{
    m_extraFields = extraFields_;
}

QString DBRequestInfo::namingConversion(const QString &name_)
{
    QString res = name_;
    res.replace("-", "_");
    return res;
}

QStringList DBRequestInfo::getSqlNames(const QList<JsonFieldInfo> &tableFieldsInfo_)
{
    QStringList res;
    for(const JsonFieldInfo &fi : qAsConst(tableFieldsInfo_))
    {
        res.push_back(fi.sqlName);
    }
    return res;
}

QStringList DBRequestInfo::getSqlBindNames(const QList<JsonFieldInfo> &tableFieldsInfo_)
{
    QStringList res;
    for(const JsonFieldInfo &fi : qAsConst(tableFieldsInfo_))
    {
        res.push_back(fi.getBindName());
    }
    return res;
}
