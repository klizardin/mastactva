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

.pragma library
.import QtQuick 2.0 as QtQuickModuleImportedInJS

function description_first_part(str) {
    return str.substring(0, 128);
}

function textLFtoBr(str) {
    while(str.indexOf("\n") > -1)
    {
        str = str.replace("\n", "<br/>");
    }
    return str;
}

function clamp(v, min_, max_)
{
    return Math.min(max_, Math.max(min_, v));
}
