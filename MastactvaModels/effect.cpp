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

#include "effect.h"
#include <QTextCodec>
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/serverfiles.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaModels/effectarg.h"
#include "../MastactvaModels/artefact.h"
#include "../MastactvaModels/artefactargstorage.h"
#include "../MastactvaBase/defines.h"


Effect::Effect(EffectModel *parent_)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "Effect::Effect()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "Effect::Effect()" << this;
#endif

    m_effectModel = parent_;
    m_objectModelInfo = this;
}

Effect::Effect(EffectData &&data_, EffectModel *parent_ /*= nullptr*/)
    : QObject(parent_)
    , EffectData(std::move(data_))
{
#if defined(TRACE_THREADS)
    qDebug() << "Effect::Effect()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "Effect::Effect()" << this;
#endif

    m_effectModel = parent_;
    m_objectModelInfo = this;
}

Effect::~Effect()
{
    clear(m_artefactArgs);
    clear(m_itemsToSet);
    clear(m_itemsToDel);
    clear(m_itemsToAdd);
    m_artefactInfos.clear();

    if(m_effectObjectsModel)
    {
        m_effectObjectsModel->deleteLater();
    }
    m_effectObjectsModel = nullptr;
    if(m_effectArgModel)
    {
        m_effectArgModel->deleteLater();
    }
    m_effectArgModel = nullptr;
    if(m_effectArgSetModel)
    {
        m_effectArgSetModel->deleteLater();
    }
    m_effectArgSetModel = nullptr;
}

int Effect::id() const
{
    return m_id;
}

void Effect::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

QString Effect::name() const
{
    return m_name;
}

void Effect::setName(const QString &name_)
{
    m_name = name_;

    emit nameChanged();
}

QString Effect::description() const
{
    return m_description;
}

void Effect::setDescription(const QString &description_)
{
    m_description = description_;

    emit descriptionChanged();
}

QDateTime Effect::created() const
{
    return m_created;
}

void Effect::setCreated(const QDateTime &created_)
{
    m_created = created_;

    emit createdChanged();
}

QVariant Effect::effectObjects() const
{
    if(!m_effectObjectsModel)
    {
        const_cast<Effect *>(this)
                ->m_effectObjectsModel =
                const_cast<Effect *>(this)
                ->createEffectObjectsModel();
    }
    return QVariant::fromValue(
                static_cast<QObject *>(
                    const_cast<EffectObjectsModel *>(
                        m_effectObjectsModel
                        )
                    )
                );
}

void Effect::setEffectObjectss(const QVariant &obj_)
{
    if(obj_.isNull() && m_effectObjectsModel)
    {
        delete m_effectObjectsModel;
        m_effectObjectsModel = nullptr;

        emit effectObjectsChanged();
    }
}

QVariant Effect::args() const
{
    if(!m_effectArgModel)
    {
        const_cast<Effect *>(this)->m_effectArgModel = const_cast<Effect *>(this)->createEffectArgModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(const_cast<EffectArgModel *>(m_effectArgModel)));
}

void Effect::setArgs(const QVariant &obj_)
{
    if(obj_.isNull() && m_effectArgModel)
    {
        delete m_effectArgModel;
        m_effectArgModel = nullptr;

        emit argsChanged();
    }
}

QVariant Effect::argSets() const
{
    if(!m_effectArgSetModel)
    {
        const_cast<Effect *>(this)->m_effectArgSetModel = const_cast<Effect *>(this)->createEffectArgSetModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(const_cast<EffectArgSetModel *>(m_effectArgSetModel)));
}

void Effect::setArgSets(const QVariant &obj_)
{
    if(obj_.isNull() && m_effectArgSetModel)
    {
        delete m_effectArgSetModel;
        m_effectArgSetModel = nullptr;

        emit argSetsChanged();
    }
}

MergeId Effect::mergeid() const
{
    return m_mergeid;
}

void Effect::setMergeid(const MergeId &mergeid_)
{
    m_mergeid = mergeid_;

    emit mergeidChanged();
}

