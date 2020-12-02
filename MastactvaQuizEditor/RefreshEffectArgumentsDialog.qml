import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15
import MastactvaQuizEditor 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


Dialog {
    id : refreshArgumentsDialog
    modal: true

    property var fieldEffect: undefined

    title: "Refresh arguments"

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    ColumnLayout {
        RowLayout{
            ProgressBar {
                id: progress
                width: Constants.smallDialogWidth
            }
            Text {
                id: progressPercent
            }
        }
        Text {
            id: message
            width: Constants.smallDialogWidth
            wrapMode: Text.WordWrap
        }
    }

    function refreshArgumentsProgress(rate)
    {
        progress.value = rate
        progressPercent.text = rate*100 + qsTr(" %")
    }

    function refreshArgumentsBeforeApply()
    {
        progress.value = 1.0
        progressPercent.text = 100 + qsTr(" %")
        message.text = qsTr("Now you can apply refresh effect arguments")
    }

    function init()
    {
        fieldEffect.startRefreshArguments()
        message.text = qsTr("Wait while shader files are downloading ...")
    }

    function connect()
    {
        fieldEffect.refreshArgumentsProgress.connect(refreshArgumentsProgress)
        fieldEffect.refreshArgumentsBeforeApply.connect(refreshArgumentsBeforeApply)
    }

    function disconnect()
    {
        fieldEffect.refreshArgumentsProgress.disconnect(refreshArgumentsProgress)
        fieldEffect.refreshArgumentsBeforeApply.disconnect(refreshArgumentsBeforeApply)
    }

    standardButtons: Dialog.Cancel | Dialog.Save
}
