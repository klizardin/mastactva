#ifndef GALLERYEDITVIEWMODEL_H
#define GALLERYEDITVIEWMODEL_H

#include <QObject>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickFramebufferObject>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLFramebufferObjectFormat>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/qvector3d.h>
#include <QtGui/qmatrix4x4.h>
#include <QtGui/qopenglshaderprogram.h>
#include <QtGui/qopenglfunctions.h>
#include <QOpenGLBuffer>
#include <QSGGeometryNode>
#include <QQuickPaintedItem>
#include <QAbstractListModel>
#include <QImage>
#include <QDateTime>
#include <QString>
#include <QVector>
#include <QHash>
#include <QJsonValue>
#include <QVector2D>


class RequestData;
class JsonRequestData;
class ImagePointsModel;


QDateTime dateTimeFromJsonString(const QString& dateTimeZ);
QString dateTimeToJsonString(const QDateTime &dt_);


class ImageData : public QObject
{
    Q_OBJECT

public:
    ImageData(QObject *parent_ = nullptr, int id_ = -1, const QString& source_ = QString());
    ~ImageData() = default;

    Q_PROPERTY(int id READ getId WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString source READ getSource WRITE setSource NOTIFY sourceChanged)

    int getId() const;
    void setId(int id_);
    const QString& getSource() const;
    void setSource(const QString& source_);
    ImagePointsModel *getImagePoints(QObject *parent_) const;

signals:
    void idChanged();
    void sourceChanged();

protected:
    void initImagePointModel(QObject *parent_);

protected:
    int m_id = -1;
    QString m_source;
    ImagePointsModel *imagePointsModel = nullptr;
};

class GalleryImagesModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum RoleNames {
        IdRole = Qt::UserRole,
        ImageSourceRole = Qt::UserRole + 1,
        ImagePointsRole = Qt::UserRole + 2,
    };

public:
    GalleryImagesModel(QObject *parent = nullptr);
    virtual ~GalleryImagesModel() override;
    // QAbstractItemModel interface
public:
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;

public:
    Q_PROPERTY(bool galleryViewImages READ galleryViewImages WRITE setGalleryViewImages NOTIFY galleryViewImagesChanged)
    Q_PROPERTY(int galleryId READ galleryId WRITE setGalleryId NOTIFY galleryIdChanged)
    Q_PROPERTY(int galleryIndex READ galleryIndex WRITE setGalleryIndex NOTIFY galleryIndexChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)

    Q_INVOKABLE QString currentImageSource() const;
    Q_INVOKABLE QVariant currentImagePoints() const;
    Q_INVOKABLE QVariant currentItem();

    bool galleryViewImages() const;
    void setGalleryViewImages(bool modeShowGalleryViewImages_);
    int galleryId() const;
    void setGalleryId(int galleryId_);
    int galleryIndex() const;
    void setGalleryIndex(int galleryIndex_);
    void startLoadImages();
    int getIdOfIndex(int index_);
    const ImageData *dataItemAt(int index_) const;
    ImageData *dataItemAt(int index_);
    int currentIndex() const;
    void setCurrentIndex(int index_);
    bool isEmpty() const;

signals:
    void galleryIdChanged();
    void galleryViewImagesChanged();
    void galleryIndexChanged();
    void currentIndexChanged();

private slots:
    void onJsonRequestFinished(int errorCode_, RequestData *request_, const QJsonDocument &reply_);
    void ongalleryIdChanged();

protected:
    void clearData();

protected:
    // return the roles mapping to be used by QML
    virtual QHash<int, QByteArray> roleNames() const override;

private:
    bool m_showGalleryViewImages = true;
    int m_galleryId = -1;
    int m_galleryIndex = -1;
    int m_currentIndex = -1;
    RequestData* m_request = nullptr;
    QVector<ImageData *> m_images;
    QHash<int, QByteArray> m_roleNames;
};


