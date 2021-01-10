#ifndef QUESTIONANSWER_H
#define QUESTIONANSWER_H


#include <QObject>
#include <QString>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"


class UserQuestionAnswer : public QObject
{
    Q_OBJECT
public:
    explicit UserQuestionAnswer(QObject *parent_ = nullptr);


    Q_PROPERTY(int qaId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int  qaUserId READ userId WRITE setUserId NOTIFY userIdChanged)
    Q_PROPERTY(int qaQuestionId READ questionId WRITE setQuestionId NOTIFY questionIdChanged)
    Q_PROPERTY(QDateTime qaT READ t WRITE setT NOTIFY tChanged)
    Q_PROPERTY(int qaAnswerId READ answerId WRITE setAnswerId NOTIFY answerIdChanged)
    Q_PROPERTY(qreal qaPoints READ points WRITE setPoints NOTIFY pointsChanged)


    class DefaultLayout : public LayoutBase<UserQuestionAnswer>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("quiz-user-question-answer");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &UserQuestionAnswer::m_parentModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &UserQuestionAnswer::m_appId);
            addField<int>("id", "qaId", &UserQuestionAnswer::id, &UserQuestionAnswer::setId);
            addField<int>("user", "qaUserId", &UserQuestionAnswer::userId, &UserQuestionAnswer::setUserId);
            addField<int>("question", "qaQuestionId", &UserQuestionAnswer::questionId, &UserQuestionAnswer::setQuestionId);
            addField<QDateTime>("t", "qaT", &UserQuestionAnswer::t, &UserQuestionAnswer::setT);
            addField<int>("answer", "qaAnswerId", &UserQuestionAnswer::answerId, &UserQuestionAnswer::setAnswerId);
            addField<qreal>("points", "qaPoints", &UserQuestionAnswer::points, &UserQuestionAnswer::setPoints);
        }
    };

    int id() const;
    void setId(const int &id_);
    int userId() const;
    void setUserId(const int &userId_);
    int questionId() const;
    void setQuestionId(const int &questionId_);
    QDateTime t() const;
    void setT(const QDateTime &t_);
    int answerId() const;
    void setAnswerId(const int &answerId_);
    qreal points() const;
    void setPoints(const qreal &points_);

signals:
    void idChanged();
    void userIdChanged();
    void questionIdChanged();
    void tChanged();
    void answerIdChanged();
    void pointsChanged();

private:
    IListModelInfo *m_parentModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    int m_userId = -1;
    int m_questionId = -1;
    QDateTime m_t;
    int m_answerId = -1;
    qreal m_points = 0.0;
};


class UserQuestionAnswerModel : public ListModelBaseOfData<UserQuestionAnswer, UserQuestionAnswerModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<UserQuestionAnswer, UserQuestionAnswerModel>;

public:
    explicit UserQuestionAnswerModel(QObject *parent_ = nullptr);

    LAYOUT_MODEL_IMPL();

public slots:
    void jsonResponseSlot(int errorCode_, const QString &errorCodeStr_, RequestData *request_, const QJsonDocument &reply_)
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


#endif // QUESTIONANSWER_H
