import QtQuick 2.12
import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15
import MastactvaQuizEditor 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0

ApplicationWindow {
    id: logginWindow
    visible: true
    width: Constants.width
    height: Constants.height
    title: qsTr("Mastactva Quiz Editor (Login)")


    Loader {
        id: pageLoader
        onLoaded:
        {
            logginWindow.visible = false
        }
    }

    MastactvaAPI {
        id: mastactva
    }

    Column {
        anchors.fill: parent

        TextArea {
            anchors.horizontalCenter: parent.horizontalCenter
            id: message
            text: qsTr("")
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            Text {
                text: qsTr("Server URL : ")
            }
            TextField {
                id: serverURL
                placeholderText: qsTr("<Enter server URL>")
                text: mastactva.getHostURL()
            }
        }
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            Text {
                text: qsTr("Login : ")
            }
            TextField {
                id: login
                placeholderText: qsTr("<Enter login>")
            }
        }
        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            Text {
                text: qsTr("Password : ")
            }
            TextField {
                id: password
                placeholderText: qsTr("<Enter pasword>")
                echoMode: TextInput.Password
            }
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Login")
            onClicked: {
                mastactva.testLogin(serverURL.text, login.text, password.text)
                mastactva.onLogingIn.connect(onLogingIn)
                mastactva.onFailedLogingIn.connect(onFailedLogingIn)
            }

            function onLogingIn()
            {
                pageLoader.source = "main.qml"
            }

            function onFailedLogingIn(msg)
            {
                message.text = msg
            }
        }
    }
}
