#include "artefactarg.h"
#include "../MastactvaBase/defines.h"


ArtefactArg::ArtefactArg(ArtefactArgModel *parent_ /*= nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ArtefactArg::ArtefactArg()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    m_artefactArgModel = parent_;
}

ArtefactArg::ArtefactArg(ArtefactArgData &&data_, ArtefactArgModel *parent_ /*= nullptr*/)
    : QObject(parent_)
    , ArtefactArgData(std::move(data_))
{
#if defined(TRACE_THREADS)
    qDebug() << "ArtefactArg::ArtefactArg()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    m_artefactArgModel = parent_;
}

int ArtefactArg::id() const
{
    return m_id;
}

void ArtefactArg::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int ArtefactArg::artefactId() const
{
    return m_artefactId;
}

void ArtefactArg::setArtefactId(const int &effectArtefactArgSetId_)
{
    m_artefactId = effectArtefactArgSetId_;

    emit artefactIdChanged();
}

int ArtefactArg::argTypeId() const
{
    return m_argTypeId;
}

void ArtefactArg::setArgTypeId(const int &argTypeId_)
{
    m_argTypeId = argTypeId_;

    emit argTypeChanged();
}

int ArtefactArg::argStorageId() const
{
    return m_argStorageId;
}

void ArtefactArg::setArgStorageId(const int &argStorageId_)
{
    m_argStorageId = argStorageId_;

    emit argStorageChanged();
}

QString ArtefactArg::name() const
{
    return m_name;
}

void ArtefactArg::setName(const QString &name_)
{
    m_name = name_;

    emit nameChanged();
}

QString ArtefactArg::defaultValue() const
{
    return m_defaultValue;
}

void ArtefactArg::setDefaultValue(const QString &defaultValue_)
{
    m_defaultValue = defaultValue_;

    emit defaultValueChanged();
}

QString ArtefactArg::description() const
{
    return m_description;
}

void ArtefactArg::setDescription(const QString &description_)
{
    m_description = description_;

    emit descriptionChanged();
}

QDateTime ArtefactArg::created() const
{
    return m_created;
}

void ArtefactArg::setCreated(const QDateTime &created_)
{
    m_created = created_;

    emit createdChanged();
}


ArtefactArgModel::ArtefactArgModel(
        QObject *parent_ /*= nullptr*/,
        std::shared_ptr<QVector<ArtefactArgData *>> data_
            /*= std::shared_ptr<QVector<ArtefactArgData *>>{nullptr}*/
        )
    :base(parent_, data_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ArtefactArgModel::ArtefactArgModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    init(this);
    setAutoCreateChildrenModelsImpl(true);
}
