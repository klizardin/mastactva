import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15
import MastactvaQuizEditor 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


Dialog {
    id : artefactEditDialog
    modal: true

    property bool fieldNewItem: false
    property var fieldObjectArtefact: undefined
    property var fieldArtefact: undefined
    property var mastactva: undefined
    property var artefactTypeModel: undefined

    // private:
    property int fieldArtefactTypeId: -1

    title: fieldNewItem ? qsTr("Add new artefact") : qsTr("Edit artefact info")

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    FileDialog {
        id: openArtefactDialog
        title: qsTr("Please choose an artefact file to upload to the server")
        folder: shortcuts.pictures
        nameFilters: [ "Shader files (*.vert *.vertex *.frag *.fragment *vsh *.fsh)","Texture files (*.png *.jpg *.jpeg)","Json data files (*.json)","3D Objext files (*.obj)","Lua script file (*.lua)" ]
        selectExisting: true
        selectMultiple: false

        onAccepted: {
            editArtefactFilename.text = fileUrl
            editArtefactHash.text = mastactva.calculateHash(fileUrl)
            if(mastactva.isShaderUrl(fileUrl))
            {
                editArtefactDescription.text = mastactva.getShaderDescription(fileUrl)
            }
            else if(mastactva.isLuaUrl(fileUrl))
            {
                editArtefactDescription.text = mastactva.getLuaDescription(fileUrl)
            }
            else
            {
                editArtefactDescription.text = ""
            }
        }

        onRejected: {
            // TODO: add no image upload reaction
        }
    }

    FontMetrics{
        id: editArtafactFilenameFontMetrics
        font: editArtefactFilename.font
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
                    text: qsTr("Object artefact id : ")
                }
                Text {
                    id: editObjectArtefactId
                }
            }
            Row {
                Label {
                    text: qsTr("Artefact id : ")
                }
                Text {
                    id: editArtefactId
                }
            }

            Label {
                text: qsTr("Object artefact step : ")
            }
            TextField {
                id: editObjectArtefactStep
                placeholderText: qsTr("Object artefact step")
                focus: true
                KeyNavigation.priority: KeyNavigation.BeforeItem
                KeyNavigation.tab: editArtefactTypeList
            }

            Label {
                text: qsTr("Type  : ")
            }
            Rectangle{
                id: editArtefactTypeListRect
                width: Constants.minConstValueWidth
                height: Constants.minConstValueHeight

                ListView {
                    id: editArtefactTypeList
                    anchors.fill: parent
                    clip: true
                    spacing: Constants.smallListSmallSpacing
                    model: artefactTypeModel
                    delegate: artefactTypeItem
                    highlight: artefactTypeItemHighlight
                    highlightFollowsCurrentItem: false
                }
            }
            Label {
                text: qsTr("Filename  : ")
            }
            Row {
                TextField {
                    id: editArtefactFilename
                    placeholderText: qsTr("Enter artefact filename")
                    readOnly: true
                    focus: true
                    KeyNavigation.priority: KeyNavigation.BeforeItem
                    KeyNavigation.backtab: editArtefactTypeList
                    KeyNavigation.tab: editArtefactBrowse
                }
                Button {
                    id: editArtefactBrowse
                    text: qsTr("Browse")
                    onClicked: {
                        openArtefactDialog.open()
                    }
                }
            }
            Label {
                text: qsTr("Hash  : ")
            }
            TextField {
                id: editArtefactHash
                placeholderText: qsTr("Artefact hash value")
                readOnly: true
                focus: true
                KeyNavigation.priority: KeyNavigation.BeforeItem
                KeyNavigation.backtab: editArtefactBrowse
                KeyNavigation.tab: editArtefactDescription
            }
            Label {
                text: qsTr("Description  : ")
            }
            TextArea {
                id: editArtefactDescription
                placeholderText: qsTr("Enter artefact description")
                focus: true
                KeyNavigation.priority: KeyNavigation.BeforeItem
                KeyNavigation.backtab: editArtefactHash
            }
        }
    }

    function init()
    {
        fieldArtefactTypeId = -1

        var w = 0
        for(var i = 0; i < artefactTypeModel.size(); i++)
        {
            w = Math.max(w, editArtafactFilenameFontMetrics.tightBoundingRect(artefactTypeModel.itemAt(i).artefactTypeType).width)
        }
        editArtefactTypeListRect.width = w
        editArtefactTypeListRect.height = (editArtafactFilenameFontMetrics.height + Constants.smallListSmallSpacing) * artefactTypeModel.size() * 1.1
        editObjectArtefactId = fieldObjectArtefact.objectArtefactId
        editObjectArtefactStep.text = fieldObjectArtefact.objectArtefactStepIndex
        editArtefactId.text = fieldArtefact.artefactId
        if(!editArtefactTypeList.model.selectItemById(fieldShader.artefactTypeId))
        {
            editArtefactTypeList.currentIndex = 0
            editArtefactTypeList.model.currentIndex = 0
            fieldArtefactTypeId = editArtefactTypeList.model.currentItem.artefactTypeId
        }
        else
        {
            editArtefactTypeList.currentIndex = editArtefactTypeList.model.currentIndex
            if(editArtefactTypeList.model.currentItem !== null && editArtefactTypeList.model.currentItem !== undefined)
            {
                fieldArtefactTypeId = editArtefactTypeList.model.currentItem.artefactTypeId
            }
        }
        editArtefactFilename.text = fieldArtefact.artefactFilename
        editArtefactHash.text = fieldArtefact.artefactHash
        editArtefactDescription.text = fieldArtefact.artefactDescription
    }

    function update()
    {
        fieldObjectArtefact.objectArtefactStepIndex = editObjectArtefactStep.text
        fieldArtefact.artefactFilename = editArtefactFilename.text
        fieldArtefact.artefactHash = editArtefactHash.text
        fieldArtefact.artefactDescription = editArtefactDescription.text
        fieldArtefact.artefactTypeId = fieldArtefactTypeId
    }

    standardButtons: Dialog.Cancel | Dialog.Save

    Component {
        id: artefactTypeItem

        Text {
            id: artefactTypeItemType
            width: editArtefactTypeListRect.width
            text: artefactTypeType

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton

                onClicked:
                {
                    editArtefactTypeList.currentIndex = index
                    fieldArtefactTypeId = artefactTypeId
                    mouse.accepted = false
                }
            }
        }
    }

    Component {
        id: artefactTypeItemHighlight

        Rectangle {

            SystemPalette {
                id: artefactTypeItemHighlightPallete
                colorGroup: SystemPalette.Active
            }

            border.color: artefactTypeItemHighlightPallete.highlight
            border.width: 1
            radius: 2

            y: (editArtefactTypeList.currentItem !== undefined && editArtefactTypeList.currentItem !== null) ? editArtefactTypeList.currentItem.y : 0
            x: (editArtefactTypeList.currentItem !== undefined && editArtefactTypeList.currentItem !== null) ? editArtefactTypeList.currentItem.x : 0
            width: (editArtefactTypeList.currentItem !== undefined && editArtefactTypeList.currentItem !== null) ? editArtefactTypeList.currentItem.width : 0
            height: (editArtefactTypeList.currentItem !== undefined && editArtefactTypeList.currentItem !== null) ? editArtefactTypeList.currentItem.height : 0
        }
    }
}
