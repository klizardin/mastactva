import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15
import MastactvaQuizEditor 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


Dialog {
    id : chooseEffectObjectDialog
    modal: true

    title: qsTr("Choose effect object")

    property var fieldShader: undefined
    property var artefactTypeModel: undefined
    property var artefactModel: undefined

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
                spacing: Constants.smallListSmallSpacing
                model: artefactModel
                delegate: shaderItem
                highlight: shaderItemHighlight
                highlightFollowsCurrentItem: false
            }
        }
    }

    function init()
    {
        artefactModel.loadList()
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
                fieldShader = artefactModel.itemAt(index)
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
                        text: artefactTypeModel.findItemById(artefactTypeId) !== null ? artefactTypeModel.findItemById(artefactTypeId).artefactTypeType : ""
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
                        text: artefactFilename
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
                        text: artefactHash
                        wrapMode: Text.Wrap
                    }
                }

                Text {
                    id: shaderItemDescriptionText
                    width: Constants.smallDialogWidth
                    text: showFullDescription ? mastactva.leftDoubleCR(artefactDescription) : mastactva.readMore(artefactDescription, Constants.smallListReadMoreLength, qsTr(" ..."))
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
