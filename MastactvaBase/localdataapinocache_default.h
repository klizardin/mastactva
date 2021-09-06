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

#ifndef LOCALDATAAPINOCACHE_DEFAULT_H
#define LOCALDATAAPINOCACHE_DEFAULT_H


#include "../MastactvaBase/dbutils.h"


class RequestData;


class SaveDBRequest : public DBRequestBase
{
public:
    SaveDBRequest();
    bool operator == (const RequestData *request_) const;
    void setRequest(const RequestData *request_);

private:
    const RequestData *m_request = nullptr;
};


class LocalDataAPINoCacheImpl : public ILocalDataGetAPI
{
public:
    bool canProcess(const DBRequestBase *r_) const override;
    bool getListImpl(DBRequestBase *r_) override;
};


#endif // LOCALDATAAPINOCACHE_DEFAULT_H
