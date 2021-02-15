#include "artefactarg.h"
#include "../MastactvaBase/defines.h"


ArtefactArg::ArtefactArg(ArtefactArgModel *parent_ /*= nullptr*/)
    : QObject(parent_)
{
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

int ArtefactArg::effectArtefactArgSetId() const
{
    return m_effectArtefactArgSetId;
}

void ArtefactArg::setEffectArtefactArgSetId(const int &effectArtefactArgSetId_)
{
    m_effectArtefactArgSetId = effectArtefactArgSetId_;

    emit effectArtefactArgSetIdChanged();
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


ArtefactArgModel::ArtefactArgModel(QObject *parent_ /*= nullptr*/)
    :base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectArtefactArgModel::EffectArtefactArgModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    init(this);
    setAutoCreateChildrenModelsImpl(true);
}
