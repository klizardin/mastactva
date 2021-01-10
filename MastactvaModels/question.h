#ifndef QUESTION_H
#define QUESTION_H


#include <QObject>
#include <QDateTime>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"
#include "../MastactvaModels/answer.h"
#include "../MastactvaModels/questionanswer.h"


class QuestionModel;


class Question : public QObject
{
    Q_OBJECT
public:
    explicit Question(QuestionModel *parent_ = nullptr);

    Q_PROPERTY(int questionId READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString questionText READ question WRITE setQuestion NOTIFY questionChanged)
    Q_PROPERTY(qreal questionPointsToPass READ pointsToPass WRITE setPointsToPass NOTIFY pointsToPassChanged)
    Q_PROPERTY(QDateTime questionCreated READ created WRITE setCreated NOTIFY createdChanged)
    Q_PROPERTY(QVariant questionAnswers READ answers WRITE setAnswers NOTIFY answersChanged)
    Q_PROPERTY(QVariant userQuestionAnswer READ userQuestionAnswer NOTIFY userQuestionAnswerChanged)

    class DefaultLayout : public LayoutBase<Question>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("image-question");
            addSpecial<int>(layout::SpecialFieldEn::appId, &Question::m_appId);
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &Question::m_parentModelInfo);
            addField<int>("id", "quiestionId", &Question::id, &Question::setId);
            addField<QString>("question", "questionText", &Question::question, &Question::setQuestion);
            addField<qreal>("points_to_pass", "questionPointsToPass", &Question::pointsToPass, &Question::setPointsToPass);
            addField<QDateTime>("created", "questionCreated", &Question::created, &Question::setCreated);
            addModel<AnswerModel>("questionAnswers", &Question::m_answerModel, &Question::createAnswerModel);
            addModel<UserQuestionAnswerModel>("userQuestionAnswer", &Question::m_userQuestionAnswerModel, &Question::createUserQuestionAnswerModel);
        }
    };

    int id() const;
    void setId(const int &id_);
    QString question() const;
    void setQuestion(const QString &question_);
    qreal pointsToPass() const;
    void setPointsToPass(const qreal &pointsToPass_);
    QDateTime created() const;
    void setCreated(const QDateTime &created_);
    QVariant answers() const;
    void setAnswers(const QVariant &answers_);
    QVariant userQuestionAnswer() const;

protected:
    AnswerModel *createAnswerModel();
    UserQuestionAnswerModel *createUserQuestionAnswerModel();

signals:
    void idChanged();
    void questionChanged();
    void pointsToPassChanged();
    void createdChanged();
    void answersChanged();
    void userQuestionAnswerChanged();

private:
    IListModelInfo *m_parentModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    QString m_question;
    qreal m_pointsToPass = 1.0;
    QDateTime m_created;
    AnswerModel *m_answerModel = nullptr;
    QuestionModel *m_questionModel = nullptr;
    UserQuestionAnswerModel *m_userQuestionAnswerModel = nullptr;
};


class QuestionModel : public ListModelBaseOfData<Question, QuestionModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<Question, QuestionModel>;

public:
    explicit QuestionModel(QObject *parent_ = nullptr);

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


#endif // QUESTION_H
