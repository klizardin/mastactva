#include "mastactvaapi.h"
#include <QTimeZone>
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/defines.h"


MastactvaAPI::MastactvaAPI(QObject *parent_ /*= nullptr*/)
    :QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "MastactvaAPI::MastactvaAPI()" << QThread::currentThread() << QThread::currentThreadId();
#endif
}

QString MastactvaAPI::readMore(const QString &str_, int maxSymbols_, const QString &rmt_)
{
    if(str_.length() <= maxSymbols_) { return str_; }
    int i = maxSymbols_;
    for(;i < str_.length() && isSymbol(str_.at(i)); i++) {}
    return str_.mid(0, i) + rmt_;
}

QDateTime MastactvaAPI::now() const
{
    return nowTz();
}

void MastactvaAPI::setInitialized()
{
    emit initialized();
}

void MastactvaAPI::showModelError(int errorCode_, const QString & errorCodeStr_, const QString & description_)
{
    Q_UNUSED(errorCode_);
    emit errorMessage(errorCodeStr_, description_);
}

QString MastactvaAPI::calculateHash(const QString &fileUrl_)
{
    return calculateFileURLHash(fileUrl_);
}

QString MastactvaAPI::dateTimeToISOStr(const QDateTime &dt_)
{
    return dateTimeToJsonString(dt_);
}

bool MastactvaAPI::isLocalDataAPI()
{
#if defined(LOCALDATAAPICACHE)
    return true;
#else
    return false;
#endif
}

int MastactvaAPI::getLocalDataAPIUserId()
{
    return g_userId;
}

bool MastactvaAPI::isAndroidFullscreen()
{
#if defined(ANDROID_FULLSCREEN)
    return true;
#else
    return false;
#endif
}
