#ifndef QUIZUSER_H
#define QUIZUSER_H


#include <QObject>
#include <QtQuick/QQuickItem>
#include "Layout.h"
#include "Model.h"


class QuizUser : public QObject
{
    Q_OBJECT

public:
    class DefaultLayout : public LayoutBase<QuizUser>
    {
    public:
        DefaultLayout()
        {
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
    int m_appId = -1;
};


class QuezUserModel : public ListModelBaseOfData<QuizUser>
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit QuezUserModel(QObject *parent_ = nullptr);

    LAYOUTMODEL()
};



#endif // QUIZUSER_H
