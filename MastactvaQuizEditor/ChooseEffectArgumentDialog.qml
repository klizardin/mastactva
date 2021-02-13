import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15
import MastactvaQuizEditor 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


Dialog {
    id : chooseEffectArgumentDialog
    modal: true

    title: qsTr("Choose shader")

    property var artefactArgTypeModel: undefined
    property var effectArgModel: undefined
    property var fieldEffectArg: undefined
    property var fieldExceptIds: undefined

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    Column
    {
        Text {
            id: message
            width: Constants.smallDialogWidth
            wrapMode: Text.WordWrap
        }

        Rectangle {
            id: effectArgumentsListRect
            width: Constants.smallDialogWidth
            height: Constants.smallDialogHeight

            ListView {
                id: effectArgumentsList
                anchors.fill: parent
                clip: true
                spacing: Constants.smallListSmallSpacing
                model: effectArgModel
                delegate: effectArgumentsItem
                highlight: effectArgumentsItemHighlight
                highlightFollowsCurrentItem: false
            }
        }
    }

    function init()
    {
        effectArgModel.listReloaded.connect(listReloaded)
        effectArgModel.loadList()
    }

    function listReloaded()
    {
        effectArgModel.listReloaded.disconnect(listReloaded)
        effectArgModel.filterItemsLeftExceptIDs(fieldExceptIds)
        if(effectArgModel.size() > 0)
        {
            effectArgumentsList.currentIndex = 0
            fieldEffectArg = effectArgModel.currentItem
            message.text = qsTr("Please choose argument to set up value")
        }
        else
        {
            fieldEffectArg = undefined
            message.text = qsTr("Sorry, there is no arguments to add to the argument set. Try to edit existing ones")
        }
    }

    standardButtons: Dialog.Cancel | Dialog.Save


    Component {
        id: effectArgumentsItem

        MouseArea {
            width: childrenRect.width
            height: childrenRect.height

            acceptedButtons: Qt.LeftButton | Qt.RightButton

            property bool showFullDescription: false

            onClicked:
            {
                effectArgumentsList.currentIndex = index
                effectArgModel.currentIndex = index
                fieldEffectArg = effectArgModel.currentItem
                mouse.accepted = false
            }

            Column {
                id: effectArgumentsItemRect
                width: effectArgumentsList.width

                FontMetrics{
                    id: effectArgumentsItemFontMetrics
                    font: effectArgumentsItemType.font
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: effectArgumentsItemTypeLabel
                        text: qsTr("Type : ")
                    }
                    Text {
                        id: effectArgumentsItemType
                        width: effectArgumentsList.width - effectArgumentsItemTypeLabel.width
                        text: artefactArgTypeModel.findItemById(effectArgArgTypeId) !== null ? artefactArgTypeModel.findItemById(effectArgArgTypeId).artefactArgTypeType : ""
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: effectArgumentsItemNameLabel
                        text: qsTr("Name : ")
                    }

                    Text {
                        id: effectArgumentsItemName
                        width: effectArgumentsList.width - effectArgumentsItemNameLabel.width
                        text: effectArgName
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: effectArgumentsItemDefaultValueLabel
                        text: qsTr("Default value : ")
                    }

                    Text {
                        id: effectArgumentsDefaultValue
                        width: effectArgumentsList.width - effectArgumentsItemDefaultValueLabel.width
                        text: effectArgDefaultValue
                        wrapMode: Text.Wrap
                    }
                }

                Text {
                    id: effectArgumentsItemDescriptionText
                    width: effectArgumentsList.width
                    wrapMode: Text.WordWrap
                    text: showFullDescription ? mastactva.leftDoubleCR(effectArgDescription) : mastactva.readMore(effectArgDescription, Constants.smallListReadMoreLength, qsTr(" ..."))
                }
            }
        }
    }

    Component {
        id: effectArgumentsItemHighlight

        Rectangle {
            SystemPalette {
                id: effectArgumentsItemHighlightPallete
                colorGroup: SystemPalette.Active
            }

            border.color: effectArgumentsItemHighlightPallete.highlight
            border.width: 2
            radius: 5
            y: (effectArgumentsList.currentItem !== undefined && effectArgumentsList.currentItem !== null) ? effectArgumentsList.currentItem.y : 0
            x: (effectArgumentsList.currentItem !== undefined && effectArgumentsList.currentItem !== null) ? effectArgumentsList.currentItem.x : 0
            width: (effectArgumentsList.currentItem !== undefined && effectArgumentsList.currentItem !== null) ? effectArgumentsList.currentItem.width : 0
            height: (effectArgumentsList.currentItem !== undefined && effectArgumentsList.currentItem !== null) ? effectArgumentsList.currentItem.height : 0
        }
    }
}
