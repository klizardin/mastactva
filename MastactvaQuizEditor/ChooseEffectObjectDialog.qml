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

    // public:
    property var fieldEffectObject: undefined
    property var effectObjectsModel: undefined
    property int fieldEditEffectObjectStepValue: -1

    // private:
    property int effectObjectModelIndex: -1

    Connections {
        target: chooseEffectObjectDialog

        function onEffectObjectModelIndexChanged()
        {
            if(effectObjectModelIndex >= 0 && isValidDialog())
            {
                console.log("effectObjectModelIndex=",effectObjectModelIndex)
                effectObjectsModel.currentIndex = effectObjectModelIndex
                fieldEffectObject = effectObjectsModel.currentItem
                console.log("fieldEffectObject id=",fieldEffectObject.effectObjectsId)
            }
            else
            {
                fieldEffectObject = null
            }
        }
    }

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    Column
    {
        Row {
            id: stepRow
            Label {
                text: qsTr("Effect object step : ")
            }
            TextField {
                id: editEffectObjectStep
                placeholderText: qsTr("Effect object step")
                focus: true
                KeyNavigation.priority: KeyNavigation.BeforeItem
                KeyNavigation.tab: effectObjectsList
            }
        }

        Rectangle {
            id: effectObjectsListRect
            width: Constants.smallDialogWidth
            height: Constants.smallDialogHeight - stepRow.height

            ListView {
                id: effectObjectsList

                anchors.fill: parent
                spacing: Constants.smallListViewSpacing
                clip: true
                model: 0
                delegate: effectObjectsItem
                highlight: effectObjectsItemHighlight
                highlightFollowsCurrentItem: false
                z: 0.0

                BusyIndicator {
                    id: effectObjectsListBusyIndicator
                    anchors.centerIn: parent
                    visible: false
                    running: false
                    z: 1.0
                }
            }
        }
    }

    // public:
    function init()
    {
        editEffectObjectStep.text = fieldEditEffectObjectStepValue
        effectObjectModelIndex = -1
        if(isValidDialog())
        {
            effectObjectsListBusyIndicator.visible = true
            effectObjectsListBusyIndicator.running = true
            effectObjectsList.model = 0
            effectObjectsModel.listReloaded.connect(onEffectObjectModelListLoaded)
            effectObjectsModel.loadList()
        }
    }

    function update()
    {
        fieldEditEffectObjectStepValue = editEffectObjectStep.text
        console.log("fieldEditEffectObjectStepValue=",fieldEditEffectObjectStepValue)
    }

    // private:
    function isValidDialog()
    {
        var isValidModel = effectObjectsModel !== undefined && effectObjectsModel !== null
        return isValidModel;
    }

    function onEffectObjectModelListLoaded()
    {
        effectObjectsListBusyIndicator.visible = false
        effectObjectsListBusyIndicator.running = false
        if(isValidDialog())
        {
            effectObjectsModel.listReloaded.disconnect(onEffectObjectModelListLoaded)
            effectObjectsList.model = effectObjectsModel
            if(fieldEffectObject === null || fieldEffectObject === undefined)
            {
                effectObjectModelIndex = effectObjectsModel.currentIndex
            }
            else
            {
                effectObjectModelIndex = effectObjectsModel.indexOfItem(fieldEffectObject)
            }
        }
        effectObjectsList.currentIndex = effectObjectModelIndex
    }

    standardButtons: Dialog.Cancel | Dialog.Save

    Component {
        id: effectObjectsItem

        MouseArea {
            width: childrenRect.width
            height: childrenRect.height

            acceptedButtons: Qt.LeftButton | Qt.RightButton

            property var objecInfo: effectObjectsObjectInfo.currentItem
            property bool showFullDescription: false

            Connections {
                target: effectObjectsObjectInfo

                function onListReloaded()
                {
                    objecInfo = effectObjectsObjectInfo.currentItem
                }
            }

            onClicked:
            {
                effectObjectModelIndex = index
            }

            onDoubleClicked: {
                showFullDescription = !showFullDescription
            }

            Column {
                id: effectObjectItemRect
                width: effectObjectsList.width

                FontMetrics{
                    id: effectObjectItemFontMetrics
                    font: effectObjectItemStepIndex.font
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: effectObjectItemStepIndexLabel
                        text: qsTr("Step : ")
                    }
                    Text {
                        id: effectObjectItemStepIndex
                        width: effectObjectsList.width - effectObjectItemStepIndexLabel.width
                        text: effectObjectsStepIndex
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: effectObjectItemNameLabel
                        text: qsTr("Name : ")
                    }
                    Text {
                        id: effectObjectItemName
                        width: effectObjectsList.width - effectObjectItemNameLabel.width
                        text: objecInfo !== undefined && objecInfo !== null ? objecInfo.effectObjectInfoName : ""
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: effectObjectItemProgrammerNameLabel
                        text: qsTr("Programmer name : ")
                    }
                    Text {
                        id: effectObjectItemProgrammerName
                        width: effectObjectsList.width - effectObjectItemProgrammerNameLabel.width
                        text: objecInfo !== undefined && objecInfo !== null ? objecInfo.effectObjectInfoProgrammerName : ""
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: effectObjectItemCreatedLabel
                        text: qsTr("Created : ")
                    }
                    Text {
                        id: effectObjectItemCreated
                        width: effectObjectsList.width - effectObjectItemCreatedLabel.width
                        text: objecInfo !== undefined && objecInfo !== null ? mastactva.dateTimeToUserStr(objecInfo.effectObjectInfoCreated) : ""
                        wrapMode: Text.Wrap
                    }
                }

                Text {
                    id: effectObjectItemDescriptionText
                    width: effectObjectsList.width
                    wrapMode: Text.WordWrap
                    text: objecInfo !== undefined && objecInfo !== null ? showFullDescription ? mastactva.leftDoubleCR(objecInfo.effectObjectInfoDescription ) : mastactva.readMore(objecInfo.effectObjectInfoDescription, Constants.smallListReadMoreLength, qsTr(" ..."))  : ""
                }
            }
        }
    }

    Component {
        id: effectObjectsItemHighlight

        Rectangle {
            SystemPalette {
                id: effectObjectItemHighlightPallete
                colorGroup: SystemPalette.Active
            }

            border.color: effectObjectItemHighlightPallete.highlight
            border.width: 2
            radius: 5
            y: (effectObjectsList.currentItem !== undefined && effectObjectsList.currentItem !== null) ? effectObjectsList.currentItem.y : 0
            x: (effectObjectsList.currentItem !== undefined && effectObjectsList.currentItem !== null) ? effectObjectsList.currentItem.x : 0
            width: (effectObjectsList.currentItem !== undefined && effectObjectsList.currentItem !== null) ? effectObjectsList.currentItem.width : 0
            height: (effectObjectsList.currentItem !== undefined && effectObjectsList.currentItem !== null) ? effectObjectsList.currentItem.height : 0
        }
    }
}
