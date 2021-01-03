import QtQuick 2.12
import QtQuick.Controls 2.5
import MastactvaQuiz 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


Page {
    id: galleryAllImagesPage
    width: Constants.pageWidth
    height: Constants.pageHeight

    title: qsTr("Gallery")

    property var allImagesOfGalleryModel: undefined
    property int galleryId: -1
    property var currentImage: undefined
    property var nextImage: undefined
    property string currentImageSource: Constants.noImage
    property string currentImageHash: Constants.noImageHash
    property int animationDuration: 1500
    property bool hasDescription: false
    property bool hasCrossPage: true
    property string crossPageName: qsTr("Quiz")
    property var crossPage: undefined

    signal setDescription(var descriptionModel, int galleryId, int imageId, string imageSource)

    Rectangle {
        id: quizImageClipper
        anchors.fill: parent
        color: "transparent"
        clip: true
        z: 0.0

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
                    console.log("onClicked")
                    if(!quizImageAnimation.running && currentImage.isImageLoaded())
                    {
                        var x = mouseX / width
                        var y = mouseY / height
                        console.log("x = ", x, "y = ", y);
                        var index = allImagesOfGalleryModel.indexOfItem(currentImage)
                        console.log("index = ", index)
                        if(index >= 0)
                        {
                            if(x > 0.5)
                            {
                                ++index;
                                if(index >= allImagesOfGalleryModel.size())
                                {
                                    index = 0;
                                }
                            }
                            else
                            {
                                --index;
                                if(index < 0)
                                {
                                    index = allImagesOfGalleryModel.size() - 1;
                                }
                            }
                            console.log("index = ", index)
                            var nextImgObj = allImagesOfGalleryModel.itemAt(index)
                            if(nextImgObj !== undefined && nextImgObj !== null)
                            {
                                loadChildren(nextImgObj)
                                allImagesOfGalleryModel.currentIndex = index;

                                var userStepGi = userStepModel.createItem()
                                userStepGi.usGalleryId = galleryModel.getCurrentItem().id
                                userStepGi.usImageId = currentImage.imageId
                                userStepGi.usT = mastactvaAPI.now()
                                userStepModel.addItem(userStepGi)

                                nextImage = nextImgObj
                                quizImage.toImage = [nextImage.imageSource, nextImage.imageHash]
                                quizImage.effect = null
                                quizImage.argumentSet = null
                                quizImageNumberAnimation.easing.type = Easing.Linear
                                quizImageNumberAnimation.duration = animationDuration

                                galleryAllImagesPage.setDescription(undefined, galleryId, nextImage.imageId, nextImage.imageSource)
                                quizImageAnimation.running = true
                            }
                        }
                    }
                }
            }

            SequentialAnimation {
                id: quizImageAnimation
                running: false

                NumberAnimation { id: quizImageNumberAnimation; target: quizImage; property: "t"; from: 0.0; to: 1.0; duration: animationDuration; easing.type: Easing.Linear }
            }

            Connections {
                target: quizImageAnimation

                function onFinished()
                {
                    console.log("quizImageAnimation.onFinished()")
                    currentImage = nextImage
                    currentImageSource = currentImage.imageSource
                    currentImageHash = currentImage.imageHash
                    nextImage = undefined
                    galleryAllImagesPage.setDescription(currentImage.imageDescription, galleryId, currentImage.imageId, currentImage.imageSource)
                    quizImage.effect = null
                    quizImage.argumentSet = null
                    quizImageNumberAnimation.easing.type = Easing.Linear
                    quizImageNumberAnimation.duration = animationDuration
                    quizImage.swapImages();
                }
            }
        }
    }

    BusyIndicator {
        id: busyIndicator
        anchors.centerIn: parent
        visible: false
        running: false
        z: 1.0
    }


    function init()
    {
        quizImage.fromImage = [currentImageSource, currentImageHash]
        quizImage.toImage = [currentImageSource, currentImageHash]
        quizImage.visible = true
        loadChildren(currentImage)
    }

    property var loadingImage: undefined

    function loadChildren(image)
    {
        if(!image.isImageLoaded())
        {
            busyIndicator.visible = true
            busyIndicator.running = true
            image.imageLoaded.connect(currentImageLoaded)
            image.loadChildren()
            loadingImage = image
        }
        else
        {
            loadingImage = undefined
        }
    }

    function currentImageLoaded()
    {
        if(loadingImage !== undefined)
        {
            loadingImage.imageLoaded.disconnect(currentImageLoaded)
            busyIndicator.visible = false
            busyIndicator.running = false
        }
    }
}
