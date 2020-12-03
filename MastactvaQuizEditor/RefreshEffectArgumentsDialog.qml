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
    property bool doneOk: false

    title: "Refresh arguments"

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    ColumnLayout {
        Text {
            id: message
            width: Constants.smallDialogWidth
            wrapMode: Text.WordWrap
        }
        Label {
            text: qsTr("Download shader files progress")
        }
        RowLayout{
            ProgressBar {
                id: downloadProgress
                width: Constants.smallDialogWidth
            }
            Text {
                id: downloadProgressPercent
            }
        }
        Label {
            text: qsTr("Refresh arguments progress")
        }
        RowLayout{
            ProgressBar {
                id: refreshProgress
                width: Constants.smallDialogWidth
            }
            Text {
                id: refreshProgressPercent
            }
        }
        Button {
            id: refreshEffectArguments
            Layout.alignment: Qt.AlignVCenter
            text: qsTr("Refresh")
            onClicked: {
                message.text = qsTr("Wait while effects argument are refreshing...")
                fieldEffect.applyRefreshArguments()
            }
        }
    }

    function refreshArgumentsProgress(download, rate)
    {
        if(download)
        {
            downloadProgress.value = rate
            downloadProgressPercent.text = rate*100 + qsTr(" %")
        }
        else
        {
            refreshProgress.value = rate;
            refreshProgressPercent.text = rate*100 + qsTr(" %")
        }
    }

    function refreshArgumentsBeforeApply()
    {
        downloadProgress.value = 1.0
        downloadProgressPercent.text = 100 + qsTr(" %")
        message.text = qsTr("Now you can apply refresh effect arguments")
        refreshEffectArguments.enabled = true
    }

    function refreshArgumentsAfterApply()
    {
        refreshProgress.value = 1.0;
        refreshProgressPercent.text = 100 + qsTr(" %")
        message.text = qsTr("Refresh of effect argument was done")
        doneOk = true
    }

    function init()
    {
        doneOk = false
        downloadProgress.value = 0.0
        downloadProgressPercent.text = 0 + qsTr(" %")
        refreshProgress.value = 0.0;
        refreshProgressPercent.text = 0 + qsTr(" %")
        message.text = qsTr("Wait while shader files are downloading ...")
        refreshEffectArguments.enabled = false
    }

    function connect()
    {
        fieldEffect.refreshArgumentsProgress.connect(refreshArgumentsProgress)
        fieldEffect.refreshArgumentsBeforeApply.connect(refreshArgumentsBeforeApply)
        fieldEffect.refreshArgumentsAfterApply.connect(refreshArgumentsAfterApply)
    }

    function disconnect()
    {
        fieldEffect.refreshArgumentsProgress.disconnect(refreshArgumentsProgress)
        fieldEffect.refreshArgumentsBeforeApply.disconnect(refreshArgumentsBeforeApply)
        fieldEffect.refreshArgumentsAfterApply.disconnect(refreshArgumentsAfterApply)
    }

    standardButtons: Dialog.Cancel | Dialog.Ok
}
