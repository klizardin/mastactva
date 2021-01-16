#include "gallerymodelview.h"
#include <QJsonDocument>
#include "../MastactvaBase/utils.h"


GalleryModelView::GalleryModelView(QObject * parent_ /*= nullptr*/)
    :QObject(parent_)
{
}

GalleryModelView::~GalleryModelView()
{
}

bool GalleryModelView::canProcess(const DBRequestInfo *r_) const
{
    if(nullptr == r_) { return false; }
    const QString tableName = DBRequestInfo::namingConversion(
                getDataLayout<typename GalleryModel::DataType>().getLayoutJsonName()
                );
    if(r_->getTableName() != tableName) { return false; }
    const QString requestNameGetList = RequestData::getListRequestName<typename GalleryModel::DataType>();
    if(r_->getDBRequestName() != requestNameGetList) { return false; }
    if(r_->getAPIName() != g_cachAPI) { return false; }
    return true;
}

bool GalleryModelView::getListImpl(DBRequestInfo *r_)
{
    if(nullptr == r_) { return false; }
    bool startLoad = m_requests.isEmpty();
    m_requests.push_back(r_);
    if(startLoad) { loadSteps(); }
    return true;
}

bool GalleryModelView::addItemImpl(const QVariant &appId_, const QHash<QString, QVariant> &values_, DBRequestInfo *r_)
{
    Q_UNUSED(appId_);
    Q_UNUSED(values_);
    Q_UNUSED(r_);
    return false;
}

bool GalleryModelView::setItemImpl(const QVariant &id_, const QHash<QString, QVariant> &values_, DBRequestInfo *r_)
{
    Q_UNUSED(id_);
    Q_UNUSED(values_);
    Q_UNUSED(r_);
    return false;
}

bool GalleryModelView::delItemImpl(const QVariant &id_, DBRequestInfo *r_)
{
    Q_UNUSED(id_);
    Q_UNUSED(r_);
    return false;
}

void GalleryModelView::loadSteps()
{
    QObject::connect(&m_userStepModel, SIGNAL(listReloaded()), this, SLOT(userStepModelListReloaded()));
    m_userStepModel.clearListLoaded();
    m_userStepModel.loadListImpl(
                QString(),
                QHash<QString, QVariant>({{QString(g_procedureExtraFieldName),
                    QHash<QString, QVariant>(
                        {
                            { QString(g_procedureConditionName), QVariant::fromValue(QString("user=:user")) },
                            { QString(g_procedureOrderByName), QVariant::fromValue(QString("t DESC"))},
                            { QString(g_procedureLimitName), QVariant::fromValue(1)},
                            { QString(g_procedureArguments),
                              QVariant::fromValue(
                                QHash<QString, QVariant>(
                                    {
                                        {QString(":user"), QVariant::fromValue(g_userId)},
                                    }
                                )
                              )
                            }
                        })
                    },})
                );
}

void GalleryModelView::userStepModelListReloaded()
{
    QObject::disconnect(&m_userStepModel, SIGNAL(listReloaded()), this, SLOT(userStepModelListReloaded()));
    m_lastUserStep = m_userStepModel.getCurrentDataItem();

    QObject::connect(&m_userStepPlayedGalleriesModel, SIGNAL(listReloaded()), this, SLOT(userStepPlayedGallriesModelListReloaded()));
    m_userStepPlayedGalleriesModel.clearListLoaded();
    m_userStepPlayedGalleriesModel.loadListImpl(
                QString(),
                QHash<QString, QVariant>({{QString(g_procedureExtraFieldName),
                    QHash<QString, QVariant>(
                        {
                            { QString(g_procedureSelectFunctionName), QVariant::fromValue(QString("DISTINCT")) },
                            { QString(g_procedureFilterNamesName), QVariant::fromValue(QList<QVariant>({ QVariant::fromValue(QString("gallery")),}))}
                        })
                    },})
                );
}

void GalleryModelView::userStepPlayedGallriesModelListReloaded()
{
    QObject::disconnect(&m_userStepPlayedGalleriesModel, SIGNAL(listReloaded()), this, SLOT(userStepPlayedGallriesModelListReloaded()));

    QObject::connect(&m_galleryModel, SIGNAL(listReloaded()), this, SLOT(galleryModelListReloaded()));
    m_galleryModel.clearListLoaded();
    m_galleryModel.loadListImpl();
}


static const int g_newGallriesIntervalDays = 30;


void GalleryModelView::galleryModelListReloaded()
{
    QObject::disconnect(&m_galleryModel, SIGNAL(listReloaded()), this, SLOT(galleryModelListReloaded()));

    QSet<int> playedGalleries;
    for( int i = 0; i < m_userStepPlayedGalleriesModel.sizeImpl(); i++ )
    {
        const UserStep *item = m_userStepPlayedGalleriesModel.dataItemAtImpl(i);
        playedGalleries.insert(item->galleryId());
    }
    const QDateTime newFromDate = QDateTime::currentDateTime().addDays(-g_newGallriesIntervalDays);

    m_galleryModel.randOrderImpl(false);
    m_galleryModel.sortIf([this, &playedGalleries, &newFromDate](const Gallery *i1_, const Gallery *i2_) -> int
    {
        int w1 = -1;
        int w2 = -1;

        if(nullptr != m_lastUserStep)
        {
            if(w1 < 0 && i1_->id() == m_lastUserStep->galleryId()) { w1 = 0; }
            if(w2 < 0 && i2_->id() == m_lastUserStep->galleryId()) { w2 = 0; }
        }

        if(w1 < 0 && !playedGalleries.contains(i1_->id()) && i1_->created() >= newFromDate) { w1 = 1; }
        if(w2 < 0 && !playedGalleries.contains(i2_->id()) && i2_->created() >= newFromDate) { w2 = 1; }

        if(w1 < 0 && playedGalleries.contains(i1_->id())) { w1 = 2; }
        if(w2 < 0 && playedGalleries.contains(i2_->id())) { w2 = 2; }

        if(w1 < 0) { w1 = 3; }
        if(w2 < 0) { w2 = 3; }

        return w1 - w2;
    }, false);

    DBRequestInfo *currentRequest = m_requests.front();
    if(nullptr != currentRequest)
    {
        setData(currentRequest);
        currentRequest->setProcessed(true);
    }

    if(!m_requests.isEmpty()) { loadSteps(); }
}

void GalleryModelView::setData(DBRequestInfo *request_)
{
    Q_ASSERT(nullptr != request_ && request_->getAPIName() == g_cachAPI);
    QJsonArray array;
    for(int i = 0; i < m_galleryModel.sizeImpl(); i++)
    {
        const Gallery *item = m_galleryModel.dataItemAtImpl(i);
        QHash<QString, QVariant> values;
        getDataLayout<Gallery>().getJsonValues(item, values);
        array.push_back(request_->getJsonObjectFromValues(values));
    }
    LocalDBRequest *r = static_cast<LocalDBRequest *>(request_);
    r->addJsonResult(QJsonDocument(array));
    r->setError(false);
    request_->setProcessed(true);
}