EffectObjectsModel *Effect::createEffectObjectsModel()
{
    IListModelInfoObjectImpl::setParentModelInfo(m_parentModelInfo);
    IListModelInfoObjectImpl::setObjectName(getObjectName());
    IListModelInfoObjectImpl::trace();
    EffectObjectsModel *m = new EffectObjectsModel(this, m_effectObjectsData);
    m->initResponse();
    m->setLayoutRefImpl("effect", m_effectModel->getQMLLayoutName(), "id");
    m->setCurrentRef("effect");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_effectModel->getQMLLayoutName() + QString("_Effect_") +
                        QVariant::fromValue(m_appId).toString() + QString("_EffectObjectsModel_"));
    m->registerListModel();
    m->setParentListModelInfo(this);
    m->setAutoCreateChildrenModels(true);
    m->loadList();
    return m;
}

EffectArgModel *Effect::createEffectArgModel()
{
    IListModelInfoObjectImpl::setParentModelInfo(m_parentModelInfo);
    IListModelInfoObjectImpl::setObjectName(getObjectName());
    IListModelInfoObjectImpl::trace();
    EffectArgModel *m = new EffectArgModel(this, m_effectArgsData);
    m->initResponse();
    m->setLayoutRefImpl("effect", m_effectModel->getQMLLayoutName(), "id");
    m->setCurrentRef("effect");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_effectModel->getQMLLayoutName() + QString("_Effect_") +
                        QVariant::fromValue(m_appId).toString() + QString("_EffectArgModel_"));
    m->registerListModel();
    m->setParentListModelInfo(this);
    m->setAutoCreateChildrenModels(true);
    m->loadList();
    return m;
}

EffectArgSetModel *Effect::createEffectArgSetModel()
{
    IListModelInfoObjectImpl::setParentModelInfo(m_parentModelInfo);
    IListModelInfoObjectImpl::setObjectName(getObjectName());
    IListModelInfoObjectImpl::trace();
    EffectArgSetModel *m = new EffectArgSetModel(this, m_effectArgSetsData);
    m->initResponse();
    m->setLayoutRefImpl("effect", m_effectModel->getQMLLayoutName(), "id");
    m->setCurrentRef("effect");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_effectModel->getQMLLayoutName() + QString("_Effect_") +
                        QVariant::fromValue(m_appId).toString() + QString("_EffectArgSetModel_"));
    m->setLayoutIdFieldImpl("id");
    m->registerListModel();
    m->setParentListModelInfo(this);
    m->setAutoCreateChildrenModels(true);
    m->loadList();
    return m;
}

void Effect::clear(QList<EffectArg *> &artefactArgs_)
{
    for(EffectArg *&arg_ : artefactArgs_)
    {
        const bool founded = m_effectArgModel->findDataItemByAppIdImpl(
                    getDataLayout<EffectArg>().getSpecialFieldValue(
                        layout::SpecialFieldEn::appId,
                        arg_
                        )
                    );
        if(!founded)
        {
            arg_->deleteLater();
            arg_ = nullptr;
        }
    }
    artefactArgs_.clear();
}

