import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15
import MastactvaQuizEditor 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


Dialog {
    id : imagePointEffectEditDialog
    modal: true

    property bool fieldNew: true
    property var effectModel: undefined
    property int effectOldIndex: -1
    property int effectArgumentSetsOldIndex: -1
    property int effectIndex: -1
    property int effectArgumentSetsIndex: -1
    property int effectId: -1
    property int effectArgumentSetsId: -1

    title: qsTr("Choose image point effect")

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    Column
    {
        Rectangle {
            id: effectsListRect
            width: Constants.smallDialogWidth
            height: Constants.smallDialogHeight / 2

            ListView {
                id: effectsList
                anchors.fill: parent
                clip: true
                spacing: Constants.effectShaderTypeListSpacing
                model: 0
                delegate: effectItem
                highlight: effectItemHighlight
                highlightFollowsCurrentItem: false
            }
        }

        Rectangle {
            id: effectArgumentSetsListRect
            width: Constants.smallDialogWidth
            height: Constants.smallDialogHeight / 2

            ListView {
                id: effectArgumentSetsList
                anchors.fill: parent
                clip: true
                spacing: Constants.effectShaderTypeListSpacing
                model: 0
                delegate: effectArgumentSetsItem
                highlight: effectArgumentSetsItemHighlight
                highlightFollowsCurrentItem: false
            }
        }
    }

    Connections {
        target: imagePointEffectEditDialog

        function onEffectIndexChanged()
        {
            effectsList.currentIndex = effectIndex
            if(effectModel !== undefined && effectModel !== null && effectModel.isListLoaded() && effectModel.itemAt(effectIndex) !== null)
            {
                effectModel.currentIndex = effectIndex
                var effect = effectModel.currentItem
                effectId = effect.effectId
            }
        }

        function onEffectArgumentSetsIndexChanged()
        {
            effectArgumentSetsList.currentIndex = effectArgumentSetsIndex
            if(effectModel !== undefined && effectModel !== null && effectModel.isListLoaded() && effectModel.currentItem !== null)
            {
                var effectArgumentSetsModel = effectModel.currentItem.effectArgSets
                if(effectArgumentSetsModel !== undefined && effectArgumentSetsModel !== null && effectArgumentSetsModel.isListLoaded())
                {
                    effectArgumentSetsModel.currentIndex = effectArgumentSetsIndex
                    var effectArgumentSet = effectArgumentSetsModel.currentItem
                    effectArgumentSetsId = effectArgumentSet.effectArgSetId
                }
            }
        }
    }

    function init()
    {
        if(effectModel !== undefined && effectModel !== null)
        {
            effectModel.listReloaded.connect(effectModelListReloaded)
            if(effectModel.isListLoaded())
            {
                setEffectModel()
            }
            else
            {
                clearEffectModel()
            }
        }
        else
        {
            clearEffectModel()
        }
    }

    function reject()
    {
        if(effectModel !== undefined && effectModel !== null)
        {
            if(effectModel.isListLoaded())
            {
                effectModel.currentIndex = effectOldIndex
                var effectArgumentSetsModel = effectModel.currentItem.effectArgSets
                if(effectArgumentSetsModel !== undefined && effectArgumentSetsModel !== null && effectArgumentSetsModel.isListLoaded())
                {
                    effectArgumentSetsModel.currentIndex = effectArgumentSetsOldIndex
                }
            }
        }
    }

    function clearEffectModel()
    {
        effectsList.model = 0
        effectsList.currentIndex = -1
    }

    function setEffectModel()
    {
        if(effectModel !== undefined && effectModel !== null)
        {
            effectModel.listReloaded.disconnect(effectModelListReloaded)
            if(effectModel.isListLoaded())
            {
                effectOldIndex = effectModel.currentIndex
                effectsList.model = effectModel
                effectIndex = effectModel.currentIndex
                initEffectArgumentSets()
            }
            else
            {
                clearEffectModel()
            }
        }
    }

    function effectModelListReloaded()
    {
        setEffectModel()
    }

    function initEffectArgumentSets()
    {
        if(effectModel !== undefined && effectModel !== null && effectModel.isListLoaded())
        {
            var effectArgumentSets = effectModel.currentItem.effectArgSets
            if(effectArgumentSets !== undefined && effectArgumentSets !== null)
            {
                effectArgumentSets.listReloaded.connect(effectArgumentSetsListReloaded)
                if(effectArgumentSets.isListLoaded())
                {
                    setEffectArgumentSetsModel()
                }
                else
                {
                    clearEffectArgumentSetsModel()
                }
            }
        }
    }

    function clearEffectArgumentSetsModel()
    {
        effectArgumentSetsList.model = 0
        effectArgumentSetsList.currentIndex = -1
    }

    function setEffectArgumentSetsModel()
    {
        if(effectModel !== undefined && effectModel !== null && effectModel.isListLoaded())
        {
            var effectArgumentSets = effectModel.currentItem.effectArgSets
            if(effectArgumentSets !== undefined && effectArgumentSets !== null)
            {
                effectArgumentSets.listReloaded.disconnect(effectArgumentSetsListReloaded)
                if(effectArgumentSets.isListLoaded())
                {
                    effectArgumentSetsOldIndex = effectArgumentSets.currentIndex
                    effectArgumentSetsList.model = effectArgumentSets
                    effectArgumentSetsIndex = effectArgumentSets.currentIndex
                }
                else
                {
                    clearEffectArgumentSetsModel()
                }
            }
        }
    }

    function effectArgumentSetsListReloaded()
    {
        setEffectArgumentSetsModel()
    }

    Component {
        id: effectItem
        Column {
            id: effectItemRect

            FontMetrics{
                id: effectItemFontMetrics
                font: effectItemHeaderText.font
            }

            width: effectsList.width

            property bool showFullDescription: false

            Text {
                id: effectItemHeaderText
                width: effectsList.width
                wrapMode: Text.WordWrap
                text: effectName
                padding: Constants.effectsListHeaderPadding

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton

                    onClicked:
                    {
                        if (mouse.button === Qt.RightButton)
                        {
                            effectItemMenu.popup()
                        }
                        else
                        {
                            effectIndex = index
                            mouse.accepted = false
                        }
                    }

                    onPressAndHold: {
                        if (mouse.source === Qt.MouseEventNotSynthesized)
                            effectItemMenu.popup()
                    }

                    onDoubleClicked: {
                        showFullDescription = !showFullDescription
                    }

                    AutoSizeMenu {
                        id: effectItemMenu
                        MenuItem { action: refreshEffect }
                        MenuItem { action: addEffect }
                        MenuItem { action: editEffect }
                        MenuItem { action: removeEffect }
                    }
                }
            }

            Text {
                id: effectItemDescriptionText
                width: effectsList.width
                wrapMode: Text.WordWrap
                text: showFullDescription ? mastactva.leftDoubleCR(effectDescription) : mastactva.readMore(effectDescription, Constants.effectsListReadMoreLength, qsTr(" ..."))

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton

                    onClicked:
                    {
                        if (mouse.button === Qt.RightButton)
                        {
                            effectItemMenu.popup()
                        }
                        else
                        {
                            effectCurrentIndex = index
                            mouse.accepted = false
                        }
                    }

                    onPressAndHold: {
                        if (mouse.source === Qt.MouseEventNotSynthesized)
                            effectItemMenu.popup()
                    }

                    onDoubleClicked: {
                        showFullDescription = !showFullDescription
                    }
                }
            }
        }
    }

    Component {
        id: effectItemHighlight

        Rectangle {
            SystemPalette {
                id: effectItemHighlightPallete
                colorGroup: SystemPalette.Active
            }

            border.color: effectItemHighlightPallete.highlight
            border.width: 2
            radius: 5
            y: (effectsList.currentItem !== undefined && effectsList.currentItem !== null) ? effectsList.currentItem.y : 0
            x: (effectsList.currentItem !== undefined && effectsList.currentItem !== null) ? effectsList.currentItem.x : 0
            width: (effectsList.currentItem !== undefined && effectsList.currentItem !== null) ? effectsList.currentItem.width : 0
            height: (effectsList.currentItem !== undefined && effectsList.currentItem !== null) ? effectsList.currentItem.height : 0
        }
    }

    Component {
        id: effectArgumentSetsItem

        MouseArea {
            width: childrenRect.width
            height: childrenRect.height

            acceptedButtons: Qt.LeftButton | Qt.RightButton

            property bool showFullDescription: false

            onClicked:
            {
                if (mouse.button === Qt.RightButton)
                {
                    effectArgumentSetsItemMenu.popup()
                }
                else
                {
                    effectArgumentSetsIndex = index
                    mouse.accepted = false
                }
            }

            onPressAndHold: {
                if (mouse.source === Qt.MouseEventNotSynthesized)
                    effectArgumentSetsItemMenu.popup()
            }

            onDoubleClicked: {
                showFullDescription = !showFullDescription
            }

            AutoSizeMenu {
                id: effectArgumentSetsItemMenu
                MenuItem { action: addArgumentSet }
                MenuItem { action: editArgumentSet }
                MenuItem { action: removeArgumentSet }
            }

            Column {
                id: effectArgumentSetsItemRect
                width: effectArgumentSetsList.width

                FontMetrics{
                    id: effectArgumentSetsItemFontMetrics
                    font: effectArgumentSetsItemType.font
                }

                Row {
                    padding: Constants.effectShaderListHeaderPadding
                    Label {
                        id: effectArgumentSetsItemTypeLabel
                        text: qsTr("Easing type : ")
                    }
                    Text {
                        id: effectArgumentSetsItemType
                        width: effectArgumentSetsList.width - effectArgumentSetsItemTypeLabel.width
                        text: easingTypeModel.findItemById(effectArgSetEasingId) !== null ? easingTypeModel.findItemById(effectArgSetEasingId).easingTypeType : ""
                        wrapMode: Text.Wrap
                    }
                }

                Text {
                    id: effectArgumentSetsItemDescriptionText
                    width: effectArgumentSetsList.width
                    wrapMode: Text.WordWrap
                    text: showFullDescription ? mastactva.leftDoubleCR(effectArgSetDescription) : mastactva.readMore(effectArgSetDescription, Constants.effectsListReadMoreLength, qsTr(" ..."))
                }
            }
        }
    }

    Component {
        id: effectArgumentSetsItemHighlight

        Rectangle {
            SystemPalette {
                id: effectArgumentSetsItemHighlightPallete
                colorGroup: SystemPalette.Active
            }

            border.color: effectArgumentSetsItemHighlightPallete.highlight
            border.width: 2
            radius: 5
            y: (effectArgumentSetsList.currentItem !== undefined && effectArgumentSetsList.currentItem !== null) ? effectArgumentSetsList.currentItem.y : 0
            x: (effectArgumentSetsList.currentItem !== undefined && effectArgumentSetsList.currentItem !== null) ? effectArgumentSetsList.currentItem.x : 0
            width: (effectArgumentSetsList.currentItem !== undefined && effectArgumentSetsList.currentItem !== null) ? effectArgumentSetsList.currentItem.width : 0
            height: (effectArgumentSetsList.currentItem !== undefined && effectArgumentSetsList.currentItem !== null) ? effectArgumentSetsList.currentItem.height : 0
        }
    }

}
