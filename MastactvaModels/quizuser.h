#ifndef QUIZUSER_H
#define QUIZUSER_H


#include <QObject>
#include <QtQuick/QQuickItem>
#include "../MastactvaBase/Layout.h"
#include "../MastactvaBase/Model.h"


QString getAndroidUuid();


class QuizUser : public QObject
{
    Q_OBJECT

public:
    class DefaultLayout : public LayoutBase<QuizUser>
    {
    public:
        DefaultLayout()
        {
            setLayoutJsonName("");
            addSpecial<int>(layout::SpecialFieldEn::appId, &QuizUser::m_appId);
            addField<QString>("deviceid", "deviceId", &QuizUser::deviceId, &QuizUser::setDeviceId);
        }
    };

public:
    explicit QuizUser(QObject *parent_ = nullptr);

    Q_PROPERTY(QString deviceId READ deviceId WRITE setDeviceId NOTIFY deviceIdChanged)

    QString deviceId() const;
    void setDeviceId(const QString &deviceId_);


signals:
    void deviceIdChanged();


private:
    QString m_deviceId;
    int m_appId = 0;
};


class QuizUserModel : public ListModelBaseOfData<QuizUser, QuizUserModel>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<QuizUser, QuizUserModel>;

public:
    explicit QuizUserModel(QObject *parent_ = nullptr);

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
    void error(const QString &errorCode_, const QString &description_);

protected:
    virtual void modelListLoaded(const QJsonDocument &reply_) override;
};


#endif // QUIZUSER_H
