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

#ifndef MASTACTVAAPI_H
#define MASTACTVAAPI_H


#include <QObject>
#include <QQuickItem>
#include <QDateTime>


class MastactvaAPI : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    MastactvaAPI(QObject *parent_ = nullptr);

    Q_INVOKABLE QString readMore(const QString &str_,
                                 int maxSymbols_,
                                 const QString &rmt_);
    Q_INVOKABLE QDateTime now() const;
    Q_INVOKABLE void showModelError(int errorCode_,
                                    const QString & errorCodeStr_,
                                    const QString & description_);
    Q_INVOKABLE QString calculateHash(const QString &fileUrl_);
    Q_INVOKABLE QString dateTimeToISOStr(const QDateTime &dt_);
    Q_INVOKABLE bool isLocalDataAPI();
    Q_INVOKABLE int getLocalDataAPIUserId();
    Q_INVOKABLE bool isAndroidFullscreen();

    void setInitialized();

signals:
    void initialized();
    void errorMessage(const QString &header_, const QString &description_);
};


#endif // MASTACTVAAPI_H
