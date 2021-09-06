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

#ifndef NETAPPCONSTS_H
#define NETAPPCONSTS_H


#include <QString>
#include <QVector>


class NetAppConsts
{
public:
    QString m_serverURL;
    QString m_playLogin;
    QString m_playPassword;

protected:
    NetAppConsts();

public:
    static const NetAppConsts &getInstance();
    static void set(const QString &serverURL_,
                    const QString &playLogin_,
                    const QString &playPassword_);
};


#endif // NETAPPCONSTS_H
