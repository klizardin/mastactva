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
        tScalesVector: [0.0, 5.0, 1.0, 5.0, 0.0, 2.0, 0.0, 5.0, 1.0, 2.0, 1.0, 5.0, 0.0]
    }

    Connections {
        target: quizImage0

        function onAnimationCycleStarted()
        {
            console.log("animationCycle.onStarted()")
            console.log("renderer.testIndex =", quizImage0.testIndex)
        }

        function onAnimationCycleFinished()
        {
            console.log("animationCycle.onFinished()")
            if(quizImage0.loops !== -1)
            {
                quizImage0.testIndex = quizImage0.testIndex + 1
            }
        }

        function onDoRunTestsStepByStepChanged()
        {
            if(quizImage0.doRunTestsStepByStep)
            {
                quizImage0.loops = 1
            }
            else
            {
                quizImage0.loops = -1
            }
        }
    }
}
