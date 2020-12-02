#include "effect.h"
#include "effectarg.h"
#include <QTextCodec>
#include "shader.h"
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/serverfiles.h"
#include "../MastactvaBase/utils.h"


Effect::Effect(EffectModel *parent_)
    : QObject(parent_)
{
    m_effectModel = parent_;
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

QVariant Effect::effectShaders() const
{
    if(nullptr == m_effectShadersModel)
    {
        const_cast<Effect *>(this)->m_effectShadersModel = const_cast<Effect *>(this)->createEffectShadersModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(const_cast<EffectShaderModel *>(m_effectShadersModel)));
}

void Effect::setEffectShaders(const QVariant &obj_)
{
    if(obj_.isNull() && nullptr != m_effectShadersModel)
    {
        delete m_effectShadersModel;
        m_effectShadersModel = nullptr;

        emit effectShadersChanged();
    }
}

QVariant Effect::args() const
{
    if(nullptr == m_effectArgModel)
    {
        const_cast<Effect *>(this)->m_effectArgModel = const_cast<Effect *>(this)->createEffectArgModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(const_cast<EffectArgModel *>(m_effectArgModel)));
}

void Effect::setArgs(const QVariant &obj_)
{
    if(obj_.isNull() && nullptr != m_effectArgModel)
    {
        delete m_effectArgModel;
        m_effectArgModel = nullptr;

        emit argsChanged();
    }
}

QVariant Effect::argSets() const
{
    if(nullptr == m_effectArgSetModel)
    {
        const_cast<Effect *>(this)->m_effectArgSetModel = const_cast<Effect *>(this)->createEffectArgSetModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(const_cast<EffectArgSetModel *>(m_effectArgSetModel)));
}

void Effect::setArgSets(const QVariant &obj_)
{
    if(obj_.isNull() && nullptr != m_effectArgSetModel)
    {
        delete m_effectArgSetModel;
        m_effectArgSetModel = nullptr;

        emit argSetsChanged();
    }
}

EffectShaderModel *Effect::createEffectShadersModel()
{
    EffectShaderModel *m = new EffectShaderModel(this);
    m->initResponse();
    m->setLayoutRefImpl("effect", m_effectModel->getQMLLayoutName(), "id");
    m->setCurrentRef("effect");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_effectModel->getQMLLayoutName() + QString("_EffectShaderModel_") + QVariant::fromValue(m_appId).toString());
    m->registerListModel();
    m->setAutoCreateChildrenModels(true);
    m->loadList();
    return m;
}

EffectArgModel *Effect::createEffectArgModel()
{
    EffectArgModel *m = new EffectArgModel(this);
    m->initResponse();
    m->setLayoutRefImpl("effect", m_effectModel->getQMLLayoutName(), "id");
    m->setCurrentRef("effect");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_effectModel->getQMLLayoutName() + QString("_EffectArgModel_") + QVariant::fromValue(m_appId).toString());
    m->registerListModel();
    m->setAutoCreateChildrenModels(true);
    m->loadList();
    return m;
}

EffectArgSetModel *Effect::createEffectArgSetModel()
{
    EffectArgSetModel *m = new EffectArgSetModel(this);
    m->initResponse();
    m->setLayoutRefImpl("effect", m_effectModel->getQMLLayoutName(), "id");
    m->setCurrentRef("effect");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_effectModel->getQMLLayoutName() + QString("_EffectArgSetModel_") + QVariant::fromValue(m_appId).toString());
    m->registerListModel();
    m->setAutoCreateChildrenModels(true);
    m->loadList();
    return m;
}

