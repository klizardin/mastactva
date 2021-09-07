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
