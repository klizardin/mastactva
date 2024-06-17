import QtQuick 2.0
import QuizImageDemoArtefactStepsFB 1.0
import Mastactva 1.0


Rectangle {
    id: root0
    width: Constants.width
    height: Constants.height

    QuizImageDemo {
        objectName: "quizImage0"
        id: quizImage0
        anchors.fill: parent
        anchors.margins: 10
        t: 0.0
        tScalesVector: [0.0, 5000.0, 0.0, 2000.0, 0.0, 5000.0, 1.0, 2000.0, 1.0, 5000.0, 0.0]
    }

    /*SequentialAnimation {
        objectName: "animationCycle0"
        id: animationCycle0
        NumberAnimation { target: quizImage0; property: "t"; to: 1.0; duration: 5000; easing.type: Easing.Linear }
        NumberAnimation { target: quizImage0; property: "t"; to: 0.0; duration: 5000; easing.type: Easing.Linear }
        PauseAnimation { duration: 2000 }
        NumberAnimation { target: quizImage0; property: "t"; to: 1.0; duration: 5000; easing.type: Easing.Linear }
        PauseAnimation { duration: 2000 }
        NumberAnimation { target: quizImage0; property: "t"; to: 0.0; duration: 5000; easing.type: Easing.Linear }
        PauseAnimation { duration: 2000 }
        running: true
        //loops: Animation.Infinite
    }

    Connections {
        target: animationCycle0

        function onFinished()
        {
            console.log("animationCycle.onFinished()")
            if(animationCycle0.loops !== -1)
            {
                quizImage0.testIndex = quizImage0.testIndex + 1
            }
            animationCycle0.start()
        }

        function onStarted()
        {
            console.log("animationCycle.onStarted()")
            console.log("renderer.testIndex =", quizImage0.testIndex)
        }
    }

    Connections {
        target: quizImage0

        function onDoRunTestsStepByStepChanged()
        {
            if(quizImage0.doRunTestsStepByStep)
            {
                animationCycle0.loops = 1
            }
            else
            {
                animationCycle0.loops = Animation.Infinite
            }
        }
    }*/
}