bool Effect::startRefreshArguments()
{
    qDebug() << "Effect::startRefreshArguments()";
    if(nullptr == m_effectShadersModel
            || !m_effectShadersModel->isListLoadedImpl()
            || nullptr == m_effectArgModel
            || !m_effectArgModel->isListLoadedImpl()
            ) { return false; }

    // get all shaders urls
    const int shadersCnt = m_effectShadersModel->size();
    QList<QPair<QString, QString>> urlHashPairs;
    for(int i = 0; i < shadersCnt; i++)
    {
        EffectShader* m = m_effectShadersModel->dataItemAtImpl(i);
        if(nullptr == m || nullptr == m->getShader() || !m->getShader()->isListLoadedImpl()) { return false; }
        Shader* shader = m->getShader()->dataItemAtImpl(0);
        if(nullptr == shader) { return false; }
        urlHashPairs.push_back({shader->getFilename(), shader->hash()});
    }
    for(const QPair<QString,QString> &url_: urlHashPairs)
    {
        qDebug() << url_.first << ", " << url_.second;
    }
    m_shaderUrls.clear();
    for(const QPair<QString,QString> &url_: urlHashPairs)
    {
        m_shaderUrls.push_back(url_.first);
    }
    m_shaderLocalUrls.clear();
    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
    QObject::connect(sf, SIGNAL(downloaded(const QString &)), this, SLOT(refreshArgumentsShaderDownloadedSlot(const QString &)));
    QObject::connect(sf, SIGNAL(progress()), this, SLOT(refreshArgumentsProgressSlot()));
    for(const QPair<QString,QString> &url_: urlHashPairs)
    {
        sf->add(url_.first, url_.second, g_shadersRelPath);
    }
    return true;
}

void Effect::cancelRefreshArguments()
{
    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
    QObject::disconnect(sf, SIGNAL(downloaded(const QString &)), this, SLOT(refreshArgumentsShaderDownloadedSlot(const QString &)));
    QObject::disconnect(sf, SIGNAL(progress()), this, SLOT(refreshArgumentsProgressSlot()));
    sf->cancel(m_shaderUrls);
    m_shaderUrls.clear();
    m_shaderLocalUrls.clear();
}

void Effect::refreshArgumentsShaderDownloadedSlot(const QString &url_)
{
    const auto fit = std::find(std::begin(m_shaderUrls), std::end(m_shaderUrls), url_);
    if(std::end(m_shaderUrls) == fit) { return; }
    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
    m_shaderLocalUrls.insert(url_, sf->get(url_));
    if(m_shaderLocalUrls.size() == m_shaderUrls.size())
    {
        QObject::disconnect(sf, SIGNAL(downloaded(const QString &)), this, SLOT(refreshArgumentsShaderDownloadedSlot(const QString &)));
        QObject::disconnect(sf, SIGNAL(progress()), this, SLOT(refreshArgumentsProgressSlot()));
        emit refreshArgumentsBeforeApply();
    }
}

void Effect::refreshArgumentsProgressSlot()
{
    ServerFiles *sf = QMLObjectsBase::getInstance().getServerFiles();
    qreal rate = sf->getProgressRate(m_shaderUrls);
    emit refreshArgumentsProgress(true, rate);
}

