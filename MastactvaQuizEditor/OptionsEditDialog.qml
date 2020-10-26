import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15


Dialog {
    id : optionsEditDialog
    modal: true

    property alias fieldUseColorsVoronoyDiagram : useColorsVoronoyDiagram.checked
    property alias fieldRegenerateColors: regenerateColors.checked

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
        }
    }

    standardButtons: Dialog.Cancel | Dialog.Save
}
