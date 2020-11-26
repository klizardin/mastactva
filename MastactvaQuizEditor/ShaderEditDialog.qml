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
        clip:true

        ColumnLayout {
            RowLayout {
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
            ListView {
                id: editShaderTypeList
                height: editShaderFilenameFontMetrics.height * shaderTypeModel.size() * 1.5
                width: Constants.minConstValueWidth
                clip: true
                model: shaderTypeModel
                delegate: shaderTypeItem
            }

            Label {
                text: qsTr("Filename  : ")
            }
            RowLayout {
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
                placeholderText: qsTr("Enter effect name")
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
        console.log("shaderTypeModel.size() = ", shaderTypeModel.size())
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
            text: shaderTypeType
        }
    }
}
