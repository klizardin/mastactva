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


class QuestionAnswerModelView : public QObject, public ILocalDataUpdateAPI
{
    Q_OBJECT
public:
    QuestionAnswerModelView(QObject * parent_ = nullptr);
    virtual ~QuestionAnswerModelView() override;

    virtual bool canProcess(const DBRequestBase *r_) const override;
    virtual bool addItemImpl(const QVariant &appId_,
                             const QHash<QString, QVariant> &values_,
                             DBRequestBase *r_) override;
    virtual bool setItemImpl(const QVariant &id_,
                             const QHash<QString, QVariant> &values_,
                             DBRequestBase *r_) override;
    virtual bool delItemImpl(const QVariant &id_,
                             DBRequestBase *r_) override;

protected:
    void addUserQuestionAnswer();
    void addUserQuestionAnswerItem();
    void error();

protected slots:
    void galleryStatisticsModelListReloaded();
    void userQuestionAnswerModelListReloaded();
    void galleryStatisticsModelItemAdded();
    void galleryStatisticsModelItemSet();

private:
    UserQuestionAnswerModel *m_userQuestionAnswerModel = nullptr;
    GalleryStatisticsModel *m_galleryStatisticsModel = nullptr;
    QList<QPair<DBRequestBase *, UserQuestionAnswer *>> m_requests;
    GalleryStatistics *m_galleryStatistics = nullptr;
};

#endif // QUESTIONANSWERMODELVIEW_H
