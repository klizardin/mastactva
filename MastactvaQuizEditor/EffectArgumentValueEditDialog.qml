import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15
import MastactvaQuizEditor 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


Dialog {
    id : effectArgumentValueEditDialog
    modal: true

    property var shaderArgTypeModel: undefined
    property bool fieldNew: false
    property var fieldEffectArgumentSet: undefined
    property var fieldEffectArgumentValue: undefined

    title: fieldNew ? "Add new argument set" : "Edit argument set"

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    FontMetrics{
        id: effectArgumentSetFontMetrics
        font: effectArgumentValueId.font
    }

    ColumnLayout {

        RowLayout {
            Label {
                text: qsTr("Id :")
            }

            Text {
                id: effectArgumentValueId
            }
        }

        RowLayout {
            Label {
                text: qsTr("Set description :")
            }

            TextArea {
                id: effectArgumentValueSetDescription
                width: Constants.minConstValueWidth
                height: Constants.minConstValueHeight
                readOnly: true
                focus: true
            }
        }

        RowLayout {
            Label {
                text: qsTr("Argument type :")
            }

            Text {
                id: effectArgumentValueArgType
            }
        }

        RowLayout {
            Label {
                text: qsTr("Argument name :")
            }

            Text {
                id: effectArgumentValueArgName
            }
        }

        RowLayout {
            Label {
                text: qsTr("Argument default value :")
            }

            Text {
                id: effectArgumentValueArgDefaultValue
            }
        }

        RowLayout {
            Label {
                text: qsTr("Argument description :")
            }

            TextArea {
                id: effectArgumentValueArgDescription
                width: Constants.minConstValueWidth
                height: Constants.minConstValueHeight
                readOnly: true
                focus: true
            }
        }

        RowLayout {
            Label {
                text: qsTr("Value :")
            }

            TextArea {
                id: effectArgumentValueValue
                width: Constants.minConstValueWidth
                height: Constants.minConstValueHeight
                focus: true
            }
        }

        RowLayout {
            Label {
                text: qsTr("Value description :")
            }

            TextArea {
                id: effectArgumentValueValueDescription
                width: Constants.minConstValueWidth
                height: Constants.minConstValueHeight
                focus: true
            }
        }

    }

    function init()
    {
        effectArgumentValueId.text = fieldEffectArgumentValue.effectArgValueId
        effectArgumentValueSetDescription.text = fieldEffectArgumentSet.effectArgSetDescription
        console.log("fieldEffectArgumentValue.effectArgValueArg.isListLoaded() = ", fieldEffectArgumentValue.effectArgValueArg.isListLoaded())
        effectArgumentValueArgType.text = shaderArgTypeModel.findItemById(fieldEffectArgumentValue.effectArgValueArg.currentItem.effectArgArgTypeId).shaderArgTypeType
        effectArgumentValueArgName.text = fieldEffectArgumentValue.effectArgValueArg.effectArgName
        effectArgumentValueArgDefaultValue.text = fieldEffectArgumentValue.effectArgValueArg.effectArgDefaultValue
        effectArgumentValueArgDescription.text = fieldEffectArgumentValue.effectArgValueArg.effectArgDescription
        effectArgumentValueValue = fieldEffectArgumentValue.effectArgValueValue
        effectArgumentValueValueDescription = fieldEffectArgumentValue.effectArgValueDescription
    }

    function update()
    {
        fieldEffectArgumentValue.effectArgValueValue = effectArgumentValueValue
        fieldEffectArgumentValue.effectArgValueDescription = effectArgumentValueValueDescription
    }

    standardButtons: Dialog.Cancel | Dialog.Save
}
