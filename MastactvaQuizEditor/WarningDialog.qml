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

import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15
import MastactvaQuizEditor 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0

Popup {
    id: popupMessage
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: Constants.warningDialogWidth
    height: Constants.warningDialogHeight
    modal: true
    focus: true
    padding: Constants.warningDialogPadding

    property alias fieldPopupMessageShortText: popupMessageShortText.text
    property alias fieldPopupMessageDescriptionText: popupMessageDescriptionText.text

    Column {
        height: parent.height - closeButton.height
        width: parent.width
        TextArea {
            id: popupMessageShortText
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
            padding: 10
            wrapMode: Text.Wrap
            readOnly: true
        }
        TextArea {
            id: popupMessageDescriptionText
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
            padding: 10
            wrapMode: Text.Wrap
            readOnly: true
        }
    }
    Button {
        id: closeButton
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("Close")
        onClicked: {
            popupMessage.close()
        }
    }
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
}
