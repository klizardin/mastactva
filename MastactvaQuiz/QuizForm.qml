import QtQuick 2.12
import QtQuick.Controls 2.5
import MastactvaQuiz 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


Page {
    id: quizPage
    width: Constants.pageWidth
    height: Constants.pageHeight

    title: qsTr("Quiz")

    property var userStepModel: undefined
    property var allImagesOfGalleryModel: undefined
    property int galleryId: -1
    property var currentImage: undefined
    property var nextImage: undefined
    property string currentImageSource: Constants.noImage
    property string currentImageHash: Constants.noImageHash
    property bool hasDescription: false
    property int animationDuration: 3000

    signal showQuestion(var question, int imageId)
    signal setDescription(var descriptionModel, int galleryId, int imageId)

    Rectangle {
        id: quizImageClipper
        anchors.fill: parent
        color: "transparent"
        clip: true

        QuizImage {
            id: quizImage
            width: parent.width
            height: parent.height
            fromImage: [Constants.noImage, Constants.noImageHash]
            toImage: [Constants.noImage, Constants.noImageHash]
            clip: true
            t: 0.0
            visible: false

            MouseArea {
                x: (quizImage.width - quizImage.paintedWidth) / 2
                y: (quizImage.height - quizImage.paintedHeight) / 2
                width: quizImage.paintedWidth
                height: quizImage.paintedHeight

                onClicked: {
                    if(!quizImageAnimation.running && currentImage.isImageLoaded())
                    {
                        var x = mouseX / width
                        var y = mouseY / height
                        if(currentImage.imagePoints.isNextObjImageByCoords(x, y))
                        {
                            var ip = currentImage.imagePoints.nextObjByCoords(x, y)
                            var nextImgObj = allImagesOfGalleryModel.findItemById(ip.nextImage.getCurrentItem().iptniNextImage)
                            if(nextImgObj !== undefined && nextImgObj !== null)
                            {
                                // log jump to next image
                                var userStep = userStepModel.createItem()
                                userStep.usGalleryId = galleryModel.getCurrentItem().id
                                userStep.usImageId = currentImage.id
                                userStep.usNextImageId = nextImgObj.id
                                userStep.usT = mastactvaAPI.now()
                                userStepModel.addItem(userStep)

                                nextImage = nextImgObj
                                quizImage.toImage = [nextImage.imageSource, nextImage.imageHash]
                                quizImageAnimation.running = true
                            }
                        }
                        else if(currentImage.imagePoints.isNextObjQuestionByCoords(x, y))
                        {
                            var ip = currentImage.imagePoints.nextObjByCoords(x, y)
                            var question = ip.nextQuestion.getCurrentItem().iptqQuestionObj.getCurrentItem()
                            if(question !== undefined && question !== null)
                            {
                                // log jump to question
                                var userStep = userStepModel.createItem()
                                userStep.usGalleryId = galleryModel.getCurrentItem().id
                                userStep.usImageId = currentImage.id
                                userStep.usQuestionId = question.questionId
                                userStep.usT = mastactvaAPI.now()
                                userStepModel.addItem(userStep)

                                quizPage.showQuestion(question, currentImage.id)
                            }
                        }
                    }
                }
            }

            SequentialAnimation {
                id: quizImageAnimation
                running: false

                NumberAnimation { target: quizImage; property: "t"; from: 0.0; to: 1.0; duration: animationDuration; easing.type: Easing.Linear }
            }

            Connections {
                target: quizImageAnimation

                function onFinished()
                {
                    currentImage = nextImage
                    currentImageSource = currentImage.imageSource
                    currentImageHash = currentImage.imageHash
                    nextImage = undefined
                    quizPage.setDescription(currentImage.imageDescription, galleryId, nextImgObj.id)
                    quizImage.swapImages();
                }
            }
        }
    }

    function init()
    {
        quizImage.fromImage = [currentImageSource, currentImageHash]
        quizImage.toImage = [currentImageSource, currentImageHash]
        quizImage.visible = true
    }
}
