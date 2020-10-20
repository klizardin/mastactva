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
    width: Constants.loginWidth
    height: Constants.loginHeight
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
        spacing: Constants.logingItemsSpacing

        TextArea {
            id: message
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - Constants.logingItemsSpacing * 2
            text: qsTr("")
        }

        Text {
            x: Constants.logingItemsSpacing
            text: qsTr("Server URL : ")
        }
        TextField {
            id: serverURL
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - Constants.logingItemsSpacing * 2
            placeholderText: qsTr("<Enter server URL>")
            text: mastactva.getHostURL()
        }
        Text {
            x: Constants.logingItemsSpacing
            text: qsTr("Login : ")
        }
        TextField {
            id: login
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - Constants.logingItemsSpacing * 2
            placeholderText: qsTr("<Enter login>")
        }
        Text {
            x: Constants.logingItemsSpacing
            text: qsTr("Password : ")
        }
        TextField {
            id: password
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - Constants.logingItemsSpacing * 2
            placeholderText: qsTr("<Enter pasword>")
            echoMode: TextInput.Password
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
