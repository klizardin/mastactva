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

#include "imagepoint.h"
#include <math.h>
#include "../MastactvaBase/utils.h"
#include "../MastactvaModels/image.h"
#include "../MastactvaBase/defines.h"


ImagePointToQuestion::ImagePointToQuestion(ImagePointToQuestionModel *parent_ /*= nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ImagePointToQuestion::ImagePointToQuestion()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "ImagePointToQuestion::ImagePointToQuestion()" << this;
#endif

    m_imagePointToQuestionModel = parent_;
}

ImagePointToQuestion::~ImagePointToQuestion()
{
    if(m_questionModel)
    {
        m_questionModel->deleteLater();
    }
    m_questionModel = nullptr;
}

int ImagePointToQuestion::id() const
{
    return m_id;
}

void ImagePointToQuestion::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int ImagePointToQuestion::imagePointId() const
{
    return m_imagePointId;
}

void ImagePointToQuestion::setImagePointId(const int &imagePointId_)
{
    m_imagePointId = imagePointId_;

    emit imagePointIdChanged();
}

int ImagePointToQuestion::questionId() const
{
    return m_questionId;
}

void ImagePointToQuestion::setQuestionId(const int &questionId_)
{
    m_questionId = questionId_;

    emit questionObjChanged();
}

QVariant ImagePointToQuestion::questionObj() const
{
    if(!m_questionModel)
    {
        const_cast<ImagePointToQuestion *>(this)->m_questionModel = const_cast<ImagePointToQuestion *>(this)
                ->createQuestionModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(
                                   const_cast<QuestionModel *>(
                                       m_questionModel)
                                   )
                               );
}

void ImagePointToQuestion::setQuestionObj(const QVariant &obj_)
{
    if(obj_.isNull() && m_questionModel)
    {
        delete m_questionModel;
        m_questionModel = nullptr;

        emit questionObjChanged();
    }
}

QuestionModel *ImagePointToQuestion::getQuestions()
{
    (void)questionObj();
    return m_questionModel;
}

QuestionModel *ImagePointToQuestion::createQuestionModel()
{
    QuestionModel *m = new QuestionModel(this);
    m->initResponse();
    m->setLayoutRefImpl("id", m_imagePointToQuestionModel->getQMLLayoutName(), "question", false);
    m->setCurrentRef("id");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_imagePointToQuestionModel->getQMLLayoutName() + QString("_ImagePointToQuestion_") +
                        QVariant::fromValue(m_appId).toString() + QString("_QuestionModel_"));
    m->registerListModel();
    m->setAutoCreateChildrenModels(true);
    m->setParentListModelInfo(m_parentModelInfo);
    m->loadList();
    return m;
}


ImagePointToQuestionModel::ImagePointToQuestionModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ImagePointToQuestionModel::ImagePointToQuestionModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "ImagePointToQuestionModel::ImagePointToQuestionModel()" << this;
#endif

    init(this);
}


ImagePointToNextImage::ImagePointToNextImage(ImagePointToNextImageModel *parent_ /*= nullptr*/)
    :QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ImagePointToNextImage::ImagePointToNextImage()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "ImagePointToNextImage::ImagePointToNextImage()" << this;
#endif

    m_objectModelInfo = this;
    m_parentModel = parent_;
}

int ImagePointToNextImage::id() const
{
    return m_id;
}

void ImagePointToNextImage::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int ImagePointToNextImage::imagePointId() const
{
    return m_imagePointId;
}

void ImagePointToNextImage::setImagePointId(const int &imagePointId_)
{
    m_imagePointId = imagePointId_;

    emit imagePointIdChanged();
}

int ImagePointToNextImage::nextImage() const
{
    return m_nextImage;
}

void ImagePointToNextImage::setNextImage(const int &nextImage_)
{
    m_nextImage = nextImage_;

    emit nextImageChanged();
}

void ImagePointToNextImage::loadChildrenVF()
{
    IListModelInfoObjectImpl::setParentModelInfo(m_parentModelInfo);
    IListModelInfoObjectImpl::setObjectName(getObjectName());
    IListModelInfoObjectImpl::trace();
    IListModelInfoObjectImpl::loadChildrenVF();
}

void ImagePointToNextImage::objectLoadedVF()
{
    ImageModel *images = m_parentModel->getImageModel();
    if(images)
    {
        Image *image = images->findDataItemByIdImpl(QVariant::fromValue(nextImage()));
        if(image)
        {
            image->downloadImage();
        }
    }
    IListModelInfoObjectImpl::objectLoadedVF();
}

QString ImagePointToNextImage::getObjectName() const
{
    return m_parentModel->getQMLLayoutName() + QString("_ImagePointToNextImage_") +
            QVariant::fromValue(m_appId).toString();
}