class GalleryItemData : public QObject
{
    Q_OBJECT

protected:
    int id = -1;
    QString description;
    QString keywords;
    QDateTime created;
    qreal pointsToPass = 1.0;
    GalleryImagesModel *images = nullptr;
    bool m_ownGallery = false;

public:
    GalleryItemData(QObject* parent_,
                int id_ = -1,
                const QString &description_ = QString(),
                const QString &keywords_ = QString(),
                const QDateTime &created_ = QDateTime(),
                qreal pointsToPass_ = 1.0,
                bool ownGallery_ = false);
    virtual ~GalleryItemData();

    Q_PROPERTY(int id READ getId WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString description READ getDescription WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString keywords READ getKeywords WRITE setKeywords NOTIFY keywordsChanged)
    Q_PROPERTY(QDateTime created READ getCreated WRITE setCreated NOTIFY createdChanged)
    Q_PROPERTY(QString pointsToPass READ getPointsToPass WRITE setPointsToPass NOTIFY pointsToPassChanged)
    Q_PROPERTY(bool ownGallery READ ownGallery WRITE setOwnGallery NOTIFY ownGalleryChanged)
    Q_INVOKABLE QObject* getImages();

    int getId() const;
    const QString &getDescription() const;
    const QString &getKeywords() const;
    const QDateTime &getCreated() const;
    QString getPointsToPass() const;
    GalleryImagesModel *getImagesModel();
    bool ownGallery() const;

    void setId(int id_);
    void setDescription(const QString& description_);
    void setKeywords(const QString& keywords_);
    void setCreated(const QDateTime& created_);
    void setPointsToPass(const QString &pointsToPass_);
    void setOwnGallery(bool ownGallery_);

    bool isEmpty() const;

signals:
    void idChanged();
    void descriptionChanged();
    void keywordsChanged();
    void createdChanged();
    void pointsToPassChanged();
    void ownGalleryChanged();

public:
    static GalleryItemData* fromJson(QObject *parent_, const QJsonValue &jsonValue_, bool &anyError_);
    static GalleryItemData* fromJson(QObject *parent_, const QJsonDocument &jsonValue_, bool &anyError_);
};


class GalleryEditViewModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    // Define the role names to be used
    enum RoleNames {
        GalleryIDRole = Qt::UserRole,
        DescriptionRole = Qt::UserRole+1,
        CreatedRole = Qt::UserRole+2,
        PointsToPassRole = Qt::UserRole+3,
        ImagesRole = Qt::UserRole+4,
        OwnRole = Qt::UserRole+5,
    };

public:
    // constructions
    explicit GalleryEditViewModel(QObject *parent = nullptr);
    virtual ~GalleryEditViewModel() override;

    // QAbstractItemModel interface
public:
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    // properies
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(int currentId READ currentId WRITE setCurrentId NOTIFY currentIdChanged)
    Q_INVOKABLE QObject* getCurrentItem();

    //slots
private slots:
    void onJsonRequestFinished(int errorCode_, RequestData *request_, const QJsonDocument &reply_);

    // methods
public:
    void startLoadGalleries();
    int getIdOfIndex(int index_) const;
    int currentIndex() const;
    void setCurrentIndex(int index_);
    int currentId() const;
    void setCurrentId(int id_);
    void refreshItemAtIndex(int index_);
    int getIndexOfId(int id_) const;
    bool isCurrentGalleryEmpty() const;

    // protected methods
protected:
    void clearData();

    // signals
signals:
    void currentIndexChanged();
    void currentIdChanged();
    void galleryRealoded();

    // data members
protected:
    // return the roles mapping to be used by QML
    virtual QHash<int, QByteArray> roleNames() const override;

private:
    RequestData* m_request = nullptr;
    JsonRequestData* m_refreshRequest = nullptr;
    QHash<int, QByteArray> m_roleNames;
    QVector<GalleryItemData*> m_data;
    int m_currentIndex = -1;
};


