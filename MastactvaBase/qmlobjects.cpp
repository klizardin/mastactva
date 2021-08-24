#include "qmlobjects.h"
#include <QObject>
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/netapi.h"
#include "../MastactvaBase/localdata.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaBase/serverfiles.h"
#include "../MastactvaBase/defines.h"


void QMLObjectsBase::setRoot(QObject *root_)
{
#if defined(TRACE_THREADS)
    qDebug() << "QMLObjectsBase::setRoot()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "QMLObjectsBase::setRoot()" << this;
#endif

    if(m_root == root_) { return; }
    m_root = root_;
    if(!m_root) { return; }
    clearDependedFromRoot();
    searchObjects();
}

IListModel *QMLObjectsBase::getListModel(const QString &layoutName_)
{
    if(!m_root) { return nullptr; }
    IListModel *model = findListModel(layoutName_);
    if(model) { return model; }
    searchObjects();
    return findListModel(layoutName_);
}

const QVector<ILocalDataAPI *> &QMLObjectsBase::getLocalDataAPIViews()
{
    if(m_localDataAPIViews.isEmpty())
    {
        searchObjects();
    }
    return m_localDataAPIViews;
}

IListModel *QMLObjectsBase::findListModel(const QString &layoutName_) const
{
    if(!m_root) { return nullptr; }
    auto fit = std::find_if(std::begin(m_models), std::end(m_models),
                                  [&layoutName_](IListModel *model_)->bool
    {
        return nullptr != model_ && model_->getQMLLayoutName() == layoutName_;
    });
    if(std::end(m_models) == fit) { return nullptr; }
    return *fit;
}

void QMLObjectsBase::clearDependedFromRoot()
{
    delete m_serverFiles;
    m_serverFiles = nullptr;
}

NetAPI *QMLObjectsBase::getNetAPI()
{
    if(!m_root) { return nullptr; }
    if(!m_netAPI) { searchObjects(); }
    return m_netAPI;
}

LocalDataAPI *QMLObjectsBase::getDataAPI()
{
    if(!m_root) { return nullptr; }
    if(!m_dataAPI) { searchObjects(); }
    return m_dataAPI;
}

ServerFiles *QMLObjectsBase::getServerFiles()
{
    if(!m_root) { return nullptr; }
    if(!m_serverFiles) { m_serverFiles = new ServerFiles(m_root); }
    return m_serverFiles;
}

void QMLObjectsBase::registerModel(const QString &layoutName_, IListModel *m_)
{
    if(!m_root) { return; }
    IListModel *m1 = findListModel(layoutName_);
    Q_ASSERT(!m1);
    m_models.push_back(static_cast<IListModel *>(m_));
}

void QMLObjectsBase::unregisterModel(const QString &layoutName_)
{
    if(!m_root) { return; }
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "QMLObjectsBase::unregisterModel()" << this;
#endif

    auto fit = std::find_if(std::begin(m_models), std::end(m_models),
                                  [&layoutName_](IListModel *model_)->bool
    {
        return model_ && model_->getQMLLayoutName() == layoutName_;
    });
    if(std::end(m_models) == fit) { return; }
    m_models.erase(fit);
}

void QMLObjectsBase::postRegisterModel()
{
    for(IListModel *m : qAsConst(m_models))
    {
        if(!m) { continue; }
        m->initResponse();
    }
}
