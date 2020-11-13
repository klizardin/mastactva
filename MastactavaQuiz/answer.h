#ifndef ANSWER_H
#define ANSWER_H

#include <QObject>
#include "IModel.h"
#include "imagesource.h"
#include "Layout.h"
#include "Model.h"


class Answer : public QObject
{
    Q_OBJECT
public:
    explicit Answer(QObject *parent_ = nullptr);

    Q_PROPERTY(int answerId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int answerQuestionId READ question WRITE setQuestion NOTIFY questionChanged)
    Q_PROPERTY(QString answerText READ answer WRITE setAnswer NOTIFY answerChanged)
    Q_PROPERTY(qreal answerPoints READ points WRITE setPoints NOTIFY pointsChanged)

    class DefaultLayout : public LayoutBase<Answer>
    {
    public:
        DefaultLayout()
        {
            addSpecial<int>(layout::SpecialFieldEn::appId, &Answer::m_appId);
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &Answer::m_parentModelInfo);
            addField<int>("id", "answerId", &Answer::id, &Answer::setId);
            addField<int>("question", "answerQuestionId", &Answer::question, &Answer::setQuestion);
            addField<QString>("answer", "answerText", &Answer::answer, &Answer::setAnswer);
            addField<qreal>("points", "answerPoints", &Answer::points, &Answer::setPoints);
        }
    };

    int id() const;
    void setId(const int &id_);
    int question() const;
    void setQuestion(const int &questionId_);
    QString answer() const;
    void setAnswer(const QString &answer_);
    qreal points() const;
    void setPoints(const qreal &points_);

signals:
    void idChanged();
    void questionChanged();
    void answerChanged();
    void pointsChanged();

private:
    IListModelInfo *m_parentModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    int m_questionId = -1;
    QString m_answer;
    qreal m_points = 1.0;
};


class AnswerModel : public ListModelBaseOfData<Answer, AnswerModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<Answer, AnswerModel>;

public:
    explicit AnswerModel(QObject *parent_ = nullptr);

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
};



#endif // ANSWER_H
