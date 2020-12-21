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
        }

        function onFragmentFilenameChanged()
        {
            console.log("fragmentFilename = ", fragmentFilename)
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

            Text { text: qsTr("Shaders"); }
        }

        Rectangle{
            id: splitDemo
            height: parent.height
            SplitView.preferredWidth: Constants.shadersWidth
            SplitView.minimumWidth: Constants.shadersWidth / 2
            SplitView.maximumWidth: Constants.shadersWidth * 1.5

            Text { text: qsTr("Demo"); }
        }
    }
}
