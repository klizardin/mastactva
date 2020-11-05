#ifndef QUIZUSER_H
#define QUIZUSER_H


#include <QObject>
#include <QtQuick/QQuickItem>
#include "Layout.h"
#include "Model.h"


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
            addSpecial<QObject *>(layout::SpecialFieldEn::appId, &QuizUser::m_appId);
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
    QObject * m_appId = nullptr;
};


class QuizUserModel : public ListModelBaseOfData<QuizUser>
{
    Q_OBJECT
    QML_ELEMENT

protected:
    using base = ListModelBaseOfData<QuizUser>;

public:
    explicit QuizUserModel(QObject *parent_ = nullptr);

    LAYOUT_MODEL_IMPL();

public slots:
    void jsonResponseSlot(int errorCode_, RequestData *request_, const QJsonDocument &reply_)
    {
        jsonResponseSlotImpl(errorCode_, request_, reply_);
    }

signals:
    void currentIndexChanged();
    void currentRefChanged();
    void storeAfterSaveChanged();
    void refAppIdChanged();

protected:
    virtual void listLoaded(const QJsonDocument &reply_) override;
};



#endif // QUIZUSER_H
