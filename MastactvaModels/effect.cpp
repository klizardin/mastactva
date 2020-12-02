#include "effect.h"
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
    if(nullptr == m_effectShadersModel || !m_effectShadersModel->isListLoadedImpl()) { return false; }
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

void Effect::applyRefreshArguments()
{
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
    emit refreshArgumentsProgress(rate);
}


EffectModel::EffectModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
    init(this);
}
