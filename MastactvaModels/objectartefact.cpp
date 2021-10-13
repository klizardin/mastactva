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

#include "objectartefact.h"
#include "../MastactvaBase/defines.h"


ObjectArtefact::ObjectArtefact(ObjectArtefactModel *parent_ /* = nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ObjectArtefact::ObjectArtefact()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "ObjectArtefact::ObjectArtefact()" << this;
#endif

    m_effectArtefactModel = parent_;
}

ObjectArtefact::ObjectArtefact(ObjectArtefactData &&data_, ObjectArtefactModel *parent_ /*= nullptr*/)
    : QObject(parent_)
    , ObjectArtefactData(std::move(data_))
{
#if defined(TRACE_THREADS)
    qDebug() << "ObjectArtefact::ObjectArtefact()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "ObjectArtefact::ObjectArtefact()" << this;
#endif

    m_effectArtefactModel = parent_;
}

ObjectArtefact::~ObjectArtefact()
{
    if(m_artefactModel)
    {
        m_artefactModel->deleteLater();
    }
    m_artefactModel = nullptr;
}

int ObjectArtefact::id() const
{
    return m_id;
}

void ObjectArtefact::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int ObjectArtefact::objectInfoId() const
{
    return m_objectInfoId;
}

void ObjectArtefact::setObjectInfoId(const int &objectInfoId_)
{
    m_objectInfoId = objectInfoId_;

    emit objectInfoIdChanged();
}

void ObjectArtefact::updateObjectInfoId(int objectInfoId_)
{
    setObjectInfoId(objectInfoId_);
}

int ObjectArtefact::artefactId() const
{
    return m_artefactId;
}

void ObjectArtefact::setArtefactId(const int &artefactId_)
{
    m_artefactId = artefactId_;
    if(m_artefactModel)
    {
        m_artefactModel->parentItemChanged();
    }

    emit artefactChanged();
}

QVariant ObjectArtefact::artefact() const
{
    if(!m_artefactModel)
    {
        const_cast<ObjectArtefact *>(this)->m_artefactModel = const_cast<ObjectArtefact *>(this)
                ->createArtefactModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(
                                   const_cast<ArtefactModel *>(
                                       m_artefactModel)
                                   )
                               );
}

void ObjectArtefact::setArtefact(const QVariant &obj_)
{
    if(obj_.isNull() && m_artefactModel)
    {
        delete m_artefactModel;
        m_artefactModel = nullptr;

        emit artefactChanged();
    }
}

ArtefactModel *ObjectArtefact::getArtefact()
{
    return m_artefactModel;
}

const ArtefactModel *ObjectArtefact::getArtefact() const
{
    return m_artefactModel;
}

int ObjectArtefact::stepIndex() const
{
    return m_stepIndex;
}

void ObjectArtefact::setStepIndex(const int &stepIndex_)
{
    m_stepIndex = stepIndex_;

    emit stepIndexChanged();
}

QString ObjectArtefact::mergeid() const
{
    return m_mergeid;
}

void ObjectArtefact::setMergeid(const QString &mergeid_)
{
    m_mergeid = mergeid_;

    emit mergeidChanged();
}

bool ObjectArtefact::updateArtefactId(const QVariant &artefact_)
{
    if(!m_artefactModel
            || 0 == m_artefactModel->sizeImpl()
            )
    {
        return false;
    }
    QObject *obj = qvariant_cast<QObject *>(artefact_);
    const Artefact *artefact = qobject_cast<Artefact *>(obj);
    if(!artefact)
    {
        return false;
    }
    const int index = m_artefactModel->indexOfDataItemImpl(artefact);
    if(index < 0)
    {
        return false;
    }
    m_artefactModel->setCurrentIndexImpl(index);
    m_artefactId = m_artefactModel->getCurrentDataItem()->id();
    return true;
}

ArtefactModel *ObjectArtefact::createArtefactModel()
{
    ArtefactModel *m = new ArtefactModel(this, m_artefactData);
    m->initResponse();
    m->setLayoutRefImpl("id", m_effectArtefactModel->getQMLLayoutName(), "artefact");
    m->setCurrentRef("id");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_effectArtefactModel->getQMLLayoutName() + QString("_EffectArtefact_") +
                        QVariant::fromValue(m_appId).toString() + QString("_ArtefactModel_"));
    m->registerListModel();
    m->setParentListModelInfo(m_parentModelInfo);
    m->setAutoCreateChildrenModels(true);
    m->loadList();
    return m;
}


ObjectArtefactModel::ObjectArtefactModel(
        QObject *parent_ /*= nullptr*/,
        std::shared_ptr<QVector<ObjectArtefactData *>> data_
        /*= std::shared_ptr<QVector<ObjectArtefactData *>>{nullptr}*/,
        IModelConfig *config_ /*= &DefaultModelConfig::instance()*/
        )
    : base(parent_, data_, config_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ObjectArtefactModel::ObjectArtefactModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "ObjectArtefactModel::ObjectArtefactModel()" << this;
#endif

    init(this);
}
