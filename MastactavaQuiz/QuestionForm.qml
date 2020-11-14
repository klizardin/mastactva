import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.15
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


Page {
    id: questionPage
    width: Constants.pageWidth
    height: Constants.pageHeight

    title: qsTr("Question")

    property var netAPI: undefined
    property var question: undefined
    property int currentAnswerIndex: -1
    property bool hasDescription: false

    signal answer(int answerIndex)

    ColumnLayout {

        Text {
            padding: Constants.questionPadding
            text: qsTr("Question : ")
        }

        Text {
            id: questionText
            padding: Constants.questionPadding
            width: questionPage.width
        }

        Row {
            padding: Constants.questionPadding
            Label {
                text: qsTr("Points for question : ")
            }
            Text {
                id: questionPoints
            }
        }

        Rectangle {
            width: Constants.pageWidth
            height: Constants.answersHeight

            ListView {
                id: answers
                anchors.fill: parent
                width: Constants.pageWidth
                model: undefined
                delegate: answerItem
            }
        }

        Button {
            width: Constants.pageWidth
            text: qsTr("Choose")
            Layout.alignment: Qt.AlignVCenter
            onClicked: {
                chooseAnswer()
            }
        }
    }

    Component {
        id: answerItem

        ColumnLayout {
            Rectangle {
                width: Constants.pageWidth
                height: Constants.answersHeight / 3

                SystemPalette {
                    id: answerItemPallete
                    colorGroup: SystemPalette.Active
                }

                border.width: 2
                border.color: currentAnswerIndex === index ? answerItemPallete.highlight : answerItemPallete.window

                Text {
                    padding: Constants.questionPadding
                    width: Constants.pageWidth
                    text : answerText
                }

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        currentAnswerIndex = index
                    }
                }
            }
        }
    }

    function init()
    {
        currentAnswerIndex = -1
        questionText.text = question.questionText
        questionPoints.text = question.questionPointsToPass
        answers.model = question.questionAnswers
    }

    function chooseAnswer()
    {
        if(currentAnswerIndex >= 0)
        {
            questionPage.answer(currentAnswerIndex)
        }
        else
        {
        }
    }
}
