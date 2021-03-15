#include "artefact.h"
#include "../MastactvaBase/serverfiles.h"
#include "../MastactvaBase/qmlobjects.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/defines.h"


Artefact::Artefact(ArtefactModel *parent_ /* = nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "Artefact::Artefact()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    m_artefactModel = parent_;
    m_objectModelInfo = this;
}

Artefact::~Artefact()
{
    if(nullptr != m_artefactArgModel)
    {
        m_artefactArgModel->deleteLater();
    }
    m_artefactArgModel = nullptr;
}

int Artefact::id() const
{
    return m_id;
}

void Artefact::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

QString Artefact::name() const
{
    return m_name;
}

void Artefact::setName(const QString &name_)
{
    m_name = name_;

    emit nameChanged();
}

QString Artefact::filename() const
{
    return getFilename();
}

FileSource Artefact::getFilename() const
{
    return m_filename;
}

void Artefact::setFilename(const QString &filename_)
{
    setFilename(FileSource(filename_));
}

void Artefact::setFilename(const FileSource &filename_)
{
    m_filename = filename_;

    emit filenameChanged();
}

QString Artefact::hash() const
{
    return m_hash;
}

void Artefact::setHash(const QString &hash_)
{
    m_hash = hash_;

    emit hashChanged();
}

int Artefact::type() const
{
    return m_typeId;
}

void Artefact::setType(const int &type_)
{
    if(m_typeId == type_) { return; }
    m_typeId = type_;

    emit typeChanged();
}

QString Artefact::description() const
{
    return m_description;
}

void Artefact::setDescription(const QString &description_)
{
    m_description = description_;

    emit descriptionChanged();
}

QDateTime Artefact::created() const
{
    return m_created;
}

void Artefact::setCreated(const QDateTime &created_)
{
    m_created = created_;

    emit createdChanged();
}

QVariant Artefact::artefactArg() const
{
    if(nullptr == m_artefactArgModel)
    {
        const_cast<Artefact *>(this)->m_artefactArgModel = const_cast<Artefact *>(this)
                ->createArtefactArgModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(
                                   const_cast<ArtefactArgModel *>(
                                       m_artefactArgModel)
                                   )
                               );
}

void Artefact::setArtefactArg(const QVariant &obj_)
{
    if(obj_.isNull() && nullptr != m_artefactArgModel)
    {
        delete m_artefactArgModel;
        m_artefactArgModel = nullptr;

        emit artefactArgChanged();
    }
}

ArtefactArgModel *Artefact::getArtefactArg()
{
    return m_artefactArgModel;
}

const ArtefactArgModel *Artefact::getArtefactArg() const
{
    return m_artefactArgModel;
}

bool Artefact::isObjectLoaded() const
{
    return IListModelInfoObjectImpl::isListLoadedImpl();
}

void Artefact::loadChildrenVF()
{
    IListModelInfoObjectImpl::setParentModelInfo(m_parentModelInfo);
    IListModelInfoObjectImpl::setObjectName(getObjectName());
    IListModelInfoObjectImpl::trace();
    IListModelInfoObjectImpl::loadChildrenVF();
}

void Artefact::objectLoadedVF()
{
    IListModelInfoObjectImpl::objectLoadedVF();
    ServerFiles * sf = QMLObjectsBase::getInstance().getServerFiles();
    if(nullptr != sf)
    {
        QObject::connect(sf, SIGNAL(downloaded(QString)), this, SLOT(artefactFileDownloaded(QString)));
        if(nullptr != m_objectModelInfo)
        {
            m_objectModelInfo->startLoadChildModel();
        }
        sf->add(filename(), hash(), g_artefactsRelPath);
    }
}

void Artefact::artefactFileDownloaded(const QString &url_)
{
    if(url_ != filename()) { return; }
    ServerFiles * sf = QMLObjectsBase::getInstance().getServerFiles();
    if(nullptr != sf)
    {
        QObject::disconnect(sf, SIGNAL(downloaded(QString)), this, SLOT(artefactFileDownloaded(QString)));
        if(nullptr != m_objectModelInfo)
        {
            m_objectModelInfo->endLoadChildModel();
        }
    }
}

QString Artefact::getObjectName() const
{
    return m_artefactModel->getQMLLayoutName() + QString("_Artefact_")
            + QVariant::fromValue(m_appId).toString();
}

ArtefactArgModel *Artefact::createArtefactArgModel()
{
    ArtefactArgModel *m = new ArtefactArgModel(this);
    m->initResponse();
    m->setLayoutRefImpl("artefact", m_artefactModel->getQMLLayoutName(), "id", false);
    m->setCurrentRef("artefact");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_artefactModel->getQMLLayoutName() + QString("_Artefact_") +
                        QVariant::fromValue(m_appId).toString() + QString("_ArtefactArgModel_"));
    m->registerListModel();
    m->setParentListModelInfo(m_artefactModel);
    m->setAutoCreateChildrenModels(true);
    m->loadList();
    return m;
}


ArtefactModel::ArtefactModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ArtefactModel::ArtefactModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
    init(this);
    setAutoCreateChildrenModelsImpl(true);
}
