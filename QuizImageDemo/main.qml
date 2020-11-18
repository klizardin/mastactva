import QtQuick 2.12
import QtQuick.Controls 2.5
import QuizImageDemo 1.0


ApplicationWindow {
    id: window
    width: 640
    height: 480
    visible: true
    title: qsTr("Quiz Image Demo")

    QuizImage {
        id: quizImage
        anchors.fill: parent

        MouseArea {
            anchors.fill: parent

            onClicked: {
                //quizImage.updateState()
            }
        }
    }
}
