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
import QuizImageDemoArtefactStepsFB 1.0
import Mastactva 1.0


ApplicationWindow {
    id: root
    width: Constants.width
    height: Constants.height
    visible: true
    title: qsTr("Quiz Image Demo")


    QuizImageDemo {
        id: renderer
        anchors.fill: parent
        anchors.margins: 10
        t: 0.0
    }

    /*SequentialAnimation {
        id: animationCycle
        NumberAnimation { target: renderer; property: "t"; to: 1.0; duration: 5000; easing.type: Easing.Linear }
        NumberAnimation { target: renderer; property: "t"; to: 0.0; duration: 5000; easing.type: Easing.Linear }
        PauseAnimation { duration: 2000 }
        NumberAnimation { target: renderer; property: "t"; to: 1.0; duration: 5000; easing.type: Easing.Linear }
        PauseAnimation { duration: 2000 }
        NumberAnimation { target: renderer; property: "t"; to: 0.0; duration: 5000; easing.type: Easing.Linear }
        PauseAnimation { duration: 2000 }
        running: true
    }

    Connections {
        target: animationCycle

        function onFinished()
        {
            console.log("animationCycle.onFinished()")
            renderer.testIndex = renderer.testIndex + 1
            animationCycle.start()
        }

        function onStarted()
        {
            console.log("animationCycle.onStarted()")
            console.log("renderer.testIndex =", renderer.testIndex)
        }
    }*/
}
