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

    property bool fieldNew: false
    property int fieldDuration: 1000
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

    ScrollView
    {
        width: Constants.smallDialogWidth
        height: Constants.smallDialogHeight
        clip: true

        Column
        {
            Button {
                width: Constants.smallDialogWidth
                text: qsTr("Refresh effects list")
                onClicked: {
                    refreshEffects()
                }
            }

            Label {
                width: Constants.smallDialogWidth
                text: qsTr("Duration (ms) : ")
            }

            TextField {
                id: imagePointEffectDuration
                x: Constants.effectShaderTypeListSpacing / 2
                width: Constants.smallDialogWidth - Constants.effectShaderTypeListSpacing
                placeholderText: qsTr("Input duration of effect in ms")
            }

            Rectangle {
                id: effectsListRect
                x: Constants.effectShaderTypeListSpacing / 2
                width: Constants.smallDialogWidth - Constants.effectShaderTypeListSpacing
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
                    z: 0.0
                }

                BusyIndicator {
                    id: busyIndicatorEffectsList
                    anchors.centerIn: parent
                    visible: false
                    running: false
                    z: 1.0
                }
            }

            Rectangle {
                id: effectArgumentSetsListRect
                x: Constants.effectShaderTypeListSpacing / 2
                width: Constants.smallDialogWidth - Constants.effectShaderTypeListSpacing
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
                    z: 0.0
                }

                BusyIndicator {
                    id: busyIndicatorEffectArgumentSetsList
                    anchors.centerIn: parent
                    visible: false
                    running: false
                    z: 1.0
                }
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
                effectId = effect !== null ? effect.effectId : -1
                //console.log("onEffectIndexChanged() effectId = ", effectId)
                initEffectArgumentSets()
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
                    effectArgumentSetsId = effectArgumentSet !== null ? effectArgumentSet.effectArgSetId : -1
                    //console.log("onEffectArgumentSetsIndexChanged() effectArgumentSetsId = ", effectArgumentSetsId)
                }
            }
        }
    }

    function init()
    {
        console.log("init()")
        if(effectModel !== undefined && effectModel !== null)
        {
            effectModel.listReloaded.connect(effectModelListReloaded)
            //effectModel.loadList()
            if(effectModel.isListLoaded())
            {
                setEffectModel()
            }
            else
            {
                busyIndicatorEffectsList.visible = true
                busyIndicatorEffectsList.running = true
                clearEffectModel()
            }
        }
        else
        {
            clearEffectModel()
        }
        imagePointEffectDuration.text = fieldDuration
    }

    function refreshEffects()
    {
        if(effectModel !== undefined && effectModel !== null)
        {
            effectModel.listReloaded.connect(effectModelListReloaded)
            effectModel.loadList()
            if(effectModel.isListLoaded())
            {
                setEffectModel()
            }
            else
            {
                busyIndicatorEffectsList.visible = true
                busyIndicatorEffectsList.running = true
                clearEffectModel()
            }
        }
        else
        {
            clearEffectModel()
        }
    }

    function update()
    {
        fieldDuration = imagePointEffectDuration.text
    }

    function reject()
    {
        //console.log("reject()")
        if(effectModel !== undefined && effectModel !== null && effectModel.isListLoaded())
        {
            effectModel.currentIndex = effectOldIndex
            //console.log("effectModel.currentIndex = ", effectModel.currentIndex)
            var effectArgumentSetsModel = effectModel.currentItem.effectArgSets
            if(effectArgumentSetsModel !== undefined && effectArgumentSetsModel !== null && effectArgumentSetsModel.isListLoaded())
            {
                effectArgumentSetsModel.currentIndex = effectArgumentSetsOldIndex
                //console.log("effectArgumentSetsModel.currentIndex = ", effectArgumentSetsModel.currentIndex)
            }
        }
    }

    function clearDialogData()
    {
        clearEffectArgumentSetsModel()
        clearEffectModel()
        effectArgumentSetsIndex = -1
        effectIndex = -1
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
                //console.log("effectId = ", effectId)
                //console.log("effectModel.currentIndex = ", effectModel.currentIndex)
                if(effectId >= 0)
                {
                    var effect = effectModel.findItemById(effectId)
                    var initIndex = effectModel.indexOfItem(effect)
                    if(effect !== null && initIndex >= 0)
                    {
                        effectModel.currentIndex = initIndex
                    }
                    else
                    {
                        effectId = -1
                    }
                }
                //console.log("effectModel.currentIndex = ", effectModel.currentIndex)
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
        busyIndicatorEffectsList.visible = false
        busyIndicatorEffectsList.running = false
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
                    //console.log("effectArgumentSetsId = ", effectArgumentSetsId)
                    //console.log("effectArgumentSets.currentIndex = ", effectArgumentSets.currentIndex)
                    if(effectArgumentSetsId >= 0)
                    {
                        var effectArgumentSet = effectArgumentSets.findItemById(effectArgumentSetsId)
                        var initIndex = effectArgumentSets.indexOfItem(effectArgumentSet)
                        if(effectArgumentSet !== null && initIndex >= 0)
                        {
                            effectArgumentSets.currentIndex = initIndex
                        }
                        else
                        {
                            effectArgumentSetsId = -1
                        }
                    }
                    //console.log("effectArgumentSets.currentIndex = ", effectArgumentSets.currentIndex)
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

    standardButtons: Dialog.Cancel | Dialog.Save

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
