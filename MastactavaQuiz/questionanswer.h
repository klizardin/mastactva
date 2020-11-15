#ifndef QUESTIONANSWER_H
#define QUESTIONANSWER_H

#include <QObject>
#include <QString>
#include "IModel.h"
#include "imagesource.h"
#include "Layout.h"
#include "Model.h"


class QuestionAnswer : public QObject
{
    Q_OBJECT
public:
    explicit QuestionAnswer(QObject *parent_ = nullptr);


    Q_PROPERTY(int qaId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int  qaUserId READ userId WRITE setUserId NOTIFY userIdChanged)
    Q_PROPERTY(int qaQuestionId READ questionId WRITE setQuestionId NOTIFY questionIdChanged)
    Q_PROPERTY(QDateTime qaT READ t WRITE setT NOTIFY tChanged)
    Q_PROPERTY(int qaAnswerId READ answerId WRITE setAnswerId NOTIFY answerIdChanged)
    Q_PROPERTY(qreal qaPoints READ points WRITE setPoints NOTIFY pointsChanged)


    class DefaultLayout : public LayoutBase<QuestionAnswer>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("user-question-answers");
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &QuestionAnswer::m_parentModelInfo);
            addSpecial<int>(layout::SpecialFieldEn::appId, &QuestionAnswer::m_appId);
            addField<int>("id", "qaId", &QuestionAnswer::id, &QuestionAnswer::setId);
            addField<int>("user", "qaUserId", &QuestionAnswer::userId, &QuestionAnswer::setUserId);
            addField<int>("question", "qaQuestionId", &QuestionAnswer::questionId, &QuestionAnswer::setQuestionId);
            addField<QDateTime>("t", "qaT", &QuestionAnswer::t, &QuestionAnswer::setT);
            addField<int>("answer", "qaAnswerId", &QuestionAnswer::answerId, &QuestionAnswer::setAnswerId);
            addField<qreal>("points", "qaPoints", &QuestionAnswer::points, &QuestionAnswer::setPoints);
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

class QuestionAnswerModel : public ListModelBaseOfData<QuestionAnswer, QuestionAnswerModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<QuestionAnswer, QuestionAnswerModel>;

public:
    explicit QuestionAnswerModel(QObject *parent_ = nullptr);

    LAYOUT_MODEL_IMPL();

public slots:
    void jsonResponseSlot(int errorCode_, RequestData *request_, const QJsonDocument &reply_)
    {
        jsonResponseSlotImpl(errorCode_, request_, reply_);
    }

    void refreshChildrenSlot(QString modelName_)
    {
        refreshChildrenSlotImpl(modelName_);
    }

signals:
    void currentIndexChanged();
    void currentRefChanged();
    void storeAfterSaveChanged();
    void refAppIdChanged();
    void layoutQMLNameChanged();
    void layoutIdFieldChanged();
    void refreshChildren(QString);
    void jsonParamsGetChanged();
    void autoCreateChildrenModelsChanged();
    void listReloaded();
};


#endif // QUESTIONANSWER_H
