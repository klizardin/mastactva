#ifndef IMAGEPOINT_H
#define IMAGEPOINT_H


#include <QObject>
#include <QDateTime>
#include <QVariant>
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaModels/imagepointeffect.h"
#include "../MastactvaModels/question.h"


class ImagePointToQuestionModel;
class ImageModel;


class ImagePointToQuestion : public QObject
{
    Q_OBJECT
public:
    explicit ImagePointToQuestion(ImagePointToQuestionModel *parent_ = nullptr);

    Q_PROPERTY(int iptqId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int iptqImagePointId READ imagePointId WRITE setImagePointId NOTIFY imagePointIdChanged)
    Q_PROPERTY(int iptqQuestionId READ questionId WRITE setQuestionId NOTIFY questionIdChanged)
    Q_PROPERTY(QVariant iptqQuestionObj READ questionObj WRITE setQuestionObj NOTIFY questionObjChanged)


    class DefaultLayout : public LayoutBase<ImagePointToQuestion>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("image-point-to-question");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &ImagePointToQuestion::m_parentModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &ImagePointToQuestion::m_appId);
            addField<int>("id", "iptqId", &ImagePointToQuestion::id, &ImagePointToQuestion::setId);
            addField<int>("image_point", "iptqImagePointId", &ImagePointToQuestion::imagePointId, &ImagePointToQuestion::setImagePointId);
            addField<int>("question", "iptqQuestionId", &ImagePointToQuestion::questionId, &ImagePointToQuestion::setQuestionId);
            addModel<QuestionModel>("iptqQuestionObj", &ImagePointToQuestion::m_questionModel, &ImagePointToQuestion::createQuestionModel);
            /* 1:1 */
            setIdField("id");
        }
    };


    int id() const;
    void setId(const int &id_);
    int imagePointId() const;
    void setImagePointId(const int &imagePointId_);
    int questionId() const;
    void setQuestionId(const int &questionId_);
    QVariant questionObj() const;
    void setQuestionObj(const QVariant &obj_);
    QuestionModel *getQuestions();

protected:
    QuestionModel *createQuestionModel();

signals:
    void idChanged();
    void imagePointIdChanged();
    void questionIdChanged();
    void questionObjChanged();

private:
    ImagePointToQuestionModel *m_imagePointToQuestionModel = nullptr;
    IListModelInfo *m_parentModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    int m_imagePointId = -1;
    int m_questionId = -1;
    QuestionModel *m_questionModel = nullptr;
};


class ImagePointToQuestionModel : public ListModelBaseOfData<
        ImagePointToQuestion,
        ImagePointToQuestionModel
        >
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<ImagePointToQuestion, ImagePointToQuestionModel>;

public:
    explicit ImagePointToQuestionModel(QObject *parent_ = nullptr);

    LAYOUT_MODEL_IMPL();

public slots:
    void jsonResponseSlot(int errorCode_,
                          const QString &errorCodeStr_,
                          RequestData *request_,
                          const QJsonDocument &reply_)
    {
        jsonResponseSlotImpl(errorCode_, errorCodeStr_, request_, reply_);
    }

    void refreshChildrenSlot(QString modelName_)
    {
        refreshChildrenSlotImpl(modelName_);
    }

signals:
    void currentIndexChanged();
    void currentItemChanged();
    void currentRefChanged();
    void storeAfterSaveChanged();
    void refAppIdChanged();
    void refValueChanged();
    void layoutQMLNameChanged();
    void layoutIdFieldChanged();
    void refreshChildren(QString);
    void jsonParamsGetChanged();
    void autoCreateChildrenModelsChanged();
    void autoCreateChildrenModelsOnSelectChanged();
    void listReloaded();
    void itemAdded();
    void itemSet();
    void itemDeleted();
    void outputModelChanged();
    void sortFieldsChanged();
    void readonlyChanged();
    void error(const QString &errorCode_, const QString &description_);
};


class ImagePointToNextImageModel;


class ImagePointToNextImage : public QObject, protected IListModelInfoObjectImpl
{
    Q_OBJECT

public:
    explicit ImagePointToNextImage(ImagePointToNextImageModel *parent_ = nullptr);

    Q_PROPERTY(int iptniId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int iptniImagePointId READ imagePointId WRITE setImagePointId NOTIFY imagePointIdChanged)
    Q_PROPERTY(int iptniNextImage READ nextImage WRITE setNextImage NOTIFY nextImageChanged)


