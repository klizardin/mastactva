#include "quizuser.h"

QuizUser::QuizUser(QObject *parent_ /*= nullptr*/) : QObject(parent_)
{

}

QString QuizUser::deviceId() const
{
    return m_deviceId;
}

void QuizUser::setDeviceId(const QString &deviceId_)
{
    m_deviceId = deviceId_;

    emit deviceIdChanged();
}


QuizUserModel::QuizUserModel(QObject *parent_ /*= nullptr*/)
    : ListModelBaseOfData<QuizUser>(parent_)
{
    addDataItemImpl(createDataItemImpl());
}
