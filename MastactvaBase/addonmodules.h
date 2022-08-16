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

#ifndef ADDONMODULES_H
#define ADDONMODULES_H


#include <QStringList>
#include <QJsonDocument>
#include <QString>


/*
 * inteface to call addons from the dynamic libraries
 * the utility abstaction for lua scripting and so on
*/
class IAddonModules
{
public:
    virtual QStringList getNames() const = 0;
    virtual QJsonDocument call(const QString &name_, const QJsonDocument &arguments_) const = 0;
};


#endif // ADDONMODULES_H