bool Effect::startRefreshArguments()
{
    qDebug() << "Effect::startRefreshArguments()";
    if(!m_effectObjectsModel
            || !m_effectObjectsModel->isListLoadedImpl()
            || !m_effectArgModel
            || !m_effectArgModel->isListLoadedImpl()
            ) { return false; }

    ArtefactTypeModel *artefactTypeModel = static_cast<ArtefactTypeModel *>(
                QMLObjectsBase::getInstance().getListModel(g_artefactTypeModel)
                );
    Q_ASSERT(artefactTypeModel && artefactTypeModel->sizeImpl() > 0);


    if(!m_effectObjectsModel || !m_effectObjectsModel->isListLoadedImpl()) { return false; }

    // get all artefacts urls
    const int objecsCnt = m_effectObjectsModel->size();
    m_artefactInfos.clear();
    clear(m_artefactArgs);

    for(int i = 0; i < objecsCnt; i++)
    {
        EffectObjects *effectObjects = m_effectObjectsModel->dataItemAtImpl(i);
        if(!effectObjects) { return false; }

        ObjectArtefactModel *effectObjectArtefactsModel = effectObjects->getObjectArtefacts();
        if(!effectObjectArtefactsModel ||
                !effectObjectArtefactsModel->isListLoadedImpl()) { return false; }

        for(int j = 0; j < effectObjectArtefactsModel->sizeImpl(); j++)
        {
            ObjectArtefact *effectObjectArtefact = effectObjectArtefactsModel->dataItemAtImpl(j);
            if(!effectObjectArtefact) { return false; }

            ArtefactModel *artefactsModel = effectObjectArtefact->getArtefact();
            if(!artefactsModel ||
                    !artefactsModel->isListLoadedImpl()) { return false; }
            Q_ASSERT(1 == artefactsModel->sizeImpl());

            for(int k = 0; k < artefactsModel->sizeImpl(); k++)
            {
                Artefact *artefact = artefactsModel->dataItemAtImpl(k);
                if(!artefact) { return false; }

                // get default arguments from artefacts
                ArtefactArgModel *artefactArgModel = artefact->getArtefactArg();
                if(!artefactArgModel
                        || !artefactArgModel->isListLoadedImpl()) { return false; }

                for(int m = 0; m < artefactArgModel->sizeImpl(); ++m)
                {
                    ArtefactArg *artefactArg = artefactArgModel->dataItemAtImpl(m);
                    if(!artefactArg) { return false; }

                    EffectArg *newArg = m_effectArgModel->createDataItemImpl();
                    newArg->copyFrom(artefactArg, this->id(), effectObjectArtefact->id());
                }

                ArtefactType *artefactType = artefactTypeModel->findDataItemByIdImpl(artefact->type());
                if(!artefactType) { return false; }

                if(g_artefactTypeVertex == artefactType->type() ||
                        g_artefactTypeFragment == artefactType->type())
                {
                    // for reading arguments info from code of shaders
                    m_artefactInfos.push_back(
                                {
                                    artefact->getFilename(),
                                    artefact->hash(),
                                    effectObjectArtefact->id(),
                                    QString()
                                });
                }

            }
        }
    }
    m_artefactsLocalUrlsCount = 0;

    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
    QObject::connect(sf, SIGNAL(downloaded(const QString &)),
                     this, SLOT(refreshArgumentsArtefactDownloadedSlot(const QString &)));
    QObject::connect(sf, SIGNAL(progress()),
                     this, SLOT(refreshArgumentsProgressSlot()));

    for(const ArtefactInfo &ai_: qAsConst(m_artefactInfos))
    {
        sf->add(ai_.filename, ai_.hash, g_artefactsRelPath, m_effectModel->getNamespace());
    }
    return true;
}

void Effect::cancelRefreshArguments()
{
    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
    QObject::disconnect(sf, SIGNAL(downloaded(const QString &)),
                        this, SLOT(refreshArgumentsArtefactDownloadedSlot(const QString &)));
    QObject::disconnect(sf, SIGNAL(progress()),
                        this, SLOT(refreshArgumentsProgressSlot()));

    QStringList artefactsUrls;
    for(const ArtefactInfo &ai_: qAsConst(m_artefactInfos))
    {
        artefactsUrls.push_back(ai_.filename);
    }
    sf->cancel(artefactsUrls);
    m_artefactInfos.clear();
    m_artefactsLocalUrlsCount = 0;
}

void Effect::refreshArgumentsArtefactDownloadedSlot(const QString &url_)
{
    auto fit = std::find_if(std::begin(m_artefactInfos), std::end(m_artefactInfos),
                                  [&url_](const ArtefactInfo &ai_)->bool
    {
        return ai_.filename == url_;
    });
    if(std::end(m_artefactInfos) == fit) { return; }

    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
    fit->localUrl = sf->get(fit->filename);
    ++m_artefactsLocalUrlsCount;

    if(m_artefactsLocalUrlsCount == m_artefactInfos.size())
    {
        QObject::disconnect(sf, SIGNAL(downloaded(const QString &)),
                            this, SLOT(refreshArgumentsArtefactDownloadedSlot(const QString &)));
        QObject::disconnect(sf, SIGNAL(progress()),
                            this, SLOT(refreshArgumentsProgressSlot()));
        emit refreshArgumentsBeforeApply();
    }
}

