import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15


Dialog {
    id : editQuestion
    modal: true

    property alias fieldId: editQuestionID.text
    property alias fieldQuestion: editQuestionQuestionText.text
    property alias fieldPointsToPass: editQuestionPointsToPass.text

    title: qsTr("Edit Question")
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    FontMetrics{
        id: editQuestionQuestionTextFontMetrics
        font: editQuestionQuestionText.font
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
                    id: editQuestionID
                }
            }
            Label {
                text: qsTr("Question : ")
            }
            TextArea {
                id: editQuestionQuestionText
                placeholderText: qsTr("Enter question text")
                focus: true
                KeyNavigation.priority: KeyNavigation.BeforeItem
                KeyNavigation.tab: editQuestionPointsToPass
            }
            RowLayout {
                Label {
                    text: qsTr("Points To Pass Question : ")
                }
                TextField {
                    id: editQuestionPointsToPass
                    placeholderText: qsTr("Enter points to pass")
                    focus: true
                }
            }
        }
    }

    standardButtons: Dialog.Cancel | Dialog.Save
}
