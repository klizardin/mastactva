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

#include "quizuser.h"
#include <QNetworkInterface>
#include <QCryptographicHash>
//#include <QAndroidJniObject>
#include "../MastactvaBase/defines.h"


// TODO : investigate android defines for code and implement android code in defines

QString getSha1Hash(const QString &str_)
{
    return QString("%1").arg(QString(QCryptographicHash::hash(str_.toUtf8(),QCryptographicHash::Sha1).toHex()));
}

// https://forum.qt.io/topic/77166/unique-android-device-id/4
QString getAndroidUuid()
{
    for(const QNetworkInterface &netInterface : QNetworkInterface::allInterfaces())
    {
        if (!(netInterface.flags() & QNetworkInterface::IsLoopBack))
        {
            QString macAddress = netInterface.hardwareAddress();
            // On Android 6 you cannot retrieve MAC address with regular way, this is a workaround for getting it.
            if(macAddress.isEmpty())
            {
                QFile macFile("/sys/class/net/"+netInterface.name()+"/address");
                if (macFile.open(QFile::ReadOnly))
                {
                    QTextStream textStream(&macFile);
                    macAddress = QString(textStream.readLine());
                    macFile.close();
                }
            }
            if(!macAddress.isEmpty() && !macAddress.endsWith("02:00:00:00:00:00"))
            {
                return getSha1Hash(macAddress);
            }
        }
    }
/*    // Trying to get ANDROID_ID from system
    QAndroidJniObject myID = QAndroidJniObject::fromString("android_id");
    QAndroidJniObject activity = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative", "activity", "()Landroid/app/Activity;");
    QAndroidJniObject appctx = activity.callObjectMethod("getApplicationContext","()Landroid/content/Context;");
    QAndroidJniObject contentR = appctx.callObjectMethod("getContentResolver", "()Landroid/content/ContentResolver;");

    QAndroidJniObject androidId = QAndroidJniObject::callStaticObjectMethod("android/provider/Settings$Secure","getString",
                                                                         "(Landroid/content/ContentResolver;Ljava/lang/String;)Ljava/lang/String;",
                                                                         contentR.object<jobject>(),
                                                                         myID.object<jstring>());
    if(!_isJvmException()){
        if(androidId.isValid() && !androidId.toString().isEmpty()){
            if(androidId.toString().endsWith("9774d56d682e549c")){
                qWarning()<< Q_FUNC_INFO <<"This device has the bug with unique id"; //https://code.google.com/p/android/issues/detail?id=10603
            }else{
                qDebug()<< Q_FUNC_INFO <<"Using androidId"<<androidId.toString();
                return _getSha1Hash(androidId.toString());
            }
        }
    }

    qDebug()<< Q_FUNC_INFO <<"Using randomUuid";
    return _getSha1Hash(QUuid::createUuid().toString());*/
    return getSha1Hash(QUuid::createUuid().toString());
}


QuizUser::QuizUser(QObject *parent_ /*= nullptr*/) : QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "QuizUser::QuizUser()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "QuizUser::QuizUser()" << this;
#endif

    m_deviceId = getAndroidUuid();
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
    : ListModelBaseOfData<QuizUser, QuizUserModel>(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "QuizUserModel::QuizUserModel()" << QThread::currentThread() << QThread::currentThreadId();
#endif
#if defined(TRACE_MODEL_LIFETIME)
    qDebug() << "QuizUserModel::QuizUserModel()" << this;
#endif

    init(this);
    setReadonlyImpl(false);
}

void QuizUserModel::setEmptyModel()
{
    m_emptyModel = true;
    //setLayoutJsonNameImpl("quiz-user");
}

void QuizUserModel::modelListLoaded(const QJsonDocument &reply_)
{
    base::modelListLoaded(reply_);
    if(m_data.isEmpty() && !m_emptyModel)
    {
        QuizUser* qu = createDataItemImpl();
        addDataItemImpl(qu, true);
    }
}
