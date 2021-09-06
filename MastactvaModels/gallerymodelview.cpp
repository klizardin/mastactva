/*
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

#include "gallerymodelview.h"
#include <QJsonDocument>
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/defines.h"


GalleryModelView::GalleryModelView(QObject * parent_ /*= nullptr*/)
    :QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "GalleryModelView::GalleryModelView()" << QThread::currentThread() << QThread::currentThreadId();
#endif

    m_galleryModel = new GalleryModel(this);
    m_galleryModel->initResponse();
    m_galleryModel->setCurrentRef("");
    m_galleryModel->setLayoutQMLName("LocalData_GalleryModel");
    m_galleryModel->setLayoutIdFieldImpl("id");
    m_galleryModel->registerListModel();
    m_galleryModel->setAutoCreateChildrenModels(false);

    m_userStepModel = new UserStepModel(this);
    m_userStepModel->initResponse();
    m_userStepModel->setCurrentRef("");
    m_userStepModel->setLayoutQMLName("LocalData_UserStepModel");
    m_userStepModel->setLayoutIdFieldImpl("id");
    m_userStepModel->registerListModel();
    m_userStepModel->setAutoCreateChildrenModels(false);
    m_userStepModel->setReadonlyImpl(false);
    m_userStepModel->setStoreAfterSave(true);

    m_userStepPlayedGalleriesModel = new UserStepModel(this);
    m_userStepPlayedGalleriesModel->initResponse();
    m_userStepPlayedGalleriesModel->setCurrentRef("");
    m_userStepPlayedGalleriesModel->setLayoutQMLName("LocalData_UserStepModelPlayedGalleries");
    m_userStepPlayedGalleriesModel->setLayoutIdFieldImpl("id");
    m_userStepPlayedGalleriesModel->registerListModel();
    m_userStepPlayedGalleriesModel->setAutoCreateChildrenModels(false);
    m_userStepPlayedGalleriesModel->setReadonlyImpl(false);
    m_userStepPlayedGalleriesModel->setStoreAfterSave(true);
}

GalleryModelView::~GalleryModelView()
{
    delete m_galleryModel;
    m_galleryModel = nullptr;
    delete m_userStepModel;
    m_userStepModel = nullptr;
    delete m_userStepPlayedGalleriesModel;
    m_userStepPlayedGalleriesModel = nullptr;
}

bool GalleryModelView::canProcess(const DBRequestBase *r_) const
{
    if(!r_) { return false; }
    const QString tableName = db::jsonToSql(
                getDataLayout<typename GalleryModel::DataType>().getLayoutJsonName()
                );
    if(r_->getTableName() != tableName) { return false; }
    const QString requestNameGetList = RequestData::getListRequestName<typename GalleryModel::DataType>();
    if(r_->getDBRequestName() != requestNameGetList) { return false; }
    if(r_->getAPIName() != g_cachAPI) { return false; }
    if(r_->getCurrentRef().isEmpty()) { return false; }
    return true;
}

bool GalleryModelView::getListImpl(DBRequestBase *r_)
{
    if(!r_) { return false; }
    bool startLoad = m_requests.isEmpty();
    m_requests.push_back(r_);
    if(startLoad) { loadSteps(); }
    return true;
}

bool GalleryModelView::addItemImpl(const QVariant &appId_,
                                   const QHash<QString, QVariant> &values_,
                                   DBRequestBase *r_)
{
    Q_UNUSED(appId_);
    Q_UNUSED(values_);
    Q_UNUSED(r_);
    return false;
}

bool GalleryModelView::setItemImpl(const QVariant &id_,
                                   const QHash<QString, QVariant> &values_,
                                   DBRequestBase *r_)
{
    Q_UNUSED(id_);
    Q_UNUSED(values_);
    Q_UNUSED(r_);
    return false;
}

bool GalleryModelView::delItemImpl(const QVariant &id_, DBRequestBase *r_)
{
    Q_UNUSED(id_);
    Q_UNUSED(r_);
    return false;
}

