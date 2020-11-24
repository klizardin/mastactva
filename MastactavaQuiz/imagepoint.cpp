#include "imagepoint.h"
#include <math.h>
#include "../MastactvaBase/utils.h"


ImagePointToQuestion::ImagePointToQuestion(ImagePointToQuestionModel *parent_ /*= nullptr*/)
    : QObject(parent_)
{
    m_imagePointToQuestionModel = parent_;
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
    if(nullptr == m_questionModel)
    {
        const_cast<ImagePointToQuestion *>(this)->m_questionModel = const_cast<ImagePointToQuestion *>(this)->createQuestionModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(const_cast<QuestionModel *>(m_questionModel)));
}

void ImagePointToQuestion::setQuestionObj(const QVariant &obj_)
{
    if(obj_.isNull() && nullptr != m_questionModel)
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
    m->setLayoutQMLName(m_imagePointToQuestionModel->getQMLLayoutName() + QString("_QuestionModel_") + QVariant::fromValue(m_appId).toString());
    m->registerListModel();
    m->setAutoCreateChildrenModels(true);
    m->setParentListModelInfo(m_parentModelInfo);
    m->loadList();
    return m;
}


ImagePointToQuestionModel::ImagePointToQuestionModel(QObject *parent_ /*= nullptr*/)
    : base(parent_)
{
    init(this);
}


ImagePointToNextImage::ImagePointToNextImage(QObject *parent_ /*= nullptr*/)
    :QObject(parent_)
{
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


ImagePointToNextImageModel::ImagePointToNextImageModel(QObject *parent_ /*= nullptr*/)
    :ListModelBaseOfData<ImagePointToNextImage, ImagePointToNextImageModel>(parent_)
{
    init(this);
}

ImagePoint::ImagePoint(ImagePointModel *parent_ /*= nullptr*/)
    : QObject(parent_)
{
    m_imagePointModel = parent_;
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
    if(nullptr == m_imagePointToNextImage)
    {
        const_cast<ImagePoint *>(this)->m_imagePointToNextImage = const_cast<ImagePoint *>(this)->createImagePointToNextImage();
    }
    return QVariant::fromValue(static_cast<QObject *>(const_cast<ImagePointToNextImageModel *>(m_imagePointToNextImage)));
}

void ImagePoint::setNextImage(const QVariant &obj_)
{
    if(obj_.isNull() && nullptr != m_imagePointToNextImage)
    {
        delete m_imagePointToNextImage;
        m_imagePointToNextImage = nullptr;

        emit nextImageChanged();
    }
}

QVariant ImagePoint::nextQuestion() const
{
    if(nullptr == m_imagePointToQuestionModel)
    {
        const_cast<ImagePoint *>(this)->m_imagePointToQuestionModel = const_cast<ImagePoint *>(this)->createImagePointToQuestionModel();
    }
    return QVariant::fromValue(static_cast<QObject *>(const_cast<ImagePointToQuestionModel *>(m_imagePointToQuestionModel)));
}

void ImagePoint::setNextQuestion(const QVariant &obj_)
{
    if(obj_.isNull() && nullptr != m_imagePointToQuestionModel)
    {
        delete m_imagePointToQuestionModel;
        m_imagePointToQuestionModel = nullptr;

        emit nextQuestionChanged();
    }
}


ImagePointToNextImageModel *ImagePoint::getNextImage() const
{
    nextImage();
    return m_imagePointToNextImage;
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
    m->setLayoutQMLName(m_imagePointModel->getQMLLayoutName() + QString("_ImagePointToNextImageModel_") + QVariant::fromValue(m_appId).toString());
    m->registerListModel();
    m->setParentListModelInfo(m_parentModelInfo);
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
    m->setLayoutQMLName(m_imagePointModel->getQMLLayoutName() + QString("_ImagePointToQuestionModel_") + QVariant::fromValue(m_appId).toString());
    m->registerListModel();
    m->setAutoCreateChildrenModels(true);
    m->setParentListModelInfo(m_parentModelInfo);
    m->loadList();
    return m;
}

ImagePointModel::ImagePointModel(QObject *parent_ /*= nullptr*/)
    :ListModelBaseOfData<ImagePoint, ImagePointModel>(parent_)
{
    init(this);
}

void ImagePointModel::listLoaded(const QJsonDocument &reply_)
{
    base::listLoaded(reply_);
    for(ImagePoint *ip: m_data)
    {
        if(nullptr == ip) { continue; }
        ip->nextImage();
    }
}

ImagePoint *ImagePointModel::nextImagePointByCoords(qreal x_, qreal y_)
{
    if(!isListLoadedImpl()) { return nullptr; }

    ImagePoint *minIp = nullptr;
    qreal mind = 0.0;
    for(ImagePoint *ip: m_data)
    {
        if(nullptr == ip) { continue; }
        ImagePointToNextImageModel *m = ip->getNextImage();
        if(nullptr == m) { continue; }
        qreal d = ip->distanceCoef(x_, y_);
        if(nullptr == minIp || d < mind)
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

    ImagePoint *nearIp = nextImagePointByCoords(x_, y_);
    return QVariant::fromValue(static_cast<QObject *>(nearIp));
}

bool ImagePointModel::isNextObjImageByCoords(qreal x_, qreal y_)
{
    if(!isListLoadedImpl()) { return false; }

    ImagePoint *nearIp = nextImagePointByCoords(x_, y_);
    if(nullptr == nearIp) { return false; }
    return !nearIp->getNextImage()->isEmpty();
}

bool ImagePointModel::isNextObjQuestionByCoords(qreal x_, qreal y_)
{
    if(!isListLoadedImpl()) { return false; }

    ImagePoint *nearIp = nextImagePointByCoords(x_, y_);
    if(nullptr == nearIp) { return false; }
    ImagePointToQuestion* iptq = nearIp->getNextQuestion()->getCurrentDataItem();
    return nullptr != iptq && !iptq->getQuestions()->isEmpty();
}

