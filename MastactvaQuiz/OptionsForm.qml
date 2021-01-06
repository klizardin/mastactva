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
                    from: Constants.animationSpeedMax / Constants.animationSpeedMax * 100
                    to: Constants.animationSpeedMax / Constants.animationSpeedMin * 100
                    value: Constants.animationSpeedMax / Constants.animationSpeedNorm * 100
                }
                Button {
                    text: qsTr("Set default speed value")
                    onClicked: {
                        animationSpeedSlider.value = Constants.animationSpeedMax / Constants.animationSpeedNorm * 100
                        optionsPage.speedChanged(Constants.animationSpeedMax / animationSpeedSlider.value * 100)
                    }
                }
            }
        }
    }

    Connections {
        target: animationSpeedSlider

        function onMoved()
        {
            optionsPage.speedChanged(Constants.animationSpeedMax / animationSpeedSlider.value * 100)
        }
    }

    function init()
    {
        animationSpeedSlider.value = Constants.animationSpeedMax / animationSpeed * 100
    }
}
