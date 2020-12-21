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


    property string vertexFilename: ""
    property string fragmentFilename: ""

    Connections{
        target: root

        function onVertexFilenameChanged()
        {
            console.log("vertexFilename = ", vertexFilename)
            quizImageDemo.vertexShaderFilename = vertexFilename
        }

        function onFragmentFilenameChanged()
        {
            console.log("fragmentFilename = ", fragmentFilename)
            quizImageDemo.fragmentShaderFilename = fragmentFilename
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
        id: updateVertexShaderFile
        text: qsTr("Update &vertex file")
        onTriggered: {
        }
    }

    Action {
        id: updateFragmentShaderFile
        text: qsTr("Update &fragment file")
        onTriggered: {
        }
    }

    MenuBar {
        id: mainMenu

        AutoSizeMenu {
            title: qsTr("&Shaders")
            MenuItem { action: openVertexShaderFile }
            MenuItem { action: openFragmentShaderFile }
            MenuItem { action: updateVertexShaderFile }
            MenuItem { action: updateFragmentShaderFile }
        }

        AutoSizeMenu {
            title: qsTr("&Effect")
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
            SplitView.maximumWidth: Constants.shadersWidth * 1.5
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
                    TextArea {
                        id: vertexShaderEdit
                        anchors.fill: parent
                        readOnly: false
                    }
                }

                Item {
                    TextArea {
                        id: fragmentShaderEdit
                        anchors.fill: parent
                        readOnly: false
                    }
                }
            }
        }

        Rectangle{
            id: splitDemo
            height: parent.height
            SplitView.preferredWidth: Constants.shadersWidth
            SplitView.minimumWidth: Constants.shadersWidth / 2
            SplitView.maximumWidth: Constants.shadersWidth * 1.5

            QuizImage {
                id: quizImageDemo
                width: splitDemo.width
                height: (splitDemo.width * Constants.aspectY) / Constants.aspectX
                fromImage: Constants.noImageResource
                toImage: Constants.noImageResource
                t: 0.5
            }
            Label {
                id: shadersBuildLogLabel
                anchors.top: quizImageDemo.bottom
                text: qsTr("Build log");
            }
            Text {
                id: shadersBuildLog
                anchors.top: shadersBuildLogLabel.bottom
                width: splitDemo.width
                height: splitDemo.height - (quizImageDemo.height + shadersBuildLogLabel.height)
                text: quizImageDemo.shadersBuildLog
                wrapMode: Text.WordWrap
            }
        }
    }
}