void Effect::refreshArgumentsProgressSlot()
{
    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
    QStringList artefactsUrls;
    artefactsUrls.reserve(m_artefactInfos.size());
    for(const ArtefactInfo &ai_: qAsConst(m_artefactInfos))
    {
        artefactsUrls.push_back(ai_.filename);
    }
    qreal rate = sf->getProgressRate(artefactsUrls);
    emit refreshArgumentsProgress(true, rate);
}

inline bool checkRequeredFields(const Comment &comment_)
{
    return comment_.values().contains(g_argumentName) &&
        comment_.values().contains(g_nameName) &&
        comment_.values().contains(g_typeName) &&
        comment_.values().contains(g_storageName)
        ;
}

void Effect::applyRefreshArguments()
{
    QList<EffectArg *> &newArguments = m_artefactArgs;
    ArtefactArgTypeModel *argTypesModel = static_cast<ArtefactArgTypeModel *>(
                QMLObjectsBase::getInstance().getListModel(g_artefactArgTypeModel)
                );
    Q_ASSERT(argTypesModel);
    ArtefactArgStorageModel *argStoragesModel = static_cast<ArtefactArgStorageModel *>(
                QMLObjectsBase::getInstance().getListModel(g_artefactArgStorageModel)
                );
    Q_ASSERT(argStoragesModel);

    // read comments from artefacts files
    // get file, form commens list
    // form arguments data
    for(const ArtefactInfo &ai_: qAsConst(m_artefactInfos))
    {
        const QUrl url(ai_.localUrl);
        const QString filename = url.toLocalFile();
        QFile f(filename);
        if(!f.open(QIODevice::ReadOnly)) { continue; }

        QByteArray fd = f.readAll();

        QTextCodec *codec = QTextCodec::codecForUtfText(fd);
        QString shaderText = codec->toUnicode(fd);

        QVector<Comment> comments;
        getShaderComments(shaderText, comments);

        for(const Comment &comment: qAsConst(comments))
        {
            if(!checkRequeredFields(comment)) { continue; }

            const QString argName = comment.values().value(g_nameName).trimmed();
            const QString argTypeStr = comment.values().value(g_typeName).trimmed();
            const QString argStorageStr = comment.values().value(g_storageName).trimmed();
            const QString argDefaultValue = comment.values().value(g_defaultValueName, QString()).trimmed();
            const QString argDescription = comment.values().value(g_descriptionName, QString()).trimmed();
            //qDebug() << "argName : " << argName << " argTypeStr : " << argTypeStr << " argDefaultValue : " << argDefaultValue;

            const ArtefactArgType *artefactArgType = argTypesModel->findDataItemByFieldValueImpl(
                        "artefactArgTypeType",
                        QVariant::fromValue(argTypeStr)
                        );
            Q_ASSERT(artefactArgType);
            const int argTypeId = artefactArgType->id();

            const ArtefactArgStorage *artefactArgStorage = argStoragesModel->findDataItemByFieldValueImpl(
                        "artefactArgStorageStorage",
                        QVariant::fromValue(argStorageStr)
                        );
            Q_ASSERT(artefactArgType);
            const int argStorageId = artefactArgStorage->id();

            auto fitni = std::find_if(
                        std::begin(newArguments),
                        std::end(newArguments),
                        [&argName, &ai_] (EffectArg *effectArg) -> bool
                        {
                            return effectArg &&
                                    effectArg->name() == argName &&
                                    ai_.objectArtefactId == effectArg->objectArtefactId()
                                    ;
                        });
            EffectArg *newArg = nullptr;
            const bool newArgCreated = std::end(newArguments) != fitni;
            if(newArgCreated && *fitni)
            {
                newArg = *fitni;
            }
            else
            {
                newArg = m_effectArgModel->createDataItemImpl();
                newArg->setName(argName);
                newArg->setEffectId(id());
                newArg->setObjectArtefactId(ai_.objectArtefactId);
                newArg->setCreated(QDateTime::currentDateTime());
            }
            newArg->setArgTypeId(argTypeId);
            newArg->setArgStorageId(argStorageId);
            newArg->setDefaultValue(argDefaultValue);
            newArg->setDescription(argDescription);

            const EffectArg *existingArg = m_effectArgModel->dataItemFindIf
                    ([&newArg] (const EffectArg *arg_) -> bool
                    {
                        return arg_ &&
                                newArg &&
                                arg_->name() == newArg->name() &&
                                arg_->objectArtefactId() == newArg->objectArtefactId()
                                ;
                    });
            if(existingArg)
            {
                if(existingArg->argTypeId() == argTypeId &&
                        existingArg->defaultValue() == argDefaultValue &&
                        existingArg->description() == argDescription)
                {
                    delete newArg;
                    newArg = nullptr;
                }
            }

            if(!newArgCreated)
            {
                if(newArg)
                {
                    newArguments.push_back(newArg);
                }
            }
            else
            {
                if(!newArg)
                {
                    newArguments.erase(fitni);
                }
            }
        }
    }

    // form lists: 1) to set items (index, and new value) 2) to del items (items) 3) to add items (items)
    clear(m_itemsToSet);
    clear(m_itemsToDel);
    clear(m_itemsToAdd);

    for(EffectArg *newArg: qAsConst(newArguments))
    {
        const bool existingItemFounded = m_effectArgModel->dataItemFindIf
                ([&newArg] (const EffectArg *arg_) -> bool
                {
                    return arg_ &&
                            newArg &&
                            arg_->name() == newArg->name() &&
                            arg_->objectArtefactId() == newArg->objectArtefactId()
                            ;
                });
        if(existingItemFounded)
        {
            m_itemsToSet.push_back(newArg);
        }
        else
        {
            m_itemsToAdd.push_back(newArg);
        }
    }
    for(int i = 0; i < m_effectArgModel->sizeImpl(); i++)
    {
        const EffectArg *existingArg = m_effectArgModel->dataItemAtImpl(i);
        const auto fitni = std::find_if(
                    std::cbegin(newArguments),
                    std::cend(newArguments),
                    [&existingArg] (EffectArg *effectArg) -> bool
                    {
                        return effectArg &&
                            existingArg &&
                            effectArg->name() == existingArg->name() &&
                            effectArg->objectArtefactId() == existingArg->objectArtefactId()
                            ;
                    });
        const bool newItemFounded = std::cend(newArguments) == fitni;
        if(!newItemFounded)
        {
            m_itemsToDel.push_back(m_effectArgModel->dataItemAtImpl(i));
        }
    }

    m_itemsToChangeCount = m_itemsToSet.size() + m_itemsToDel.size() + m_itemsToAdd.size();
    QObject::connect(m_effectArgModel, SIGNAL(itemAdded()), this, SLOT(itemAddedSlot()));
    QObject::connect(m_effectArgModel, SIGNAL(itemSet()), this, SLOT(itemSetSlot()));
    QObject::connect(m_effectArgModel, SIGNAL(itemDeleted()), this, SLOT(itemDeletedSlot()));

    applyRefreshArgumentsStep();
}

