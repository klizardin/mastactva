import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15
import MastactvaQuizEditor 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


Dialog {
    id : chooseEffectObjectArtefactDialog
    modal: true

    title: qsTr("Choose artefact (for effect object)")

    property var artefactTypeModel: undefined
    property var artefactModel: undefined

    // result:
    property var fieldArtefact: undefined

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    Column
    {
        Rectangle {
            id: artefactsListRect
            width: Constants.smallDialogWidth
            height: Constants.smallDialogHeight

            ListView {
                id: artefactsList
                anchors.fill: parent
                clip: true
                spacing: Constants.smallListSmallSpacing
                model: artefactModel
                delegate: artefactItem
                highlight: artefactItemHighlight
                highlightFollowsCurrentItem: false
            }
        }
    }

    function init()
    {
        fieldArtefact = undefined
        artefactModel.listReloaded.connect(artefactModelListReloaded)
        artefactModel.loadList()
    }

    function artefactModelListReloaded()
    {
        artefactModel.listReloaded.disconnect(artefactModelListReloaded)
        fieldArtefact = artefactModel.currentItem
    }

    standardButtons: Dialog.Cancel | Dialog.Save

    Component {
        id: artefactItem

        MouseArea {
            width: Constants.smallDialogWidth
            height: childrenRect.height
            acceptedButtons: Qt.LeftButton | Qt.RightButton

            property bool showFullDescription: false

            onClicked:
            {
                artefactsList.currentIndex = index
                fieldArtefact = artefactModel.itemAt(index)
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
                        id: artefactItemTypeLabel
                        text: qsTr("Type : ")
                    }

                    Text {
                        id: artefactItemType
                        width: Constants.smallDialogWidth - artefactItemTypeLabel
                        text: artefactTypeModel.findItemById(artefactTypeId) !== null ? artefactTypeModel.findItemById(artefactTypeId).artefactTypeType : ""
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    Label {
                        id: artefactItemFilenameLabel
                        text: qsTr("Filename : ")
                    }

                    Text {
                        id: artefactItemFilename
                        width: Constants.smallDialogWidth - artefactItemFilenameLabel.width
                        text: artefactFilename
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    Label {
                        id: artefactItemHashLabel
                        text: qsTr("Hash : ")
                    }

                    Text {
                        id: artefactItemHash
                        width: Constants.smallDialogWidth - artefactItemHashLabel.width
                        text: artefactHash
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    Label {
                        id: artefactItemNameLabel
                        text: qsTr("Name : ")
                    }

                    Text {
                        id: artefactItemName
                        width: Constants.smallDialogWidth - artefactItemNameLabel.width
                        text: artefactName
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    Label {
                        id: artefactItemCreatedLabel
                        text: qsTr("Created : ")
                    }

                    Text {
                        id: artefactItemCreated
                        width: Constants.smallDialogWidth - artefactItemCreatedLabel.width
                        text: mastactva.dateTimeToUserStr(artefactCreated)
                        wrapMode: Text.Wrap
                    }
                }

                Text {
                    id: artefactItemDescriptionText
                    width: Constants.smallDialogWidth
                    text: showFullDescription ? mastactva.leftDoubleCR(artefactDescription) : mastactva.readMore(artefactDescription, Constants.smallListReadMoreLength, qsTr(" ..."))
                    wrapMode: Text.WordWrap
                }
            }
        }
    }

    Component {
        id: artefactItemHighlight

        Rectangle {
            SystemPalette {
                id: artefactItemHighlightPallete
                colorGroup: SystemPalette.Active
            }

            border.color: artefactItemHighlightPallete.highlight
            border.width: 2
            radius: 5
            y: (artefactsList.currentItem !== undefined && artefactsList.currentItem !== null) ? artefactsList.currentItem.y : 0
            x: (artefactsList.currentItem !== undefined && artefactsList.currentItem !== null) ? artefactsList.currentItem.x : 0
            width: (artefactsList.currentItem !== undefined && artefactsList.currentItem !== null) ? artefactsList.currentItem.width : 0
            height: (artefactsList.currentItem !== undefined && artefactsList.currentItem !== null) ? artefactsList.currentItem.height : 0
        }
    }
}