    class DefaultLayout : public LayoutBase<ImagePointToNextImage>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("image-point-to-next-image");
            addSpecial<int>(layout::SpecialFieldEn::appId, &ImagePointToNextImage::m_appId);
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &ImagePointToNextImage::m_parentModelInfo);
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::objectModelInfo, &ImagePointToNextImage::m_objectModelInfo);
            addField<int>("id", "iptniId", &ImagePointToNextImage::id, &ImagePointToNextImage::setId);
            addField<int>("image_point", "iptniImagePointId", &ImagePointToNextImage::imagePointId, &ImagePointToNextImage::setImagePointId);
            addField<int>("next_image", "iptniNextImage", &ImagePointToNextImage::nextImage, &ImagePointToNextImage::setNextImage);
            setIdField("id");
        }
    };

    int id() const;
    void setId(const int &id_);
    int imagePointId() const;
    void setImagePointId(const int &imagePointId_);
    int nextImage() const;
    void setNextImage(const int &nextImage_);

protected:
    virtual void loadChildrenVF() override;
    virtual void objectLoadedVF() override;

    QString getObjectName() const;

signals:
    void idChanged();
    void imagePointIdChanged();
    void nextImageChanged();

private:
    ImagePointToNextImageModel *m_parentModel = nullptr;
    IListModelInfo *m_parentModelInfo = nullptr;
    IListModelInfo *m_objectModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    int m_imagePointId = -1;
    int m_nextImage = -1;
};


class ImagePointToNextImageModel : public ListModelBaseOfData<
        ImagePointToNextImage,
        ImagePointToNextImageModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<ImagePointToNextImage, ImagePointToNextImageModel>;

public:
    explicit ImagePointToNextImageModel(QObject *parent_ = nullptr);

    void setImageModel(ImageModel *imageModel_);
    ImageModel *getImageModel();


    LAYOUT_MODEL_IMPL();

public slots:
    void jsonResponseSlot(int errorCode_,
                          const QString &errorCodeStr_,
                          RequestData *request_,
                          const QJsonDocument &reply_)
    {
        jsonResponseSlotImpl(errorCode_, errorCodeStr_, request_, reply_);
    }

    void refreshChildrenSlot(QString modelName_)
    {
        refreshChildrenSlotImpl(modelName_);
    }

signals:
    void currentIndexChanged();
    void currentItemChanged();
    void currentRefChanged();
    void storeAfterSaveChanged();
    void refAppIdChanged();
    void refValueChanged();
    void layoutQMLNameChanged();
    void layoutIdFieldChanged();
    void refreshChildren(QString);
    void jsonParamsGetChanged();
    void autoCreateChildrenModelsChanged();
    void autoCreateChildrenModelsOnSelectChanged();
    void listReloaded();
    void itemAdded();
    void itemSet();
    void itemDeleted();
    void outputModelChanged();
    void sortFieldsChanged();
    void readonlyChanged();
    void error(const QString &errorCode_, const QString &description_);

private:
    ImageModel *m_parentImageModel = nullptr;
};


class ImagePointModel;


class ImagePoint : public QObject
{
    Q_OBJECT
public:
    explicit ImagePoint(ImagePointModel *parent_ = nullptr);

    Q_PROPERTY(int ipId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int ipImageId READ imageId WRITE setImageId NOTIFY imageIdChanged)
    Q_PROPERTY(qreal ipXCoord READ xCoord WRITE setXCoord NOTIFY xCoordChanged)
    Q_PROPERTY(qreal ipYCoord READ yCoord WRITE setYCoord NOTIFY yCoordChanged)
    Q_PROPERTY(qreal ipWeight READ weight WRITE setWeight NOTIFY weightChanged)
    Q_PROPERTY(QDateTime ipCreated READ created WRITE setCreated NOTIFY createdChanged)
    Q_PROPERTY(QVariant nextImage READ nextImage WRITE setNextImage NOTIFY nextImageChanged)
    Q_PROPERTY(QVariant nextQuestion READ nextQuestion WRITE setNextQuestion NOTIFY nextQuestionChanged)
    Q_PROPERTY(QVariant ipEffect READ effect WRITE setEffect NOTIFY effectChanged)