void Effect::itemAddedSlot()
{
    applyRefreshArgumentsStep();
}

void Effect::itemSetSlot()
{
    applyRefreshArgumentsStep();
}

void Effect::itemDeletedSlot()
{
    applyRefreshArgumentsStep();
}

void Effect::applyRefreshArgumentsStep()
{
    const int leftItems = m_itemsToSet.size() + m_itemsToDel.size() + m_itemsToAdd.size();
    if(0 == m_itemsToChangeCount)
    {
        emit refreshArgumentsProgress(false, 1.0);
    }
    else
    {
        emit refreshArgumentsProgress(
                    false,
                    qreal(
                        ( std::min(leftItems, m_itemsToChangeCount) * 1000 )
                        / m_itemsToChangeCount)
                        / 1000.0
                    );
    }

    if(m_itemsToSet.isEmpty() && m_itemsToDel.isEmpty() && m_itemsToAdd.isEmpty())
    {
        QObject::disconnect(m_effectArgModel, SIGNAL(itemAdded()), this, SLOT(itemAddedSlot()));
        QObject::disconnect(m_effectArgModel, SIGNAL(itemSet()), this, SLOT(itemSetSlot()));
        QObject::disconnect(m_effectArgModel, SIGNAL(itemDeleted()), this, SLOT(itemDeletedSlot()));

        clear(m_artefactArgs);

        emit refreshArgumentsAfterApply();
        return;
    }

    if(!m_itemsToSet.isEmpty())
    {
        EffectArg *arg = m_itemsToSet.back();
        m_itemsToSet.pop_back();
        EffectArg *existingArg = m_effectArgModel->findDataItemByFieldValueImpl(
                    "effectArgName",
                    QVariant::fromValue(arg->name())
                    );
        Q_ASSERT(existingArg);
        m_effectArgModel->setDataItemImpl(m_effectArgModel->indexOfDataItemImpl(existingArg), arg);
        return;
    }

    if(!m_itemsToDel.isEmpty())
    {
        EffectArg *arg = m_itemsToDel.back();
        m_itemsToDel.pop_back();
        m_effectArgModel->delDataItemImpl(arg);
        return;
    }

    if(!m_itemsToAdd.isEmpty())
    {
        EffectArg *arg = m_itemsToAdd.back();
        m_itemsToAdd.pop_back();
        m_effectArgModel->addDataItemImpl(arg);
        return;
    }
}

