import QtQuick 2.12
import QtQuick.Controls 2.5
import MastactvaQuiz 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


ApplicationWindow {
    id: window
    width: Constants.width
    height: Constants.height
    visible: true
    title: qsTr("Mastactva Quiz")

    NetAPI {
        id: netAPI
        objectName: "NetAPI"
    }

    MastactvaAPI {
        id: mastactvaAPI
        objectName: "MastactvaAPI"
    }

    QuizUserModel {
        id: quizUserModel
        objectName: "QuizUserModel"
        layoutQMLName: "QuizUserModel"
        layoutIdField: "deviceid"
        autoCreateChildrenModels: false
    }

    GalleryModel {
        id: galleryModel
        objectName: "GalleryModel"
        layoutQMLName: "GalleryModel"
        layoutIdField: "id"
        currentRef: "deviceid"
        jsonParamsGet: true
        autoCreateChildrenModels: false
    }

    ImageModel {
        id: allImagesOfGallery
        objectName: "AllImagesOfGallery"
        layoutQMLName: "AllImagesOfGallery"
        layoutIdField: "id"
        currentRef: "gallery"
        jsonParamsGet: false
        autoCreateChildrenModels: true
    }

    function initGalleryModel()
    {
        galleryModel.setLayoutRef("deviceid", "QuizUserModel", "deviceid")
    }

    function initAllImagesOfGalleryModel()
    {
        allImagesOfGallery.setLayoutRef("gallery", "GalleryModel", "id")
        allImagesOfGallery.addModelParam("use_in_gallery_view", "0")
    }

    Connections {
        target: netAPI

        function onInitialized()
        {
            galleryPage.netAPI = netAPI
            quizPage.netAPI = netAPI
            quizPage.allImagesOfGalleryModel = allImagesOfGallery
            descriptionPage.netAPI = netAPI
            questionPage.netAPI = netAPI
            questionPage.mastactavaAPI = mastactvaAPI
            galleryPage.galleryModel = galleryModel
            galleryPage.mastactvaAPI = mastactvaAPI

            initGalleryModel()
            initAllImagesOfGalleryModel()
            quizUserModel.loadList()
        }
    }

    GalleryForm {
        id: galleryPage
        galleryModel: galleryModel
    }

    Connections {
        target: galleryPage

        function onStartQuiz(startImage)
        {
            quizPage.currentImage = startImage
            quizPage.currentImageSource = startImage.imageSource
            setDescription(startImage.imageDescription)
            stackView.push(quizPage)
        }
    }

    QuizForm {
        id: quizPage
    }

    Connections {
        target: quizPage

        function onShowQuestion(question)
        {
            questionPage.question = question
            questionPage.init()
            stackView.push(questionPage)
        }

        function onSetDescription(descriptionModel)
        {
            setDescription(descriptionModel)
        }
    }

    function setDescription(imageDescription)
    {
        //console.log("imageDescription = ", imageDescription)
        //console.log("imageDescription.isEmpty() = ", imageDescription.isEmpty())
        //console.log("imageDescription.getCurrentItem() = ", imageDescription.getCurrentItem())
        //console.log("imageDescription.getCurrentItem().idDescriptionText = ", imageDescription.getCurrentItem().idDescriptionText)
        quizPage.hasDescription = !imageDescription.isEmpty() && imageDescription.getCurrentItem().idDescriptionText.trim() !== ""
        if(quizPage.hasDescription)
        {
            descriptionPage.descriptionTextArg = imageDescription.getCurrentItem().idDescriptionText
        }
        else
        {
            descriptionPage.descriptionTextArg = ""
        }
    }

    QuestionForm {
        id: questionPage
    }

    Connections {
        target: questionPage

        function onAnswered()
        {
            if (stackView.depth > 1) {
                stackView.pop()
            }
        }
    }

    DescriptionForm {
        id: descriptionPage
    }

    Connections {
        target: descriptionPage

        function onCloseDescriptionPage()
        {
            if (stackView.depth > 1) {
                stackView.pop()
            }
        }
    }


    header: ToolBar {
        contentHeight: toolButton.implicitHeight

        ToolButton {
            id: toolButton
            text: stackView.depth > 1 ? "\u25C0" : "\u2630"
            font.pixelSize: Qt.application.font.pixelSize * 1.6
            onClicked: {
                if (stackView.depth > 1) {
                    stackView.pop()
                } else {
                    drawer.open()
                }
            }
        }

        Label {
            text: stackView.currentItem.title
            anchors.centerIn: parent
        }

        ToolButton {
            id: infoButton
            anchors.right: parent.right
            text: qsTr("Description")
            visible: stackView.currentItem.hasDescription
            onClicked: {
                stackView.push(descriptionPage)
                descriptionPage.init()
            }
        }
    }

    Drawer {
        id: drawer
        width: window.width * 0.66
        height: window.height

        Column {
            anchors.fill: parent

            ItemDelegate {
                text: qsTr("Quiz")
                width: parent.width
                onClicked: {
                    stackView.push(quizPage)
                    drawer.close()
                }
            }
            ItemDelegate {
                text: qsTr("Description")
                width: parent.width
                onClicked: {
                    stackView.push(descriptionPage)
                    drawer.close()
                }
            }
        }
    }

    StackView {
        id: stackView
        initialItem: galleryPage
        anchors.fill: parent
    }
}
