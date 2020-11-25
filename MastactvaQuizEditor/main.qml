import QtQuick 2.12
import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15
import MastactvaQuizEditor 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


ApplicationWindow {
    id: root
    visible: true
    width: Constants.width
    height: Constants.height
    title: qsTr("Mastactva Quiz Editor")

    property alias appMode: topicTabBar.currentIndex
    property alias galleryCurrentIndex: mastactva.galleryIndex
    property alias galleryCurrentId: mastactva.galleryId
    property alias galleryImagesCurrentIndex: mastactva.imageOfGalleryIndex
    property alias galleryImagesCurrentId: mastactva.imageOfGalleryId
    property alias imageOfGalleryDescriptionIndex: mastactva.imageOfGalleryDescriptionIndex
    property alias imageOfGalleryDescriptionId: mastactva.imageOfGalleryDescriptionId
    property alias imageOfGalleryPointIndex: mastactva.imageOfGalleryPointIndex
    property alias imageOfGalleryAnswerIndex: mastactva.imageOfGalleryAnswerIndex
    property bool showImagePoints: false
    property bool showImagePointsVoronoyDiagram: false
    property var currentImagePointsModel: undefined

    property int effectCurrentIndex: -1

    Connections {
        target: root
        function onGalleryCurrentIndexChanged()
        {
            galleries.currentIndex = galleryCurrentIndex
            galleries.model.currentIndex = galleryCurrentIndex
            images_of_gallery.model.galleryIndex = galleryCurrentIndex
            editCurrentGalleryDialog.setFields(galleries.model.getCurrentItem())
            addImageDialog.currentGalleryId = galleries.model.currentId
            galleryImagesCurrentIndex = -1
        }

        function onGalleryImagesCurrentIndexChanged()
        {
            images_of_gallery.currentIndex = galleryImagesCurrentIndex >= 0 ? galleryImagesCurrentIndex : 0
            images_of_gallery.model.currentIndex = galleryImagesCurrentIndex >= 0 ? galleryImagesCurrentIndex : 0
            currentImagePointsModel = images_of_gallery.model.currentImagePoints()
            imageOfGalleryPointIndex = -1
            imageOfGalleryDescriptionIndex = -1
            imageOfGalleryAnswerIndex = -1
        }

        function onGalleryImagesCurrentIdChanged()
        {
            imageOfGalleryDescriptionListView.model.imageID = galleryImagesCurrentId
            if(currentImagePointsModel !== undefined)
            {
                currentImagePointsModel.setSourceImageId(galleryImagesCurrentId)
                currentImagePointsModel.startLoadImagePoints()
            }
        }

        function onImageOfGalleryPointIndexChanged()
        {
            if(currentImagePointsModel !== undefined)
            {
                currentImagePointsModel.currentIndex = imageOfGalleryPointIndex
            }
            if(imageOfGalleryPointIndex == -1)
            {
                clearImageSource()
                clearQuestionInfo()
            }
            else
            {
                if(currentImagePointsModel === undefined)
                {
                    return;
                }
                var imgPoint = currentImagePointsModel.currentItem
                var nextImage = imgPoint.toNextImage
                var question = imgPoint.toQuestion
                if(nextImage.loaded())
                {
                    imageOfGalleryNextImageText.text = nextImage.noImageSource
                            ? Constants.notANextImagePoint
                            : Constants.aNextImagePoint
                    imageOfGalleryNextImageNextImage.source = nextImage.imageSource
                }
                else
                {
                    nextImage.noImageSourceChanged.connect(noImageSourceChanged)
                    clearImageSource()
                }
                if(question.questionIdLoaded())
                {
                    imageOfGalleryQuestionAnswersListView.model.questionId = question.questionId
                    if(question.questionDataLoaded())
                    {
                        setQuestionInfo(question)
                    }
                    else
                    {
                        clearQuestionInfo()
                    }
                }
                else
                {
                    question.questionIdChanged.connect(questionIdLoaded)
                    clearQuestionInfo()
                }
            }
        }

        function noImageSourceChanged()
        {
            var imgPoint = currentImagePointsModel.currentItem
            var nextImage = imgPoint.toNextImage
            nextImage.noImageSourceChanged.disconnect(noImageSourceChanged)
            if(nextImage.loaded())
            {
                imageOfGalleryNextImageText.text = nextImage.noImageSource
                        ? Constants.notANextImagePoint
                        : Constants.aNextImagePoint
                imageOfGalleryNextImageNextImage.source = nextImage.imageSource
            }
            else
            {
                clearImageSource()
            }
        }

        function clearImageSource()
        {
            imageOfGalleryNextImageNextImage.source = Constants.noImage
            imageOfGalleryNextImageText.text = Constants.selectImagePoint
        }

        function questionIdLoaded()
        {
            var imgPoint = currentImagePointsModel.currentItem
            var question = imgPoint.toQuestion
            question.questionIdChanged.disconnect(questionIdLoaded)
            imageOfGalleryQuestionAnswersListView.model.questionId = question.questionId
            if(question.questionId >= 0)
            {
                if(question.questionDataLoaded())
                {
                    setQuestionInfo(question)
                }
                else
                {
                    clearQuestionInfo()
                    question.questionObjChanged.connect(questionObjLoaded)
                }
            }
            else
            {
                clearQuestionInfo()
            }
        }

        function questionObjLoaded()
        {
            var imgPoint = currentImagePointsModel.currentItem
            var question = imgPoint.toQuestion
            question.questionObjChanged.disconnect(questionObjLoaded)
            setQuestionInfo(question)
        }

        function setQuestionInfo(question)
        {
            imageOfGalleryQuestionText.text = question.question
            imageOfGalleryQuestionPointsToPassRect.visible = true
            imageOfGalleryQuestionPointsToPass.text = question.pointsToPass
            imageOfGalleryAnswerIndex = 0
        }

        function clearQuestionInfo()
        {
            imageOfGalleryQuestionText.text = Constants.selectImagePoint
            imageOfGalleryQuestionPointsToPass.text = ""
            imageOfGalleryAnswerIndex = 0
            imageOfGalleryQuestionPointsToPassRect.visible = false
        }

        function onImageOfGalleryAnswerIndexChanged()
        {
            imageOfGalleryQuestionAnswersListView.currentIndex = imageOfGalleryAnswerIndex
        }

        function onEffectCurrentIndexChanged()
        {
            effectsList.currentIndex = effectCurrentIndex
        }
    }

    MastactvaAPI {
        id: mastactva
        objectName: "MastactvaAPI"
    }

    EffectModel {
        id: effectModel
        objectName: "EffectModel"
        layoutQMLName: "EffectModel"
        layoutIdField: "id"
        jsonParamsGet: false
        autoCreateChildrenModels: false
    }

    Connections {
        target: mastactva

        function onErrorMessage(header, description) {
            if(header !== "")
            {
                popupMessage.fieldPopupMessageShortText = header
            }
            else
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Application Error");
            }
            popupMessage.fieldPopupMessageDescriptionText = description
            popupMessage.open()
        }

        function onInitialized()
        {
            console.log("MastactvaAPI::onInitialized() at main.qml")
        }
    }

    ConfirmDialog {
        id: confirmDialog

        signal confirm
        signal skip

        onAccepted: {
            confirm()
        }
        onRejected: {
            skip()
        }
    }

    GalleryEditDialog {
        id: createNewGalleryDialog

        onOpened: {
            fieldId = -1
            fieldDescription = ""
            fieldKeywords = ""
            fieldCreated = "now()"
            fieldPointsToPass = 1.0
        }

        onAccepted: {
            mastactva.onNewGalleryCreated.connect(onNewGalleryCreated)
            mastactva.createNewGallery(fieldDescription, fieldKeywords, fieldPointsToPass)
        }

        function onNewGalleryCreated()
        {
            mastactva.onNewGalleryCreated.disconnect(onNewGalleryCreated)
            mastactva.galleryReloaded.connect(onGalleryReloaded)
            mastactva.reloadGalleriesModel()
        }

        function onGalleryReloaded()
        {
            mastactva.galleryReloaded.disconnect(onGalleryReloaded)
            galleries.currentIndex = galleryCurrentIndex;
        }

        onRejected: {
        }
    }

    GalleryEditDialog {
        id: editCurrentGalleryDialog

        property int oldId: -1
        property string oldDescription: ""
        property string oldKeywords: ""
        property string oldCreated: ""
        property string oldPointsToPass: "1.0"

        onOpened: {
            oldId = fieldId
            oldDescription = fieldDescription
            oldKeywords = fieldKeywords
            oldCreated = fieldCreated
            oldPointsToPass = fieldPointsToPass
        }

        onAccepted: {
            mastactva.onGalleryEdited.connect(onNewGalleryCreated)
            mastactva.editGallery(fieldId, fieldDescription, fieldKeywords, fieldCreated, fieldPointsToPass)
        }

        function onNewGalleryCreated()
        {
            mastactva.onGalleryEdited.disconnect(onNewGalleryCreated)
            mastactva.galleryReloaded.connect(onGalleryReloaded)
            mastactva.reloadGalleriesModel()
        }

        function onGalleryReloaded()
        {
            mastactva.galleryReloaded.disconnect(onGalleryReloaded)
            galleries.currentIndex = galleryCurrentIndex;
        }

        function setFields(item)
        {
            fieldId = item.id
            fieldDescription = item.description
            fieldKeywords = item.keywords
            fieldCreated = item.created
            fieldPointsToPass = item.pointsToPass
        }

        onRejected: {
            fieldId = oldId
            fieldDescription = oldDescription
            fieldKeywords = oldKeywords
            fieldCreated = oldCreated
            fieldPointsToPass = oldPointsToPass
        }
    }

    ImageEditDialog {
        id: addImageDialog

        property int currentGalleryId: -1

        onOpened: {
            if(mastactva.isCurrentGalleryEmpty())
            {
                fieldTopImage = true
                fieldCanChooseTopImage = false
            }
            else
            {
                fieldTopImage = false
                fieldCanChooseTopImage = true
            }
            fieldImageFileName = ""
        }

        onAccepted: {
            mastactva.onImageAdded.connect(onImageAdded)
            mastactva.addImage(currentGalleryId, fieldImageFileName, fieldTopImage)
        }

        function onImageAdded()
        {
            mastactva.onImageAdded.disconnect(onImageAdded)
            mastactva.galleryReloaded.connect(onGalleryReloaded)
            mastactva.imagesOfGalleryReloaded.connect(onGalleryImagesReloaded)
            mastactva.reloadGalleriesModel()
            mastactva.reloadAllImagesOfGalleryViewModel()
        }

        function onGalleryReloaded()
        {
            mastactva.galleryReloaded.disconnect(onGalleryReloaded)
            galleries.currentIndex = galleryCurrentIndex;
        }

        function onGalleryImagesReloaded()
        {
            mastactva.imagesOfGalleryReloaded.disconnect(onGalleryImagesReloaded)
            galleries.images_of_gallery = galleryImagesCurrentIndex;
        }
    }

    DescriptionEditDialog {
        id: createNewDescriptionDialog

        property int imageID: -1

        onOpened: {
            imageID = galleryImagesCurrentId
            fieldDescription = ""
            fieldFrom = mastactva.dateTimeToUserStr(mastactva.nowJsonStr())
        }

        onAccepted: {
            mastactva.newDescription(imageID, fieldDescription, mastactva.dateTimeFromUserStr(fieldFrom))
            mastactva.onNewDescriptionAdded.connect(onNewDescriptionAdded)
        }

        function onNewDescriptionAdded()
        {
            mastactva.refreshDescriptions();
        }
    }

    DescriptionEditDialog {
        id: editDescriptionDialog

        property int oldDescriptionIndex: -1

        onOpened: {
            var descr = mastactva.getCurrentDescription()
            if(descr !== undefined) {
                fieldId = descr.id
                fieldImageId = descr.imageId
                fieldDescription = descr.descriptionStr
                fieldFrom = mastactva.dateTimeToUserStr(descr.fromDateTime)
            } else {
                fieldId = -1
                fieldImageId = -1
                fieldDescription = ""
                fieldFrom = mastactva.nowJsonStr()
            }
            oldDescriptionIndex = imageOfGalleryDescriptionIndex
        }

        onAccepted: {
            mastactva.editDescription(fieldId, fieldImageId, fieldDescription, mastactva.dateTimeFromUserStr(fieldFrom))
            mastactva.onDescriptionEdited.connect(onDescriptionEdited)
        }

        function onDescriptionEdited()
        {
            mastactva.refreshDescriptions();
            mastactva.onRefreshDescriptions.connect(onRefreshDescriptions)
        }

        function onRefreshDescriptions()
        {
            imageOfGalleryDescriptionIndex = oldDescriptionIndex
        }
    }

    QuestionEditDialog {
        id: editQuestionDialog

        property var fieldCreated: mastactva.nowJsonStr()

        onOpened: {
            var question = mastactva.getCurrentQuestion()
            if(question !== undefined)
            {
                fieldId = question.id
                fieldQuestion = question.questionText
                fieldPointsToPass = question.pointsToPass
                fieldCreated = question.created
            }
            else
            {
                fieldId = -1
                fieldQuestion = ""
                fieldPointsToPass = "1.0"
                fieldCreated = mastactva.nowJsonStr()
            }
        }

        onAccepted: {
            mastactva.editQuestion(fieldId, fieldQuestion, fieldPointsToPass, fieldCreated);
            mastactva.onQuestionEdited.connect(onQuestionEdited);
        }

        function onQuestionEdited()
        {
            mastactva.refreshCurrentImagePointToQuestion();
        }
    }

    AnswerEditDialog {
        id: newAnswerDialog

        property int questionId: -1

        onOpened: {
            fieldId = -1
            var question = mastactva.getCurrentQuestion()
            if(question !== undefined)
            {
                questionId = question.id
                fieldQuestion = question.questionText
                fieldQuestionPointsToPass = question.pointsToPass
                fieldPointsToPass = question.pointsToPass
            }
            else
            {
                questionId = -1
                fieldQuestion = ""
                fieldQuestionPointsToPass = "1.0"
                fieldPointsToPass = "1.0"
            }
            fieldAnswer = ""
        }

        onAccepted: {
            mastactva.addNewAnswer(questionId, fieldAnswer, fieldPointsToPass)
            mastactva.onNewAnswerAdded.connect(onNewAnswerAdded)
        }

        function onNewAnswerAdded()
        {
            mastactva.refreshCurrentImagePointToQuestion();
        }
    }

    AnswerEditDialog {
        id: editAnswerDialog

        property int questionId: -1

        onOpened: {
            var question = mastactva.getCurrentQuestion()
            var answer = mastactva.getCurrentAnswer()
            if(question !== undefined)
            {
                questionId = question.id
                fieldQuestion = question.questionText
                fieldQuestionPointsToPass = question.pointsToPass
            }
            else
            {
                questionId = -1
                fieldQuestion = ""
                fieldQuestionPointsToPass = "1.0"
            }
            if(answer !== undefined)
            {
                fieldId = answer.id
                fieldAnswer = answer.answer
                fieldPointsToPass = answer.pointsToPass
            }
            else
            {
                fieldId = -1
                fieldAnswer = ""
                if(question !== undefined)
                {
                    fieldPointsToPass = question.pointsToPass
                }
                else
                {
                    fieldPointsToPass = "1.0"
                }
            }
        }

        onAccepted: {
            mastactva.editAnswer(fieldId, questionId, fieldAnswer, fieldPointsToPass)
            mastactva.onAnswerEdited.connect(onAnswerEdited)
        }

        function onAnswerEdited()
        {
            mastactva.refreshCurrentImagePointToQuestion();
        }
    }

    ImagePointDialog {
        id: imagePointDialog

        property bool addNewPoint: true
        property int oldPointIndex: -1
        property real oldX: 0.5
        property real oldY: 0.5
        property real oldWeight: 0.5

        onOpened: {
            if(pointIndex >= 0)
            {
                oldPointIndex = imageOfGalleryPointIndex
                var pt = pointsModel.itemAt(pointIndex)
                if(pt === undefined)
                {
                    pointIndex = -1
                }
                else
                {
                    addNewPoint = false
                    oldX = point_x = pt.xCoord
                    oldY = point_y = pt.yCoord
                    oldWeight = point_weight = pt.weight
                }
            }
            if(pointIndex < 0)
            {
                addNewPoint = true

                oldX = point_x = 0.5
                oldY = point_y = 0.5
                oldWeight = point_weight = 1.0
                pointIndex = pointsModel.addTempPoint(point_x, point_y, point_weight)
            }
        }

        onAccepted: {
            pointsModel.onDataSaved.connect(onDataSaved)
            if(addNewPoint)
            {
                oldPointIndex = pointIndex
                pointsModel.savePointTemplate(pointIndex, pointToNextImage)
            }
            else
            {
                pointsModel.savePoint(pointIndex)
            }
        }

        onRejected: {
            pointsModel.onDataSaved.connect(onDataSaved)
            if(addNewPoint)
            {
                oldPointIndex = -1
                pointsModel.removeTempPoint(pointIndex)
            }
            else
            {
                pointsModel.resetValuesAtIndex(pointIndex, oldX, oldY, oldWeight)
            }
        }

        function onDataSaved()
        {
            pointsModel.onDataSaved.disconnect(onDataSaved)
            pointsModel = undefined
            imageOfGalleryPointIndex = oldPointIndex
        }
    }

    OptionsEditDialog {
        id: optionsEditDialog

        onOpened: {
            fieldRegenerateColors = false;
            fieldUseColorsVoronoyDiagram = mastactva.useColorsVoronoyDiagram()
            fieldLanguageIndex = mastactva.getLanguageIndex()
        }

        onAccepted: {
            if(fieldRegenerateColors)
            {
                mastactva.regenerateVoronoyDiagramColors()
            }
            mastactva.switchLanguage(fieldLanguage)
            mastactva.setUseColorsVoronoyDiagram(fieldUseColorsVoronoyDiagram)
            mastactva.saveAppConsts()
        }
    }

    NextImageEditDialog {
        id: nextImageEditDialog

        onOpened: {
            fieldGalleryId = galleryCurrentId
            var img = images_of_gallery.model.currentItem()
            if(img === undefined)
            {
                fieldImageSource = Constants.noImage
                fieldImageId = -1
            }
            else
            {
                fieldImageSource = img.source
                fieldImageId = img.id
            }
            fieldPointsModel = images_of_gallery.model.currentImagePoints()
            fieldPointIndex = imageOfGalleryPointIndex >= 0 ? imageOfGalleryPointIndex : fieldPointsModel.getSize() - 1
            fieldPointsModel.currentIndex = fieldPointIndex

            var nextImg = fieldPointsModel.currentItem.toNextImage

            fieldNextImageCurrentIndex = -1
            fieldNextImageId = nextImg === undefined ? -1 : nextImg.id
            fieldNextImageSource = nextImg === undefined ? Constants.noImage : nextImg.imageSource
        }

        onAccepted: {
            mastactva.editNextImageOfCurrentImagePoint(fieldNextImageId)
            mastactva.nextImageEdited.connect(nextImageEdited)
        }

        function nextImageEdited()
        {
            mastactva.nextImageEdited.disconnect(nextImageEdited)
            mastactva.refreshNextImage()
        }
    }

    EffectEditDialog {
        id: effectEditDialog

        onOpened: {
            init()
        }

        onAccepted: {
            update()
            if(fieldNewItem)
            {
                effectModel.addItem(fieldEffect)
            }
            else
            {
                effectModel.setItem(effectCurrentIndex, fieldEffect)
            }
        }
    }

    WarningDialog {
        id: popupMessage
    }

    Action {
        id: refreshGalleriesModel
        text: qsTr("&Refresh Galleries")
        onTriggered: mastactva.reloadGalleriesModel()
    }

    Action {
        id: refreshAllGalleryImagesModel
        text: qsTr("Refresh &All Images")
        onTriggered: mastactva.reloadAllImagesOfGalleryViewModel()
    }

    Action {
        id: createNewGallery
        text: qsTr("&Create New Gallery")
        onTriggered: {
            createNewGalleryDialog.open()
        }
    }

    Action {
        id: editCurrentGallery
        text: qsTr("&Edit Current Gallery")
        onTriggered: {
            if(galleryCurrentIndex < 0 || galleryCurrentId < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select gallery first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To edit the gallery you should first select a gallery")
                popupMessage.open()
            }
            else
            {
                editCurrentGalleryDialog.open()
            }
        }
    }

    Action {
        id: removeCurrentGallery
        text: qsTr("&Remove Current Gallery")
        onTriggered: {
            if(galleryCurrentIndex < 0 || galleryCurrentId < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select gallery first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To edit the gallery you should first select a gallery")
                popupMessage.open()
            }
            else
            {
                connectConfirmDialog()
                confirmDialog.confirmText = qsTr("Do you really want to remove the current gallery")
                confirmDialog.imageSource = galleries.model.getCurrentItem().getImages().currentImageSource()
                confirmDialog.showImage = true
                confirmDialog.open()
            }
        }

        function connectConfirmDialog()
        {
            confirmDialog.onSkip.connect(onCancelConfirm)
            confirmDialog.onConfirm.connect(onConfirmed)
        }

        function disconnectConfirmDialog()
        {
            confirmDialog.onSkip.disconnect(onCancelConfirm)
            confirmDialog.onConfirm.disconnect(onConfirmed)
        }

        function onCancelConfirm()
        {
            disconnectConfirmDialog()
        }

        function onConfirmed()
        {
            disconnectConfirmDialog()
            remove()
        }

        function remove()
        {
            mastactva.currentGalleryRemoved.connect(currentGalleryRemoved)
            mastactva.removeCurrentGallery()
        }

        function currentGalleryRemoved()
        {
            mastactva.galleryReloaded.connect(onGalleryReloaded)
            mastactva.reloadGalleriesModel()
        }

        function onGalleryReloaded()
        {
            mastactva.galleryReloaded.disconnect(onGalleryReloaded)
            galleryCurrentIndex = -1
        }
    }

    Action {
        id: addGalleryImage
        text: qsTr("Add &Image To Current Gallery")
        onTriggered: {
            if(galleryCurrentIndex < 0 || galleryCurrentId < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select gallery first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To add an image to the gallery you should first select a gallery")
                popupMessage.open()
            }
            else
            {
                addImageDialog.open()
            }
        }
    }

    Action {
        id: takeOwnshipOfGallery
        text: qsTr("Take &Ownship of Current Gallery")
        onTriggered: {
            if(galleryCurrentIndex < 0 || galleryCurrentId < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select gallery first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To take ownship of the gallery you should first select a gallery")
                popupMessage.open()
            }
            else
            {
                mastactva.takeCurrentGalleryOwnship()
                mastactva.currentGalleryOwnshipTaken.connect(currentGalleryOwnshipTaken)
            }
        }

        function currentGalleryOwnshipTaken()
        {
            mastactva.refreshCurrentGallery()
        }
    }

    Action {
        id: freeOwnshipOfGallery
        text: qsTr("&Free Ownship of Current Gallery")
        onTriggered: {
            if(galleryCurrentIndex < 0 || galleryCurrentId < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select gallery first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To free ownship of the gallery you should first select a gallery")
                popupMessage.open()
            }
            else
            {
                mastactva.freeCurrentGalleryOwnship()
                mastactva.currentGalleryOwnshipFreed.connect(currentGalleryOwnshipFreed)
            }
        }

        function currentGalleryOwnshipFreed()
        {
            mastactva.refreshCurrentGallery()
        }
    }

    Action {
        id: removeCurrentImageOfGallery
        text: qsTr("&Remove Current Image")
        onTriggered: {
            if(galleryImagesCurrentId < 0 || galleryImagesCurrentIndex < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select image of gallery first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To remove an image of the gallery you should first select a image")
                popupMessage.open()
            }
            else
            {
                connectConfirmDialog()
                confirmDialog.confirmText = qsTr("Do you really want to remove the image from the gallery")
                confirmDialog.imageSource = images_of_gallery.model.currentImageSource()
                confirmDialog.showImage = true
                confirmDialog.open()
            }
        }

        function connectConfirmDialog()
        {
            confirmDialog.onSkip.connect(onCancelConfirm)
            confirmDialog.onConfirm.connect(onConfirmed)
        }

        function disconnectConfirmDialog()
        {
            confirmDialog.onSkip.disconnect(onCancelConfirm)
            confirmDialog.onConfirm.disconnect(onConfirmed)
        }

        function onCancelConfirm()
        {
            disconnectConfirmDialog()
        }

        function onConfirmed()
        {
            disconnectConfirmDialog()
            mastactva.removeCurrentImage()
            mastactva.imageOfGalleryRemoved.connect(onImageOfGalleryRemoved)
        }

        function onImageOfGalleryRemoved()
        {
            mastactva.reloadGalleriesModel()
            mastactva.galleryReloaded.connect(onGalleryReloaded)
            mastactva.reloadAllImagesOfGalleryViewModel()
            mastactva.imagesOfGalleryReloaded.connect(onGalleryImagesReloaded)
        }

        function onGalleryReloaded()
        {
            galleries.currentIndex = galleryCurrentIndex;
        }

        function onGalleryImagesReloaded()
        {
            galleries.currentIndex = -1;
        }
    }

    Action {
        id: setImageOfGalleryAsTop
        text: qsTr("Make Image &Top")
        onTriggered: {
            if(galleryImagesCurrentId < 0 || galleryImagesCurrentIndex < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select an image of the gallery first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To make an image of the gallery top you should first select a image")
                popupMessage.open()
            }
            else
            {
                mastactva.setImageOfGalleryAsTop()
                mastactva.imageOfGalleryAsTopSet.connect(imageOfGalleryAsTopSet)
            }
        }

        function imageOfGalleryAsTopSet()
        {
            mastactva.reloadGalleriesModel()
            mastactva.galleryReloaded.connect(onGalleryReloaded)
        }

        function onGalleryReloaded()
        {
            galleries.currentIndex = galleryCurrentIndex;
        }
    }

    Action {
        id: resetImageOfGalleryAsTop
        text: qsTr("Make Image &Non Top")
        onTriggered: {
            if(galleryImagesCurrentId < 0 || galleryImagesCurrentIndex < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select an image of the gallery first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To make an image of the gallery non top you should first select a image")
                popupMessage.open()
            }
            else
            {
                mastactva.resetImageOfGalleryAsTop()
                mastactva.imageOfGalleryAsTopReset.connect(imageOfGalleryAsTopReset)
            }
        }

        function imageOfGalleryAsTopReset()
        {
            mastactva.reloadGalleriesModel()
            mastactva.galleryReloaded.connect(onGalleryReloaded)
        }

        function onGalleryReloaded()
        {
            galleries.currentIndex = galleryCurrentIndex;
        }
    }

    Action {
        id: showImagePointsAction
        text: qsTr("Show image &points")
        onTriggered: {
            showImagePoints = !showImagePoints
        }
    }

    Action {
        id: showImagePointsVoronoyDiagramAction
        text: qsTr("Show &voronoy diagram")
        onTriggered: {
            showImagePointsVoronoyDiagram = !showImagePointsVoronoyDiagram
        }
    }

    Action {
        id: imageOfGalleryCreateDescription
        text: qsTr("&New Description")
        onTriggered: {
            if(galleryImagesCurrentId < 0 || galleryImagesCurrentIndex < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select an image of the gallery first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To create a description of the image of the gallery you should first select an image")
                popupMessage.open()
            }
            else
            {
                createNewDescriptionDialog.open()
            }
        }
    }

    Action {
        id: imageOfGalleryEditDescription
        text: qsTr("&Edit Description")
        onTriggered: {
            if(galleryImagesCurrentId < 0 || galleryImagesCurrentIndex < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select an image of the gallery first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To create a description of the image of the gallery you should first select an image")
                popupMessage.open()
            }
            else if(imageOfGalleryDescriptionIndex < 0 || imageOfGalleryDescriptionId < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select a description of the image first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To edit a description of the image of the gallery you should first select a description")
                popupMessage.open()
            }
            else
            {
                editDescriptionDialog.open()
            }
        }
    }

    Action {
        id: imageOfGalleryDeleteDescription
        text: qsTr("&Delete Description")
        onTriggered: {
            if(galleryImagesCurrentId < 0 || galleryImagesCurrentIndex < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select an image of the gallery first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To create a description of the image of the gallery you should first select an image")
                popupMessage.open()
            }
            else if(imageOfGalleryDescriptionIndex < 0 || imageOfGalleryDescriptionId < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select a description of the image first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To edit a description of the image of the gallery you should first select a description")
                popupMessage.open()
            }
            else
            {
                var descr = mastactva.getCurrentDescription()
                if(descr !== undefined) {
                    confirmDialog.confirmText = qsTr("Do you really want to delete description?") + qsTr("<br/>") + qsTr("<br/>") + qsTr("Description text : ") + GalleryFunctions.textLFtoBr(mastactva.readMore(descr.descriptionStr, 128, qsTr(" ..."))) + qsTr("<br/>") + qsTr("From : ") + mastactva.dateTimeToUserStr(descr.fromDateTime);
                    confirmDialog.showImage = false
                    connectConfirmDialog()
                    confirmDialog.open()
                }
            }
        }

        function connectConfirmDialog()
        {
            confirmDialog.onSkip.connect(onCancelConfirm)
            confirmDialog.onConfirm.connect(onConfirmed)
        }

        function disconnectConfirmDialog()
        {
            confirmDialog.onSkip.disconnect(onCancelConfirm)
            confirmDialog.onConfirm.disconnect(onConfirmed)
        }

        function onCancelConfirm()
        {
            disconnectConfirmDialog()
        }

        function onConfirmed()
        {
            disconnectConfirmDialog()
            mastactva.removeCurrentDescription()
            mastactva.onDescriptionDeleted.connect(onDescriptionDeleted)
        }

        function onDescriptionDeleted()
        {
            imageOfGalleryDescriptionIndex = -1;
            mastactva.refreshDescriptions();
        }
    }

    Action {
        id: imageOfGalleryEditQuestion
        text: qsTr("&Edit question")
        onTriggered: {
            if(galleryImagesCurrentId < 0 || galleryImagesCurrentIndex < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select an image of the gallery first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To create a description of the image of the gallery you should first select an image")
                popupMessage.open()
            }
            else if(imageOfGalleryPointIndex < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select an image point of the image first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To edit a question of an image point of an image of the gallery you should first select an image point")
                popupMessage.open()
            }
            else
            {
                editQuestionDialog.open()
            }
        }
    }

    Action {
        id: imageOfGalleryCreateAnswer
        text: qsTr("&New answer")
        onTriggered: {
            if(galleryImagesCurrentId < 0 || galleryImagesCurrentIndex < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select an image of the gallery first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To create a description of the image of the gallery you should first select an image")
                popupMessage.open()
            }
            else if(imageOfGalleryPointIndex < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select an image point of the image first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To add new answer of the question of the image point of the image of the gallery you should first select an image point")
                popupMessage.open()
            }
            else
            {
                newAnswerDialog.open()
            }
        }
    }

    Action {
        id: imageOfGalleryEditAnswer
        text: qsTr("&Edit answer")
        onTriggered: {
            if(galleryImagesCurrentId < 0 || galleryImagesCurrentIndex < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select an image of the gallery first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To create a description of the image of the gallery you should first select an image")
                popupMessage.open()
            }
            else if(imageOfGalleryPointIndex < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select an image point of the image first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To edit an answer of the question of the image point of the image of the gallery you should first select an image point")
                popupMessage.open()
            }
            else if(imageOfGalleryAnswerIndex < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select an answer of the image point of the image first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To edit an answer of the question of the image point of the image of the gallery you should first select an answer")
                popupMessage.open()
            }
            else
            {
                editAnswerDialog.open()
            }
        }
    }

    Action {
        id: imageOfGalleryDeleteAnswer
        text: qsTr("&Delete answer")
        onTriggered: {
            if(galleryImagesCurrentId < 0 || galleryImagesCurrentIndex < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select an image of the gallery first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To create a description of the image of the gallery you should first select an image")
                popupMessage.open()
            }
            else if(imageOfGalleryPointIndex < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select an image point of the image first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To remove the answer of the question of the image point of the image of the gallery you should first select an image point")
                popupMessage.open()
            }
            else if(imageOfGalleryAnswerIndex < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select an answer of the image point of the image first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To remove an answer of the question of the image point of the image of the gallery you should first select an answer")
                popupMessage.open()
            }
            else
            {
                var answer = mastactva.getCurrentAnswer()
                if(answer !== undefined)
                {
                    confirmDialog.confirmText = qsTr("Do you really want to remove answer?") + qsTr("<br/>") + qsTr("<br/>") + qsTr("Answer : ") + qsTr("<br/>") + GalleryFunctions.textLFtoBr(mastactva.readMore(answer.answer, 128, qsTr(" ..."))) + qsTr("<br/>") + qsTr("Points for answer : ") + answer.pointsToPass;
                    confirmDialog.showImage = false
                    connectConfirmDialog()
                    confirmDialog.open()
                }
             }
        }

        function connectConfirmDialog()
        {
            confirmDialog.onSkip.connect(onCancelConfirm)
            confirmDialog.onConfirm.connect(onConfirmed)
        }

        function disconnectConfirmDialog()
        {
            confirmDialog.onSkip.disconnect(onCancelConfirm)
            confirmDialog.onConfirm.disconnect(onConfirmed)
        }

        function onCancelConfirm()
        {
            disconnectConfirmDialog()
        }

        function onConfirmed()
        {
            disconnectConfirmDialog()
            mastactva.removeCurrentAnswer()
            mastactva.onCurrentAnswerRemoved.connect(onCurrentAnswerRemoved)
        }

        function onCurrentAnswerRemoved()
        {
            mastactva.refreshCurrentImagePointToQuestion();
        }
    }

    Action {
        id: addPointOfImage
        text: qsTr("Add point template")
        onTriggered: {
            if(galleryImagesCurrentId < 0 || galleryImagesCurrentIndex < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select an image of the gallery first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To add an image point of the image of the gallery you should first select an image")
                popupMessage.open()
            }
            else
            {
                create()
            }
        }

        function create()
        {
            imagePointDialog.titleText = qsTr("Create image point template")
            imagePointDialog.pointsModel = images_of_gallery.model.currentImagePoints()
            imagePointDialog.imageSource = images_of_gallery.model.currentImageSource()
            imagePointDialog.imageId = galleryImagesCurrentId
            imagePointDialog.pointIndex = -1
            imagePointDialog.choosePointType = true
            imagePointDialog.open()
        }
    }

    Action {
        id: editPointOfImage
        text: qsTr("Edit point of image")
        onTriggered: {
            if(galleryImagesCurrentId < 0 || galleryImagesCurrentIndex < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select an image of the gallery first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To create a description of the image of the gallery you should first select an image")
                popupMessage.open()
            }
            else if(imageOfGalleryPointIndex < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select an image point of the image first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To edit properties of the image point of the image of the gallery you should first select an image point")
                popupMessage.open()
            }
            else
            {
                startEdit()
            }
        }

        function startEdit()
        {
            if(imageOfGalleryPointIndex >= 0 || images_of_gallery.model.currentImagePoints().getSize() > 0)
            {
                imagePointDialog.titleText = qsTr("Edit image point")
                imagePointDialog.pointsModel = images_of_gallery.model.currentImagePoints()
                imagePointDialog.imageSource = images_of_gallery.model.currentImageSource()
                imagePointDialog.imageId = galleryImagesCurrentId
                imagePointDialog.pointIndex = imageOfGalleryPointIndex >= 0 ? imageOfGalleryPointIndex : images_of_gallery.model.currentImagePoints().getSize() - 1
                imagePointDialog.choosePointType = false
                imagePointDialog.open()
            }
        }
    }

    Action {
        id: removePointOfImage
        text: qsTr("Remove point of image")
        onTriggered: {
            if(galleryImagesCurrentId < 0 || galleryImagesCurrentIndex < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select an image of the gallery first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To create a description of the image of the gallery you should first select an image")
                popupMessage.open()
            }
            else if(imageOfGalleryPointIndex < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select an image point of the image first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To remove an image point of the image of the gallery you should first select an image point")
                popupMessage.open()
            }
            else
            {
                var imagePoint = mastactva.getCurrentImagePoint()
                if(imagePoint !== undefined)
                {
                    confirmDialog.confirmText = qsTr("Do you really want to remove image point?") + qsTr("<br/>") + qsTr("<br/>") + qsTr("x : ") + imagePoint.xCoord + qsTr("<br/>") + qsTr("y : ") + imagePoint.xCoord + qsTr("<br/>") + qsTr("weight : ") + imagePoint.weight + qsTr("<br/>");
                    confirmDialog.showImage = false
                    connectConfirmDialog()
                    confirmDialog.open()
                }
            }
        }

        function connectConfirmDialog()
        {
            confirmDialog.onSkip.connect(onCancelConfirm)
            confirmDialog.onConfirm.connect(onConfirmed)
        }

        function disconnectConfirmDialog()
        {
            confirmDialog.onSkip.disconnect(onCancelConfirm)
            confirmDialog.onConfirm.disconnect(onConfirmed)
        }

        function onCancelConfirm()
        {
            disconnectConfirmDialog()
        }

        function onConfirmed()
        {
            disconnectConfirmDialog()
            remove()
        }

        function remove()
        {
            mastactva.removeCurrentImagePoint()
            mastactva.currentImagePointRemoved.connect(currentImagePointRemoved)
        }

        function currentImagePointRemoved()
        {
            mastactva.currentImagePointRemoved.disconnect(currentImagePointRemoved)
            imageOfGalleryPointIndex = -1
        }
    }

    Action {
        id: editOptionsAction
        text: qsTr("&Edit options")
        onTriggered: {
            optionsEditDialog.open()
        }
    }

    Action {
        id: nextImageEditAction
        text: qsTr("Edit next image")
        onTriggered: {
            if(galleryImagesCurrentId < 0 || galleryImagesCurrentIndex < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select an image of the gallery first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To create a description of the image of the gallery you should first select an image")
                popupMessage.open()
            }
            else if(imageOfGalleryPointIndex < 0)
            {
                popupMessage.fieldPopupMessageShortText = qsTr("Select an image point of the image first")
                popupMessage.fieldPopupMessageDescriptionText = qsTr("To edit next image of the image point of the image of the gallery you should first select an image point")
                popupMessage.open()
            }
            else
            {
                nextImageEditDialog.open()
            }
        }
    }

    MenuBar
    {
        id: galleriesMenuBar
        AutoSizeMenu {
            title: qsTr("&Galleries")
            MenuItem { action: refreshGalleriesModel }
            MenuItem { action: createNewGallery }
            MenuItem { action: editCurrentGallery }
            MenuItem { action: addGalleryImage }
            MenuItem { action: takeOwnshipOfGallery }
            MenuItem { action: freeOwnshipOfGallery }
            MenuItem { action: removeCurrentGallery }
        }
        AutoSizeMenu {
            title: qsTr("All &Images of Gallery")
            MenuItem { action: refreshAllGalleryImagesModel }
            MenuItem { action: setImageOfGalleryAsTop }
            MenuItem { action: resetImageOfGalleryAsTop }
            MenuItem { action: removeCurrentImageOfGallery }
        }
        AutoSizeMenu {
            title: qsTr("Image &point")
            MenuItem {
                action: showImagePointsAction
                checkable: true
                checked: showImagePoints
            }
            MenuItem {
                action: showImagePointsVoronoyDiagramAction
                checkable: true
                checked: showImagePointsVoronoyDiagram
            }
            MenuItem { action: addPointOfImage }
            MenuItem { action: editPointOfImage }
            MenuItem { action: removePointOfImage }
        }
        AutoSizeMenu {
            title: qsTr("&Description")
            MenuItem { action: imageOfGalleryCreateDescription }
            MenuItem { action: imageOfGalleryEditDescription }
            MenuItem { action: imageOfGalleryDeleteDescription }
        }
        AutoSizeMenu {
            title: qsTr("&Next Image")
            MenuItem { action: nextImageEditAction }
        }
        AutoSizeMenu {
            title: qsTr("&Question")
            MenuItem { action: imageOfGalleryEditQuestion }
        }
        AutoSizeMenu {
            title: qsTr("&Answer")
            MenuItem { action: imageOfGalleryCreateAnswer }
            MenuItem { action: imageOfGalleryEditAnswer }
            MenuItem { action: imageOfGalleryDeleteAnswer }
        }
        AutoSizeMenu {
            title: qsTr("&Options")
            MenuItem { action: editOptionsAction }
        }
    }

    Action {
        id: addEffect
        text: qsTr("&Add effect")
        onTriggered: {
            effectEditDialog.fieldEffect = effectModel.createItem()
            effectEditDialog.fieldNewItem = true
            effectEditDialog.open()
        }
    }

    Action {
        id: editEffect
        text: qsTr("&Edit effect")
        onTriggered: {
            effectEditDialog.fieldEffect = effectModel.getCurrentItem()
            effectEditDialog.fieldNewItem = false
            effectEditDialog.open()
        }
    }

    Action {
        id: removeEffect
        text: qsTr("&Remove effect")
        onTriggered: {

        }
    }

    Action {
        id: addNewShader
        text: qsTr("&Add new shader")
        onTriggered: {

        }
    }

    Action {
        id: addExistingShader
        text: qsTr("Add &existing shader")
        onTriggered: {

        }
    }

    Action {
        id: editShaderInfo
        text: qsTr("Edit shader &info")
        onTriggered: {

        }
    }

    Action {
        id: removeShader
        text: qsTr("&Remove shader")
        onTriggered: {

        }
    }

    Action {
        id: addEffectArgument
        text: qsTr("&Add effect argument")
        onTriggered: {

        }
    }

    Action {
        id: editEffectArgument
        text: qsTr("&Edit effect argument")
        onTriggered: {

        }
    }

    Action {
        id: removeEffectArgument
        text: qsTr("&Remove effect argument")
        onTriggered: {

        }
    }

    Action {
        id: addArgumentSet
        text: qsTr("&Add argument set")
        onTriggered: {

        }
    }

    Action {
        id: addArgumentOfArgumentSet
        text: qsTr("Add a&rgument of argument set")
        onTriggered: {

        }
    }

    Action {
        id: editArgumentOfArgumentSet
        text: qsTr("&Edit argument of argument set")
        onTriggered: {

        }
    }

    Action {
        id: removeArgumentOfArgumentSet
        text: qsTr("Remove argument of argument set")
        onTriggered: {

        }
    }

    Action {
        id: removeArgumentSet
        text: qsTr("Remove argument set")
        onTriggered: {

        }
    }

    Action {
        id: playEffectDemo
        text: qsTr("&Play effect demo")
        onTriggered: {

        }
    }

    Action {
        id: pauseEffectDemo
        text: qsTr("P&ause effect demo")
        onTriggered: {

        }
    }

    Action {
        id: stopEffectDemo
        text: qsTr("&Stop effect demo")
        onTriggered: {

        }
    }

    MenuBar {
        id: effectsMenuBar
        AutoSizeMenu {
            title: qsTr("&Effects")
            MenuItem { action: addEffect }
            MenuItem { action: editEffect }
            MenuItem { action: removeEffect }
        }
        AutoSizeMenu {
            title: qsTr("&Shaders")
            MenuItem { action: addNewShader }
            MenuItem { action: addExistingShader }
            MenuItem { action: editShaderInfo }
            MenuItem { action: removeShader }
        }
        AutoSizeMenu {
            title: qsTr("&Arguments")
            MenuItem { action: addEffectArgument }
            MenuItem { action: editEffectArgument }
            MenuItem { action: removeEffectArgument }
        }
        AutoSizeMenu {
            title: qsTr("Argument Se&ts")
            MenuItem { action: addArgumentSet }
            MenuItem { action: addArgumentOfArgumentSet }
            MenuItem { action: editArgumentOfArgumentSet }
            MenuItem { action: removeArgumentOfArgumentSet }
            MenuItem { action: removeArgumentSet }
        }
        AutoSizeMenu {
            title: qsTr("&Demo")
            MenuItem { action: playEffectDemo }
            MenuItem { action: pauseEffectDemo }
            MenuItem { action: stopEffectDemo }
        }
    }

    menuBar: appMode == 0 ? galleriesMenuBar : effectsMenuBar

    Rectangle
    {
        anchors.fill: parent

        TabBar {
            id: topicTabBar
            anchors.top: parent.top
            width: parent.width
            TabButton {
                text: qsTr("Galleries")
            }
            TabButton {
                text: qsTr("Effects")
            }
        }

        StackLayout {
            anchors.top: topicTabBar.bottom
            anchors.left: parent.left
            width: parent.width
            height: parent.height - topicTabBar.height
            currentIndex: topicTabBar.currentIndex

            Item {
                id: topicGalleries
                SplitView {
                    id: slitGalleriesAndImagesOfGallery
                    anchors.fill: parent
                    orientation: Qt.Horizontal

                    Rectangle{

                        id: splitViewPaneGallery

                        SplitView.preferredWidth: Constants.leftSideBarWidth
                        SplitView.minimumWidth: Constants.leftSideBarWidth/2
                        SplitView.maximumWidth: Constants.leftSideBarWidth*2
                        height: parent.height

                        ListView {
                            id: galleries

                            anchors.fill: parent
                            spacing: Constants.galleriesListViewSpacing
                            clip: true

                            model: GalleryEditViewModel {
                                objectName: "GalleryModel"
                            }

                            delegate: gallery
                        }
                    }

                    Rectangle{

                        id: slitViewPaneImageOfGallery
                        SplitView.preferredWidth: root.width - Constants.leftSideBarWidth
                        height: parent.height

                        SplitView {
                            id: slitImagesOfGalleryAndImageInfo
                            anchors.fill: parent
                            orientation: Qt.Vertical

                            Rectangle{

                                id: slitViewPaneImagesOfGallery

                                SplitView.preferredHeight: Constants.height
                                SplitView.minimumHeight: Constants.height/2

                                ListView {
                                    id: images_of_gallery

                                    anchors.fill: parent
                                    orientation: ListView.Horizontal
                                    clip: true
                                    spacing: Constants. imagesOfGalleryListViewSpacing

                                    model: GalleryImagesModel {
                                        objectName: "AllGalleryImagesModel"
                                        galleryViewImages: false
                                        galleryId: -1
                                    }

                                    delegate : gallery_image

                                    Connections{
                                        target: images_of_gallery
                                        function onCurrentIndexChanged()
                                        {
                                            if(imageOfGalleryDescriptionIndex == -1)
                                            {
                                                imageOfGalleryNextImageNextImage.source = Constants.noImage;
                                            }
                                        }
                                    }

                                }
                            }

                            Rectangle {

                                id: slitViewPaneImageInfo
                                SplitView.preferredHeight: parent.height - Constants.height

                                TabBar {
                                    id: imageOfGalleryInfoBar
                                    anchors.top: parent.top
                                    width: parent.width
                                    TabButton {
                                        text: qsTr("Description")
                                    }
                                    TabButton {
                                        text: qsTr("Next image")
                                    }
                                    TabButton {
                                        text: qsTr("Question")
                                    }
                                }

                                StackLayout {
                                    anchors.top: imageOfGalleryInfoBar.bottom
                                    width: slitViewPaneImageOfGallery.width
                                    height: slitViewPaneImageInfo.height - imageOfGalleryInfoBar.height
                                    currentIndex: imageOfGalleryInfoBar.currentIndex
                                    Item {
                                        id: imageOfGalleryDescriptionTab
                                        ScrollView {
                                            anchors.fill: parent
                                            clip:true
                                            ListView {
                                                id: imageOfGalleryDescriptionListView
                                                anchors.fill: parent
                                                clip: true
                                                model: DescriptionModel {
                                                    objectName: "ImageOfGalleryDescriptionModel"
                                                    imageID: -1
                                                }
                                                delegate : imageOfGalleryDescriptionListViewItem
                                            }
                                        }
                                    }
                                    Item {
                                        id: imageOfGalleryNextImageTab
                                        Text {
                                            anchors.verticalCenter: parent.verticalCenter
                                            anchors.left: parent.left
                                            anchors.fill: parent
                                            visible: imageOfGalleryPointIndex === -1
                                            text: Constants.selectImagePoint
                                        }
                                        Item {
                                            anchors.fill: parent
                                            visible: imageOfGalleryPointIndex >= 0

                                            Column{

                                                Text {
                                                    id: imageOfGalleryNextImageText
                                                    visible: imageOfGalleryPointIndex >= 0
                                                    width: (slitViewPaneImageInfo.height - imageOfGalleryInfoBar.height) * Constants.aspectX / Constants.aspectY
                                                    text: Constants.notANextImagePoint
                                                }

                                                Image {
                                                    height: slitViewPaneImageInfo.height - imageOfGalleryInfoBar.height - imageOfGalleryNextImageText.height
                                                    width: (slitViewPaneImageInfo.height - imageOfGalleryInfoBar.height) * Constants.aspectX / Constants.aspectY
                                                    id: imageOfGalleryNextImageNextImage
                                                    visible: imageOfGalleryPointIndex >= 0
                                                    fillMode: Image.PreserveAspectFit
                                                    source: Constants.noImage
                                                }
                                            }
                                        }
                                    }
                                    Item {
                                        id: imageOfGalleryQuestionTab
                                        Text {
                                            anchors.verticalCenter: parent.verticalCenter
                                            anchors.left: parent.left
                                            anchors.fill: parent
                                            visible: imageOfGalleryPointIndex === -1
                                            text: Constants.selectImagePoint
                                        }
                                        Item {
                                            anchors.fill: parent
                                            visible: imageOfGalleryPointIndex >= 0

                                            Column{
                                                anchors.left: parent.left

                                                Rectangle{
                                                    width: imageOfGalleryQuestionTab.width
                                                    height: (imageOfGalleryQuestionTab.height * 2) / 5
                                                    visible: imageOfGalleryPointIndex >= 0

                                                    FontMetrics{
                                                        id: imageOfGalleryQuestionPointsToPassFontMetrics
                                                        font: imageOfGalleryQuestionPointsToPass.font
                                                    }

                                                    Column{
                                                        anchors.fill: parent

                                                        TextArea {
                                                            id: imageOfGalleryQuestionText
                                                            visible: imageOfGalleryPointIndex >= 0
                                                            width: imageOfGalleryQuestionTab.width
                                                            height: (imageOfGalleryQuestionTab.height * 2) / 5 - imageOfGalleryQuestionPointsToPassFontMetrics.height * 1.5
                                                            readOnly: true
                                                            text: qsTr("Question")
                                                        }

                                                        Rectangle {
                                                            id: imageOfGalleryQuestionPointsToPassRect
                                                            width: imageOfGalleryQuestionTab.width
                                                            height: imageOfGalleryQuestionPointsToPassFontMetrics.height * 1.5
                                                            Row {
                                                                Label {
                                                                    visible: imageOfGalleryPointIndex >= 0
                                                                    text: qsTr("Points to pass : ")
                                                                }
                                                                Text {
                                                                    id: imageOfGalleryQuestionPointsToPass
                                                                    visible: imageOfGalleryPointIndex >= 0
                                                                    text: qsTr("PointsToPass")
                                                                }
                                                            }
                                                        }
                                                    }
                                                }

                                                Rectangle{
                                                    width: imageOfGalleryQuestionTab.width
                                                    height: (imageOfGalleryQuestionTab.height * 3) / 5
                                                    visible: imageOfGalleryPointIndex >= 0

                                                    ListView {
                                                        id: imageOfGalleryQuestionAnswersListView
                                                        visible: imageOfGalleryPointIndex >= 0
                                                        anchors.fill: parent
                                                        clip: true
                                                        highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
                                                        model: QuestionAnswersModel {
                                                            objectName: "ImageOfGalleryQuestionAnswersModel"
                                                            questionId: -1
                                                        }

                                                        delegate: imageOfGalleryQuestionAnswersListViewItem
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            Item {
                id: topicEffects
                SplitView {
                    id: slitEffectsInfoDemo
                    anchors.fill: parent
                    orientation: Qt.Horizontal
                    Rectangle{
                        id: splitEffects

                        SplitView.preferredWidth: Constants.leftSideBarWidth
                        SplitView.minimumWidth: Constants.leftSideBarWidth/2
                        SplitView.maximumWidth: Constants.leftSideBarWidth*2
                        height: parent.height

                        ListView {
                            id: effectsList

                            anchors.fill: parent
                            spacing: Constants.effectsListViewSpacing
                            clip: true
                            model: effectModel
                            delegate: effectItem
                        }
                    }
                    Rectangle{
                        id: splitEffectsInfo

                        width: parent.width - (splitEffects.width + splitEffectsDemo.width)
                        height: parent.height

                        TabBar {
                            id: effectInfoTabBar
                            anchors.top: parent.top
                            width: parent.width
                            TabButton {
                                text: qsTr("Info")
                            }
                            TabButton {
                                text: qsTr("Shaders")
                            }
                            TabButton {
                                text: qsTr("Arguments")
                            }
                            TabButton {
                                text: qsTr("Argument Sets")
                            }
                        }

                        StackLayout {
                            anchors.top: effectInfoTabBar.bottom
                            anchors.left: parent.left
                            width: parent.width
                            height: parent.height - effectInfoTabBar.height
                            currentIndex: effectInfoTabBar.currentIndex

                            Item {
                                id: effectInfoCommon
                                Text { text: qsTr("Effect info") }
                            }
                            Item {
                                id: effectInfoShaders
                                Text { text: qsTr("Shaders list") }
                            }
                            Item {
                                id: effectInfoArguments
                                Text { text: qsTr("Arguments list") }
                            }
                            Item {
                                id: effectInfoArgumentSets
                                Text { text: qsTr("Argument Sets list") }
                            }
                        }
                    }
                    Rectangle{
                        id: splitEffectsDemo

                        SplitView.preferredWidth: Constants.leftSideBarWidth
                        SplitView.minimumWidth: Constants.leftSideBarWidth/2
                        SplitView.maximumWidth: Constants.leftSideBarWidth*2
                        height: parent.height

                        Text {
                            text: qsTr("Effects Demo")
                        }
                    }
                }
            }
        }
    }

    Component {
        id: gallery

        Column {
            SystemPalette {
                id: galleryItemPallete
                colorGroup: SystemPalette.Active
            }

            property int gallery_index: index
            property int galleryPaneWidth: splitViewPaneGallery.width

            CheckBox {
                id: gallery_own
                text: qsTr("You own gallery")
                checked: own_gallery
                enabled: false
                x: (galleryPaneWidth-width)/2
                width: galleryPaneWidth - (Constants.leftSideBarWidth - Constants.leftSideBarTextWidth)
            }

            Rectangle {
                width: galleryPaneWidth
                height: (galleryPaneWidth / Constants.aspectX) * Constants.aspectY
                border.width: 2
                border.color: galleryCurrentIndex === gallery_index ? galleryItemPallete.highlight : galleryItemPallete.window

                SwipeView {
                    id: gallery_images

                    x: Constants.galleryImageSpacing / 2
                    y: Constants.galleryImageSpacing / 2
                    width: (galleryPaneWidth - Constants.galleryImageSpacing)
                    height: ((galleryPaneWidth - Constants.galleryImageSpacing) / Constants.aspectX) * Constants.aspectY
                    clip: true

                    Repeater {
                        id: gallery_image_repeater
                        model: images
                        Image {
                            id: image_of_gallery_view
                            width: (galleryPaneWidth - Constants.galleryImageSpacing)
                            height: ((galleryPaneWidth - Constants.galleryImageSpacing) / Constants.aspectX) * Constants.aspectY
                            source: image_source
                            fillMode: Image.PreserveAspectFit

                            MouseArea {
                                anchors.fill: parent
                                acceptedButtons: Qt.LeftButton | Qt.RightButton

                                onClicked:
                                {
                                    if (mouse.button === Qt.RightButton)
                                    {
                                        galleryContextMenu.popup()
                                    }
                                    else
                                    {
                                        galleryCurrentIndex = gallery_index
                                        mouse.accepted = false
                                    }
                                }

                                onPressAndHold: {
                                    if (mouse.source === Qt.MouseEventNotSynthesized)
                                        galleryContextMenu.popup()
                                }

                                AutoSizeMenu {
                                    id: galleryContextMenu
                                    MenuItem { action: refreshGalleriesModel }
                                    MenuItem { action: createNewGallery }
                                    MenuItem { action: editCurrentGallery }
                                    MenuItem { action: addGalleryImage }
                                    MenuItem { action: takeOwnshipOfGallery }
                                    MenuItem { action: freeOwnshipOfGallery }
                                    MenuItem { action: removeCurrentGallery }
                                }
                            }
                        }
                    }
                }

            }

            PageIndicator {
               x:(galleryPaneWidth-width)/2
               height: Constants.leftSideBarPageIndicatorHeight
               currentIndex: gallery_images.currentIndex
               count: gallery_images.count
            }

            property bool showFullDescription: false

            Text {
                id : gallery_description
                text: showFullDescription ? description : mastactva.readMore(description, 128, qsTr(" ..."))
                x: (galleryPaneWidth-width)/2
                width: galleryPaneWidth - (Constants.leftSideBarWidth - Constants.leftSideBarTextWidth)
                wrapMode: Text.WordWrap
                MouseArea {
                    anchors.fill: gallery_description

                    onDoubleClicked: {
                        showFullDescription = !showFullDescription;
                    }
                }
            }
        }
    }

    Component {
        id: gallery_image

        Rectangle {
            id: gallery_image_rect

            property int imageOfGallery_index: index
            property int imageOfGalleryHeight: slitViewPaneImagesOfGallery.height

            SystemPalette {
                id: imageOfGalleryItemPallete
                colorGroup: SystemPalette.Active
            }

            width: (imageOfGalleryHeight * Constants.aspectX) / Constants.aspectY
            height: imageOfGalleryHeight
            z: 0
            border.width: 2
            border.color: galleryImagesCurrentIndex === imageOfGallery_index ? imageOfGalleryItemPallete.highlight : imageOfGalleryItemPallete.window

            Image  {
                id: image_of_gallery
                x: Constants.imageOfGalleryImageSpacing / 2
                y: Constants.imageOfGalleryImageSpacing / 2
                width: ((imageOfGalleryHeight - Constants.imageOfGalleryImageSpacing) * Constants.aspectX) / Constants.aspectY
                height: (imageOfGalleryHeight - Constants.imageOfGalleryImageSpacing)
                source: image_source
                fillMode: Image.PreserveAspectFit
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton

                    onClicked:
                    {
                        if (mouse.button === Qt.RightButton)
                        {
                            imagesOfGalleryContextMenu.popup()
                        }
                        else
                        {
                            if(galleryImagesCurrentIndex !== imageOfGallery_index)
                            {
                                if(imageOfGalleryPointIndex >= 0)
                                {
                                    //imageOfGalleryNextImageText.text = Constants.selectImagePoint
                                    //images_of_gallery.currentItem.imagePointsRepeater.itemAt(imageOfGalleryPointIndex).source = Constants.inactivePoint
                                }
                                galleryImagesCurrentIndex = imageOfGallery_index
                            }
                            mouse.accepted = false
                        }
                    }

                    onPressAndHold: {
                        if (mouse.source === Qt.MouseEventNotSynthesized)
                            imagesOfGalleryContextMenu.popup()
                    }

                    AutoSizeMenu {
                        id: imagesOfGalleryContextMenu
                        MenuItem { action: refreshAllGalleryImagesModel }
                        MenuItem { action: setImageOfGalleryAsTop }
                        MenuItem { action: resetImageOfGalleryAsTop }
                        MenuItem { action: removeCurrentImageOfGallery }
                        MenuSeparator {
                            padding: 0
                            topPadding: 6
                            bottomPadding: 6
                            contentItem: Rectangle {
                                implicitWidth: 100
                                implicitHeight: 3
                                color: "#1E1E1E"
                            }
                        }
                        MenuItem {
                            action: showImagePointsAction
                            checkable: true
                            checked: showImagePoints
                        }
                        MenuItem {
                            action: showImagePointsVoronoyDiagramAction
                            checkable: true
                            checked: showImagePointsVoronoyDiagram
                        }
                        MenuItem { action: addPointOfImage }
                        MenuItem { action: editPointOfImage }
                        MenuItem { action: removePointOfImage }
                    }
                }
                onStatusChanged: {
                    loadVoronoyDiagram(image_points, status);
                }
            }

            property var voronoyDiagramDlg: undefined

            Connections {
                target: image_points
                function onImagePointsLoaded() {
                    loadVoronoyDiagram(image_points, image_of_gallery.status);
                }

                function onImagePointsCountChanged()
                {
                    if(voronoyDiagramDlg !== undefined)
                    {
                        voronoyDiagramDlg.destroy()
                        voronoyDiagramDlg = undefined
                        loadVoronoyDiagram(image_points, image_of_gallery.status);
                    }
                }
            }

            function loadVoronoyDiagram(imagePoints, imageStatus)
            {
                if(imagePoints !== undefined && imagePoints !== null && !imagePoints.isEmpty() && imageStatus === Image.Ready)
                {
                    if(voronoyDiagramDlg === undefined)
                    {
                        voronoyDiagramDlg = Qt.createQmlObject(
                                   "import MastactvaQuizEditor 1.0
                                    VoronoyDiagram {
                                        x: (image_of_gallery.width - image_of_gallery.paintedWidth)/2 + image_of_gallery.x
                                        y: (image_of_gallery.height - image_of_gallery.paintedHeight)/2 + image_of_gallery.y
                                        width: image_of_gallery.paintedWidth
                                        height: image_of_gallery.paintedHeight
                                        visible: showImagePointsVoronoyDiagram
                                        opacity: 0.5
                                        z: 0.5
                                        model: image_points
                                        color: \"#000080\"
                                        clip: true
                                    }",
                                   gallery_image_rect,
                                   "voronoyDiagram")
                    }
                    else
                    {
                        voronoyDiagramDlg.x = (image_of_gallery.width - image_of_gallery.paintedWidth)/2 + image_of_gallery.x
                        voronoyDiagramDlg.y = (image_of_gallery.height - image_of_gallery.paintedHeight)/2 + image_of_gallery.y
                        voronoyDiagramDlg.width = image_of_gallery.paintedWidth
                        voronoyDiagramDlg.height = image_of_gallery.paintedHeight
                        voronoyDiagramDlg.modelUpdated()
                    }
                }
            }

            Item {
                id: imagePoints
                anchors.fill: parent
                visible: showImagePoints
                opacity: 0.5
                z: 1.0
                Repeater {
                    id: imagePointsRepeater
                    model: image_points
                    Image {
                        id: pointImage

                        x: ptX() - Constants.pointImageSize/4
                        y: ptY() - Constants.pointImageSize/4
                        width: Constants.pointImageSize/2
                        height: Constants.pointImageSize/2

                        source: imageOfGalleryPointIndex === index ? Constants.activePoint : Constants.inactivePoint
                        function ptX()
                        {
                            return (gallery_image_rect.width - image_of_gallery.paintedWidth)/2 + (imagePoint_x * image_of_gallery.paintedWidth) + image_of_gallery.x
                        }

                        function ptY()
                        {
                            return (gallery_image_rect.height - image_of_gallery.paintedHeight)/2 + (imagePoint_y * image_of_gallery.paintedHeight) + image_of_gallery.y
                        }

                        Connections {
                            target: root
                            function onImageOfGalleryPointIndexChanged()
                            {
                                if(imageOfGalleryPointIndex == -1)
                                {
                                    pointImage.source = Constants.inactivePoint
                                }
                            }
                        }

                        MouseArea {
                            anchors.fill: parent

                            onClicked:
                            {
                                if (mouse.button === Qt.RightButton)
                                {
                                }
                                else
                                {
                                    selectCurrentImagePoint()
                                    mouse.accepted = false;
                                }
                            }
                            onDoubleClicked: {
                                selectCurrentImagePoint()
                                editPointOfImage.startEdit()
                                mouse.accepted = false;
                            }

                            function selectCurrentImagePoint()
                            {
                                if(imageOfGallery_index == galleryImagesCurrentIndex)
                                {
                                    if(imageOfGalleryPointIndex !== -1)
                                    {
                                        imagePointsRepeater.itemAt(imageOfGalleryPointIndex).source = Constants.inactivePoint
                                    }

                                    imageOfGalleryPointIndex = index;
                                    pointImage.source = Constants.activePoint
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Component {
        id: imageOfGalleryDescriptionListViewItem
        Item {
            id: imageOfGalleryDescriptionListViewItemRect
            property int imageOfGalleryDescriptionWidth: slitViewPaneImageOfGallery.width
            property bool showFullDescription: false

            width: imageOfGalleryDescriptionWidth

            Column {
                z : 1
                id: imageOfGalleryDescriptionListViewItemRectColumn
                TextArea {
                    focus: true
                    readOnly: true
                    width: imageOfGalleryDescriptionWidth
                    text: imageOfGalleryDescriptionListViewItemRect.showFullDescription ? description_description : mastactva.readMore(description_description, 128, qsTr(" ..."))
                }
                Text { text: qsTr("From : ") + mastactva.dateTimeToUserStr(description_fromDateTime) }
            }

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: imageOfGalleryDescriptionListViewItemRectColumn.height
                color: "lightsteelblue"
                radius: 5
                visible: imageOfGalleryDescriptionListView.currentIndex === index
                z: 0
            }

            MouseArea {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: imageOfGalleryDescriptionListViewItemRectColumn.height
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                z:1

                onClicked:
                {
                    if (mouse.button === Qt.RightButton)
                    {
                        imageOfGalleryDescriptionListViewItemMenu.popup()
                    }
                    else
                    {
                        imageOfGalleryDescriptionListView.currentIndex = index
                        imageOfGalleryDescriptionIndex = index
                        mouse.accepted = false
                    }
                }

                onDoubleClicked: {
                    imageOfGalleryDescriptionListViewItemRect.showFullDescription = !imageOfGalleryDescriptionListViewItemRect.showFullDescription;
                }

                onPressAndHold: {
                    if (mouse.source === Qt.MouseEventNotSynthesized)
                        imageOfGalleryDescriptionListViewItemMenu.popup()
                }

                AutoSizeMenu {
                    id: imageOfGalleryDescriptionListViewItemMenu
                    MenuItem { action: imageOfGalleryCreateDescription }
                    MenuItem { action: imageOfGalleryEditDescription }
                    MenuItem { action: imageOfGalleryDeleteDescription }
                }
            }
        }
    }

    Component {
        id: imageOfGalleryQuestionAnswersListViewItem
        Item {
            FontMetrics{
                id: imageOfGalleryQuestionAnswersListViewItemTextFontMetrics
                font: imageOfGalleryQuestionAnswersListViewItemText.font
            }

            width: imageOfGalleryQuestionAnswersListView.width
            height: imageOfGalleryQuestionAnswersListViewItemTextFontMetrics.height * 2

            Row {
                Text {
                    id: imageOfGalleryQuestionAnswersListViewItemText
                    text: questionAnswer_text
                }
                Text {
                    text: qsTr(" ( points = ") +  questionAnswer_pointsToPass + qsTr(")")
                }
            }
            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton

                onClicked:
                {
                    if (mouse.button === Qt.RightButton)
                    {
                        imageOfGalleryQuestionAnswersListViewItemMenu.popup()
                    }
                    else
                    {
                        imageOfGalleryAnswerIndex = index
                        imageOfGalleryQuestionAnswersListView.currentIndex = index
                        mouse.accepted = false
                    }
                }

                onPressAndHold: {
                    if (mouse.source === Qt.MouseEventNotSynthesized)
                        imageOfGalleryQuestionAnswersListViewItemMenu.popup()
                }

                AutoSizeMenu {
                    id: imageOfGalleryQuestionAnswersListViewItemMenu
                    MenuItem { action: imageOfGalleryCreateAnswer }
                    MenuItem { action: imageOfGalleryEditAnswer }
                    MenuItem { action: imageOfGalleryDeleteAnswer }
                }
            }
        }
    }
    Component {
        id: effectItem
        Rectangle {
            id: effectItemRect

            FontMetrics{
                id: effectItemFontMetrics
                font: effectItemHeaderText.font
            }

            width: effectsList.width

            property bool fullDescriptionText: false

            Column {
                Text {
                    id: effectItemHeaderText
                    width: effectsList.width
                    wrapMode: Text.WordWrap
                    text: effectName
                    padding: Constants.effectsListHeaderPadding
                }
                Text {
                    id: effectItemDescriptionText
                    width: effectsList.width
                    wrapMode: Text.WordWrap
                    text: fullDescriptionText ? mastactva.leftDoubleCR(effectDescription) : mastactva.readMore(effectDescription, Constants.effectsListReadMoreLength, qsTr(" ..."))
                }
            }
            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton

                onClicked:
                {
                    if (mouse.button === Qt.RightButton)
                    {
                        effectItemMenu.popup()
                    }
                    else
                    {
                        effectCurrentIndex = index
                        mouse.accepted = false
                    }
                }

                onPressAndHold: {
                    if (mouse.source === Qt.MouseEventNotSynthesized)
                        effectItemMenu.popup()
                }

                onDoubleClicked: {
                    fullDescriptionText = !fullDescriptionText
                }

                AutoSizeMenu {
                    id: effectItemMenu
                    MenuItem { action: addEffect }
                    MenuItem { action: editEffect }
                    MenuItem { action: removeEffect }
                }
            }
        }
    }
}

