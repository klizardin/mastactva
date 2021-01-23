import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import MastactvaQuiz 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


ApplicationWindow {
    id: window
    width: Constants.width
    height: Constants.height
    visible: false
    opacity: 0.0
    title: qsTr("Mastactva Quiz")

    property real animationSpeed: Constants.animationSpeedNorm
    property var quizCurrentImage: undefined

    MastactvaAPI {
        id: mastactvaAPI
        objectName: "MastactvaAPI"
    }

    function back()
    {
        if (stackView.depth > 1)
        {
            stackView.pop()
            if(stackView.currentItem.hasCrossPage)
            {
                var item = stackView.currentItem
                var next = stackView.currentItem.crossPage
                if(next === quizPage)
                {
                    // return to quiz, so use quiz current image for description
                    setDescription(quizCurrentImage)
                }
            }
            return true
        }
        else
        {
            return false
        }
    }

    onClosing: {
        close.accepted = !back()
    }

    // constant models
    ShaderTypeModel {
        id: shaderTypeModel
        objectName: "ShaderTypeModel"
        layoutQMLName: "ShaderTypeModel"
        jsonParamsGet: false
        autoCreateChildrenModels: false
    }

    ShaderArgTypeModel {
        id: shaderArgTypeModel
        objectName: "ShaderArgTypeModel"
        layoutQMLName: "ShaderArgTypeModel"
        jsonParamsGet: false
        autoCreateChildrenModels: false
    }

    EasingTypeModel {
        id: easingTypeModel
        objectName: "EasingTypeModel"
        layoutQMLName: "EasingTypeModel"
        jsonParamsGet: false
        autoCreateChildrenModels: false
    }

    // data models
    QuizUserModel {
        id: quizUserModel
        objectName: "QuizUserModel"
        layoutQMLName: "QuizUserModel"
        autoCreateChildrenModels: false
    }

    GalleryModel {
        id: galleryModel
        objectName: "GalleryModel"
        layoutQMLName: "GalleryModel"
        currentRef: "deviceid"
        jsonParamsGet: true
        autoCreateChildrenModels: false
    }

    ImageModel {
        id: allImagesOfGallery
        objectName: "AllImagesOfGallery"
        layoutQMLName: "AllImagesOfGallery"
        currentRef: "gallery"
        jsonParamsGet: false
        autoCreateChildrenModels: false
        autoCreateChildrenModelsOnSelect: true
    }

    UserStepModel {
        id: userStepModel
        objectName: "UserStepModel"
        layoutQMLName: "UserStepModel"
        jsonParamsGet: false
        autoCreateChildrenModels: false
        outputModel: true
        storeAfterSave: false
        readonly: false
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
        if(!mastactvaAPI.isLocalDataAPI())
        {
            userStepModel.addLayoutExtraFields("QuizUserModel", undefined)
            userStepModel.addExtraFieldRename("deviceid", "user")
        }
        else
        {
            userStepModel.addModelParam("user", mastactvaAPI.getLocalDataAPIUserId())
        }
    }

    Connections {
        target: mastactvaAPI

        function onInitialized()
        {
            if(mastactvaAPI.isAndroidFullscreen())
            {
                Constants.init()
            }
            window.visible = true
            window.opacity = 1.0
            Constants.noImageHash = mastactvaAPI.calculateHash(Constants.noImageResource)

            galleryPage.galleryModel = galleryModel
            galleryPage.mastactvaAPI = mastactvaAPI
            galleryPage.userStepModel = userStepModel

            quizPage.allImagesOfGalleryModel = allImagesOfGallery
            quizPage.userStepModel = userStepModel
            quizPage.crossPage = galleryAllImagesPage

            galleryAllImagesPage.allImagesOfGalleryModel = allImagesOfGallery
            galleryAllImagesPage.crossPage = quizPage

            descriptionPage.mastactvaAPI = mastactvaAPI
            galleryAllImagesPage.mastactvaAPI = mastactvaAPI
            galleryPage.mastactvaAPI = mastactvaAPI
            helpPage.mastactvaAPI = mastactvaAPI
            optionsPage.mastactvaAPI = mastactvaAPI
            questionPage.mastactvaAPI = mastactvaAPI
            quizPage.mastactvaAPI = mastactvaAPI

            questionPage.userStepModel = userStepModel


            initGalleryModel()
            initAllImagesOfGalleryModel()
            initUserStepModel()

            quizUserModel.loadList()
            easingTypeModel.loadList()
            shaderTypeModel.loadList()
            shaderArgTypeModel.loadList()
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
            allImagesOfGallery.loadList()

            var userStepG = userStepModel.createItem()
            userStepG.usGalleryId = galleryModel.currentItem.id
            userStepG.usImageId = startImage.imageId
            userStepG.usNextImageId = startImage.imageId
            userStepG.usT = mastactvaAPI.now()
            userStepModel.addItem(userStepG)

            //console.log("galleryModel.currentItem.id = ", galleryModel.currentItem.id)
            // jump to quiz image
            quizPage.animationSpeed = animationSpeed
            quizPage.galleryId = galleryModel.currentItem.id
            quizPage.currentImage = startImage
            quizPage.currentImageSource = startImage.imageSource
            quizPage.currentImageHash = startImage.imageHash
            quizPage.nextImage = undefined
            galleryAllImagesPage.animationSpeed = animationSpeed
            galleryAllImagesPage.galleryId = galleryModel.currentItem.id
            galleryAllImagesPage.currentImage = startImage
            galleryAllImagesPage.currentImageSource = startImage.imageSource
            galleryAllImagesPage.currentImageHash = startImage.imageHash
            quizPage.init()
            //setDescription(startImage.imageDescription, galleryModel.currentItem.id, startImage.imageId, startImage.localImageSource)
            quizCurrentImage = startImage
            setDescription(startImage)
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

        function onSetDescription(image)
        {
            setDescription(image)
        }

        function onJumpToImage(image)
        {
            galleryAllImagesPage.currentImage = image
            galleryAllImagesPage.currentImageSource = image.imageSource
            galleryAllImagesPage.currentImageHash = image.imageHash
            quizCurrentImage = image
        }
    }

    //function setDescription(imageDescription, galleryId, imageId, localImageSource)
    function setDescription(image)
    {
        var galleryId = galleryModel.currentItem.id
        var imageDescription = undefined
        var imageId = -1
        var localImageSource = Constants.noImage
        if(image !== null && image !== undefined)
        {
            imageDescription = image.imageDescription
            imageId = image.imageId
            localImageSource = image.localImageSource
        }
        var hasDescription = imageDescription !== undefined && !imageDescription.isEmpty() && imageDescription.getCurrentItem().idDescriptionText.trim() !== ""
        quizPage.hasDescription = hasDescription
        galleryAllImagesPage.hasDescription = hasDescription
        //console.log("galleryAllImagesPage.hasDescription = ", galleryAllImagesPage.hasDescription)
        if(quizPage.hasDescription)
        {
            descriptionPage.galleryId = galleryId
            descriptionPage.imageId = imageId
            descriptionPage.descriptionId = imageDescription.getCurrentItem().idId
            descriptionPage.descriptionTextArg = imageDescription.getCurrentItem().idDescriptionText
            descriptionPage.imageSource = localImageSource
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
            back()
            if(galleryModel.currentItem !== null)
            {
                galleryModel.currentItem.galleryStatistics.loadList()
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
            back()
        }
    }

    GalleryAllImageForm {
        id: galleryAllImagesPage
    }

    Connections {
        target: galleryAllImagesPage

        function onSetDescription(descriptionModel, galleryId, imageId, localImageSource)
        {
            setDescription(descriptionModel, galleryId, imageId, localImageSource)
        }
    }

    OptionsForm {
        id: optionsPage
    }

    Connections {
        target: optionsPage

        function onSpeedChanged(speed)
        {
            animationSpeed = speed
            quizPage.animationSpeed = animationSpeed
            galleryAllImagesPage.animationSpeed = animationSpeed
            optionsPage.animationSpeed = animationSpeed
        }
    }

    HelpForm {
        id: helpPage
    }

    Connections {
        target: helpPage

        function onViewed()
        {
            back()
        }
    }

    SystemPalette {
        id: activePallete
        colorGroup: SystemPalette.Active
    }

    FontLoader { id: emojiFont; name: "Helvetica Neue" }

    header: ToolBar {
        contentHeight: toolButton.implicitHeight

        RowLayout {
            height: toolButton.implicitHeight
            ToolButton {
                id: toolButton
                Layout.alignment: Qt.AlignLeft
                text: stackView.depth > 1 ? "<" : "\u22EE" // TODO: "\u2630"
                font.pixelSize: Qt.application.font.pixelSize * (mastactvaAPI.isAndroidFullscreen() ? 1.0 : 1.6)
                font.family: emojiFont.name
                onClicked: {
                    if (stackView.depth > 1) {
                        back()
                    } else {
                        drawer.open()
                    }
                }
            }

            ToolButton {
                id: currentPage
                text: stackView.currentItem.title
                Layout.minimumWidth: (Constants.width - toolButton.width) / 4 * (mastactvaAPI.isAndroidFullscreen() ?  1.4 : 2.1)
                font.pixelSize: Qt.application.font.pixelSize * (mastactvaAPI.isAndroidFullscreen() ?  0.8 : 1.0)
                background: Rectangle {
                    color: activePallete.highlight
                }
                onClicked: {
                    if(stackView.currentItem.hasHelpPage)
                    {
                        helpPage.helpText = stackView.currentItem.helpPageText
                        helpPage.helpForPage = stackView.currentItem.title
                        helpPage.needTextCaption = true
                        helpPage.init()
                        stackView.push(helpPage)
                    }
                }
            }

            ToolButton {
                id: crossPage
                Layout.minimumWidth: (Constants.width - toolButton.width) / 4 * (mastactvaAPI.isAndroidFullscreen() ?  1.3 : 0.95)
                text: stackView.currentItem.hasCrossPage ? stackView.currentItem.crossPageName : qsTr("")
                font.pixelSize: Qt.application.font.pixelSize * (mastactvaAPI.isAndroidFullscreen() ?  0.8 : 1.0)
                visible: stackView.currentItem.hasCrossPage
                onClicked: {
                    if(stackView.currentItem.hasCrossPage)
                    {
                        var item = stackView.currentItem
                        var next = stackView.currentItem.crossPage
                        if(next === quizPage)
                        {
                            setDescription(quizCurrentImage)
                        }
                        next.init()
                        stackView.replace(item, next)
                    }
                }
            }

            ToolButton {
                id: infoButton
                Layout.minimumWidth: (Constants.width - toolButton.width) / 4 * (mastactvaAPI.isAndroidFullscreen() ?  1.3 : 0.95)
                text: qsTr("Description")
                font.pixelSize: Qt.application.font.pixelSize * (mastactvaAPI.isAndroidFullscreen() ?  0.8 : 1.0)
                visible: stackView.currentItem.hasDescription
                onClicked: {
                    stackView.push(descriptionPage)
                    descriptionPage.init()
                }
            }
        }
    }

    Drawer {
        id: drawer
        width: window.width * 0.61
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

            ItemDelegate {
                text: qsTr("Options")
                width: parent.width
                visible: stackView.currentItem !== optionsPage
                onClicked: {
                    optionsPage.animationSpeed = animationSpeed
                    optionsPage.init()
                    stackView.push(optionsPage)
                    drawer.close()
                }
            }

            ItemDelegate {
                text: qsTr("Help")
                width: parent.width
                visible: stackView.currentItem !== helpPage
                onClicked: {
                    helpPage.helpText = helpPage.defaultHelpText
                    helpPage.helpForPage = window.title
                    helpPage.needTextCaption = false
                    helpPage.init()
                    stackView.push(helpPage)
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
