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

#include "mastactvaapi.h"
#include <QTimeZone>
#include "../MastactvaBase/timeutils.h"
#include "../MastactvaBase/utils.h"
#include "../MastactvaBase/names.h"
#include "../MastactvaBase/defines.h"


MastactvaAPI::MastactvaAPI(QObject *parent_ /*= nullptr*/)
    :QObject(parent_)
{
#if defined(TRACE_THREADS)
    qDebug() << "MastactvaAPI::MastactvaAPI()" << QThread::currentThread() << QThread::currentThreadId();
#endif
}

QString MastactvaAPI::readMore(const QString &str_,
                               int maxSymbols_,
                               const QString &rmt_)
{
    if(str_.length() <= maxSymbols_) { return str_; }
    int i = maxSymbols_;
    for(;i < str_.length() && isSymbol(str_.at(i)); i++) {}
    return str_.mid(0, i) + rmt_;
}

QDateTime MastactvaAPI::now() const
{
    return date_time::nowTz();
}

void MastactvaAPI::setInitialized()
{
    emit initialized();
}

void MastactvaAPI::showModelError(int errorCode_,
                                  const QString & errorCodeStr_,
                                  const QString & description_)
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
    return date_time::dateTimeToJsonString(dt_);
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
