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

#include "effectarg.h"
#include "../MastactvaBase/defines.h"


EffectArg::EffectArg(EffectArgModel *parent_ /*= nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectArg::EffectArg()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "EffectArg::EffectArg()" << this;
#endif

    m_effectArgModel = parent_;
}

EffectArg::EffectArg(EffectArgData &&data_,EffectArgModel *parent_ /*= nullptr*/)
    : QObject(parent_),
      EffectArgData(std::move(data_))
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectArg::EffectArg()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "EffectArg::EffectArg()" << this;
#endif

    m_effectArgModel = parent_;
}

EffectArg::~EffectArg()
{
    if(m_objectArtefactModel)
    {
        m_objectArtefactModel->deleteLater();
    }
    m_objectArtefactModel = nullptr;
}

int EffectArg::id() const
{
    return m_id;
}

void EffectArg::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int EffectArg::effectId() const
{
    return m_effectId;
}

void EffectArg::setEffectId(const int &effectId_)
{
    m_effectId = effectId_;

    emit effectIdChanged();
}

int EffectArg::objectArtefactId() const
{
    return m_objectArtefactId;
}

void EffectArg::setObjectArtefactId(const int &objectArtefactId_)
{
    m_objectArtefactId = objectArtefactId_;

    emit objectArtefactIdChanged();
}

int EffectArg::argTypeId() const
{
    return m_argTypeId;
}

void EffectArg::setArgTypeId(const int &argTypeId_)
{
    if(argTypeId_ == m_argTypeId) { return; }
    m_argTypeId = argTypeId_;
    emit argTypeChanged();
}

int EffectArg::argStorageId() const
{
    return m_argStorageId;
}

void EffectArg::setArgStorageId(const int &argStorageId_)
{
    m_argStorageId = argStorageId_;

    emit argStorageChanged();
}

QString EffectArg::name() const
{
    return m_name;
}

void EffectArg::setName(const QString &name_)
{
    m_name = name_;

    emit nameChanged();
}

QString EffectArg::defaultValue() const
{
    return m_defaultValue;
}

void EffectArg::setDefaultValue(const QString &defaultValue_)
{
    m_defaultValue = defaultValue_;

    emit defaultValueChanged();
}

QString EffectArg::description() const
{
    return m_description;
}

void EffectArg::setDescription(const QString &description_)
{
    m_description = description_;

    emit descriptionChanged();
}

QDateTime EffectArg::created() const
{
    return m_created;
}

void EffectArg::setCreated(const QDateTime &created_)
{
    m_created = created_;

    emit createdChanged();
}

QString EffectArg::mergeid() const
{
    return m_mergeid;
}

void EffectArg::setMergeid(const QString &mergeid_)
{
    m_mergeid = mergeid_;

    emit mergeidChanged();
}

QVariant EffectArg::objectArtefact() const
{
    if(!m_objectArtefactModel)
    {
        const_cast<EffectArg *>(this)->m_objectArtefactModel = const_cast<EffectArg *>(this)
                ->createObjectArtefactModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(
                                   const_cast<ObjectArtefactModel *>(
                                       m_objectArtefactModel)
                                   )
                               );
}

void EffectArg::setObjectArtefact(const QVariant &obj_)
{
    if(obj_.isNull() && m_objectArtefactModel)
    {
        delete m_objectArtefactModel;
        m_objectArtefactModel = nullptr;

        emit objectArtefactChanged();
    }
}

void EffectArg::copyFrom(const ArtefactArg *artefactArg_, int effectId, int objectArtefactId_)
{
    m_effectId = effectId;
    m_objectArtefactId = objectArtefactId_;
    m_argTypeId = artefactArg_->argTypeId();
    m_argStorageId = artefactArg_->argStorageId();
    m_name = artefactArg_->name();
    m_defaultValue = artefactArg_->defaultValue();
    m_description = artefactArg_->description();
    m_created = QDateTime::currentDateTime();
}

ObjectArtefactModel *EffectArg::createObjectArtefactModel()
{
    ObjectArtefactModel *m = new ObjectArtefactModel(this);
    m->initResponse();
    m->setLayoutRefImpl("id", m_effectArgModel->getQMLLayoutName(), "object_artefact", false);
    m->setCurrentRef("id");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_effectArgModel->getQMLLayoutName() + QString("_EffectArg_") +
                        QVariant::fromValue(m_appId).toString() + QString("_ObjectArtefactModel_"));
    m->registerListModel();
    m->setParentListModelInfo(m_parentModelInfo);
    m->setAutoCreateChildrenModels(true);
    m->setReadonlyImpl(true);
    m->loadList();
    return m;
}


EffectArgModel::EffectArgModel(
        QObject *parent_ /*= nullptr*/,
        std::shared_ptr<QVector<EffectArgData *>> data_ /*= std::shared_ptr<QVector<EffectArgData *>>{nullptr}*/
        )
    : base(parent_, data_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectArgModel::EffectArgModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "EffectArgModel::EffectArgModel()" << this;
#endif

    init(this);
}
