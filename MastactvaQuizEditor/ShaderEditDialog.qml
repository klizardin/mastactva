import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15
import MastactvaQuizEditor 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


Dialog {
    id : shaderEditDialog
    modal: true

    property bool fieldNewItem: false
    property var fieldEffectShader: undefined
    property var fieldShader: undefined
    property var mastactva: undefined
    property var shaderTypeModel: undefined

    title: fieldNewItem ? qsTr("Add new shader") : qsTr("Edit shader info")

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    FileDialog {
        id: openShaderDialog
        title: qsTr("Please choose a shader file to upload to the server")
        folder: shortcuts.pictures
        nameFilters: [ "Shader files (*.vert *.vertex *.frag *.fragment *vsh *.fsh *.*)" ]
        selectExisting: true
        selectMultiple: false

        onAccepted: {
            editArtefactFilename.text = fileUrl
            editArtefactHash.text = mastactva.calculateHash(fileUrl)
            editShaderDescription.text = mastactva.getShaderDescription(fileUrl)
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
                    text: qsTr("Id : ")
                }
                Text {
                    id: editArtefactId
                }
            }
            Label {
                text: qsTr("Type  : ")
            }
            Rectangle{
                id: editShaderTypeListRect
                width: Constants.minConstValueWidth
                height: Constants.minConstValueHeight

                ListView {
                    id: editShaderTypeList
                    anchors.fill: parent
                    clip: true
                    spacing: Constants.effectShaderTypeListSpacing
                    model: shaderTypeModel
                    delegate: shaderTypeItem
                    highlight: shaderTypeItemHighlight
                    highlightFollowsCurrentItem: false
                }
            }
            Label {
                text: qsTr("Filename  : ")
            }
            Row {
                TextField {
                    id: editArtefactFilename
                    placeholderText: qsTr("Enter shader filename")
                    readOnly: true
                    focus: true
                    KeyNavigation.priority: KeyNavigation.BeforeItem
                    KeyNavigation.tab: editShaderBrowse
                }
                Button {
                    id: editShaderBrowse
                    text: qsTr("Browse")
                    onClicked: {
                        openShaderDialog.open()
                    }
                }
            }
            Label {
                text: qsTr("Hash  : ")
            }
            TextField {
                id: editArtefactHash
                placeholderText: qsTr("Shader hash value")
                readOnly: true
                focus: true
                KeyNavigation.priority: KeyNavigation.BeforeItem
                KeyNavigation.backtab: editShaderBrowse
            }
            Label {
                text: qsTr("Description  : ")
            }
            TextArea {
                id: editShaderDescription
                placeholderText: qsTr("Enter shader description")
                focus: true
                KeyNavigation.priority: KeyNavigation.BeforeItem
                KeyNavigation.backtab: editArtefactHash
            }
        }
    }

    function init()
    {
        var w = 0
        for(var i = 0; i < shaderTypeModel.size(); i++)
        {
            w = Math.max(w, editArtafactFilenameFontMetrics.tightBoundingRect(shaderTypeModel.itemAt(i).shaderTypeType).width)
        }
        editShaderTypeListRect.width = w
        editShaderTypeListRect.height = (editArtafactFilenameFontMetrics.height + Constants.effectShaderTypeListSpacing) * shaderTypeModel.size() * 1.1
        editArtefactId.text = fieldShader.artefactId
        if(!editShaderTypeList.model.selectItemById(fieldShader.artefactTypeId))
        {
            editShaderTypeList.currentIndex = 0
            editShaderTypeList.model.currentIndex = 0
            fieldShader.artefactTypeId = editShaderTypeList.model.getCurrentItem().artefactTypeId
        }
        else
        {
            editShaderTypeList.currentIndex = editShaderTypeList.model.currentIndex
        }
        editArtefactFilename.text = fieldShader.artefactFilename
        editArtefactHash.text = fieldShader.artefactHash
        editShaderDescription.text = fieldShader.artefactDescription
    }

    function update()
    {
        //fieldShader.artefactTypeId = editShaderTypeList.currentItem.artefactTypeId
        fieldShader.artefactFilename = editArtefactFilename.text
        fieldShader.artefactHash = editArtefactHash.text
        fieldShader.artefactDescription = editShaderDescription.text
    }

    standardButtons: Dialog.Cancel | Dialog.Save

    Component {
        id: shaderTypeItem

        Text {
            id: shaderTypeItemType
            width: editShaderTypeListRect.width
            text: shaderTypeType

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton

                onClicked:
                {
                    editShaderTypeList.currentIndex = index
                    fieldShader.artefactTypeId = artefactTypeId
                    mouse.accepted = false
                }
            }
        }
    }

    Component {
        id: shaderTypeItemHighlight

        Rectangle {

            SystemPalette {
                id: shaderTypeItemHighlightPallete
                colorGroup: SystemPalette.Active
            }

            border.color: shaderTypeItemHighlightPallete.highlight
            border.width: 1
            radius: 2

            y: (editShaderTypeList.currentItem !== undefined && editShaderTypeList.currentItem !== null) ? editShaderTypeList.currentItem.y : 0
            x: (editShaderTypeList.currentItem !== undefined && editShaderTypeList.currentItem !== null) ? editShaderTypeList.currentItem.x : 0
            width: (editShaderTypeList.currentItem !== undefined && editShaderTypeList.currentItem !== null) ? editShaderTypeList.currentItem.width : 0
            height: (editShaderTypeList.currentItem !== undefined && editShaderTypeList.currentItem !== null) ? editShaderTypeList.currentItem.height : 0
        }
    }
}
