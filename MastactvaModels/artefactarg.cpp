/*
    Copyright 2021

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "artefactarg.h"
#include "../MastactvaModels/artefactargtype.h"
#include "../MastactvaModels/artefactargstorage.h"
#include "../MastactvaBase/defines.h"


ArtefactArg::ArtefactArg(ArtefactArgModel *parent_ /*= nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ArtefactArg::ArtefactArg()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "ArtefactArg::ArtefactArg()" << this;
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
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "ArtefactArg::ArtefactArg()" << this;
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

void ArtefactArg::updateArtefactId(int effectArtefactId_)
{
    setArtefactId(effectArtefactId_);
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
    emit filenameChanged();
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

MergeId ArtefactArg::mergeid() const
{
    return m_mergeid;
}

void ArtefactArg::setMergeid(const MergeId &mergeid_)
{
    m_mergeid = mergeid_;

    emit mergeidChanged();
}

bool ArtefactArg::hasFileReference() const
{
    return m_hasFileReference;
}

void ArtefactArg::setHasFileReference(const bool &value_)
{
    m_hasFileReference = value_;

    emit hasFileReferenceChanged();
    emit filenameChanged();
}

bool ArtefactArg::hasFile() const
{
    return m_hasFile;
}

void ArtefactArg::setHasFile(const bool &value_)
{
    m_hasFile = value_;

    emit hasFileChanged();
    emit filenameChanged();
}

FileSource ArtefactArg::getFilename() const
{
    return m_file;
}

void ArtefactArg::setFilename(const QString &filename_)
{
    m_file = filename_;

    emit filenameChanged();
}

void ArtefactArg::setFilename(const FileSource &filename_)
{
    m_file = filename_;

    emit filenameChanged();
}

QString ArtefactArg::hash() const
{
    return m_fileHash;
}

void ArtefactArg::setHash(const QString &hash_)
{
    m_fileHash = hash_;

    emit hashChanged();
}

bool ArtefactArg::createFrom(const int &effectArtefactId_, const Comment &comment_)
{
    if(!comment_.values().contains(g_argumentName))
    {
        return false;
    }
    setArtefactId(effectArtefactId_);
    setName(comment_.values().value(g_nameName, QString()));
    setDefaultValue(comment_.values().value(g_defaultValueName, QString()));
    setDescription(comment_.values().value(g_descriptionName, QString()));

    const QString storage = comment_.values().value(g_storageName, QString());
    const QString type = comment_.values().value(g_typeName, QString());

    ArtefactArgTypeModel *argTypeModel = static_cast<ArtefactArgTypeModel *>(
                QMLObjectsBase::getInstance().getListModel(g_artefactArgTypeModel)
                );
    ArtefactArgType *typeItem = argTypeModel->findDataItemByFieldValueImpl(
                "artefactArgTypeType",
                QVariant::fromValue(type)
                );
    if(!typeItem)
    {
        return false;
    }
    setArgTypeId(typeItem->id());
    ArtefactArgStorageModel *argStorageModel = static_cast<ArtefactArgStorageModel *>(
                QMLObjectsBase::getInstance().getListModel(g_artefactArgStorageModel)
                );
    ArtefactArgStorage *storageItem = argStorageModel->findDataItemByFieldValueImpl(
                "artefactArgStorageStorage",
                QVariant::fromValue(storage)
                );
    if(!storageItem)
    {
        return false;
    }
    setArgStorageId(storageItem->id());
    return true;
}

void ArtefactArg::copyFrom(const ArtefactArg *arg_)
{
    if(!arg_
            || name() != arg_->name()
            )
    {
        return;
    }
    setDefaultValue(arg_->defaultValue());
    setDescription(arg_->description());
    setCreated(arg_->created());
}


ArtefactArgModel::ArtefactArgModel(
        QObject *parent_ /*= nullptr*/,
        std::shared_ptr<QVector<ArtefactArgData *>> data_ /*= std::shared_ptr<QVector<ArtefactArgData *>>{nullptr}*/
        )
    :base(parent_, data_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ArtefactArgModel::ArtefactArgModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "ArtefactArgModel::ArtefactArgModel()" << this;
#endif

    init(this);
    setAutoCreateChildrenModelsImpl(true);
}
