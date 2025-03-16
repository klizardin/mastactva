import QtQuick 2.0
import WallWalkDemoSimple 1.0
import Mastactva 1.0


Rectangle {
    id: root
    width: Constants.width
    height: Constants.height

    QuizImageDemo {
        objectName: "quizImage"
        id: quizImage
        anchors.fill: parent
        anchors.margins: 10
        t: 0.0
        tScalesVector: [0.0, 5.0, 1.0,
            5.0, 0.0,
            5.0, 0.0,
            5.0, 1.0,
            5.0, 1.0,
            5.0, 0.0]
    }

    Connections {
        target: quizImage

        function onAnimationCycleStarted()
        {
            console.log("animationCycle.onStarted()")
            console.log("renderer.testIndex =", quizImage.testIndex)
        }

        function onAnimationCycleFinished()
        {
            console.log("animationCycle.onFinished()")
            if(quizImage.loops !== -1)
            {
                quizImage.testIndex = quizImage.testIndex + 1
            }
            console.log("renderer.testIndex =", quizImage.testIndex)
        }

        function onDoRunTestsStepByStepChanged()
        {
            if(quizImage.doRunTestsStepByStep)
            {
                quizImage.loops = 1
            }
            else
            {
                quizImage.loops = -1
            }
        }
    }
}
