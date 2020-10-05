import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15


Dialog {
    id : editDescription
    modal: true

    property alias fieldId: editDescriptionID.text
    property alias fieldImageId: editDescriptionImageID.text
    property alias fieldDescription: editDescriptionText.text
    property alias fieldFrom: editDescriptionFrom.text

    title: qsTr("Edit Image Description")
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    FontMetrics{
        id: editDescriptionTextFontMetrics
        font: editDescriptionText.font
    }

    ScrollView {
        anchors.fill: parent
        clip:true

        ColumnLayout {
            RowLayout {
                Label {
                    text: qsTr("Id : ")
                }
                Text {
                    id: editDescriptionID
                }
            }
            RowLayout {
                Label {
                    text: qsTr("Image id : ")
                }
                Text {
                    id: editDescriptionImageID
                }
            }
            Label {
                text: qsTr("Description : ")
            }
            TextArea {
                id: editDescriptionText
                placeholderText: qsTr("Enter the description of image")
                focus: true
                KeyNavigation.priority: KeyNavigation.BeforeItem
                KeyNavigation.tab: editDescriptionFrom
            }
            RowLayout {
                Label {
                    text: qsTr("From : ")
                }
                TextField {
                    id: editDescriptionFrom
                    placeholderText: qsTr("Enter description activation date")
                    focus: true
                }
            }
        }
    }

    standardButtons: Dialog.Cancel | Dialog.Save
}
