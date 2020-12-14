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

    MastactvaAPI {
        id: mastactvaAPI
        objectName: "MastactvaAPI"
    }

    // constant models
    /*ShaderTypeModel {
        id: shaderTypeModel
        objectName: "ShaderTypeModel"
        layoutQMLName: "ShaderTypeModel"
        layoutIdField: "id"
        jsonParamsGet: false
        autoCreateChildrenModels: false
    }

    ShaderArgTypeModel {
        id: shaderArgTypeModel
        objectName: "ShaderArgTypeModel"
        layoutQMLName: "ShaderArgTypeModel"
        layoutIdField: "id"
        jsonParamsGet: false
        autoCreateChildrenModels: false
    }*/

    EasingTypeModel {
        id: easingTypeModel
        objectName: "EasingTypeModel"
        layoutQMLName: "EasingTypeModel"
        layoutIdField: "id"
        jsonParamsGet: false
        autoCreateChildrenModels: false
    }

    // data models
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
        autoCreateChildrenModels: false
    }

    UserStepModel {
        id: userStepModel
        objectName: "UserStepModel"
        layoutQMLName: "UserStepModel"
        layoutIdField: "id"
        jsonParamsGet: false
        autoCreateChildrenModels: false
        outputModel: true
        storeAfterSave: false
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

    function initUserStepModel()
    {
        userStepModel.addLayoutExtraFields("QuizUserModel", undefined)
        userStepModel.addExtraFieldRename("deviceid", "user")
    }

    Connections {
        target: mastactvaAPI

        function onInitialized()
        {
            Constants.noImageHash = mastactvaAPI.calculateHash(Constants.noImageResource)

            galleryPage.galleryModel = galleryModel
            galleryPage.mastactvaAPI = mastactvaAPI
            galleryPage.userStepModel = userStepModel

            quizPage.allImagesOfGalleryModel = allImagesOfGallery
            quizPage.userStepModel = userStepModel

            questionPage.mastactvaAPI = mastactvaAPI
            questionPage.userStepModel = userStepModel


            initGalleryModel()
            initAllImagesOfGalleryModel()
            initUserStepModel()

            quizUserModel.loadList()
            easingTypeModel.loadList()
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
            // log quiz start
            var userStepG = userStepModel.createItem()
            userStepG.usGalleryId = galleryModel.getCurrentItem().id
            userStepG.usImageId = startImage.imageId
            userStepG.usNextImageId = startImage.imageId
            userStepG.usT = mastactvaAPI.now()
            userStepModel.addItem(userStepG)

            // jump to quiz image
            quizPage.galleryId = galleryModel.getCurrentItem().id
            quizPage.currentImage = startImage
            quizPage.currentImageSource = startImage.imageSource
            quizPage.currentImageHash = startImage.imageHash
            quizPage.nextImage = undefined
            quizPage.init()
            setDescription(startImage.imageDescription, galleryModel.getCurrentItem().id, startImage.imageId, startImage.imageSource)
            stackView.push(quizPage)
        }
    }

    QuizForm {
        id: quizPage
    }

    Connections {
        target: quizPage

        function onShowQuestion(question, imageId)
        {
            questionPage.question = question
            questionPage.imageId = imageId
            questionPage.init()
            stackView.push(questionPage)
        }

        function onSetDescription(descriptionModel, galleryId, imageId, imageSource)
        {
            setDescription(descriptionModel, galleryId, imageId, imageSource)
        }
    }

    function setDescription(imageDescription, galleryId, imageId, imageSource)
    {
        quizPage.hasDescription = imageDescription !== undefined && !imageDescription.isEmpty() && imageDescription.getCurrentItem().idDescriptionText.trim() !== ""
        if(quizPage.hasDescription)
        {
            descriptionPage.galleryId = galleryId
            descriptionPage.imageId = imageId
            descriptionPage.descriptionId = imageDescription.getCurrentItem().idId
            descriptionPage.descriptionTextArg = imageDescription.getCurrentItem().idDescriptionText
            descriptionPage.imageSource = imageSource
        }
        else
        {
            descriptionPage.galleryId = -1
            descriptionPage.imageId = -1
            descriptionPage.descriptionId = -1
            descriptionPage.descriptionTextArg = ""
            descriptionPage.imageSource = Constants.noImage
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
            text: stackView.depth > 1 ? "<" : "\u2630"
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
                visible: quizPage.currentImage !== undefined
                onClicked: {
                    stackView.push(quizPage)
                    drawer.close()
                }
            }
            ItemDelegate {
                text: qsTr("Description")
                width: parent.width
                visible: quizPage.hasDescription
                onClicked: {
                    stackView.push(descriptionPage)
                    descriptionPage.init()
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
