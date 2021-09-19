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

#ifndef ARTEFACTTYPE_DATA_H
#define ARTEFACTTYPE_DATA_H


#include <QString>


enum class ArtefactTypeEn{
    shaderVertex = 1,
    shaderFragmet,
    texture1D,
    texture2D,
    texture3D,
    dataJson,
    dataObj3D,
    convertNamesJson,
    scriptLua,
    scriptLuaRuntime,
    scriptLibrary
};


QString getArtefactFileFilter(int artefactTypeId_);
QStringList getArtefactFileExtensions(int artefactTypeId_);

#endif // ARTEFACTTYPE_DATA_H