ImagePointToNextImageModel::ImagePointToNextImageModel(QObject *parent_ /*= nullptr*/)
    :ListModelBaseOfData<ImagePointToNextImage, ImagePointToNextImageModel>(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ImagePointToNextImageModel::ImagePointToNextImageModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "ImagePointToNextImageModel::ImagePointToNextImageModel()" << this;
#endif

    init(this);
}

void ImagePointToNextImageModel::setImageModel(ImageModel *imageModel_)
{
    m_parentImageModel = imageModel_;
}

ImageModel *ImagePointToNextImageModel::getImageModel()
{
    return m_parentImageModel;
}


ImagePoint::ImagePoint(ImagePointModel *parent_ /*= nullptr*/)
    : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ImagePoint::ImagePoint()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "ImagePoint::ImagePoint()" << this;
#endif

    m_imagePointModel = parent_;
}

ImagePoint::~ImagePoint()
{
    if(m_imagePointToNextImageModel)
    {
        m_imagePointToNextImageModel->deleteLater();
    }
    m_imagePointToNextImageModel = nullptr;
    if(m_imagePointToQuestionModel)
    {
        m_imagePointToQuestionModel->deleteLater();
    }
    m_imagePointToQuestionModel = nullptr;
    if(m_imagePointEffectModel)
    {
        m_imagePointEffectModel->deleteLater();
    }
    m_imagePointEffectModel = nullptr;
}

int ImagePoint::id() const
{
    return m_id;
}

void ImagePoint::setId(const int &id_)
{
    m_id = id_;

    emit idChanged();
}

int ImagePoint::imageId() const
{
    return m_imageId;
}

void ImagePoint::setImageId(const int &imageId_)
{
    m_imageId = imageId_;

    emit imageIdChanged();
}

qreal ImagePoint::xCoord() const
{
    return m_x;
}

void ImagePoint::setXCoord(const qreal &xCoord_)
{
    m_x = xCoord_;

    emit xCoordChanged();
}

qreal ImagePoint::yCoord() const
{
    return m_y;
}

void ImagePoint::setYCoord(const qreal &yCoord_)
{
    m_y = yCoord_;

    emit yCoordChanged();
}

qreal ImagePoint::weight() const
{
    return m_weight;
}

void ImagePoint::setWeight(const qreal &weight_)
{
    m_weight = weight_;

    emit weightChanged();
}

QDateTime ImagePoint::created() const
{
    return m_created;
}

void ImagePoint::setCreated(const QDateTime &created_)
{
    m_created = created_;

    emit createdChanged();
}

QVariant ImagePoint::nextImage() const
{
    if(!m_imagePointToNextImageModel)
    {
        const_cast<ImagePoint *>(this)->m_imagePointToNextImageModel = const_cast<ImagePoint *>(this)
                ->createImagePointToNextImage();
    }
    return QVariant::fromValue(static_cast<QObject *>(
                                   const_cast<ImagePointToNextImageModel *>(
                                       m_imagePointToNextImageModel)
                                   )
                               );
}

void ImagePoint::setNextImage(const QVariant &obj_)
{
    if(obj_.isNull() && m_imagePointToNextImageModel)
    {
        delete m_imagePointToNextImageModel;
        m_imagePointToNextImageModel = nullptr;

        emit nextImageChanged();
    }
}

QVariant ImagePoint::nextQuestion() const
{
    if(!m_imagePointToQuestionModel)
    {
        const_cast<ImagePoint *>(this)->m_imagePointToQuestionModel = const_cast<ImagePoint *>(this)
                ->createImagePointToQuestionModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(
                                   const_cast<ImagePointToQuestionModel *>(
                                       m_imagePointToQuestionModel)
                                   )
                               );
}

void ImagePoint::setNextQuestion(const QVariant &obj_)
{
    if(obj_.isNull() && m_imagePointToQuestionModel)
    {
        delete m_imagePointToQuestionModel;
        m_imagePointToQuestionModel = nullptr;

        emit nextQuestionChanged();
    }
}

QVariant ImagePoint::effect() const
{
    if(!m_imagePointToQuestionModel)
    {
        const_cast<ImagePoint *>(this)->m_imagePointEffectModel = const_cast<ImagePoint *>(this)
                ->createImagePointEffectModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(
                                   const_cast<ImagePointEffectModel *>(
                                       m_imagePointEffectModel)
                                   )
                               );
}

void ImagePoint::setEffect(const QVariant &obj_)
{
    if(obj_.isNull() && m_imagePointEffectModel)
    {
        delete m_imagePointEffectModel;
        m_imagePointEffectModel = nullptr;

        emit effectChanged();
    }
}

ImagePointToNextImageModel *ImagePoint::getNextImage() const
{
    nextImage();
    return m_imagePointToNextImageModel;
}

ImagePointToQuestionModel *ImagePoint::getNextQuestion() const
{
    nextQuestion();
    return m_imagePointToQuestionModel;
}

