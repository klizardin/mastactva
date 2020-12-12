#include "mastactvaapi.h"
#include "../MastactvaBase/utils.h"
#include <QTimeZone>


MastactvaAPI::MastactvaAPI(QObject *parent_ /*= nullptr*/)
    :QObject(parent_)
{
}


inline
bool isSymbol(const QChar &ch_)
{
    return ch_.category() >= QChar::Letter_Uppercase;
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
