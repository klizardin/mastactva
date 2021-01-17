#ifndef QUESTIONANSWERMODELVIEW_H
#define QUESTIONANSWERMODELVIEW_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaBase/dbrequestinfo.h"
#include "../MastactvaModels/questionanswer.h"
#include "../MastactvaModels/gallerystatistics.h"
#include "../MastactvaModels/imagepoint.h"
#include "../MastactvaModels/image.h"


class QuestionAnswerModelView : public QObject, public ILocalDataAPI
{
    Q_OBJECT
public:
    QuestionAnswerModelView(QObject * parent_ = nullptr);
    virtual ~QuestionAnswerModelView() override;

    virtual bool canProcess(const DBRequestInfo *r_) const override;
    virtual bool getListImpl(DBRequestInfo *r_) override;
    virtual bool addItemImpl(const QVariant &appId_, const QHash<QString, QVariant> &values_, DBRequestInfo *r_) override;
    virtual bool setItemImpl(const QVariant &id_, const QHash<QString, QVariant> &values_, DBRequestInfo *r_) override;
    virtual bool delItemImpl(const QVariant &id_, DBRequestInfo *r_) override;

protected:
    void loadStatisticsForGallery();
    void imagePointModelListReloaded();

protected slots:
    void imagePointToQuestionModelListReloaded();

private:
    UserQuestionAnswerModel *m_userQuestionAnswerModel = nullptr;
    ImagePointToQuestionModel *m_imagePointToQuestionModel = nullptr;
    ImagePointModel *m_imagePointModel = nullptr;
    ImageModel *m_imageModel = nullptr;
    GalleryStatisticsModel *m_galleryStatisticsModel = nullptr;
    QList<QPair<DBRequestInfo *, UserQuestionAnswer *>> m_requests;
    ImagePointToQuestion *m_imagePointToQuestion = nullptr;
    ImagePoint *m_imagePoint = nullptr;
};

#endif // QUESTIONANSWERMODELVIEW_H
