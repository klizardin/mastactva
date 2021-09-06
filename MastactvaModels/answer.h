/*
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

#ifndef ANSWER_H
#define ANSWER_H


#include <QObject>
#include "../MastactvaBase/IModel.h"
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"


class Answer : public QObject, public IListModelItem
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
            setLayoutJsonName("image-question-answer");
            addSpecial<int>(layout::SpecialFieldEn::appId, &Answer::m_appId);
            addSpecial<IListModelInfo *>(layout::SpecialFieldEn::modelInfo, &Answer::m_parentModelInfo);
            addField<int>("id", "answerId", &Answer::id, &Answer::setId);
            addField<int>("question", "answerQuestionId", &Answer::question, &Answer::setQuestion);
            addField<QString>("answer", "answerText", &Answer::answer, &Answer::setAnswer);
            addField<qreal>("points", "answerPoints", &Answer::points, &Answer::setPoints);
            setIdField("id");
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


#endif // ANSWER_H
