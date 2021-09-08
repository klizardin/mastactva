/*
    Copyright 2021

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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

    property var mastactvaAPI: undefined
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
                    x: Constants.questionPadding
                    width: optionsPage.width - Constants.questionPadding * 2
                }
                Slider {
                    id: animationSpeedSlider
                    x: Constants.questionPadding
                    width: optionsPage.width - Constants.questionPadding * 2
                    from: fromSpeed(Constants.animationSpeedMax)
                    to: fromSpeed(Constants.animationSpeedMin)
                    value: fromSpeed(Constants.animationSpeedNorm)
                }
                Button {
                    text: qsTr("Set default speed value")
                    x: Constants.questionPadding
                    width: optionsPage.width - Constants.questionPadding * 2
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