qreal ImagePoint::distanceCoef(qreal x_, qreal y_) const
{
    return sqrt(sqr(m_x - x_) + sqr(m_y - y_)) * m_weight;
}


ImagePointToNextImageModel *ImagePoint::createImagePointToNextImage()
{
    ImagePointToNextImageModel *m = new ImagePointToNextImageModel(this);
    m->initResponse();
    m->setLayoutRefImpl("image_point", m_imagePointModel->getQMLLayoutName(), "id", false);
    m->setCurrentRef("image_point");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_imagePointModel->getQMLLayoutName() + QString("_ImagePoint_") +
                        QVariant::fromValue(m_appId).toString() + QString("_ImagePointToNextImageModel_"));
    m->registerListModel();
    m->setParentListModelInfo(m_parentModelInfo);
    m->setImageModel(m_imagePointModel->getImageModel());
    m->loadList();
    return m;
}

ImagePointToQuestionModel *ImagePoint::createImagePointToQuestionModel()
{
    ImagePointToQuestionModel *m = new ImagePointToQuestionModel(this);
    m->initResponse();
    m->setLayoutRefImpl("image_point", m_imagePointModel->getQMLLayoutName(), "id", false);
    m->setCurrentRef("image_point");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_imagePointModel->getQMLLayoutName() + QString("_ImagePoint_") +
                        QVariant::fromValue(m_appId).toString() + QString("_ImagePointToQuestionModel_"));
    m->registerListModel();
    m->setAutoCreateChildrenModels(true);
    m->setParentListModelInfo(m_parentModelInfo);
    m->loadList();
    return m;
}

ImagePointEffectModel *ImagePoint::createImagePointEffectModel()
{
    ImagePointEffectModel *m = new ImagePointEffectModel(this);
    m->initResponse();
    m->setLayoutRefImpl("image_point", m_imagePointModel->getQMLLayoutName(), "id", false);
    m->setCurrentRef("image_point");
    m->setRefAppId(QVariant::fromValue(m_appId));
    m->setLayoutQMLName(m_imagePointModel->getQMLLayoutName() + QString("_ImagePoint_") +
                        QVariant::fromValue(m_appId).toString() + QString("_ImagePointEffectModel_"));
    m->registerListModel();
    m->setAutoCreateChildrenModels(true);
    m->setParentListModelInfo(m_parentModelInfo);
    m->loadList();
    return m;
}


ImagePointModel::ImagePointModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "ImagePointModel::ImagePointModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "ImagePointModel::ImagePointModel()" << this;
#endif

    init(this);
}

void ImagePointModel::modelListLoaded(const QJsonDocument &reply_)
{
    base::modelListLoaded(reply_);
    for(const ImagePoint *ip: qAsConst(m_data))
    {
        if(!ip) { continue; }
        ip->nextImage();
    }
}

void ImagePointModel::setImageModel(ImageModel *imageModel_)
{
    m_parentImageModel = imageModel_;
}

ImageModel *ImagePointModel::getImageModel()
{
    return m_parentImageModel;
}

const ImagePoint *ImagePointModel::nextImagePointByCoords(qreal x_, qreal y_) const
{
    if(!isListLoadedImpl()) { return nullptr; }

    const ImagePoint *minIp = nullptr;
    qreal mind = 0.0;
    for(const ImagePoint *ip: qAsConst(m_data))
    {
        if(!ip) { continue; }
        ImagePointToNextImageModel *m = ip->getNextImage();
        if(!m) { continue; }
        qreal d = ip->distanceCoef(x_, y_);
        if(!minIp || d < mind)
        {
            mind = d;
            minIp = ip;
        }
    }
    return minIp;
}

QVariant ImagePointModel::nextObjByCoords(qreal x_, qreal y_)
{
    if(!isListLoadedImpl()) { return QVariant(); }

    const ImagePoint *nearIp = nextImagePointByCoords(x_, y_);
    return QVariant::fromValue(static_cast<QObject *>(const_cast<ImagePoint *>(nearIp)));
}

bool ImagePointModel::isNextObjImageByCoords(qreal x_, qreal y_)
{
    if(!isListLoadedImpl()) { return false; }

    const ImagePoint *nearIp = nextImagePointByCoords(x_, y_);
    if(!nearIp) { return false; }
    return !nearIp->getNextImage()->isEmpty();
}

bool ImagePointModel::isNextObjQuestionByCoords(qreal x_, qreal y_)
{
    if(!isListLoadedImpl()) { return false; }

    const ImagePoint *nearIp = nextImagePointByCoords(x_, y_);
    if(!nearIp) { return false; }
    ImagePointToQuestion* iptq = nearIp->getNextQuestion()->getCurrentDataItem();
    return iptq && !iptq->getQuestions()->isEmpty();
}