class ImagePointToNextImage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString imageSource READ imageSource WRITE setImageSource NOTIFY imageSourceChanged)
    Q_PROPERTY(bool noImageSource READ noImageSource WRITE setNoImageSource NOTIFY noImageSourceChanged)

public:
    ImagePointToNextImage(QObject* parent_ = nullptr, int imagePointId_ = -1);
    virtual ~ImagePointToNextImage() override = default;
    const QString &imageSource();
    void setImageSource(const QString &imageSource_, bool emitFlag_ = true);
    bool noImageSource() const;
    void setNoImageSource(bool noImageSource_, bool emitFlag_ = true);
    void startLoad();
    bool loaded() const;
    void setNextImage(int nextImageId_);
    int id() const;
    void setId(int id_, bool emitFlag_ = true);

protected slots:
    void onJsonRequestFinished(int errorCode_, RequestData *request_, const QJsonDocument &reply_);
    void onSetNextImageJsonRequestFinished(int errorCode_, RequestData *request_, const QJsonDocument &reply_);

    void loadData(const QJsonDocument &reply_);

signals:
    void imagePointToImageLoaded();
    void idChanged();
    void imageSourceChanged();
    void noImageSourceChanged();
    void nextImageSet();

private:
    int m_id = -1;
    int m_imagePointId = -1;
    QString m_imageSource;
    bool m_noImageSource = true;
    RequestData *m_request = nullptr;
    JsonRequestData *m_setNextImageRequest = nullptr;
};

class AnswerData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int questionId READ questionId WRITE setQuestionId NOTIFY questionIdChanged)
    Q_PROPERTY(QString answer READ answer WRITE setAnswer NOTIFY answerChanged)
    Q_PROPERTY(qreal pointsToPass READ pointsToPass WRITE setPointsToPass NOTIFY pointsToPassChanged)


public:
    AnswerData(QObject *parent_ = nullptr,
               int id_ = -1,
               int questionId_ = -1,
               const QString &answer_ = QString(),
               qreal pointsToPass_ = 1.0);
    virtual ~AnswerData() override;

    int id() const;
    void setId(int id_);
    int questionId() const;
    void setQuestionId(int questionId_);
    const QString &answer() const;
    void setAnswer(const QString &answer_);
    qreal pointsToPass() const;
    void setPointsToPass(qreal pointsToPass_);

    static AnswerData *fromJson(QObject *parent_, int questionId_, const QJsonValue &jsonValue_, bool &error_);

signals:
    void idChanged();
    void questionIdChanged();
    void answerChanged();
    void pointsToPassChanged();

private:
    int m_id = -1;
    int m_questionId = -1;
    QString m_answer;
    qreal m_pointsToPass = 1.0;
};

class QuestionAnswersModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int questionId READ questionId WRITE setQuestionId NOTIFY questionIdChanged)

public:
    // Define the role names to be used
    enum RoleNames {
        IDRole = Qt::UserRole,
        QuestionIDRole = Qt::UserRole+1,
        AnswerRole = Qt::UserRole+2,
        PointsToPassRole = Qt::UserRole+3,
    };

public:
    QuestionAnswersModel(QObject *parent_ = nullptr, int questionId_ = -1);
    virtual ~QuestionAnswersModel() override;

    // QAbstractItemModel interface
public:
    virtual int rowCount(const QModelIndex &parent_) const override;
    virtual QVariant data(const QModelIndex &index_, int role_) const override;

    const AnswerData *dataItemAt(int index_) const;
    AnswerData *dataItemAt(int index_);
    QVariant itemAt(int index_);

protected:
    int questionId() const;
    void setQuestionId(int questionId_);
    void startLoad();
    void clearData();

protected:
    // return the roles mapping to be used by QML
    virtual QHash<int, QByteArray> roleNames() const override;


    //slots
private slots:
    void onJsonRequestFinished(int errorCode_, RequestData *request_, const QJsonDocument &reply_);

signals:
    void questionIdChanged();
    void answersLoaded();

    // data members
