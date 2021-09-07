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

pragma Singleton
import QtQuick 2.6

QtObject {
    readonly property int width: 800
    readonly property int height: 600
    readonly property int shadersWidth: 400
    readonly property int aspectX: 4
    readonly property int aspectY: 3
    readonly property string noImage: "qrc:///resources/no-image.png"
    readonly property string noImageResource: ":/resources/no-image.png"
    readonly property string noNameVertex: "no-name.vert"
    readonly property string noNameFragment: "no-name.frag"
    property string noImageHash: ""
}