void Effect::applyRefreshArguments()
{
    QList<EffectArg *> newArguments;
    ShaderArgTypeModel *argTypesModel = static_cast<ShaderArgTypeModel *>(QMLObjectsBase::getInstance().getListModel("ShaderArgTypeModel"));
    Q_ASSERT(nullptr != argTypesModel);

    // read comments from shaders files
    // get file, form commens list
    // form arguments data
    for(const QString &localUrl : m_shaderLocalUrls.values())
    {
        const QUrl url(localUrl);
        const QString filename = url.toLocalFile();
        QFile f(filename);
        if(!f.open(QIODevice::ReadOnly)) { continue; }
        QByteArray fd = f.readAll();

        QTextCodec *codec = QTextCodec::codecForUtfText(fd);
        QString shaderText = codec->toUnicode(fd);

        QVector<Comment> comments;
        getShaderComments(shaderText, comments);
        for(const Comment &comment: comments)
        {
            if(!comment.values().contains(g_argumentName) ||
                    !comment.values().contains(g_nameName) ||
                    !comment.values().contains(g_typeName)
                    ) { continue; }
            const QString argName = comment.values().value(g_nameName);
            const QString argTypeStr = comment.values().value(g_typeName);
            const QString argDefaultValue = comment.values().value(g_defaultValueName, QString());
            const QString argDescription = comment.values().value(g_descriptionName, QString());
            ShaderArgType *shaderArgType = argTypesModel->findDataItemByFieldValueImpl("effectArgName", QVariant::fromValue(argTypeStr));
            Q_ASSERT(nullptr != shaderArgType);
            const int argTypeId = shaderArgType->id();

            auto fitni = std::find_if(std::begin(newArguments), std::end(newArguments),
                                      [&argName](EffectArg *effectArg)->bool
            {
                return nullptr != effectArg && effectArg->name() == argName;
            });
            EffectArg *newArg = nullptr;
            const bool newArgCreated = std::end(newArguments) != fitni;
            if(newArgCreated)
            {
                newArg = *fitni;
            }
            else
            {
                newArg = m_effectArgModel->createDataItemImpl();
                newArg->setName(argName);
            }
            newArg->setArgTypeId(argTypeId);
            newArg->setDefaultValue(argDefaultValue);
            newArg->setDescription(argDescription);

            EffectArg *existingArg = m_effectArgModel->findDataItemByFieldValueImpl("effectArgName", QVariant::fromValue(argName));
            if(nullptr != existingArg)
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
                if(nullptr != newArg)
                {
                    newArguments.push_back(newArg);
                }
            }
            else
            {
                if(nullptr == newArg)
                {
                    newArguments.erase(fitni);
                }
            }
        }
    }

    // form lists: 1) to set items (index, and new value) 2) to del items (items) 3) to add items (items)
    m_itemsToSet.clear();
    m_itemsToDel.clear();
    m_itemsToAdd.clear();
    for(EffectArg *newArg: newArguments)
    {
        EffectArg *existingArg = m_effectArgModel->findDataItemByFieldValueImpl("effectArgName", QVariant::fromValue(newArg->name()));
        if(nullptr != existingArg)
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
        const QString argName = m_effectArgModel->dataItemAtImpl(i)->name();
        const auto fitni = std::find_if(std::begin(newArguments), std::end(newArguments),
                                  [&argName](EffectArg *effectArg)->bool
        {
            return nullptr != effectArg && effectArg->name() == argName;
        });
        if(std::end(newArguments) == fitni)
        {
            m_itemsToDel.push_back(m_effectArgModel->dataItemAtImpl(i));
        }
    }
    newArguments.clear();
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
        emit refreshArgumentsProgress(false, qreal((std::min(leftItems, m_itemsToChangeCount) * 1000) / m_itemsToChangeCount) / 1000.0);
    }

    if(m_itemsToSet.isEmpty() && m_itemsToDel.isEmpty() && m_itemsToAdd.isEmpty())
    {
        QObject::disconnect(m_effectArgModel, SIGNAL(itemAdded()), this, SLOT(itemAddedSlot()));
        QObject::disconnect(m_effectArgModel, SIGNAL(itemSet()), this, SLOT(itemSetSlot()));
        QObject::disconnect(m_effectArgModel, SIGNAL(itemDeleted()), this, SLOT(itemDeletedSlot()));

        emit refreshArgumentsAfterApply();
        return;
    }

    if(!m_itemsToSet.isEmpty())
    {
        EffectArg *arg = m_itemsToSet.back();
        m_itemsToSet.pop_back();
        EffectArg *existingArg = m_effectArgModel->findDataItemByFieldValueImpl("effectArgName", QVariant::fromValue(arg->name()));
        Q_ASSERT(nullptr != existingArg);
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


EffectModel::EffectModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
    init(this);
}