QString Effect::getObjectName() const
{
    return m_effectModel->getQMLLayoutName() + QString("_Effect_") + QVariant::fromValue(m_appId).toString();
}

void Effect::loadChildrenVF()
{
    IListModelInfoObjectImpl::setParentModelInfo(m_parentModelInfo);
    IListModelInfoObjectImpl::setObjectName(getObjectName());
    IListModelInfoObjectImpl::trace();
    IListModelInfoObjectImpl::loadChildrenVF();
}

void Effect::listLoadedVF()
{
    emit childrenLoaded();
}

void Effect::loadChildren()
{
    IListModelInfoObjectImpl::loadChildrenVF();
}

bool Effect::isChildrenLoaded() const
{
    return IListModelInfoObjectImpl::isListLoadedImpl();
}

void Effect::addDefaultObject()
{
    if(!m_effectObjectsModel ||
            !m_effectObjectsModel->isListLoadedImpl()) { return; }

    const EffectObjects *existingDefaultObject = m_effectObjectsModel->dataItemFindIf(
                [](const EffectObjects *effectObj_)->bool
    {
        if(!effectObj_) { return false; }
        const ObjectInfoModel *objectInfoModel = effectObj_->getObjectInfoModel();
        if(!objectInfoModel) { return false; }

        for(int i = 0; i < objectInfoModel->sizeImpl(); i++)
        {
            const ObjectInfo *objectInfo = objectInfoModel->dataItemAtImpl(i);
            if(!objectInfo) { continue; }
            if(const_cast<ObjectInfo *>(objectInfo)->isInitializeObject()) { return true; }
        }
        return false;
    });

    if(existingDefaultObject) { return; }

    m_effectObjectsModel->procedureImpl(
                g_createDefaultEffectObjectProcedureName,
                QHash<QString, QVariant>
                ({
                     {g_effectIDName,
                      QVariant::fromValue(this->id())},
                     {g_defaultObjectInfoNameName,
                      QVariant::fromValue(tr("Default Effect Object"))},
                     {g_defaultObjectInfoDescriptionName,
                      QVariant::fromValue(tr("This is the default effect object"))},
                     {g_defaultObjectInfoCreatedName,
                      QVariant::fromValue(date_time::dateTimeToJsonString(QDateTime::currentDateTime()))}
                 }));
}

EffectObjectsModel *Effect::getEffectObjects()
{
    return m_effectObjectsModel;
}

const EffectObjectsModel *Effect::getEffectObjects() const
{
    return m_effectObjectsModel;
}

EffectArgModel *Effect::getEffectArguments()
{
    return m_effectArgModel;
}

const EffectArgModel *Effect::getEffectArguments() const
{
    return m_effectArgModel;
}


EffectModel::EffectModel(
        QObject *parent_ /*= nullptr*/,
        IModelConfig *config_ /*= &DefaultModelConfig::instance()*/
        )
    : base(parent_, std::shared_ptr<QVector<Effect *>>{nullptr}, config_)
{
#if defined(TRACE_THREADS)
    qDebug() << "EffectModel::EffectModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "EffectModel::EffectModel()" << this;
#endif

    init(this);
}
