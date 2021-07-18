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
                text: artefactArgName
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
                    //fieldArtefact.artefactTypeId = artefactTypeId
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
                    //fieldArtefact.artefactTypeId = artefactTypeId
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
