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
        nameFilters: [ "Shader files (*.vert *.vertex *.frag *.fragment *.*)" ]
        selectExisting: true
        selectMultiple: false

        onAccepted: {
            editShaderFilename.text = fileUrl
            editShaderHash.text = mastactva.calculateHash(fileUrl)
        }

        onRejected: {
            // TODO: add no image upload reaction
        }
    }

    FontMetrics{
        id: editShaderFilenameFontMetrics
        font: editShaderFilename.font
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
                    id: editShaderID
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
                    id: editShaderFilename
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
                id: editShaderHash
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
                KeyNavigation.backtab: editShaderHash
            }
        }
    }

    function init()
    {
        var w = 0
        for(var i = 0; i < shaderTypeModel.size(); i++)
        {
            w = Math.max(w, editShaderFilenameFontMetrics.tightBoundingRect(shaderTypeModel.itemAt(i).shaderTypeType).width)
        }
        editShaderTypeListRect.width = w
        editShaderTypeListRect.height = (editShaderFilenameFontMetrics.height + Constants.effectShaderTypeListSpacing) * shaderTypeModel.size() * 1.1
        editShaderID.text = fieldShader.shaderId
        if(!editShaderTypeList.model.selectItemById(fieldShader.shaderTypeId))
        {
            editShaderTypeList.currentIndex = 0
        }
        editShaderFilename.text = fieldShader.shaderFilename
        editShaderHash.text = fieldShader.shaderHash
        editShaderDescription.text = fieldShader.shaderDescription
    }

    function update()
    {
        fieldShader.shaderTypeId = editShaderTypeList.currentItem.shaderTypeId
        fieldShader.shaderFilename = editShaderFilename.text
        fieldShader.shaderHash = editShaderHash.text
        fieldShader.shaderDescription = editShaderDescription.text
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
