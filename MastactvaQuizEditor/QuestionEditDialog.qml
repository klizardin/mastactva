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