private:
    QHash<int, QByteArray> m_roleNames;
    int m_questionId = -1;
    RequestData *m_request = nullptr;
    QVector<AnswerData *> m_data;
};

class QuestionData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString questionText READ questionText WRITE setQuestionText NOTIFY questionTextChanged)
    Q_PROPERTY(qreal pointsToPass READ pointsToPass WRITE setPointsToPass NOTIFY pointsToPassChanged)
    Q_PROPERTY(QDateTime created READ created WRITE setCreated NOTIFY createdChanged)

public:
    QuestionData(QObject *parent_ = nullptr,
                 int id_ = -1,
                 const QString &questionText_ = QString(),
                 qreal pointsToPass_ = 1.0,
                 const QDateTime &created_ = QDateTime());
    virtual ~QuestionData() override = default;

public:
    int id() const;
    void setId(int id_);
    const QString &questionText() const;
    void setQuestionText(const QString &questionText_);
    qreal pointsToPass() const;
    void setPointsToPass(qreal pointsToPass_);
    const QDateTime &created() const;
    void setCreated(const QDateTime &created_);

    static QuestionData *fromJson(QObject *parent_, const QJsonValue &jsonValue_, bool &error_);
    static QuestionData *fromJson(QObject *parent_, const QJsonDocument &jsonValue_, bool &error_);

signals:
    void idChanged();
    void questionTextChanged();
    void pointsToPassChanged();
    void createdChanged();

private:
    int m_id = -1;
    QString m_questionText;
    qreal m_pointsToPass = 1.0;
    QDateTime m_created;
};

class ImagePointToQuestion : public QObject
{
    Q_OBJECT
    //Q_PROPERTY(QVariant answers READ answers WRITE setAnswers NOTIFY answersChanged)
    Q_PROPERTY(int questionId READ questionId WRITE setQuestionId NOTIFY questionIdChanged)
    Q_PROPERTY(QString question READ question WRITE setQuestion NOTIFY questionChanged)
    Q_PROPERTY(qreal pointsToPass READ pointsToPass WRITE setPointsToPass NOTIFY pointsToPassChanged)
    Q_PROPERTY(QVariant questionObj READ questionObj WRITE setQuestionObj NOTIFY questionObjChanged)

public:
    ImagePointToQuestion(QObject *parent_ = nullptr, int imagePointId_ = -1);
    virtual ~ImagePointToQuestion() override;

    int questionId() const;
    void setQuestionId(int questionId_);
    QString question() const;
    void setQuestion(const QString &question_);
    qreal pointsToPass() const;
    void setPointsToPass(qreal pointsToPass_);
    void startLoad();
    void startLoad2();
    QVariant questionObj() const;
    void setQuestionObj(const QVariant &questionObj_);
    void refresh();
    bool questionIdLoaded() const;

    QuestionData *questionData();

protected slots:
    void onJsonRequestFinished(int errorCode_, RequestData *request_, const QJsonDocument &reply_);

signals:
    //void answersChanged();
    void questionIdChanged();
    void questionChanged();
    void pointsToPassChanged();
    void imagePointToQuestionLoaded();
    void imagePointToQuestionTextLoaded();
    void questionObjChanged();

private:
    int m_imagePointId = -1;
    int m_questionId = -1;
    QuestionData *m_question = nullptr;
    RequestData *m_request1 = nullptr;
    RequestData *m_request2 = nullptr;
    bool m_forceRefresh = false;
};


