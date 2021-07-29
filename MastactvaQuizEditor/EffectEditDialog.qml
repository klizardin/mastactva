import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15


Dialog {
    id : effectEditDialog
    modal: true

    property bool fieldNewItem: false
    property var fieldEffect: undefined

    title: fieldNewItem ? qsTr("Create New Effect") : qsTr("Edit Effect")

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    ScrollView {
        anchors.fill: parent
        clip:true

        ColumnLayout {
            RowLayout {
                Label {
                    text: qsTr("Id : ")
                }
                Text {
                    id: editEffectID
                }
            }
            Label {
                text: qsTr("Name  : ")
            }
            TextField {
                id: editEffectName
                placeholderText: qsTr("Enter effect name")
                focus: true
                KeyNavigation.priority: KeyNavigation.BeforeItem
                KeyNavigation.tab: editEffectDescription
            }
            Label {
                text: qsTr("Created : ")
            }
            Text {
                id: editEffectCreated
            }
            Label {
                text: qsTr("Description  : ")
            }
            TextArea {
                id: editEffectDescription
                placeholderText: qsTr("Enter effect description")
                focus: true
                KeyNavigation.priority: KeyNavigation.BeforeItem
                KeyNavigation.backtab: editEffectName
            }
        }
    }

    function init()
    {
        editEffectID.text = fieldEffect.effectId
        editEffectName.text = fieldEffect.effectName
        editEffectDescription.text = fieldEffect.effectDescription
        editEffectCreated.text = fieldEffect.effectCreated
    }

    function update()
    {
        fieldEffect.effectId = editEffectID.text
        fieldEffect.effectName = editEffectName.text
        fieldEffect.effectDescription = editEffectDescription.text
    }

    standardButtons: Dialog.Cancel | Dialog.Save
}
