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

#include "netappconsts.h"


NetAppConsts::NetAppConsts()
{
}

const NetAppConsts &NetAppConsts::getInstance()
{
    static NetAppConsts instance;
    return instance;
}

void NetAppConsts::set(const QString &serverURL_,
                       const QString &playLogin_,
                       const QString &playPassword_)
{
    NetAppConsts & v = const_cast<NetAppConsts &>(getInstance());
    v.m_serverURL = serverURL_;
    v.m_playLogin = playLogin_;
    v.m_playPassword = playPassword_;
}
