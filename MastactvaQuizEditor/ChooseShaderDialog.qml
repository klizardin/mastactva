import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15
import MastactvaQuizEditor 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


Dialog {
    id : chooseShaderDialog
    modal: true

    title: qsTr("Choose shader")

    property var fieldShader: undefined
    property var shaderTypeModel: undefined
    property var shaderModel: undefined

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    Column
    {
        Rectangle {
            id: shadersListRect
            width: Constants.smallDialogWidth
            height: Constants.smallDialogHeight

            ListView {
                id: shadersList
                anchors.fill: parent
                clip: true
                spacing: Constants.effectShaderTypeListSpacing
                model: shaderModel
                delegate: shaderItem
                highlight: shaderItemHighlight
                highlightFollowsCurrentItem: false
            }
        }
    }

    function init()
    {
        shaderModel.loadList()
    }

    standardButtons: Dialog.Cancel | Dialog.Save

    Component {
        id: shaderItem

        MouseArea {
            width: Constants.smallDialogWidth
            height: childrenRect.height
            acceptedButtons: Qt.LeftButton | Qt.RightButton

            property bool showFullDescription: false

            onClicked:
            {
                shadersList.currentIndex = index
                fieldShader = shaderModel.itemAt(index)
                console.log("fieldShader = ", fieldShader)
                console.log("fieldShader.shaderId = ", fieldShader.shaderId)
                mouse.accepted = false
            }

            onDoubleClicked: {
                showFullDescription = !showFullDescription
            }

            Column {
                width: Constants.smallDialogWidth

                FontMetrics{
                    id: shaderItemFontMetrics
                    font: shaderItemType.font
                }

                Row {
                    Label {
                        id: shaderItemTypeLabel
                        text: qsTr("Type : ")
                    }

                    Text {
                        id: shaderItemType
                        width: Constants.smallDialogWidth - shaderItemTypeLabel
                        text: shaderTypeModel.findItemById(shaderTypeId) !== null ? shaderTypeModel.findItemById(shaderTypeId).shaderTypeType : ""
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    Label {
                        id: shaderItemFilenameLabel
                        text: qsTr("Filename : ")
                    }

                    Text {
                        id: shaderItemFilename
                        width: Constants.smallDialogWidth - shaderItemFilenameLabel.width
                        text: shaderFilename
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    Label {
                        id: shaderItemHashLabel
                        text: qsTr("Hash : ")
                    }

                    Text {
                        id: shaderItemHash
                        width: Constants.smallDialogWidth - shaderItemHashLabel.width
                        text: shaderHash
                        wrapMode: Text.Wrap
                    }
                }

                Text {
                    id: shaderItemDescriptionText
                    width: Constants.smallDialogWidth
                    text: showFullDescription ? mastactva.leftDoubleCR(shaderDescription) : mastactva.readMore(shaderDescription, Constants.effectsListReadMoreLength, qsTr(" ..."))
                    wrapMode: Text.WordWrap
                }
            }
        }
    }

    Component {
        id: shaderItemHighlight

        Rectangle {
            SystemPalette {
                id: shaderItemHighlightPallete
                colorGroup: SystemPalette.Active
            }

            border.color: shaderItemHighlightPallete.highlight
            border.width: 2
            radius: 5
            y: (shadersList.currentItem !== undefined && shadersList.currentItem !== null) ? shadersList.currentItem.y : 0
            x: (shadersList.currentItem !== undefined && shadersList.currentItem !== null) ? shadersList.currentItem.x : 0
            width: (shadersList.currentItem !== undefined && shadersList.currentItem !== null) ? shadersList.currentItem.width : 0
            height: (shadersList.currentItem !== undefined && shadersList.currentItem !== null) ? shadersList.currentItem.height : 0
        }
    }
}
