import QtQuick 2.12
import QtQuick.Controls 2.5
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


Page {
    id: quizPage
    width: Constants.pageWidth
    height: Constants.pageHeight

    title: qsTr("Quiz")

    property var netAPI: undefined
    property var allImagesOfGalleryModel: undefined
    property var currentImage: undefined
    property alias currentImageSource: quizImage.source
    property bool hasDescription: false

    signal showQuestion(var question)
    signal setDescription(var descriptionModel)

    Image {
        id: quizImage
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit

        MouseArea {
            x: (quizImage.width - quizImage.paintedWidth) / 2
            y: (quizImage.height - quizImage.paintedHeight) / 2
            width: quizImage.paintedWidth
            height: quizImage.paintedHeight

            onClicked: {
                if(currentImage.isImageLoaded())
                {
                    var x = mouseX / width
                    var y = mouseY / height
                    if(currentImage.imagePoints.isNextObjImageByCoords(x, y))
                    {
                        var ip = currentImage.imagePoints.nextObjByCoords(x, y)
                        var nextImgObj = allImagesOfGalleryModel.findItemById(ip.nextImage.getCurrentItem().iptniNextImage)
                        if(nextImgObj !== undefined && nextImgObj !== null)
                        {
                            currentImage = nextImgObj
                            currentImageSource = currentImage.imageSource
                            quizPage.setDescription(currentImage.imageDescription)
                        }
                    }
                    else if(currentImage.imagePoints.isNextObjQuestionByCoords(x, y))
                    {
                        var ip = currentImage.imagePoints.nextObjByCoords(x, y)
                        var question = ip.nextQuestion.getCurrentItem().iptqQuestionObj.getCurrentItem()
                        if(question !== undefined && question !== null)
                        {
                            quizPage.showQuestion(question)
                        }
                    }
                }
            }
        }
    }
}
