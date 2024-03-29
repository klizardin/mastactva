/*
    Copyright 2021

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

#ifndef USERSTEP_H
#define USERSTEP_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/mergeid.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"


class UserStep : public QObject, public IListModelItem
{
    Q_OBJECT
public:
    explicit UserStep(QObject *parent_ = nullptr);
    virtual ~UserStep() override;

    Q_PROPERTY(int usId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int usUserId READ userId WRITE setUserId NOTIFY userIdChanged)
    Q_PROPERTY(int usGalleryId READ galleryId WRITE setGalleryId NOTIFY galleryIdChanged)
    Q_PROPERTY(int usImageId READ imageId WRITE setImageId NOTIFY imageIdChanged)
    Q_PROPERTY(int usNextImageId READ nextImageId WRITE setNextImageId NOTIFY nextImageIdChanged)
    Q_PROPERTY(int usDescriptionId READ descriptionId WRITE setDescriptionId NOTIFY descriptionIdChanged)
    Q_PROPERTY(int usQuestionId READ questionId WRITE setQuestionId NOTIFY questionIdChanged)
    Q_PROPERTY(int usAnswerId READ answerId WRITE setAnswerId NOTIFY answerIdChanged)
    Q_PROPERTY(QDateTime usT READ t WRITE setT NOTIFY tChanged)


    class DefaultLayout : public LayoutBase<UserStep>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("quiz-user-step");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &UserStep::m_parentModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &UserStep::m_appId);
            addField<int>("id", "usId", &UserStep::id, &UserStep::setId);
            addField<int>("user", "usUserId", &UserStep::userId, &UserStep::setUserId);
            addField<int>("gallery", "usGalleryId", &UserStep::galleryId, &UserStep::setGalleryId);
            addField<int>("image", "usImageId", &UserStep::imageId, &UserStep::setImageId);
            addField<int>("jump_to_next_image", "usNextImageId", &UserStep::nextImageId, &UserStep::setNextImageId);
            addField<int>("view_description", "usDescriptionId", &UserStep::descriptionId, &UserStep::setDescriptionId);
            addField<int>("view_question", "usQuestionId", &UserStep::questionId, &UserStep::setQuestionId);
            addField<int>("set_answer", "usAnswerId", &UserStep::answerId, &UserStep::setAnswerId);
            addField<QDateTime>("t", "usT", &UserStep::t, &UserStep::setT);
            setIdField("id");
            setStoreAfterSave(false);
        }
    };

    int id() const;
    void setId(const int &id_);
    int userId() const;
    void setUserId(const int &userId_);
    int galleryId() const;
    void setGalleryId(const int &galleryId_);
    int imageId() const;
    void setImageId(const int &imageId_);
    int nextImageId() const;
    void setNextImageId(const int &nextImageId_);
    int descriptionId() const;
    void setDescriptionId(const int &descriptionId_);
    int questionId() const;
    void setQuestionId(const int &questionId_);
    int answerId() const;
    void setAnswerId(const int &answerId_);
    QDateTime t() const;
    void setT(const QDateTime &t_);

signals:
    void idChanged();
    void userIdChanged();
    void galleryIdChanged();
    void imageIdChanged();
    void nextImageIdChanged();
    void descriptionIdChanged();
    void questionIdChanged();
    void answerIdChanged();
    void tChanged();

private:
    IListModelInfo *m_parentModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    int m_userId = -1;
    int m_galleryId = -1;
    int m_imageId = -1;
    int m_nextImageId = -1;
    int m_descriptionId = -1;
    int m_questionId = -1;
    int m_answerId = -1;
    QDateTime m_t;
};


class UserStepModel : public ListModelBaseOfData<UserStep, UserStepModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<UserStep, UserStepModel>;

public:
    explicit UserStepModel(QObject *parent_ = nullptr);

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
    void objectNameDerivedChanged();
    void error(const QString &errorCode_, const QString &description_);
};


#endif // USERSTEP_H