void GalleryModelView::loadSteps()
{
    QObject::connect(m_userStepModel, SIGNAL(listReloaded()),
                     this, SLOT(userStepModelListReloaded()));
    m_userStepModel->clearListLoaded();
    m_userStepModel->loadListImpl(
                QString(),
                QHash<QString, QVariant>({
                    {QString(g_procedureExtraFieldName),
                    QVariant::fromValue(QHash<QString, QVariant>
                    ({
                        { QString(g_procedureConditionName), QVariant::fromValue(QString("\"user\"=:user")) },
                        { QString(g_procedureOrderByName), QVariant::fromValue(QString("t DESC"))},
                        { QString(g_procedureLimitName), QVariant::fromValue(1)},
                        { QString(g_procedureArguments),
                          QVariant::fromValue(QHash<QString, QVariant>
                          ({
                              {QString(":user"), QVariant::fromValue(g_userId)},
                          }))
                        },
                     }))
                 },})
                );
}

void GalleryModelView::userStepModelListReloaded()
{
    QObject::disconnect(m_userStepModel, SIGNAL(listReloaded()),
                        this, SLOT(userStepModelListReloaded()));
    m_lastUserStep = m_userStepModel->getCurrentDataItem();

    QObject::connect(m_userStepPlayedGalleriesModel, SIGNAL(listReloaded()),
                     this, SLOT(userStepPlayedGallriesModelListReloaded()));
    m_userStepPlayedGalleriesModel->clearListLoaded();
    m_userStepPlayedGalleriesModel->loadListImpl(
                QString(),
                QHash<QString, QVariant>({
                    {QString(g_procedureExtraFieldName),
                    QVariant::fromValue(QHash<QString, QVariant>(
                        {
                            { QString(g_procedureSelectFunctionName), QVariant::fromValue(QString("DISTINCT")) },
                            { QString(g_procedureFilterNamesName),
                              QVariant::fromValue(
                                QList<QVariant>({ QVariant::fromValue(QString("gallery")), })
                              )
                            }
                        }))
                    },})
                );
}

void GalleryModelView::userStepPlayedGallriesModelListReloaded()
{
    QObject::disconnect(m_userStepPlayedGalleriesModel, SIGNAL(listReloaded()),
                        this, SLOT(userStepPlayedGallriesModelListReloaded()));

    QObject::connect(m_galleryModel, SIGNAL(listReloaded()),
                     this, SLOT(galleryModelListReloaded()));
    m_galleryModel->clearListLoaded();
    m_galleryModel->loadListImpl();
}


static const int g_newGallriesIntervalDays = 30;


void GalleryModelView::galleryModelListReloaded()
{
    QObject::disconnect(m_galleryModel, SIGNAL(listReloaded()),
                        this, SLOT(galleryModelListReloaded()));

    QSet<int> playedGalleries;
    for( int i = 0; i < m_userStepPlayedGalleriesModel->sizeImpl(); i++ )
    {
        const UserStep *item = m_userStepPlayedGalleriesModel->dataItemAtImpl(i);
        playedGalleries.insert(item->galleryId());
    }
    const QDateTime newFromDate = QDateTime::currentDateTime().addDays(-g_newGallriesIntervalDays);

    m_galleryModel->randOrderImpl(false);
    m_galleryModel->sortIf([this, &playedGalleries, &newFromDate](const Gallery *i1_, const Gallery *i2_) -> int
    {
        int w1 = -1;
        int w2 = -1;

        if(m_lastUserStep)
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

    DBRequestBase *currentRequest = m_requests.front();
    if(currentRequest)
    {
        setData(currentRequest);
        currentRequest->setProcessed(true);
    }
    m_requests.pop_front();
    if(!m_requests.isEmpty()) { loadSteps(); }
}

void GalleryModelView::setData(DBRequestBase *request_)
{
    Q_ASSERT(request_ && request_->getAPIName() == g_cachAPI);
    QJsonArray array;
    for(int i = 0; i < m_galleryModel->sizeImpl(); i++)
    {
        const Gallery *item = m_galleryModel->dataItemAtImpl(i);
        QHash<QString, QVariant> values;
        getDataLayout<Gallery>().getJsonValues(item, values);
        array.push_back(db::getJsonObject(values, request_->getTableFieldsInfo()));
    }
    LocalDBRequest *r = static_cast<LocalDBRequest *>(request_);
    r->addJsonResult(QJsonDocument(array));
    r->setError(false);
    request_->setProcessed(true);
}
