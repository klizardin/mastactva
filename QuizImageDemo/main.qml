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
                quizImage.updateState()
            }
        }

        SequentialAnimation {
            id: switchImages

            PauseAnimation { duration: 500 }
            NumberAnimation { target: quizImage; property: "t"; from: 0.0; to: 1.0; duration: 2000 }
            PauseAnimation { duration: 500 }
            NumberAnimation { target: quizImage; property: "t"; from: 1.0; to: 0.0; duration: 2000 }

            running: true
            loops: Animation.Infinite
        }
    }
}
