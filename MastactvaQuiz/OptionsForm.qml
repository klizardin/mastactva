import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.15
import MastactvaQuiz 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


Page {
    id: optionsPage
    width: Constants.pageWidth
    height: Constants.pageHeight

    title: qsTr("Options")

    property int animationSpeed: Constants.animationSpeedNorm
    property bool hasDescription: false
    property bool hasCrossPage: false
    property bool hasHelpPage: true
    property string helpPageText: qsTr("Help for the options page. " +
                                       "Here you can set options for the quiz. ")

    signal speedChanged(var speed)

    Rectangle {
        anchors.fill: parent

        ScrollView {
            anchors.fill: parent

            Column {
                Label {
                    text: qsTr("Animation speed")
                }
                Slider {
                    id: animationSpeedSlider
                    from: fromSpeed(Constants.animationSpeedMax)
                    to: fromSpeed(Constants.animationSpeedMin)
                    value: fromSpeed(Constants.animationSpeedNorm)
                }
                Button {
                    text: qsTr("Set default speed value")
                    onClicked: {
                        animationSpeedSlider.value = fromSpeed(Constants.animationSpeedNorm)
                        optionsPage.speedChanged(toSpeed(animationSpeedSlider.value))
                    }
                }
            }
        }
    }

    Connections {
        target: animationSpeedSlider

        function onMoved()
        {
            optionsPage.speedChanged(toSpeed(animationSpeedSlider.value))
        }
    }

    function init()
    {
        animationSpeedSlider.value = fromSpeed(animationSpeed)
    }

    function fromSpeed(speed)
    {
        if(speed >= Constants.animationSpeedNorm)
        {
            return (speed - Constants.animationSpeedNorm)/(Constants.animationSpeedMax - Constants.animationSpeedNorm) * 100.0
        }
        else
        {
            return (speed - Constants.animationSpeedNorm)/(Constants.animationSpeedNorm - Constants.animationSpeedMin) * 100.0
        }
    }

    function toSpeed(value)
    {
        if(value > 0.0)
        {
            return value * (Constants.animationSpeedMax - Constants.animationSpeedNorm) / 100.0 +  Constants.animationSpeedNorm
        }
        else
        {
            return (Constants.animationSpeedNorm - Constants.animationSpeedMin) * value / 100.0 + Constants.animationSpeedNorm
        }
    }
}
