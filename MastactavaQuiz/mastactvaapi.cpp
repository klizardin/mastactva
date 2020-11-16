#include "mastactvaapi.h"


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
    return QDateTime::currentDateTime();
}
