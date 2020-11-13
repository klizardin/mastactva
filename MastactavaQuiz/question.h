#ifndef QUESTION_H
#define QUESTION_H


#include <QObject>
#include <QDateTime>
#include "IModel.h"
#include "Layout.h"
#include "Model.h"


class Question : public QObject
{
    Q_OBJECT
public:
    explicit Question(QObject *parent_ = nullptr);

    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString question READ question WRITE setQuestion NOTIFY questionChanged)
    Q_PROPERTY(qreal pointsToPass READ pointsToPass WRITE setPointsToPass NOTIFY pointsToPassChanged)
    Q_PROPERTY(QDateTime created READ created WRITE setCreated NOTIFY createdChanged)

    class DefaultLayout : public LayoutBase<Question>
    {
    public:
        DefaultLayout()
        {
            addSpecial<int>(layout::SpecialFieldEn::appId, &Question::m_appId);
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &Question::m_parentModelInfo);
            addField<int>("id", "quiestionId", &Question::id, &Question::setId);
            addField<QString>("question", "questionText", &Question::question, &Question::setQuestion);
            addField<qreal>("points_to_pass", "questionPointsToPass", &Question::pointsToPass, &Question::setPointsToPass);
            addField<QDateTime>("created", "questionCreated", &Question::created, &Question::setCreated);
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

signals:
    void idChanged();
    void questionChanged();
    void pointsToPassChanged();
    void createdChanged();

private:
    IListModelInfo *m_parentModelInfo = nullptr;
    int m_appId = -1;
    int m_id = -1;
    QString m_question;
    qreal m_pointsToPass = 1.0;
    QDateTime m_created;
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


#endif // QUESTION_H