    class DefaultLayout : public LayoutBase<ImagePoint>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("image-point");
            addSpecial<int>(layout::SpecialFieldEn::appId, &ImagePoint::m_appId);
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &ImagePoint::m_parentModelInfo);
            addField<int>("id", "ipId", &ImagePoint::id, &ImagePoint::setId);
            addField<int>("image", "ipImageId", &ImagePoint::imageId, &ImagePoint::setImageId);
            addField<qreal>("x", "ipXCoord", &ImagePoint::xCoord, &ImagePoint::setXCoord);
            addField<qreal>("y", "ipYCoord", &ImagePoint::yCoord, &ImagePoint::setYCoord);
            addField<qreal>("weight", "ipWeight", &ImagePoint::weight, &ImagePoint::setWeight);
            addField<QDateTime>("created", "ipCreated", &ImagePoint::created, &ImagePoint::setCreated);
            addModel<ImagePointToNextImageModel>("nextImage", &ImagePoint::m_imagePointToNextImage, &ImagePoint::createImagePointToNextImage);
            /* 1:N */
            addModel<ImagePointToQuestionModel>("nextQuestion", &ImagePoint::m_imagePointToQuestionModel, &ImagePoint::createImagePointToQuestionModel);
            /* 1:N */
            addModel<ImagePointEffectModel>("ipEffect", &ImagePoint::m_imagePointEffectModel, &ImagePoint::createImagePointEffectModel);
            /* 1:N(1) */
            setIdField("id");
        }
    };

    int id() const;
    void setId(const int &id_);
    int imageId() const;
    void setImageId(const int &imageId_);
    qreal xCoord() const;
    void setXCoord(const qreal &xCoord_);
    qreal yCoord() const;
    void setYCoord(const qreal &yCoord_);
    qreal weight() const;
    void setWeight(const qreal &weight_);
    QDateTime created() const;
    void setCreated(const QDateTime &created_);
    QVariant nextImage() const;
    void setNextImage(const QVariant &obj_);
    QVariant nextQuestion() const;
    void setNextQuestion(const QVariant &obj_);
    QVariant effect() const;
    void setEffect(const QVariant &obj_);

    ImagePointToNextImageModel *getNextImage() const;
    ImagePointToQuestionModel *getNextQuestion() const;

    qreal distanceCoef(qreal x_, qreal y_) const;

signals:
    void idChanged();
    void imageIdChanged();
    void xCoordChanged();
    void yCoordChanged();
    void weightChanged();
    void createdChanged();
    void nextImageChanged();
    void nextQuestionChanged();
    void effectChanged();

protected:
    ImagePointToNextImageModel *createImagePointToNextImage();
    ImagePointToQuestionModel *createImagePointToQuestionModel();
    ImagePointEffectModel *createImagePointEffectModel();

private:
    ImagePointModel *m_imagePointModel = nullptr;
    IListModelInfo *m_parentModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    int m_imageId = -1;
    qreal m_x = 0.0;
    qreal m_y = 0.0;
    qreal m_weight = 1.0;
    QDateTime m_created;
    ImagePointToNextImageModel *m_imagePointToNextImage = nullptr;
    ImagePointToQuestionModel *m_imagePointToQuestionModel = nullptr;
    ImagePointEffectModel *m_imagePointEffectModel = nullptr;
};


class ImagePointModel : public ListModelBaseOfData<
        ImagePoint,
        ImagePointModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<ImagePoint, ImagePointModel>;

public:
    explicit ImagePointModel(QObject *parent_ = nullptr);

    LAYOUT_MODEL_IMPL();

    Q_INVOKABLE QVariant nextObjByCoords(qreal x_, qreal y_);
    Q_INVOKABLE bool isNextObjImageByCoords(qreal x_, qreal y_);
    Q_INVOKABLE bool isNextObjQuestionByCoords(qreal x_, qreal y_);

    void setImageModel(ImageModel *imageModel_);
    ImageModel *getImageModel();

protected:
    const ImagePoint *nextImagePointByCoords(qreal x_, qreal y_) const;

public slots:
    void jsonResponseSlot(int errorCode_,
                          const QString &errorCodeStr_,
                          RequestData *request_,
                          const QJsonDocument &reply_)
    {
        jsonResponseSlotImpl(errorCode_, errorCodeStr_, request_, reply_);
    }

    void refreshChildrenSlot(QString modelName_)
    {
        refreshChildrenSlotImpl(modelName_);
    }

    virtual void modelListLoaded(const QJsonDocument &reply_) override;

signals:
    void currentIndexChanged();
    void currentItemChanged();
    void currentRefChanged();
    void storeAfterSaveChanged();
    void refAppIdChanged();
    void refValueChanged();
    void layoutQMLNameChanged();
    void layoutIdFieldChanged();
    void refreshChildren(QString);
    void jsonParamsGetChanged();
    void autoCreateChildrenModelsChanged();
    void autoCreateChildrenModelsOnSelectChanged();
    void listReloaded();
    void itemAdded();
    void itemSet();
    void itemDeleted();
    void outputModelChanged();
    void sortFieldsChanged();
    void readonlyChanged();
    void error(const QString &errorCode_, const QString &description_);

private:
    ImageModel *m_parentImageModel = nullptr;
};


#endif // IMAGEPOINT_H
