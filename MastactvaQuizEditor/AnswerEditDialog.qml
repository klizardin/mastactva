import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15


Dialog {
    id : editAnswerDialog
    modal: true

    property alias fieldId: editAnswerDialogID.text
    property alias fieldQuestion: editAnswerDialogQuestionText.text
    property alias fieldQuestionPointsToPass: editAnswerDialogQuestionPointsToPassText.text
    property alias fieldAnswer: editAnswerDialogAnswerText.text
    property alias fieldPointsToPass: editAnswerDialogPointsToPass.text

    title: qsTr("Edit Answer")
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
                    id: editAnswerDialogID
                }
            }
            Label {
                text: qsTr("Question : ")
            }
            TextArea {
                id: editAnswerDialogQuestionText
                readOnly: true
            }
            RowLayout {
                Label {
                    text: qsTr("Points to Pass Question : ")
                }
                Text {
                    id: editAnswerDialogQuestionPointsToPassText
                }
            }
            Label {
                text: qsTr("Answer : ")
            }
            TextArea {
                id: editAnswerDialogAnswerText
                placeholderText: qsTr("Enter answer for the question")
                focus: true
                KeyNavigation.priority: KeyNavigation.BeforeItem
                KeyNavigation.tab: editAnswerDialogPointsToPass
            }
            RowLayout {
                Label {
                    text: qsTr("Points for Answer : ")
                }
                TextField {
                    id: editAnswerDialogPointsToPass
                    placeholderText: qsTr("Enter points for answer")
                    focus: true
                }
            }
        }
    }

    standardButtons: Dialog.Cancel | Dialog.Save
}
