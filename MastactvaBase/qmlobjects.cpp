#include "qmlobjects.h"
#include <QObject>
#include "imagesource.h"
#include "IModel.h"
#include "netapi.h"
#include "Model.h"


void QMLObjectsBase::setRoot(QObject *root_)
{
    m_root = root_;
    searchObjects();
}

IListModel *QMLObjectsBase::getListModel(const QString &layoutName_)
{
    IListModel *model = findListModel(layoutName_);
    if(nullptr != model) { return model; }
    searchObjects();
    return findListModel(layoutName_);
}

IListModel *QMLObjectsBase::findListModel(const QString &layoutName_) const
{
    const auto fit = std::find_if(std::begin(m_models), std::end(m_models),
                                  [&layoutName_](IListModel *model_)->bool
    {
        return nullptr != model_ && model_->getQMLLayoutName() == layoutName_;
    });
    if(std::end(m_models) == fit) { return nullptr; }
    return *fit;
}

NetAPI *QMLObjectsBase::getNetAPI()
{
    if(nullptr == m_netAPI) { searchObjects(); }
    return m_netAPI;
}

void QMLObjectsBase::registerModel(const QString &layoutName_, IListModel *m_)
{
    IListModel *m1 = findListModel(layoutName_);
    Q_ASSERT(nullptr == m1);
    m_models.push_back(static_cast<IListModel *>(m_));
}

void QMLObjectsBase::unregisterModel(const QString &layoutName_)
{
    const auto fit = std::find_if(std::begin(m_models), std::end(m_models),
                                  [&layoutName_](IListModel *model_)->bool
    {
        return nullptr != model_ && model_->getQMLLayoutName() == layoutName_;
    });
    if(std::end(m_models) == fit) { return; }
    m_models.erase(fit);
}

