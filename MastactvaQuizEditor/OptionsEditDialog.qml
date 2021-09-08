/*
    Copyright 2021

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


Dialog {
    id : optionsEditDialog
    modal: true

    property alias fieldUseColorsVoronoyDiagram : useColorsVoronoyDiagram.checked
    property alias fieldRegenerateColors: regenerateColors.checked
    property alias fieldLanguage : language.currentText
    property alias fieldLanguageIndex : language.currentIndex

    title: qsTr("Edit Options")
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    ScrollView {
        anchors.fill: parent
        clip:true

        ColumnLayout {
            CheckBox {
                id: useColorsVoronoyDiagram
                text: qsTr("Use colors voronoy diagram")
            }
            CheckBox {
                id: regenerateColors
                text: qsTr("Regenerate colors")
            }
            ComboBox {
                id: language
                editable: false
                model: [ qsTr("English"), qsTr("Belarus") ]
            }
        }
    }

    standardButtons: Dialog.Cancel | Dialog.Save
}
