import QtQuick 2.12
import QtQuick.Controls 2.5
import QuizImageDemo 1.0


ApplicationWindow {
    id: window
    width: 640
    height: 480
    visible: true
    title: qsTr("Quiz Image Demo")

    property var images : [":/images/side2.png", ":/images/side3.png", ":/images/side4.png", ":/images/side5.png", ":/images/side6.png"]
    property int currentImage: 0

    QuizImage {
        id: quizImage
        anchors.fill: parent
        fromImage: ":/images/side1.png"
        toImage: images[currentImage]

        SequentialAnimation {
            id: switchImages

            PropertyAction { target: quizImage; property: "color"; value: "yellow" }
            NumberAnimation { target: quizImage; property: "t"; from: 0.0; to: 1.0; duration: 2000; easing.type: Easing.InOutQuad }
            PauseAnimation { duration: 500 }
            NumberAnimation { target: quizImage; property: "t"; from: 1.0; to: 0.0; duration: 2000; easing.type: Easing.InOutQuad  }
            PauseAnimation { duration: 500 }

            PropertyAction { target: quizImage; property: "color"; value: "blue" }
            NumberAnimation { target: quizImage; property: "t"; from: 0.0; to: 1.0; duration: 2000; easing.type: Easing.InOutQuad  }
            PauseAnimation { duration: 500 }
            NumberAnimation { target: quizImage; property: "t"; from: 1.0; to: 0.0; duration: 2000; easing.type: Easing.InOutQuad  }
            PauseAnimation { duration: 500 }

            PropertyAction { target: quizImage; property: "color"; value: "green" }
            NumberAnimation { target: quizImage; property: "t"; from: 0.0; to: 1.0; duration: 2000; easing.type: Easing.InOutQuad  }
            PauseAnimation { duration: 500 }
            NumberAnimation { target: quizImage; property: "t"; from: 1.0; to: 0.0; duration: 2000; easing.type: Easing.InOutQuad  }
            PauseAnimation { duration: 500 }

            PropertyAction { target: quizImage; property: "color"; value: "white" }
            NumberAnimation { target: quizImage; property: "t"; from: 0.0; to: 1.0; duration: 2000; easing.type: Easing.InOutQuad  }
            PauseAnimation { duration: 500 }
            NumberAnimation { target: quizImage; property: "t"; from: 1.0; to: 0.0; duration: 2000; easing.type: Easing.InOutQuad  }
            PauseAnimation { duration: 500 }

            running: true
            loops: Animation.Infinite
        }

        MouseArea {
            anchors.fill: parent

            onClicked: {
                if(currentImage + 1 >= 5) { currentImage = 0 }
                else { ++currentImage }
            }
        }
    }
}