class ImagePointData : public QObject
{
    Q_OBJECT

public:
    ImagePointData(QObject *parent_,
                   int sourceImageId_ = -1,
                   int pointId_ = -1,
                   qreal x_ = 0.5,
                   qreal y_ = 0.5,
                   qreal weight_ = 1.0,
                   const QDateTime &created_ = QDateTime());
    virtual ~ImagePointData() override;

public:
    Q_PROPERTY(int imageSourceId READ getSourceImageId WRITE setSourceImageId NOTIFY imageSourceIdChanged)
    Q_PROPERTY(int pointId READ pointId WRITE setPointId NOTIFY pointIdChanged)
    Q_PROPERTY(qreal xCoord READ xCoord WRITE setXCoord NOTIFY xCoordChanged)
    Q_PROPERTY(qreal yCoord READ yCoord WRITE setYCoord NOTIFY yCoordChanged)
    Q_PROPERTY(qreal weight READ weight WRITE setWeight NOTIFY weightChanged)
    Q_PROPERTY(QVariant toNextImage READ toNextImage WRITE setToNextImage NOTIFY toNextImageChanged)
    Q_PROPERTY(QVariant toQuestion READ toQuestion WRITE setToQuestion NOTIFY toQuestionChanged)
    Q_PROPERTY(QDateTime created READ created WRITE setCreated NOTIFY createdChanged)

public:
    int getSourceImageId() const;
    void setSourceImageId(int sourceImageId_);
    int pointId() const;
    void setPointId(int id_);
    qreal xCoord() const;
    void setXCoord(qreal x_);
    qreal yCoord() const;
    void setYCoord(qreal y_);
    qreal weight() const;
    void setWeight(qreal weight_);
    QVariant toNextImage() const;
    void setToNextImage(QVariant toNextImage_);
    QVariant toQuestion() const;
    void setToQuestion(QVariant toQuestion_);
    ImagePointToQuestion *getQuestion();
    ImagePointToNextImage *getNextImageData();
    const QDateTime &created() const;
    void setCreated(const QDateTime &created_);

    static ImagePointData *fromJson(QObject *parent_, int sourceImageId_, const QJsonValue& jsonObject_, bool& error_);

    void saveData();
    void createData(bool pointToNextImage_);
    void createTemplateData();

    void removePoint();

private:
    void createTemplImageData();
    void cretateTemplQuestionData();
    //void removePoint(bool tryToLoad_);
    void removeImagePoint();
    //void removeQuestion();

protected slots:
    void onDataSavedRequestFinished(int errorCode_, RequestData *request_, const QJsonDocument &reply_);
    void onDataCreatedRequestFinished(int errorCode_, RequestData *request_, const QJsonDocument &reply_);
    void onQuestionCreatedRequestFinished(int errorCode_, RequestData *request_, const QJsonDocument &reply_);
    void onAnswerCreatedRequestFinished(int errorCode_, RequestData *request_, const QJsonDocument &reply_);
    void onPointToQuestionRequestFinished(int errorCode_, RequestData *request_, const QJsonDocument &reply_);
    void onPointToImageRequestFinished(int errorCode_, RequestData *request_, const QJsonDocument &reply_);
    void onPointRemovedSlot(int errorCode_, RequestData *request_, const QJsonDocument &reply_);
    //void onQuestionRemovedSlot(int errorCode_, RequestData *request_, const QJsonDocument &reply_);
    //void imagePointToQuestionLoadedSlot();


signals:
    void imageSourceIdChanged();
    void pointIdChanged();
    void xCoordChanged();
    void yCoordChanged();
    void weightChanged();
    void toNextImageChanged();
    void toQuestionChanged();
    void onDataSaved(int pointId_);
    void onDataCreated(int pointId_);
    void onTemplateDataCreated(int pointId_);
    void onPointRemoved();
    void createdChanged();

private:
    int m_sourceImageId = -1;
    int m_pointId = -1;
    qreal m_x = 0.5;
    qreal m_y = 0.5;
    qreal m_weight = 1.0;
    ImagePointToNextImage *m_imagePointToNextImage = nullptr;
    ImagePointToQuestion *m_imagePointToQuestion = nullptr;
    QDateTime m_created;
    bool m_pointToNextImage = true;
    int m_questionID = -1;
    int m_nextImageId = -1;

