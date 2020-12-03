import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15
import MastactvaQuizEditor 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


Dialog {
    id : effectArgumentSetEditDialog
    modal: true

    property var easingTypeModel: undefined
    property bool fieldNew: false
    property var fieldEffectArgumentSet: undefined

    title: fieldNew ? "Add new argument set" : "Edit argument set"

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    FontMetrics{
        id: effectArgumentSetFontMetrics
        font: effectArgumentSetId.font
    }

    ColumnLayout {

        RowLayout {
            Label {
                text: qsTr("Id :")
            }

            Text {
                id: effectArgumentSetId
            }
        }

        Label {
            text: qsTr("Easing :")
        }

        Rectangle{
            id: easingTypeListRect
            width: Constants.minConstValueWidth
            height: Constants.minConstValueHeight

            ListView {
                id: easingTypeList
                anchors.fill: parent
                clip: true
                spacing: Constants.effectShaderTypeListSpacing
                model: easingTypeModel
                delegate: easingTypeItem
                highlight: easingTypeItemHighlight
                highlightFollowsCurrentItem: false
            }
        }

        Label {
            text: qsTr("Description :")
        }

        TextArea {
            id: effectArgumentSetDescription
            width: Constants.minConstValueWidth
            height: Constants.minConstValueHeight
            placeholderText: qsTr("Enter argument set description")
            focus: true
        }
    }

    function init()
    {
        var w = 0
        for(var i = 0; i < easingTypeModel.size(); i++)
        {
            w = Math.max(w, effectArgumentSetFontMetrics.tightBoundingRect(easingTypeModel.itemAt(i).easingTypeType).width)
        }
        easingTypeListRect.width = w
        easingTypeListRect.height = (effectArgumentSetFontMetrics.height + Constants.effectShaderTypeListSpacing) * shaderTypeModel.size() * 1.1

        effectArgumentSetId.text = fieldEffectArgumentSet.effectArgSetId
        if(!easingTypeList.model.selectItemById(fieldEffectArgumentSet.effectArgSetEasingId))
        {
            easingTypeList.currentIndex = 0
            easingTypeList.model.currentIndex = 0
            fieldEffectArgumentSet.effectArgSetEasingId = easingTypeList.model.getCurrentItem().easingTypeId
        }
        effectArgumentSetDescription.text = fieldEffectArgumentSet.effectArgSetDescription
    }

    function update()
    {
        console.log("easingTypeList.model.currentIndex = ", easingTypeList.model.currentIndex)
        console.log("easingTypeList.model.getCurrentItem().easingTypeId = ", easingTypeList.model.getCurrentItem().easingTypeId)
        fieldEffectArgumentSet.effectArgSetId = effectArgumentSetId.text
        fieldEffectArgumentSet.effectArgSetEasingId = easingTypeList.model.getCurrentItem().easingTypeId
        fieldEffectArgumentSet.effectArgSetDescription = effectArgumentSetDescription.text
    }

    standardButtons: Dialog.Cancel | Dialog.Save

    Component {
        id: easingTypeItem

        Text {
            id: easingTypeItemType
            width: easingTypeListRect.width
            text: easingTypeType

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton

                onClicked:
                {
                    easingTypeList.currentIndex = index
                    easingTypeList.model.currentIndex = index
                    console.log("easingTypeList.model.currentIndex = ", easingTypeList.model.currentIndex)
                    mouse.accepted = false
                }
            }
        }
    }

    Component {
        id: easingTypeItemHighlight

        Rectangle {

            SystemPalette {
                id: easingTypeItemHighlightPallete
                colorGroup: SystemPalette.Active
            }

            border.color: easingTypeItemHighlightPallete.highlight
            border.width: 1
            radius: 2

            y: (easingTypeList.currentItem !== undefined && easingTypeList.currentItem !== null) ? easingTypeList.currentItem.y : 0
            x: (easingTypeList.currentItem !== undefined && easingTypeList.currentItem !== null) ? easingTypeList.currentItem.x : 0
            width: (easingTypeList.currentItem !== undefined && easingTypeList.currentItem !== null) ? easingTypeList.currentItem.width : 0
            height: (easingTypeList.currentItem !== undefined && easingTypeList.currentItem !== null) ? easingTypeList.currentItem.height : 0
        }
    }
}
