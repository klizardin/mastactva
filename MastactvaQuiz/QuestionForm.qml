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
    property bool hasCrossPage: false

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
                    question.questionAnswers.currentIndex = index
                    answers.currentIndex = index
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
        question.questionAnswers.randOrder()
        answers.model = question.questionAnswers
        question.userQuestionAnswer.listReloaded.connect(userQuestionAnswerListReloaded)
        question.userQuestionAnswer.loadList()
    }

    function userQuestionAnswerListReloaded()
    {
        question.userQuestionAnswer.listReloaded.disconnect(userQuestionAnswerListReloaded)
        //for(var i1 = 0; i1 < question.userQuestionAnswer.size(); i1++)
        //{
        //    var item1 = question.userQuestionAnswer.itemAt(i1)
        //    console.log("at ", i1 , " qaId =", item1.qaId, "answerId = ", item1.qaAnswerId, "qaT = ", mastactvaAPI.dateTimeToISOStr(item1.qaT))
        //}
        question.userQuestionAnswer.sortByFields(["-t"])
        //for(var i2 = 0; i2 < question.userQuestionAnswer.size(); i2++)
        //{
        //    var item2 = question.userQuestionAnswer.itemAt(i2)
        //    console.log("at ", i2 , " qaId =", item2.qaId, "answerId = ", item2.qaAnswerId, "qaT = ", mastactvaAPI.dateTimeToISOStr(item2.qaT))
        //}

        if(question.userQuestionAnswer.size() > 0)
        {
            //for(var i3 = 0; i3 < question.questionAnswers.size(); i3++)
            //{
            //    var qa = question.questionAnswers.itemAt(i3);
            //    console.log("at ", i3, "answerId =", qa.answerId, "answerText =", qa.answerText)
            //}

            var userQuestionAnswer = question.userQuestionAnswer.itemAt(0)
            if(userQuestionAnswer !== undefined && userQuestionAnswer !== null)
            {
                //console.log("userQuestionAnswer.qaAnswerId =", userQuestionAnswer.qaAnswerId)
                question.questionAnswers.selectItemById(userQuestionAnswer.qaAnswerId)
                currentAnswerIndex = question.questionAnswers.currentIndex
                answers.currentIndex = currentAnswerIndex
                //console.log("userQuestionAnswerListReloaded() question.questionAnswers.currentItem.answerId =",question.questionAnswers.currentItem.answerId)
            }
        }
    }

    function chooseAnswer()
    {
        if(currentAnswerIndex >= 0 && question.questionAnswers.currentItem !== null)
        {
            // log answer the question
            //console.log("chooseAnswer() question.questionAnswers.currentItem.answerId =",question.questionAnswers.currentItem.answerId)
            var userStepQ = userStepModel.createItem()
            userStepQ.usGalleryId = galleryModel.currentItem.id
            userStepQ.usImageId = imageId
            userStepQ.usQuestionId = question.questionId
            userStepQ.usT = mastactvaAPI.now()
            userStepQ.usAnswerId = question.questionAnswers.currentItem.answerId
            userStepModel.addItem(userStepQ)

            var userQuestionAnswerModel = question.userQuestionAnswer
            var newAnswer = userQuestionAnswerModel.createItem()
            newAnswer.qaQuestionId = question.questionId
            newAnswer.qaT = mastactvaAPI.now()
            newAnswer.qaAnswerId = question.questionAnswers.currentItem.answerId
            userQuestionAnswerModel.addItem(newAnswer)

            questionPage.answered()
        }
        else
        {
            popupText.open()
        }
    }

    Popup {
        id: popupText
        anchors.centerIn: parent
        padding: 10
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        contentItem: Text {
            text: qsTr("Please choose answer")
        }
    }
}
