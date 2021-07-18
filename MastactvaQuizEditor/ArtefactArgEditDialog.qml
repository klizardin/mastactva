import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15
import MastactvaQuizEditor 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


Dialog {
    id : artefactArgEditDialog
    modal: true

    property bool fieldNewItem: false
    property var artefactArgTypeMode: undefined
    property var artefactArgStorageMode: undefined

    title: fieldNewItem ? qsTr("Add new artefact argument") : qsTr("Edit artefact argument info")

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    FontMetrics{
        id: editAnswerDialogQuestionTextFontMetrics
        font: editAnswerDialogQuestionText.font
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
                    id: artefactArgIdText
                    text: artefactArgId
                }
            }
        }
    }

    standardButtons: Dialog.Cancel | Dialog.Save
}
