import QtQuick 2.0
import QuizImageDemoArtefactStepsFB 1.0
import Mastactva 1.0


Rectangle {
    id: root2
    width: Constants.width
    height: Constants.height

    QuizImageDemo {
        objectName: "quizImage2"
        id: quizImage2
        anchors.fill: parent
        anchors.margins: 10
        t: 0.0
    }
}
