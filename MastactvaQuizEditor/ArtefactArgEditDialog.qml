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


Dialog {
    id : artefactArgEditDialog
    modal: true

    property bool fieldNewItem: false
    property var artefactArgTypeModel: undefined
    property var artefactArgStorageModel: undefined
    property var fieldArtefactArg: undefined

    // private:
    property int fieldArtefactArgArgTypeId: -1
    property int fieldArtefactArgArgStorageId: -1

    title: fieldNewItem ? qsTr("Add new artefact argument") : qsTr("Edit artefact argument info")

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    FontMetrics{
        id: artefactArgEditDialogTextFontMetrics
        font: artefactArgIdText.font
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
                }
            }

            Label {
                text: qsTr("Type  : ")
            }
            Rectangle{
                id: editArtefactArgTypeListRect
                width: Constants.minConstValueWidth
                height: Constants.minConstValueHeight

                ListView {
                    id: editArtefactArgTypeList
                    anchors.fill: parent
                    clip: true
                    spacing: Constants.smallListSmallSpacing
                    model: artefactArgTypeModel
                    delegate: artefactArgTypeItem
                    highlight: artefactArgTypeItemHighlight
                    highlightFollowsCurrentItem: false
                }
            }

            Label {
                text: qsTr("Storage  : ")
            }
            Rectangle{
                id: editArtefactArgStorageListRect
                width: Constants.minConstValueWidth
                height: Constants.minConstValueHeight

                ListView {
                    id: editArtefactArgStorageList
                    anchors.fill: parent
                    clip: true
                    spacing: Constants.smallListSmallSpacing
                    model: artefactArgStorageModel
                    delegate: artefactArgStorageItem
                    highlight: artefactArgStorageItemHighlight
                    highlightFollowsCurrentItem: false
                }
            }

            Label {
                text: qsTr("Name : ")
            }
            TextArea {
                id: editArtefactArgNameText
            }

            Label {
                text: qsTr("Default value : ")
            }
            TextArea {
                id: editArtefactArgDefaultValueText
                placeholderText: qsTr("Enter default value of the argument")
                focus: true
                KeyNavigation.priority: KeyNavigation.BeforeItem
                KeyNavigation.tab: editArtefactArgDescriptionText
                KeyNavigation.backtab: editArtefactArgNameText
            }

            Label {
                text: qsTr("Description : ")
            }
            TextArea {
                id: editArtefactArgDescriptionText
                placeholderText: qsTr("Enter description for the argument")
                focus: true
                KeyNavigation.priority: KeyNavigation.BeforeItem
                KeyNavigation.backtab: editArtefactArgDefaultValueText
            }

            RowLayout {
                Label {
                    text: qsTr("Created : ")
                }
                Text {
                    id: editArtefactArgCreatedText
                }
            }
        }
    }

    function init()
    {
        fieldArtefactArgArgTypeId = -1
        fieldArtefactArgArgStorageId = -1

        var w = 0
        for(var i1 = 0; i1 < artefactArgTypeModel.size(); i1++)
        {
            w = Math.max(w, artefactArgEditDialogTextFontMetrics.tightBoundingRect(artefactArgTypeModel.itemAt(i1).artefactArgTypeType).width)
        }
        editArtefactArgTypeListRect.width = w
        editArtefactArgTypeListRect.height = (artefactArgEditDialogTextFontMetrics.height + Constants.smallListSmallSpacing) * artefactArgTypeModel.size() * 1.1

        for(var i2 = 0; i2 < artefactArgStorageModel.size(); i2++)
        {
            w = Math.max(w, artefactArgEditDialogTextFontMetrics.tightBoundingRect(artefactArgStorageModel.itemAt(i2).artefactArgStorageStorage).width)
        }
        editArtefactArgStorageListRect.width = w
        editArtefactArgStorageListRect.height = (artefactArgEditDialogTextFontMetrics.height + Constants.smallListSmallSpacing) * artefactArgStorageModel.size() * 1.1

        if(fieldNewItem)
        {
            artefactArgIdText.text = qsTr("<new>")
            editArtefactArgCreatedText.text = mastactva.dateTimeToUserStr(mastactva.now())
        }
        else
        {
            artefactArgIdText.text = fieldArtefactArg.artefactArgId
            editArtefactArgCreatedText.text = mastactva.dateTimeToUserStr(fieldArtefactArg.artefactArgCreated)
        }
        editArtefactArgNameText.text = fieldArtefactArg.artefactArgName
        editArtefactArgDefaultValueText.text = fieldArtefactArg.artefactArgDefaultValue
        editArtefactArgDescriptionText.text = fieldArtefactArg.artefactArgDescription
        if(artefactArgTypeModel.selectItemById(fieldArtefactArg.artefactArgArgTypeId))
        {
            fieldArtefactArgArgTypeId = artefactArgTypeModel.currentItem.artefactArgTypeId
            editArtefactArgTypeList.currentIndex = artefactArgTypeModel.currentIndex
        }
        else
        {
            editArtefactArgTypeList.currentIndex = -1
        }

        if(artefactArgStorageModel.selectItemById(fieldArtefactArg.artefactArgArgStorageId))
        {
            fieldArtefactArgArgStorageId = artefactArgStorageModel.currentItem.artefactArgStorageId
            editArtefactArgStorageList.currentIndex = artefactArgStorageModel.currentIndex
        }
        else
        {
            editArtefactArgStorageList.currentIndex = -1
        }
    }

    function update()
    {
        fieldArtefactArg.artefactArgName = editArtefactArgNameText.text
        fieldArtefactArg.artefactArgDefaultValue = editArtefactArgDefaultValueText.text
        fieldArtefactArg.artefactArgDescription = editArtefactArgDescriptionText.text
        if(fieldNewItem)
        {
            fieldArtefactArg.artefactArgCreated = mastactva.dateTimeFromUserStr(editArtefactArgCreatedText.text)
        }
        fieldArtefactArg.artefactArgArgTypeId = fieldArtefactArgArgTypeId
        fieldArtefactArg.artefactArgArgStorageId = fieldArtefactArgArgStorageId
    }

    standardButtons: Dialog.Cancel | Dialog.Save

    Component {
        id: artefactArgTypeItem

        Text {
            id: artefactArgTypeItemType
            width: editArtefactArgTypeListRect.width
            text: artefactArgTypeType

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton

                onClicked:
                {
                    editArtefactArgTypeList.currentIndex = index
                    artefactArgTypeModel.currentIndex = index
                    fieldArtefactArgArgTypeId = artefactArgTypeModel.currentItem.artefactArgTypeId
                    mouse.accepted = false
                }
            }
        }
    }

    Component {
        id: artefactArgTypeItemHighlight

        Rectangle {

            SystemPalette {
                id: artefactArgTypeItemHighlightPallete
                colorGroup: SystemPalette.Active
            }

            border.color: artefactArgTypeItemHighlightPallete.highlight
            border.width: 1
            radius: 2

            y: (editArtefactArgTypeList.currentItem !== undefined && editArtefactArgTypeList.currentItem !== null) ? editArtefactArgTypeList.currentItem.y : 0
            x: (editArtefactArgTypeList.currentItem !== undefined && editArtefactArgTypeList.currentItem !== null) ? editArtefactArgTypeList.currentItem.x : 0
            width: (editArtefactArgTypeList.currentItem !== undefined && editArtefactArgTypeList.currentItem !== null) ? editArtefactArgTypeList.currentItem.width : 0
            height: (editArtefactArgTypeList.currentItem !== undefined && editArtefactArgTypeList.currentItem !== null) ? editArtefactArgTypeList.currentItem.height : 0
        }
    }

    Component {
        id: artefactArgStorageItem

        Text {
            id: artefactArgStorageItemType
            width: editArtefactArgStorageListRect.width
            text: artefactArgStorageStorage

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton

                onClicked:
                {
                    editArtefactArgStorageList.currentIndex = index
                    artefactArgStorageModel.currentIndex = index
                    fieldArtefactArgArgStorageId = artefactArgStorageModel.currentItem.artefactArgStorageId
                    mouse.accepted = false
                }
            }
        }
    }

    Component {
        id: artefactArgStorageItemHighlight

        Rectangle {

            SystemPalette {
                id: artefactArgStorageItemHighlightPallete
                colorGroup: SystemPalette.Active
            }

            border.color: artefactArgStorageItemHighlightPallete.highlight
            border.width: 1
            radius: 2

            y: (editArtefactArgStorageList.currentItem !== undefined && editArtefactArgStorageList.currentItem !== null) ? editArtefactArgStorageList.currentItem.y : 0
            x: (editArtefactArgStorageList.currentItem !== undefined && editArtefactArgStorageList.currentItem !== null) ? editArtefactArgStorageList.currentItem.x : 0
            width: (editArtefactArgStorageList.currentItem !== undefined && editArtefactArgStorageList.currentItem !== null) ? editArtefactArgStorageList.currentItem.width : 0
            height: (editArtefactArgStorageList.currentItem !== undefined && editArtefactArgStorageList.currentItem !== null) ? editArtefactArgStorageList.currentItem.height : 0
        }
    }
}