    JsonRequestData* m_saveDataRequest = nullptr;
    JsonRequestData* m_createDataRequest = nullptr;
    JsonRequestData* m_createQuestionRequest = nullptr;
    JsonRequestData* m_createAnswerRequest = nullptr;
    JsonRequestData* m_createImagePointToQuestionRequest = nullptr;
    JsonRequestData* m_createImagePointToImageRequest = nullptr;
    JsonRequestData* m_removeImagePointRequest = nullptr;
    JsonRequestData* m_removeQuestionRequest = nullptr;
};

class ImagePointsModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum RoleNames {
        IDRole = Qt::UserRole,
        XRole = Qt::UserRole + 1,
        YRole = Qt::UserRole + 2,
        WeightRole = Qt::UserRole + 3,
        ToNextImageRole = Qt::UserRole + 4,
        ToQuestionRole = Qt::UserRole + 5,
    };

public:
    ImagePointsModel(QObject *parent_);
    virtual ~ImagePointsModel() override;

    // QAbstractItemModel interface
public:
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    Q_INVOKABLE void setCurrentPointParams(int index_, qreal x_, qreal y_, qreal weight_);
    Q_INVOKABLE int addTempPoint(qreal x_, qreal y_, qreal weight_);
    Q_INVOKABLE void removeTempPoint(int index_);
    Q_INVOKABLE void resetValuesAtIndex(int index_, qreal x_, qreal y_, qreal weight_);
    Q_INVOKABLE void savePointTemplate(int index_,bool pointToNextImage_);
    Q_INVOKABLE void savePoint(int index_);
    Q_INVOKABLE int getSize() const;
    Q_INVOKABLE void refreshItems(int index_);
    Q_INVOKABLE bool isEmpty() const;
    Q_INVOKABLE QVariant itemAt(int index_);

public:
    void setSourceImageId(int sourceImageId_);
    void startLoadImagePoints();
    ImagePointData *getAt(int index_);
    ImagePointData *getById(int pointId_);
    int getIndexById(int pointId_) const;

signals:
    void imagePointsLoaded();
    void onDataSaved();
    void onImagePointsRefreshed();
    void imagePointsCountChanged();

    //slots
private slots:
    void onJsonRequestFinished(int errorCode_, RequestData *request_, const QJsonDocument &reply_);
    void onImagePointDataSavedSlot(int pointId_);
    void onImagePointDataCreated1Slot(int pointId_);
    void onImagePointDataCreated2Slot(int pointId_);

protected:
    void clearData();

protected:
    // return the roles mapping to be used by QML
    virtual QHash<int, QByteArray> roleNames() const override;

private:
    QHash<int, QByteArray> m_roleNames;
    int m_sourceImageId = -1;
    QVector<ImagePointData *> m_data;
    RequestData* m_request = nullptr;
};


class VoronoyGraphItem : public QQuickItem
{
    Q_OBJECT
    QML_NAMED_ELEMENT(VoronoyDiagram)
    Q_PROPERTY(QVariant model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QVariant color READ color WRITE setColor NOTIFY colorChanged)

public:
    explicit VoronoyGraphItem(QQuickItem *parent_ = nullptr);
    virtual ~VoronoyGraphItem() override;

    QVariant model();
    void setModel(QVariant model_);
    QVariant color();
    void setColor(QVariant color_);

    Q_INVOKABLE int pointsCount() const;
    Q_INVOKABLE void addPoint(qreal x_, qreal y_, qreal weight_);
    Q_INVOKABLE void setPoint(int index_, qreal x_, qreal y_, qreal weight_);
    Q_INVOKABLE void removePointAt(int index_);
    Q_INVOKABLE void resetPoints();
    Q_INVOKABLE void modelUpdated();

signals:
    void modelChanged();
    void colorChanged();

protected:
    virtual QSGNode *updatePaintNode(QSGNode *oldNode_, UpdatePaintNodeData *upnd_) override;
    virtual void geometryChanged(const QRectF &newGeometry_, const QRectF &oldGeometry_) override;

