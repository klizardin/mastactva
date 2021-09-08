/*
    Copyright 2021

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

#ifndef QUIZQMLOBJECTS_H
#define QUIZQMLOBJECTS_H


#include <QJsonDocument>
#include "../MastactvaBase/imagesource.h"
#include "../MastactvaBase/qmlobjects.h"


class MastactvaAPI;


class QMLObjects : public QMLObjectsBase
{
public:
    QMLObjects() = default;

    virtual void setInitialized() override;
    virtual void modelError(int errorCode_,
                            const QString &errorCodeStr_,
                            const QJsonDocument &reply_) override;

protected:
    virtual void searchObjects() override;

private:
    MastactvaAPI *m_mastactvaAPI = nullptr;
};


#endif // QUIZQMLOBJECTS_H
