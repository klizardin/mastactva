import QtQuick 2.12
import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15
import QuizImageDemoArtefactSteps 1.0
import Mastactva 1.0


ApplicationWindow {
    id: root
    width: Constants.width
    height: Constants.height
    visible: true
    title: qsTr("Quiz Image Demo")


    FileDialog {
        id: openImageDialog
        title: qsTr("Please choose image to use for effect demo")
        folder: shortcuts.pictures
        nameFilters: [ "Image files (*.jpg *.jpeg *.png)" ]
        selectExisting: true
        selectMultiple: false

        signal openFile(string fileURL)
        signal closeDialog()

        onAccepted: {
            openFile(fileUrl)
            closeDialog()
        }

        onRejected: {
            closeDialog()
        }

        function init(titleVal, nameFiltersVal)
        {
            title = titleVal
            nameFilters = nameFiltersVal
        }
    }


    Action {
        id: updateShadersLog
        text: qsTr("&Update log info")
        onTriggered: {
            quizImageDemo.updateState()
        }
    }

    Action {
        id: clearShaderLog
        text: qsTr("&Clear log")
        onTriggered: {
            quizImageDemo.shadersBuildLog = ""
        }
    }

    Action {
        id: openFromImageFile
        text: qsTr("Open from image")
        onTriggered: {
            connect()
            openImageDialog.init(qsTr("Please choose image to use for effect demo"), [ "Image files (*.jpg *.jpeg *.png)" ])
            openImageDialog.open()
        }

        function connect()
        {
            openImageDialog.closeDialog.connect(closeDialog)
            openImageDialog.openFile.connect(openFile)
        }

        function disconnect()
        {
            openImageDialog.closeDialog.disconnect(closeDialog)
            openImageDialog.openFile.disconnect(openFile)
        }

        function closeDialog()
        {
            disconnect()
        }

        function openFile(imageUrl)
        {
            quizImageDemo.fromImage = imageUrl
        }
    }

    Action {
        id: openToImageFile
        text: qsTr("Open to image")
        onTriggered: {
            connect()
            openImageDialog.init(qsTr("Please choose image to use for effect demo"), [ "Image files (*.jpg *.jpeg *.png)" ])
            openImageDialog.open()
        }

        function connect()
        {
            openImageDialog.closeDialog.connect(closeDialog)
            openImageDialog.openFile.connect(openFile)
        }

        function disconnect()
        {
            openImageDialog.closeDialog.disconnect(closeDialog)
            openImageDialog.openFile.disconnect(openFile)
        }

        function closeDialog()
        {
            disconnect()
        }

        function openFile(imageUrl)
        {
            quizImageDemo.toImage = imageUrl
        }
    }

    Action {
        id: openToProjectFile
        text: qsTr("Open project")
        onTriggered: {
            connect()
            openImageDialog.init(qsTr("Please choose project to use for effect demo"), [ "Project files (*.json)" ])
            openImageDialog.open()
        }

        function connect()
        {
            openImageDialog.closeDialog.connect(closeDialog)
            openImageDialog.openFile.connect(openFile)
        }

        function disconnect()
        {
            openImageDialog.closeDialog.disconnect(closeDialog)
            openImageDialog.openFile.disconnect(openFile)
        }

        function closeDialog()
        {
            disconnect()
        }

        function openFile(imageUrl)
        {
            quizImageDemo.projectFilename = imageUrl
        }
    }

    Action {
        id: playEffectDemo
        text: qsTr("Play current effect demo")
        onTriggered: {
            quizImageDemoAnimationForward.running = true
        }
    }

    Action {
        id: reversePlayEffectDemo
        text: qsTr("Play reverse current effect demo")
        onTriggered: {
            quizImageDemoAnimationBackward.running = true
        }
    }

    Action {
        id: stopEffectDemo
        text: qsTr("&Stop effect demo")
        onTriggered: {
            quizImageDemoAnimationForward.running = false
            quizImageDemoAnimationBackward.running = false
        }
    }

    Action {
        id: updateArguments
        text: qsTr("&Update arguments")
        onTriggered: {
            quizImageDemo.updateArguments()
        }
    }

    function testEffectDemo(t)
    {
        quizImageDemoAnimationBackward.running = false
        quizImageDemoAnimationBackward.running = false
        quizImageDemo.t = t
    }

    Action {
        id: testEffect00Demo
        text: qsTr("Test current effect with t = 0.&0")
        onTriggered: {
            testEffectDemo(0.0)
        }
    }

    Action {
        id: testEffect01Demo
        text: qsTr("Test current effect with t = 0.&1")
        onTriggered: {
            testEffectDemo(0.1)
        }
    }

    Action {
        id: testEffect02Demo
        text: qsTr("Test current effect with t = 0.&2")
        onTriggered: {
            testEffectDemo(0.2)
        }
    }

    Action {
        id: testEffect03Demo
        text: qsTr("Test current effect with t = 0.&3")
        onTriggered: {
            testEffectDemo(0.3)
        }
    }

    Action {
        id: testEffect04Demo
        text: qsTr("Test current effect with t = 0.&4")
        onTriggered: {
            testEffectDemo(0.4)
        }
    }

    Action {
        id: testEffect05Demo
        text: qsTr("Test current effect with t = 0.&5")
        onTriggered: {
            testEffectDemo(0.5)
        }
    }

    Action {
        id: testEffect06Demo
        text: qsTr("Test current effect with t = 0.&6")
        onTriggered: {
            testEffectDemo(0.6)
        }
    }

    Action {
        id: testEffect07Demo
        text: qsTr("Test current effect with t = 0.&7")
        onTriggered: {
            testEffectDemo(0.7)
        }
    }

    Action {
        id: testEffect08Demo
        text: qsTr("Test current effect with t = 0.&8")
        onTriggered: {
            testEffectDemo(0.8)
        }
    }

    Action {
        id: testEffect09Demo
        text: qsTr("Test current effect with t = 0.&9")
        onTriggered: {
            testEffectDemo(0.9)
        }
    }

    Action {
        id: testEffect10Demo
        text: qsTr("Test current effect with t = 1.0")
        onTriggered: {
            testEffectDemo(1.0)
        }
    }


    MenuBar {
        id: mainMenu

        AutoSizeMenu {
            title: qsTr("&Log")
            MenuItem { action: updateShadersLog }
            MenuItem { action: clearShaderLog }
        }

        AutoSizeMenu {
            title: qsTr("&Images")
            MenuItem { action: openFromImageFile }
            MenuItem { action: openToImageFile }
        }

        AutoSizeMenu {
            title: qsTr("&Demo")
            MenuItem { action: playEffectDemo }
            MenuItem { action: reversePlayEffectDemo }
            MenuItem { action: stopEffectDemo }
            MenuItem { action: updateArguments }
            MenuItem { action: testEffect00Demo }
            MenuItem { action: testEffect01Demo }
            MenuItem { action: testEffect02Demo }
            MenuItem { action: testEffect03Demo }
            MenuItem { action: testEffect04Demo }
            MenuItem { action: testEffect05Demo }
            MenuItem { action: testEffect06Demo }
            MenuItem { action: testEffect07Demo }
            MenuItem { action: testEffect08Demo }
            MenuItem { action: testEffect09Demo }
            MenuItem { action: testEffect10Demo }
        }
    }

    menuBar: mainMenu

    SplitView {
        id: slitShadersAndDemo
        anchors.fill: parent
        orientation: Qt.Vertical

        Rectangle {
            id: quizImageDemoSV
            width: parent.width
            SplitView.preferredHeight: parent.height / 2
            SplitView.minimumHeight: parent.height / 4
            SplitView.maximumHeight: parent.height * 0.75
            color: "transparent"
            clip: true

            QuizImageDemo {
                id: quizImageDemo
                anchors.fill: parent
                fromImage: Constants.noImageResource
                toImage: Constants.noImageResource
                clip: true
                t: 0.5

                SequentialAnimation {
                    id: quizImageDemoAnimationForward
                    running: false

                    NumberAnimation { id: numberAnimationForward; target: quizImageDemo; property: "t"; from: 0.0; to: 1.0; duration: 3000; easing.type: Easing.Linear }
                }

                SequentialAnimation {
                    id: quizImageDemoAnimationBackward
                    running: false

                    NumberAnimation { id: numberAnimationBackward;  target: quizImageDemo; property: "t"; from: 1.0; to: 0.0; duration: 3000; easing.type: Easing.Linear }
                }

                Connections {
                    target: quizImageDemoAnimationForward

                    function onFinished()
                    {
                        console.log("effectDemoImageAnimationForward.finished()");
                    }
                }

                Connections {
                    target: quizImageDemoAnimationBackward

                    function onFinished()
                    {
                        console.log("effectDemoImageAnimationBackward.finished()");
                    }
                }

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                    }
                }
            }
        }

        Rectangle {
            id: quizCompilerLogSV
            width: parent.width
            SplitView.preferredHeight: parent.height / 2
            SplitView.minimumHeight: parent.height / 4
            SplitView.maximumHeight: parent.height * 0.75

            Label {
                id: shadersBuildLogLabel
                anchors.top: quizCompilerLogSV.top
                text: qsTr("Build log");
            }
            Text {
                id: shadersBuildLog
                anchors.top: shadersBuildLogLabel.bottom
                width: quizCompilerLogSV.width
                height: quizCompilerLogSV.height - shadersBuildLogLabel.height
                text: quizImageDemo.compilerLog
                wrapMode: Text.WordWrap
            }
        }
    }
}
