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
import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15
import ShaderTest 1.0
import ShaderTestQML 1.0


ApplicationWindow {
    id: root
    visible: true
    width: Constants.width
    height: Constants.height
    title: qsTr("Shader test")


    property string vertexFilename: Constants.noNameVertex
    property string fragmentFilename: Constants.noNameFragment

    Connections{
        target: root

        function onVertexFilenameChanged()
        {
            console.log("vertexFilename = ", vertexFilename)
            quizImageDemo.vertexShaderFilename = vertexFilename
            vertexShaderEdit.text = quizImageDemo.vertexShader
        }

        function onFragmentFilenameChanged()
        {
            console.log("fragmentFilename = ", fragmentFilename)
            quizImageDemo.fragmentShaderFilename = fragmentFilename
            fragmentShaderEdit.text = quizImageDemo.fragmentShader
        }
    }

    FileDialog {
        id: openVertexDialog
        title: qsTr("Please choose vertex shader file to open")
        folder: shortcuts.pictures
        nameFilters: [ "Vertex shaders (*.vsh *.vert *.vertex)" ]
        selectExisting: true
        selectMultiple: false

        onAccepted: {
            vertexFilename = fileUrl
        }

        onRejected: {
            // TODO: add no image upload reaction
        }
    }

    FileDialog {
        id: openFragmentDialog
        title: qsTr("Please choose fragment shader file to open")
        folder: shortcuts.pictures
        nameFilters: [ "Vertex shaders (*.fsh *.frag *.fragment)" ]
        selectExisting: true
        selectMultiple: false

        onAccepted: {
            fragmentFilename = fileUrl
        }

        onRejected: {
            // TODO: add no image upload reaction
        }
    }

    FileDialog {
        id: openImageDialog
        title: qsTr("Please choose image to upload to the server")
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
    }

    Action {
        id: openVertexShaderFile
        text: qsTr("Open vertex file")
        onTriggered: {
            openVertexDialog.open()
        }
    }

    Action {
        id: openFragmentShaderFile
        text: qsTr("Open fragment file")
        onTriggered: {
            openFragmentDialog.open()
        }
    }

    Action {
        id: useShaders
        text: qsTr("Use &shaders")
        onTriggered: {
            quizImageDemo.vertexShader = vertexShaderEdit.text
            quizImageDemo.fragmentShader = fragmentShaderEdit.text
            quizImageDemo.updateShaders()
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
            title: qsTr("&Shaders")
            MenuItem { action: openVertexShaderFile }
            MenuItem { action: openFragmentShaderFile }
            MenuItem { action: useShaders }
        }

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
        orientation: Qt.Horizontal

        Rectangle{
            id: splitShaders

            SplitView.preferredWidth: Constants.shadersWidth
            SplitView.minimumWidth: Constants.shadersWidth / 2
            SplitView.maximumWidth: root.width * 0.75
            height: parent.height

            TabBar {
                id: shaderTabBar
                anchors.top: parent.top
                width: parent.width
                TabButton {
                    text: qsTr("Vertex")
                }
                TabButton {
                    text: qsTr("Fragment")
                }
            }

            StackLayout {
                anchors.top: shaderTabBar.bottom
                anchors.left: parent.left
                width: parent.width
                height: parent.height - shaderTabBar.height
                currentIndex: shaderTabBar.currentIndex

                Item {
                    ScrollView{
                        anchors.fill: parent
                        TextArea {
                            id: vertexShaderEdit
                            readOnly: false
                        }
                    }
                }

                Item {
                    ScrollView{
                        anchors.fill: parent
                        TextArea {
                            id: fragmentShaderEdit
                            anchors.fill: parent
                            readOnly: false
                        }
                    }
                }
            }
        }

        Rectangle{
            id: splitDemo
            height: parent.height
            SplitView.preferredWidth: Constants.shadersWidth
            SplitView.minimumWidth: Constants.shadersWidth / 2
            SplitView.maximumWidth: root.width * 0.75

            Rectangle {
                id: quizImageDemoClipper
                width: splitDemo.width
                height: (splitDemo.width * Constants.aspectY) / Constants.aspectX
                color: "transparent"
                clip: true

                QuizImage {
                    id: quizImageDemo
                    width: splitDemo.width
                    height: (splitDemo.width * Constants.aspectY) / Constants.aspectX
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
                }
            }
            Label {
                id: shadersBuildLogLabel
                anchors.top: quizImageDemoClipper.bottom
                text: qsTr("Build log");
            }
            Text {
                id: shadersBuildLog
                anchors.top: shadersBuildLogLabel.bottom
                width: splitDemo.width
                height: splitDemo.height - (quizImageDemoClipper.height + shadersBuildLogLabel.height)
                text: quizImageDemo.shadersBuildLog
                wrapMode: Text.WordWrap
            }
        }
    }
}
