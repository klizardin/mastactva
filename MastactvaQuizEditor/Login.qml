/*
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
            mastactva.informInitialized()
        }
    }

    MastactvaAPI {
        id: mastactva
    }

    Connections {
        target: mastactva

        function onInitialized()
        {
            console.log("MastactvaAPI::onInitialized() at Login.qml")
        }
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
        ComboBox {
            id: serverURL
            editable: true
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - Constants.logingItemsSpacing * 2
            model: mastactva.getHostURLs()
        }
        Text {
            x: Constants.logingItemsSpacing
            text: qsTr("Login : ")
        }
        TextField {
            id: login
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - Constants.logingItemsSpacing * 2
            placeholderText: qsTr("Enter login")
            text: mastactva.getLastLogin()
        }
        Text {
            x: Constants.logingItemsSpacing
            text: qsTr("Password : ")
        }
        TextField {
            id: password
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - Constants.logingItemsSpacing * 2
            placeholderText: qsTr("Enter pasword")
            echoMode: TextInput.Password

            Keys.onReturnPressed: {
                tryLogin()
            }
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Login")
            onClicked: {
                tryLogin()
            }
        }
    }

    function tryLogin()
    {
        mastactva.testLogin(serverURL.editText, login.text, password.text)
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
