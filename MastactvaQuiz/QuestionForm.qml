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

    property var mastactvaAPI: undefined
    property var userStepModel: undefined
    property var question: undefined
    property int imageId: -1
    property int currentAnswerIndex: -1
    property bool hasDescription: false

    signal answered()

    Rectangle
    {
        anchors.fill: parent

        Column {

            Text {
                padding: Constants.questionPadding
                text: qsTr("Question : ")
            }

            Text {
                id: questionText
                //padding: Constants.questionPadding
                x: Constants.questionPadding
                width: questionPage.width - Constants.questionPadding * 2
                wrapMode: Text.WordWrap
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
                    width: Constants.pageWidth - Constants.questionPadding * 2
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
    }

    Component {
        id: answerItem

        Rectangle {
            x: Constants.questionPadding
            width: Constants.pageWidth - Constants.questionPadding * 2
            height: Constants.answersHeight / 3

            SystemPalette {
                id: answerItemPallete
                colorGroup: SystemPalette.Active
            }

            border.width: 2
            border.color: currentAnswerIndex === index ? answerItemPallete.highlight : answerItemPallete.window

            Column {
                Text {
                    x: Constants.questionPadding
                    width: Constants.pageWidth - Constants.questionPadding * 4
                    text : answerText
                    wrapMode: Text.WordWrap
                }
            }
            MouseArea {
                anchors.fill: parent

                onClicked: {
                    answers.model.currentIndex = index
                    currentAnswerIndex = index
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
            // log answer the question
            var userStep = userStepModel.createItem()
            userStep.usGalleryId = galleryModel.getCurrentItem().id
            userStep.usImageId = imageId
            userStep.usQuestionId = question.questionId
            userStep.usT = mastactvaAPI.now()
            userStep.usAnswerId = answers.model.getCurrentItem().answerId
            userStepModel.addItem(userStep)

            var userQuestionAnswerModel = question.userQuestionAnswer
            var newAnswer = userQuestionAnswerModel.createItem()
            newAnswer.qaQuestionId = question.questionId
            newAnswer.qaT = mastactvaAPI.now()
            newAnswer.qaAnswerId = answers.model.getCurrentItem().answerId
            userQuestionAnswerModel.addItem(newAnswer)

            questionPage.answered()
        }
        else
        {
            // TODO: show popup "Please choose answer"
        }
    }
}
