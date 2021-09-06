/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15
import MastactvaQuizEditor 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


Dialog {
    id : chooseObjectInfoDialog
    modal: true

    title: qsTr("Choose effect object")

    // public:
    property var fieldObjectInfo: undefined
    property var objectInfoModel: undefined
    property int fieldEditEffectObjectStepValue: -1

    // private:
    property int objectInfoModelIndex: -1

    Connections {
        target: chooseObjectInfoDialog

        function onObjectInfoModelIndexChanged()
        {
            if(objectInfoModelIndex >= 0 && isValidDialog())
            {
                objectInfoModel.currentIndex = objectInfoModelIndex
                fieldObjectInfo = objectInfoModel.currentItem
            }
            else
            {
                fieldObjectInfo = null
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
                KeyNavigation.tab: objectInfoList
            }
        }

        Rectangle {
            id: objectInfoListRect
            width: Constants.smallDialogWidth
            height: Constants.smallDialogHeight - stepRow.height

            ListView {
                id: objectInfoList

                anchors.fill: parent
                spacing: Constants.smallListViewSpacing
                clip: true
                model: 0
                delegate: objectInfoItem
                highlight: objectInfoItemHighlight
                highlightFollowsCurrentItem: false
                z: 0.0

                BusyIndicator {
                    id: objectInfoListBusyIndicator
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
        objectInfoModelIndex = -1
        if(isValidDialog())
        {
            objectInfoListBusyIndicator.visible = true
            objectInfoListBusyIndicator.running = true
            objectInfoList.model = 0
            objectInfoModel.listReloaded.connect(onEffectObjectModelListLoaded)
            objectInfoModel.loadList()
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
        var isValidModel = objectInfoModel !== undefined && objectInfoModel !== null
        return isValidModel;
    }

    function onEffectObjectModelListLoaded()
    {
        objectInfoListBusyIndicator.visible = false
        objectInfoListBusyIndicator.running = false
        if(isValidDialog())
        {
            objectInfoModel.listReloaded.disconnect(onEffectObjectModelListLoaded)
            objectInfoList.model = objectInfoModel
            if(fieldObjectInfo === null || fieldObjectInfo === undefined)
            {
                objectInfoModelIndex = objectInfoModel.currentIndex
            }
            else
            {
                objectInfoModelIndex = objectInfoModel.indexOfItem(fieldObjectInfo)
            }
        }
        objectInfoList.currentIndex = objectInfoModelIndex
    }

    standardButtons: Dialog.Cancel | Dialog.Save

    Component {
        id: objectInfoItem

        MouseArea {
            width: childrenRect.width
            height: childrenRect.height

            acceptedButtons: Qt.LeftButton | Qt.RightButton

            property bool showFullDescription: false

            onClicked:
            {
                objectInfoModelIndex = index
            }

            onDoubleClicked: {
                showFullDescription = !showFullDescription
            }

            Column {
                id: objectInfoItemRect
                width: objectInfoList.width

                FontMetrics{
                    id: effectObjectItemFontMetrics
                    font: objectInfoItemName.font
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: objectInfoItemNameLabel
                        text: qsTr("Name : ")
                    }
                    Text {
                        id: objectInfoItemName
                        width: objectInfoList.width - objectInfoItemNameLabel.width
                        text: effectObjectInfoName
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: objectInfoItemProgrammerNameLabel
                        text: qsTr("Programmer name : ")
                    }
                    Text {
                        id: objectInfoItemProgrammerName
                        width: objectInfoList.width - objectInfoItemProgrammerNameLabel.width
                        text: effectObjectInfoProgrammerName
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: objectInfoItemCreatedLabel
                        text: qsTr("Created : ")
                    }
                    Text {
                        id: objectInfoItemCreated
                        width: objectInfoList.width - objectInfoItemCreatedLabel.width
                        text: mastactva.dateTimeToUserStr(effectObjectInfoCreated)
                        wrapMode: Text.Wrap
                    }
                }

                Text {
                    id: objectInfoItemDescriptionText
                    width: objectInfoList.width
                    wrapMode: Text.WordWrap
                    text: showFullDescription ? mastactva.leftDoubleCR(effectObjectInfoDescription ) : mastactva.readMore(effectObjectInfoDescription, Constants.smallListReadMoreLength, qsTr(" ..."))
                }
            }
        }
    }

    Component {
        id: objectInfoItemHighlight

        Rectangle {
            SystemPalette {
                id: effectObjectItemHighlightPallete
                colorGroup: SystemPalette.Active
            }

            border.color: effectObjectItemHighlightPallete.highlight
            border.width: 2
            radius: 5
            y: (objectInfoList.currentItem !== undefined && objectInfoList.currentItem !== null) ? objectInfoList.currentItem.y : 0
            x: (objectInfoList.currentItem !== undefined && objectInfoList.currentItem !== null) ? objectInfoList.currentItem.x : 0
            width: (objectInfoList.currentItem !== undefined && objectInfoList.currentItem !== null) ? objectInfoList.currentItem.width : 0
            height: (objectInfoList.currentItem !== undefined && objectInfoList.currentItem !== null) ? objectInfoList.currentItem.height : 0
        }
    }
}