    void setPointsFromModel();

protected slots:
    void imagePointsLoadedSlot();
    void onImagePointsRefreshedSlot();

private:
    ImagePointsModel *m_model = nullptr;
    QColor m_color;
    QVector<QVector2D> m_points;
    QVector<qreal> m_weights;
    bool m_pointsChanged = false;
    bool m_geometryChanged = true;
    int m_oldPointsCount = -1;
};

class VoronoyNode : public QSGGeometryNode
{
public:
    VoronoyNode(const QVector<QVector2D> &points_, const QVector<qreal> &weights_, const QRectF &rect_);

    void updateGeometry(const QRectF &bounds_, const QVector<QVector2D> &points_, const QVector<qreal> &weights_, const QVector3D &color_);

private:
    QSGGeometry m_geometry;
};


class DescriptionData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int imageId READ imageId WRITE setImageId NOTIFY imageIdChanged)
    Q_PROPERTY(QDateTime fromDateTime READ fromDateTime WRITE setFromDateTime NOTIFY fromDateTimeChanged)
    Q_PROPERTY(QString descriptionStr READ descriptionStr WRITE setDescriptionStr NOTIFY descriptionStrChanged)

public:
    DescriptionData(QObject * parent_ = nullptr,
                    int id_ = -1,
                    int imageId_ = -1,
                    const QDateTime &fromDateTime_ = QDateTime(),
                    const QString &descriptionStr_ = QString()
            );
    virtual ~DescriptionData() override = default;

public:
    static DescriptionData* fromJson(QObject *parent_, int imageId_, const QJsonValue &value_, bool &error_);

    int id() const;
    void setId(int id_);
    int imageId() const;
    void setImageId(int imageId_);
    const QDateTime &fromDateTime() const;
    void setFromDateTime(const QDateTime &fromDateTime_);
    const QString &descriptionStr() const;
    void setDescriptionStr(const QString &descriptionStr_);

signals:
    void idChanged();
    void imageIdChanged();
    void fromDateTimeChanged();
    void descriptionStrChanged();

private:
    int m_id = -1;
    int m_imageId = -1;
    QDateTime m_fromDateTime;
    QString m_descriptionStr;
};


class DescriptionModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int imageID READ imageID WRITE setImageID NOTIFY imageIDChanged)

public:
    enum RoleNames {
        IDRole = Qt::UserRole,
        ImageIDRole = Qt::UserRole + 1,
        FromDateTimeRole = Qt::UserRole + 2,
        DescriptionRole = Qt::UserRole + 3,
    };

public:
    DescriptionModel(QObject *parent_ = nullptr);
    virtual ~DescriptionModel() override;

public:
    // QAbstractItemModel interface
    virtual int rowCount(const QModelIndex &parent_) const override;
    virtual QVariant data(const QModelIndex &index_, int role_) const override;
    virtual bool setData(const QModelIndex &index_, const QVariant &value_, int role_ = Qt::EditRole) override;

    Q_INVOKABLE QVariant itemAt(int index_);
    Q_INVOKABLE void setItemAt(int index_, QVariant itemVal_);
    Q_INVOKABLE int getIdOfIndex(int index_) const;
    Q_INVOKABLE int getIndexOfId(int id_) const;
    Q_INVOKABLE void removeById(int id_);

    void startLoadDescriptions();

protected:
    int imageID() const;
    void setImageID(int imageId_);
    void clearData();

protected slots:
    void loadDescriptionsJsonRequestFinished(int errorCode_, RequestData *request_, const QJsonDocument &reply_);

signals:
    void imageIDChanged();
    void descriptionLoaded();

protected:
    // return the roles mapping to be used by QML
    virtual QHash<int, QByteArray> roleNames() const override;

private:
    QHash<int, QByteArray> m_roleNames;
    int m_imageId = -1;
    QVector<DescriptionData *> m_data;
    RequestData* m_request = nullptr;
};


#endif // GALLERYEDITVIEWMODEL_H
