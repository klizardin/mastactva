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
    id : effectArgumentValueEditDialog
    modal: true

    property var artefactArgTypeModel: undefined
    property bool fieldNew: false
    property var fieldEffectArgumentSet: undefined
    property var fieldEffectArgumentValue: undefined
    property var fieldEffectArg: undefined

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
        if(fieldNew)
        {
            effectArgumentValueArgType.text = artefactArgTypeModel.findItemById(fieldEffectArg.effectArgArgTypeId).artefactArgTypeType
            effectArgumentValueArgName.text = fieldEffectArg.effectArgName
            effectArgumentValueArgDefaultValue.text = fieldEffectArg.effectArgDefaultValue
            effectArgumentValueArgDescription.text = fieldEffectArg.effectArgDescription
        }
        else
        {
            effectArgumentValueArgType.text = artefactArgTypeModel.findItemById(fieldEffectArgumentValue.effectArgValueArg.currentItem.effectArgArgTypeId).artefactArgTypeType
            effectArgumentValueArgName.text = fieldEffectArgumentValue.effectArgValueArg.currentItem.effectArgName
            effectArgumentValueArgDefaultValue.text = fieldEffectArgumentValue.effectArgValueArg.currentItem.effectArgDefaultValue
            effectArgumentValueArgDescription.text = fieldEffectArgumentValue.effectArgValueArg.currentItem.effectArgDescription
        }
        effectArgumentValueValue.text = fieldEffectArgumentValue.effectArgValueValue
        effectArgumentValueValueDescription.text = fieldEffectArgumentValue.effectArgValueDescription
    }

    function update()
    {
        fieldEffectArgumentValue.effectArgValueValue = effectArgumentValueValue.text
        fieldEffectArgumentValue.effectArgValueDescription = effectArgumentValueValueDescription.text
        if(fieldNew)
        {
            fieldEffectArgumentValue.setArgId(fieldEffectArg.effectArgId)
        }
    }

    standardButtons: Dialog.Cancel | Dialog.Save
}
