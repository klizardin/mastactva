import QtQuick 2.0
import QuizImageDemoArtefactStepsFB 1.0
import Mastactva 1.0


Rectangle {
    id: root1
    width: Constants.width
    height: Constants.height

    QuizImageDemo {
        objectName: "quizImage1"
        id: quizImage1
        anchors.fill: parent
        anchors.margins: 10
        t: 0.0
    }
}
