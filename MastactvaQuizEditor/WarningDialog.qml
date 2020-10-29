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
    width: 240
    height: 320
    modal: true
    focus: true
    padding: 10

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
