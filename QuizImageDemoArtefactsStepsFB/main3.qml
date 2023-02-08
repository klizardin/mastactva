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

    SequentialAnimation {
        id: animationCycle
        NumberAnimation { target: renderer; property: "t"; to: 1.0; duration: 5000; easing.type: Easing.Linear }
        NumberAnimation { target: renderer; property: "t"; to: 0.0; duration: 5000; easing.type: Easing.Linear }
        PauseAnimation { duration: 2000 }
        NumberAnimation { target: renderer; property: "t"; to: 1.0; duration: 5000; easing.type: Easing.Linear }
        PauseAnimation { duration: 2000 }
        NumberAnimation { target: renderer; property: "t"; to: 0.0; duration: 5000; easing.type: Easing.Linear }
        PauseAnimation { duration: 2000 }
        running: true
        loops: Animation.Infinite
    }

    Connections {
        target: animationCycle

        function onFinished()
        {
            console.log("animationCycle.onFinished()")
            //renderer.testIndex = renderer.testIndex + 1
            animationCycle.start()
        }

        function onStarted()
        {
            console.log("animationCycle.onStarted()")
            console.log("renderer.testIndex =", renderer.testIndex)
        }
    }
}
