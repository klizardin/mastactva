import QtQuick 2.12
import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15
import QuizImageDemoArtefactSteps 1.0
import Mastactva 1.0


ApplicationWindow {
    id: window
    width: 640
    height: 480
    visible: true
    title: qsTr("Quiz Image Demo")

    QuizImageDemo {
        id: quizImageDemo
        anchors.fill: parent
        fromImage: ""
        toImage: ""

        SequentialAnimation {
            id: forwardAnimation

            NumberAnimation { target: quizImageDemo; property: "t"; from: 0.0; to: 1.0; duration: 3000; easing.type: Easing.Linear }
            running: false
            //loops: Animation.Infinite
        }

        MouseArea {
            anchors.fill: parent

            onClicked: {
            }
        }
    }
}
