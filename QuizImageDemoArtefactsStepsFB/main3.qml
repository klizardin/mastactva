import QtQuick 2.0
import QuizImageDemoArtefactStepsFB 1.0
import Mastactva 1.0


Rectangle {
    id: root
    width: Constants.width
    height: Constants.height

    QuizImageDemo {
        id: renderer
        anchors.fill: parent
        anchors.margins: 10
        t: 0.0
    }
}
