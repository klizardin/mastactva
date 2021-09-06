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

import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15
import Mastactva 1.0


Dialog {
    id: confirmDialog
    modal: true

    property alias confirmText: operationInfo.text
    property alias imageSource: image.source
    property bool showImage: false

    title: qsTr("Confirm operation")
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    ScrollView {
        anchors.fill: parent
        clip:true

        ColumnLayout {
            Text {
                id: operationInfo
            }
            Rectangle
            {
                Layout.alignment: Qt.AlignHCenter
                visible: showImage
                width: Constants.smallImageWidth
                height: Constants.smallImageHeight

                Image {
                    id: image
                    visible: showImage
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectFit
                }
            }
        }
    }

    standardButtons: Dialog.Ok | Dialog.Cancel
}
