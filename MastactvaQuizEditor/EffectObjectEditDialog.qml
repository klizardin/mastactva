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
import MastactvaQuizEditor 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


Dialog {
    id : effectObjectEditDialog
    modal: true

    property bool fieldNewItem: false
    property var fieldEffectObject: undefined
    property var fieldEffectObjectInfo: undefined
    property var mastactva: undefined

    title: fieldNewItem ? qsTr("Add new effect object") : qsTr("Edit effect object")

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    FontMetrics{
        id: editArtafactFilenameFontMetrics
        font: editEffectObjectId.font
    }

    ScrollView {
        anchors.fill: parent
        contentWidth: column.width
        contentHeight: column.height
        clip:true

        Column {
            id: column

            Row {
                Label {
                    text: qsTr("Id : ")
                }
                Text {
                    id: editEffectObjectId
                }
            }

            Row {
                Label {
                    text: qsTr("Object Info id : ")
                }
                Text {
                    id: editEffectObjectInfoId
                }
            }

            Label {
                text: qsTr("Effect object step : ")
            }
            TextField {
                id: editEffectObjectStep
                placeholderText: qsTr("Effect object step")
                focus: true
                KeyNavigation.priority: KeyNavigation.BeforeItem
                KeyNavigation.tab: editEffectObjectInfoName
            }

            Label {
                text: qsTr("Effect object info name : ")
            }
            TextField {
                id: editEffectObjectInfoName
                placeholderText: qsTr("Effect object info name")
                focus: true
                KeyNavigation.priority: KeyNavigation.BeforeItem
                KeyNavigation.backtab: editEffectObjectStep
                KeyNavigation.tab: editEffectObjectInfoProgrammerName
            }

            Label {
                text: qsTr("Effect object info programmer name : ")
            }
            TextField {
                id: editEffectObjectInfoProgrammerName
                placeholderText: qsTr("Effect object info programmer name")
                focus: true
                KeyNavigation.priority: KeyNavigation.BeforeItem
                KeyNavigation.backtab: editEffectObjectInfoName
                KeyNavigation.tab: editEffectObjectInfoDescription
            }

            Row {
                Label {
                    text: qsTr("Object Info created : ")
                }
                Text {
                    id: editEffectObjectInfoCreated
                }
            }

            Label {
                text: qsTr("Effect object info description : ")
            }
            TextArea {
                id: editEffectObjectInfoDescription
                placeholderText: qsTr("Effect object info description")
                focus: true
                KeyNavigation.priority: KeyNavigation.BeforeItem
                KeyNavigation.backtab: editEffectObjectInfoProgrammerName
            }
        }
    }

    function init()
    {

        if(!fieldNewItem && fieldEffectObject !== null && fieldEffectObject !== undefined)
        {
            editEffectObjectId.text = fieldEffectObject.effectObjectsId
            editEffectObjectStep.text = fieldEffectObject.effectObjectsStepIndex
        }
        else
        {
            editEffectObjectId.text = qsTr("New")
            editEffectObjectStep.text = qsTr("")
        }

        if(!fieldNewItem && fieldEffectObjectInfo !== null && fieldEffectObjectInfo !== undefined)
        {
            editEffectObjectInfoId.text = fieldEffectObjectInfo.effectObjectInfoId
            editEffectObjectInfoName.text = fieldEffectObjectInfo.effectObjectInfoName
            editEffectObjectInfoProgrammerName.text = fieldEffectObjectInfo.effectObjectInfoProgrammerName
            editEffectObjectInfoCreated.text = mastactva.dateTimeToUserStr(fieldEffectObjectInfo.effectObjectInfoCreated)
            editEffectObjectInfoDescription.text = fieldEffectObjectInfo.effectObjectInfoDescription
        }
        else
        {
            editEffectObjectInfoId.text = qsTr("New")
            editEffectObjectInfoName.text = qsTr("")
            editEffectObjectInfoProgrammerName.text = qsTr("")
            editEffectObjectInfoCreated.text = qsTr("")
            editEffectObjectInfoDescription.text = qsTr("")

            if(!fieldNewItem)
            {
                editEffectObjectInfoName.readOnly = true
                editEffectObjectInfoProgrammerName.readOnly = true
                editEffectObjectInfoCreated.text = qsTr("")
                editEffectObjectInfoDescription.readOnly = true
            }
        }
    }

    function update()
    {
        if(fieldEffectObject !== null && fieldEffectObject !== undefined)
        {
            fieldEffectObject.effectObjectsStepIndex = editEffectObjectStep.text
        }

        if(fieldEffectObjectInfo !== null && fieldEffectObjectInfo !== undefined)
        {
            fieldEffectObjectInfo.effectObjectInfoName = editEffectObjectInfoName.text
            fieldEffectObjectInfo.effectObjectInfoProgrammerName = editEffectObjectInfoProgrammerName.text
            fieldEffectObjectInfo.effectObjectInfoDescription = editEffectObjectInfoDescription.text
        }
    }

    standardButtons: Dialog.Cancel | Dialog.Save
}
