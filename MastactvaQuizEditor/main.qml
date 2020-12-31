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
    property int imageOfGalleryNextImageEffectCurrentIndex: -1
    property var imageOfGalleryNextImageEffectModel: undefined

    property int effectCurrentIndex: -1
    property var effectShadersCurrentModel: undefined
    property int effectShaderCurrentIndex: -1
    property var effectArgumentsCurrentModel: undefined
    property int effectArgumentsCurrentIndex: -1
    property var effectArgumentSetsCurrentModel: undefined
    property int effectArgumentSetsCurrentIndex: -1
    property var effectArgumentSetValuesCurrentModel: undefined
    property int effectArgumentSetValuesCurrentIndex: -1
    property var demoImageFrom: undefined
    property var demoImageTo: undefined

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
            if(currentImagePointsModel !== undefined && currentImagePointsModel !== null)
            {
                currentImagePointsModel.currentIndex = imageOfGalleryPointIndex
            }
            if(imageOfGalleryPointIndex < 0)
            {
                clearImageSource()
                clearQuestionInfo()
                clearNextImageEffect()
            }
            else
            {
                if(currentImagePointsModel === undefined || currentImagePointsModel === null)
                {
                    clearImageSource()
                    clearQuestionInfo()
                    clearNextImageEffect()
                    return;
                }
                var imgPoint = currentImagePointsModel.currentItem
                if(imgPoint === undefined || imgPoint === null)
                {
                    clearImageSource()
                    clearQuestionInfo()
                    clearNextImageEffect()
                    return
                }
                var nextImage = imgPoint.toNextImage
                var question = imgPoint.toQuestion
                var effect = imgPoint.ipEffect
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

                effect.listReloaded.connect(nextImageEffectReloaded)
                if(effect.isListLoaded())
                {
                    setNextImageEffect()
                }
                else
                {
                    clearNextImageEffect()
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

        function setNextImageEffect()
        {
            if(currentImagePointsModel === undefined || currentImagePointsModel === null)
            {
                return
            }
            var imgPoint = currentImagePointsModel.currentItem
            if(imgPoint === undefined || imgPoint === null)
            {
                return
            }
            var effect = imgPoint.ipEffect
            effect.listReloaded.disconnect(nextImageEffectReloaded)
            if(effect.isListLoaded())
            {
                imageOfGalleryNextImageEffectModel = effect
                imageOfGalleryNextImageEffectList.model = imageOfGalleryNextImageEffectModel
                imageOfGalleryNextImageEffectCurrentIndex = imageOfGalleryNextImageEffectModel.currentIndex
            }
            else
            {
                clearNextImageEffect()
            }
        }

        function clearNextImageEffect()
        {
            imageOfGalleryNextImageEffectModel = undefined
            imageOfGalleryNextImageEffectList.model = 0
            imageOfGalleryNextImageEffectCurrentIndex = -1
        }

        function onImageOfGalleryNextImageEffectCurrentIndexChanged()
        {
            imageOfGalleryNextImageEffectList.currentIndex = imageOfGalleryNextImageEffectCurrentIndex
            if(imageOfGalleryNextImageEffectModel !== undefined && imageOfGalleryNextImageEffectModel !== null)
            {
                imageOfGalleryNextImageEffectModel.currentIndex = imageOfGalleryNextImageEffectCurrentIndex
            }
        }

        function nextImageEffectReloaded()
        {
            setNextImageEffect()
        }

        function onImageOfGalleryAnswerIndexChanged()
        {
            imageOfGalleryQuestionAnswersListView.currentIndex = imageOfGalleryAnswerIndex
        }

        function onEffectCurrentIndexChanged()
        {
            effectsList.currentIndex = effectCurrentIndex
            effectModel.currentIndex = effectCurrentIndex
            if(effectCurrentIndex >= 0)
            {
                var effect = effectModel.getCurrentItem()
                effectInfoCommonName.text = effect.effectName
                effectInfoCommonDescription.text = mastactva.leftDoubleCR(effect.effectDescription)
                var shadersModel = effect.effectShaders
                if(shadersModel.isListLoaded())
                {
                    effectShadersListBusyIndicator.visible = false
                    effectShadersListBusyIndicator.running = false
                    effectShadersCurrentModel = shadersModel
                    effectShadersList.model = shadersModel
                    effectShaderCurrentIndex = effectShadersCurrentModel.size() > 0 ? effectShadersCurrentModel.currentIndex : -1
                }
                else
                {
                    effectShadersListBusyIndicator.visible = true
                    effectShadersListBusyIndicator.running = true
                    effectShadersList.model = 0
                    shadersModel.listReloaded.connect(shadersListReloaded)
                }
                var effectArgumentsModel = effect.effectArgs
                if(effectArgumentsModel.isListLoaded())
                {
                    effectArgumentsCurrentModel = effectArgumentsModel
                    effectArgumentsList.model = effectArgumentsCurrentModel
                    effectArgumentsCurrentIndex = effectArgumentsCurrentModel.size() > 0 ? effectArgumentsCurrentModel.currentIndex : -1
                }
                else
                {
                    effectArgumentsList.model = 0
                    effectArgumentsListBusyIndicator.visible = false
                    effectArgumentsListBusyIndicator.running = false
                    effectArgumentsModel.listReloaded.connect(effectArgumentsListReloaded)
                }
                var effectArgumentSetsModel = effect.effectArgSets
                if(effectArgumentSetsModel.isListLoaded())
                {
                    effectArgumentSetsCurrentModel = effectArgumentSetsModel
                    effectArgumentSetsList.model = effectArgumentSetsCurrentModel
                    effectArgumentSetsCurrentIndex = effectArgumentSetsCurrentModel.size() > 0 ? effectArgumentSetsCurrentModel.currentIndex : -1
                }
                else
                {
                    effectArgumentSetsList.model = 0
                    effectArgumentSetsListBusyIndicator.visible = true
                    effectArgumentSetsListBusyIndicator.running = true

                    effectArgumentSetsModel.listReloaded.connect(effectArgumentSetsListReloaded)
                }
            }
            else
            {
                effectInfoCommonName.text = qsTr("Please, select effect item")
                effectInfoCommonDescription.text = qsTr("")
                effectShadersCurrentModel = undefined
                effectShadersList.model = 0
                effectShaderCurrentIndex = -1
                effectArgumentsCurrentModel = undefined
                effectArgumentsList.model = 0
                effectArgumentsCurrentIndex = -1
                effectArgumentSetsCurrentModel = undefined
                effectArgumentSetsList.model = 0
                effectArgumentSetsCurrentIndex = -1
                effectArgumentSetValuesCurrentModel = undefined
                effectArgumentSetValuesList.model = 0
                effectArgumentSetValuesCurrentIndex = -1
            }
        }

        function shadersListReloaded()
        {
            effectShadersListBusyIndicator.visible = false
            effectShadersListBusyIndicator.running = false

            var effect = effectModel.getCurrentItem()
            var shadersModel = effect.effectShaders
            shadersModel.listReloaded.disconnect(shadersListReloaded)
            if(shadersModel.isListLoaded())
            {
                effectShadersList.model = shadersModel
                effectShadersCurrentModel = shadersModel
                effectShadersList.currentIndex = effectShadersCurrentModel.currentIndex
            }
            else
            {
                effectShadersList.model = 0
                effectShadersCurrentModel = undefined
            }
        }

        function effectArgumentsListReloaded()
        {
            effectArgumentsListBusyIndicator.visible = false
            effectArgumentsListBusyIndicator.running = false

            var effect = effectModel.getCurrentItem()
            var effectArgumentsModel = effect.effectArgs
            effectArgumentsModel.listReloaded.disconnect(effectArgumentsListReloaded)
            if(effectArgumentsModel.isListLoaded())
            {
                effectArgumentsCurrentModel = effectArgumentsModel
                effectArgumentsList.model = effectArgumentsCurrentModel
                effectArgumentsCurrentIndex = effectArgumentsCurrentModel.size() > 0 ? effectArgumentsCurrentModel.currentIndex : -1
            }
            else
            {
                effectArgumentsCurrentModel = undefined
                effectArgumentsList.model = 0
                effectArgumentsCurrentIndex = -1
            }
        }

        function effectArgumentSetsListReloaded()
        {
            effectArgumentSetsListBusyIndicator.visible = false
            effectArgumentSetsListBusyIndicator.running = false

            var effect = effectModel.getCurrentItem()
            var effectArgumentSetsModel = effect.effectArgSets
            effectArgumentSetsModel.listReloaded.disconnect(effectArgumentSetsListReloaded)
            if(effectArgumentSetsModel.isListLoaded())
            {
                effectArgumentSetsCurrentModel = effectArgumentSetsModel
                effectArgumentSetsList.model = effectArgumentSetsCurrentModel
                effectArgumentSetsCurrentIndex = effectArgumentSetsCurrentModel.size() > 0 ? effectArgumentSetsCurrentModel.currentIndex : -1
            }
            else
            {
                effectArgumentSetsCurrentModel = undefined
                effectArgumentSetsList.model = 0
                effectArgumentSetValuesCurrentModel = undefined
                effectArgumentSetValuesList.model = 0
                effectArgumentSetValuesCurrentIndex = -1
                effectArgumentSetsCurrentIndex = -1
            }
        }

        function onEffectShaderCurrentIndexChanged()
        {
            effectShadersList.currentIndex = effectShaderCurrentIndex
            if(effectShadersCurrentModel !== undefined && effectShadersCurrentModel !== null)
            {
                effectShadersCurrentModel.currentIndex = effectShaderCurrentIndex
            }
            else
            {
                effectArgumentSetValuesCurrentModel = undefined
                effectArgumentSetValuesList.model = 0
                effectArgumentSetValuesCurrentIndex = -1
            }
        }

        function onEffectArgumentsCurrentIndexChanged()
        {
            effectArgumentsList.currentIndex = effectArgumentsCurrentIndex
            if(effectArgumentsCurrentModel !== undefined && effectArgumentsCurrentModel !== null)
            {
                effectArgumentsCurrentModel.currentIndex = effectArgumentsCurrentIndex
            }
        }

        function onEffectArgumentSetsCurrentIndexChanged()
        {
            effectArgumentSetsList.currentIndex = effectArgumentSetsCurrentIndex
            effectArgumentSetsCurrentModel.currentIndex = effectArgumentSetsCurrentIndex
            if(effectArgumentSetsCurrentModel !== undefined && effectArgumentSetsCurrentModel !== null && effectArgumentSetsCurrentModel.currentItem !== null)
            {
                var effectSetValuesModel = effectArgumentSetsCurrentModel.currentItem.effectArgSetValues
                if(effectSetValuesModel.isListLoaded())
                {
                    effectArgumentSetValuesCurrentModel = effectSetValuesModel
                    effectArgumentSetValuesList.model = effectSetValuesModel
                    effectArgumentSetValuesCurrentIndex = effectSetValuesModel.size() > 0 ? effectSetValuesModel.currentIndex : -1
                }
                else
                {
                    effectArgumentSetValuesList.model = 0
                    effectSetValuesModel.listReloaded.connect(effectSetValuesListReloaded)
                }
            }
            else
            {
                effectArgumentSetValuesCurrentModel = undefined
                effectArgumentSetValuesList.model = 0
                effectArgumentSetValuesCurrentIndex = -1
            }
        }

        function effectSetValuesListReloaded()
        {
            if(effectArgumentSetsCurrentModel !== undefined && effectArgumentSetsCurrentModel !== null)
            {
                var effectSetValuesModel = effectArgumentSetsCurrentModel.currentItem.effectArgSetValues
                effectSetValuesModel.listReloaded.disconnect(effectSetValuesListReloaded)
                if(effectSetValuesModel.isListLoaded())
                {
                    effectArgumentSetValuesCurrentModel = effectSetValuesModel
                    effectArgumentSetValuesList.model = effectSetValuesModel
                    effectArgumentSetValuesCurrentIndex = effectSetValuesModel.size() > 0 ? effectSetValuesModel.currentIndex : -1
                }
                else
                {
                    effectArgumentSetValuesCurrentModel = undefined
                    effectArgumentSetValuesList.model = 0
                    effectArgumentSetValuesCurrentIndex = -1
                }
            }
            else
            {
                effectArgumentSetValuesCurrentModel = undefined
                effectArgumentSetValuesList.model = 0
                effectArgumentSetValuesCurrentIndex = -1
            }
        }

        function onEffectArgumentSetValuesCurrentIndexChanged()
        {
            effectArgumentSetValuesList.currentIndex = effectArgumentSetValuesCurrentIndex
            if(effectArgumentSetValuesCurrentModel !== undefined && effectArgumentSetValuesCurrentModel !== null)
            {
                effectArgumentSetValuesCurrentModel.currentIndex = effectArgumentSetValuesCurrentIndex
            }
        }

        function onDemoImageFromChanged()
        {
            if(demoImageFrom === undefined || demoImageFrom === null)
            {
                effectDemoImageFrom.source = Constants.noImage
                effectImageDemo.fromImage = [Constants.noImage, Constants.noImageHash]
            }
            else
            {
                effectDemoImageFrom.source = demoImageFrom[0]
                effectImageDemo.fromImage = demoImageFrom
            }
        }

        function onDemoImageToChanged()
        {
            if(demoImageTo === undefined || demoImageTo === null)
            {
                effectDemoImageTo.source = Constants.noImage
                effectImageDemo.toImage = [Constants.noImage, Constants.noImageHash]
            }
            else
            {
                effectDemoImageTo.source = demoImageTo[0]
                effectImageDemo.toImage = demoImageTo
            }
        }
    }

    MastactvaAPI {
        id: mastactva
        objectName: "MastactvaAPI"
    }

    // constant models
    ShaderTypeModel {
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
    }

    EasingTypeModel {
        id: easingTypeModel
        objectName: "EasingTypeModel"
        layoutQMLName: "EasingTypeModel"
        layoutIdField: "id"
        jsonParamsGet: false
        autoCreateChildrenModels: false
    }

    // data models
    EffectModel {
        id: effectModel
        objectName: "EffectModel"
        layoutQMLName: "EffectModel"
        layoutIdField: "id"
        jsonParamsGet: false
        autoCreateChildrenModels: false
        autoCreateChildrenModelsOnSelect: true
    }

    ShaderModel {
        id: shaderModel
        objectName: "ShaderModel"
        layoutQMLName: "ShaderModel"
        layoutIdField: "id"
        jsonParamsGet: false
        autoCreateChildrenModels: false
    }

    GalleryModel {
        id: galleryModel
        objectName: "GalleryModel"
        layoutQMLName: "GalleryModel"
        layoutIdField: "id"
        currentRef: ""
        jsonParamsGet: false
        autoCreateChildrenModels: false
    }

    ImageModel {
        id: allImagesOfGalleryModel
        objectName: "AllImagesOfGallery"
        layoutQMLName: "AllImagesOfGallery"
        layoutIdField: "id"
        currentRef: "gallery"
        jsonParamsGet: false
        autoCreateChildrenModels: true
    }

    function initGalleryModel()
    {
    }

    function initAllImagesOfGalleryModel()
    {
        allImagesOfGalleryModel.setLayoutRef("gallery", "GalleryModel", "id")
        allImagesOfGalleryModel.addModelParam("use_in_gallery_view", "0")
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
            Constants.noImageHash = mastactva.calculateHash(Constants.noImageResource)

            initGalleryModel()
            initAllImagesOfGalleryModel()

            //galleryModel.loadList()

            effectModel.loadList()
            effectsListBusyIndicator.visible = true
            effectsListBusyIndicator.running = true
            shaderTypeModel.loadList()
            shaderArgTypeModel.loadList()
            easingTypeModel.loadList()
            shaderEditDialog.mastactva = mastactva
            shaderEditDialog.shaderTypeModel = shaderTypeModel
            chooseShaderDialog.shaderTypeModel = shaderTypeModel
            chooseShaderDialog.shaderModel = shaderModel
            effectArgumentSetEditDialog.easingTypeModel = easingTypeModel
            effectArgumentValueEditDialog.shaderArgTypeModel = shaderArgTypeModel
            chooseEffectArgumentDialog.shaderArgTypeModel = shaderArgTypeModel
            chooseImageDialog.galleryModel = galleryModel
            chooseImageDialog.allImagesOfGalleryModel = allImagesOfGalleryModel
            chooseImageDialog.mastactva = mastactva
            imagePointEffectEditDialog.effectModel = effectModel
        }
    }

    function showModelError(errorCode, description)
    {
        popupMessage.fieldPopupMessageShortText = qsTr("Application Error");
        popupMessage.fieldPopupMessageDescriptionText = qsTr("Error code : ") + errorCode + qsTr("\n") + qsTr("\n") + qsTr("Description : ") + qsTr("\n") + description
        popupMessage.open()
    }

    Connections {
        target: effectModel

        function onListReloaded()
        {
            effectsListBusyIndicator.visible = false
            effectsListBusyIndicator.running = false
            effectCurrentIndex = effectModel.size() > 0 ? effectModel.currentIndex : -1
        }

        function onItemDeleted()
        {
            effectCurrentIndex = -1
        }

        function onError(errorCode, description)
        {
            showModelError(errorCode, description)
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
            fieldEffect = undefined
        }

        onRejected: {
            fieldEffect = undefined
        }
    }

    ShaderEditDialog {
        id: shaderEditDialog

        onOpened: {
            if(fieldEffectShader !== undefined && fieldEffectShader !== null)
            {
                fieldEffectShader.effectShaderEffectId = effectModel.getCurrentItem().effectId
            }
            init()
        }

        onAccepted: {
            update()
            if(effectShadersCurrentModel !== undefined && effectShadersCurrentModel !== null)
            {
                if(fieldNewItem)
                {
                    fieldEffectShader.effectShaderShader.itemAdded.connect(shaderAdded)
                    fieldEffectShader.effectShaderShader.addItem(fieldShader)
                }
                else
                {
                    fieldEffectShader.effectShaderShader.itemSet.connect(shaderItemSet)
                    fieldEffectShader.effectShaderShader.setItem(effectShaderCurrentIndex, fieldShader)
                }
            }
        }

        onRejected: {
            fieldShader = undefined
            fieldEffectShader = undefined
        }

        function shaderItemSet()
        {
            fieldEffectShader.effectShaderShader.itemSet.disconnect(shaderItemSet)
            fieldShader = undefined
            fieldEffectShader = undefined
        }

        function shaderAdded()
        {
            fieldEffectShader.effectShaderShader.itemAdded.disconnect(shaderAdded)
            fieldEffectShader.setShaderId(fieldShader.shaderId)
            fieldShader = undefined
            effectShadersCurrentModel.itemAdded.connect(effectShaderAdded)
            effectShadersCurrentModel.addItem(fieldEffectShader)
        }

        function effectShaderAdded()
        {
            effectShadersCurrentModel.itemAdded.disconnect(effectShaderAdded)
            fieldEffectShader = undefined
        }
    }

    ChooseShaderDialog {
        id: chooseShaderDialog

        onOpened: {
            fieldShader = undefined
            init()
        }

        onAccepted: {
            if(fieldShader !== undefined && fieldShader !== null && effectShadersCurrentModel !== undefined && effectShadersCurrentModel !== null)
            {
                var newEffectShader = effectShadersCurrentModel.createItem()
                newEffectShader.setShaderId(fieldShader.shaderId)
                newEffectShader.effectShaderEffectId = effectModel.getCurrentItem().effectId
                fieldShader = undefined
                effectShadersCurrentModel.itemAdded.connect(effectShaderAdded)
                effectShadersCurrentModel.addItem(newEffectShader)
            }
        }

        function effectShaderAdded()
        {
            effectShadersCurrentModel.itemAdded.disconnect(effectShaderAdded)
            fieldShader = undefined
        }
    }

    RefreshEffectArgumentsDialog {
        id: refreshEffectsArgumentsDialog

        onOpened : {
        }

        onAccepted : {
            if(!doneOk)
            {
                fieldEffect.cancelRefreshArguments()
            }
            disconnect()
        }

        onRejected: {
            if(!doneOk)
            {
                fieldEffect.cancelRefreshArguments()
            }
            disconnect()
        }
    }

    EffectArgumentSetEditDialog {
        id: effectArgumentSetEditDialog

        property var effectArgumentSetsModel : undefined
        property int effectArgumentSetsModelIndex : -1

        onOpened: {
            effectArgumentSetsModel = effectArgumentSetsCurrentModel
            effectArgumentSetsModelIndex = effectArgumentSetsCurrentIndex
            init()
        }

        onAccepted: {
            if(effectArgumentSetsModel !== undefined && effectArgumentSetsModel !== null && fieldEffectArgumentSet !== undefined && fieldEffectArgumentSet !== null)
            {
                update()
                if(fieldNew)
                {
                    effectArgumentSetsModel.itemAdded.connect(effectArgumentSetAdded)
                    effectArgumentSetsModel.addItem(fieldEffectArgumentSet)
                }
                else
                {
                    effectArgumentSetsModel.itemSet.connect(effectArgumentSetSet)
                    effectArgumentSetsModel.setItem(effectArgumentSetsModelIndex, fieldEffectArgumentSet)
                }
            }
        }

        onRejected: {
            fieldEffectArgumentSet = undefined
            effectArgumentSetsModel = undefined
            effectArgumentSetsModelIndex = -1
        }

        function effectArgumentSetAdded()
        {
            if(effectArgumentSetsModel !== undefined && effectArgumentSetsModel !== null && fieldEffectArgumentSet !== undefined && fieldEffectArgumentSet !== null)
            {
                effectArgumentSetsModel.itemAdded.disconnect(effectArgumentSetAdded)
                effectArgumentSetsCurrentIndex = effectArgumentSetsModel.indexOfItem(fieldEffectArgumentSet)
            }
            fieldEffectArgumentSet = undefined
            effectArgumentSetsModel = undefined
            effectArgumentSetsModelIndex = -1
        }

        function effectArgumentSetSet()
        {
            if(effectArgumentSetsModel !== undefined && effectArgumentSetsModel !== null && fieldEffectArgumentSet !== undefined && fieldEffectArgumentSet !== null)
            {
                effectArgumentSetsModel.itemSet.disconnect(effectArgumentSetSet)
            }
            fieldEffectArgumentSet = undefined
            effectArgumentSetsModel = undefined
            effectArgumentSetsModelIndex = -1
        }
    }

    EffectArgumentValueEditDialog {
        id : effectArgumentValueEditDialog

        property var effectArgumentValuesModel : undefined
        property int effectArgumentValuesModelIndex : -1


        onOpened: {
            effectArgumentValuesModel = effectArgumentSetValuesCurrentModel
            effectArgumentValuesModelIndex = effectArgumentSetValuesCurrentIndex
            init()
        }

        onAccepted: {
            if(effectArgumentValuesModel !== undefined && effectArgumentValuesModel !== null)
            {
                update()
                if(fieldNew)
                {
                    effectArgumentValuesModel.itemAdded.connect(effectArgumentValueAdded)
                    effectArgumentValuesModel.addItem(fieldEffectArgumentValue)
                }
                else
                {
                    effectArgumentValuesModel.itemSet.connect(effectArgumentValueSet)
                    effectArgumentValuesModel.setItem(effectArgumentValuesModelIndex, fieldEffectArgumentValue)
                }
            }
        }

        onRejected: {
            fieldEffectArgumentValue = undefined
            effectArgumentValuesModel = undefined
            effectArgumentValuesModelIndex = -1
        }

        function effectArgumentValueAdded()
        {
            if(effectArgumentValuesModel !== undefined && effectArgumentValuesModel !== null)
            {
                effectArgumentValuesModel.itemAdded.disconnect(effectArgumentValueAdded)
                effectArgumentSetValuesCurrentIndex = effectArgumentValuesModel.indexOfItem(fieldEffectArgumentValue)
            }
            fieldEffectArgumentValue = undefined
            effectArgumentValuesModel = undefined
            effectArgumentValuesModelIndex = -1
        }

        function effectArgumentValueSet()
        {
            if(effectArgumentValuesModel !== undefined && effectArgumentValuesModel !== null)
            {
                effectArgumentValuesModel.itemSet.disconnect(effectArgumentValueSet)
            }
            fieldEffectArgumentValue = undefined
            effectArgumentValuesModel = undefined
            effectArgumentValuesModelIndex = -1
        }
    }

    ChooseEffectArgumentDialog {
        id: chooseEffectArgumentDialog

        property var fieldEffectArgumentValue: undefined
        property var fieldEffectArgumentSet: undefined

        onOpened: {
            fieldEffectArg = undefined
            init()
        }

        onAccepted: {
            if(fieldEffectArg !== undefined && fieldEffectArg !== null)
            {
                if(effectArgumentValueEditDialog.fieldNew && fieldEffectArgumentValue !== undefined && fieldEffectArgumentValue !== null && fieldEffectArgumentSet !==undefined && fieldEffectArgumentSet !== null)
                {
                    fieldEffectArgumentValue.setArgSetId(fieldEffectArgumentSet.effectArgSetId)
                    console.log("fieldEffectArg = ", fieldEffectArg)
                    effectArgumentValueEditDialog.fieldEffectArg = fieldEffectArg
                    effectArgumentValueEditDialog.fieldEffectArgumentSet = fieldEffectArgumentSet
                    effectArgumentValueEditDialog.fieldEffectArgumentValue = fieldEffectArgumentValue
                    effectArgumentValueEditDialog.open()
                }
            }
            fieldEffectArgumentValue = undefined
            fieldEffectArg = undefined
        }

        onRejected: {
            fieldEffectArgumentValue = undefined
            fieldEffectArg = undefined
        }

        function argReloaded()
        {
            console.log("argReloaded()")
            if(fieldEffectArg !== undefined && fieldEffectArg !== null)
            {
                if(effectArgumentValueEditDialog.fieldNew && fieldEffectArgumentValue !== undefined && fieldEffectArgumentValue !== null && fieldEffectArgumentSet !==undefined && fieldEffectArgumentSet !== null)
                {
                    fieldEffectArgumentValue.effectArgValueArg.listReloaded.disconnect(argReloaded)
                }
            }
        }
    }

    ChooseImageDialog {
        id: chooseImageDialog

        signal imageChoosed()

        onOpened: {
            fieldImage = undefined
            init()
        }

        onAccepted: {
            imageChoosed()
            fieldImage = undefined
        }

        onRejected: {
            fieldImage = undefined
            imageChoosed()
        }
    }

    ImagePointEffectEditDialog {
        id: imagePointEffectEditDialog

        property var fieldImagePointEffect: undefined
        property var fieldImagePointEffectModel: undefined
        property var fieldImagePoint: undefined

        onOpened: {
            effectOldIndex = -1
            effectArgumentSetsOldIndex = -1
            if(fieldNew)
            {
                effectId = -1
                effectArgumentSetsId = -1
            }
            else
            {
                effectId = fieldImagePointEffect.effectId()
                effectArgumentSetsId = fieldImagePointEffect.argSetId()
            }
            init()
        }

        onAccepted: {
            if(!isValid())
            {
                clear()
                return
            }
            update()
            setupImagePointEffect()
            if(fieldNew)
            {
                fieldImagePointEffectModel.itemAdded.connect(itemAdded)
                fieldImagePointEffectModel.addItem(fieldImagePointEffect)
            }
            else
            {
                var index = fieldImagePointEffectModel.indexOfItem(fieldImagePointEffect)
                if(index >= 0)
                {
                    fieldImagePointEffectModel.itemSet.connect(itemSet)
                    fieldImagePointEffectModel.setItem(index, fieldImagePointEffect)
                }
                else
                {
                    clear()
                }
            }
        }

        onRejected: {
            reject()
            clear()
        }

        function isValid()
        {
            return !(fieldImagePointEffect === undefined || fieldImagePointEffect === null || fieldImagePoint === undefined || fieldImagePoint == null || fieldImagePointEffectModel === undefined || fieldImagePointEffectModel === null)
        }

        function setupImagePointEffect()
        {
            if(!isValid())
            {
                return
            }

            fieldImagePointEffect.imagePointEffectImagePointId = fieldImagePoint.pointId
            fieldImagePointEffect.setEffectId(effectId)
            fieldImagePointEffect.setArgSetId(effectArgumentSetsId)
            fieldImagePointEffect.imagePointEffectStepIndex = 0
            fieldImagePointEffect.imagePointEffectDuration = fieldDuration
        }

        function itemAdded()
        {
            if(!isValid())
            {
                return
            }
            fieldImagePointEffectModel.itemAdded.disconnect(itemAdded)
            fieldImagePoint.ipEffect.loadList()
            clear()
        }

        function itemSet()
        {
            if(!isValid())
            {
                return
            }
            fieldImagePointEffectModel.itemSet.connect(itemSet)
            clear()
        }

        function clear()
        {
            fieldImagePointEffectModel = undefined
            fieldImagePointEffect = undefined
            fieldImagePoint = undefined
            fieldNew = false
            clearDialogData()
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

    Action {
        id: refreshNextImageEffect
        text: qsTr("&Refresh next image effects")
        onTriggered: {
            if(imageOfGalleryNextImageEffectModel !== undefined && imageOfGalleryNextImageEffectModel !== null)
            {
                imageOfGalleryNextImageEffectCurrentIndex = -1
                imageOfGalleryNextImageEffectModel.listReloaded.connect(listReloaded)
                imageOfGalleryNextImageEffectModel.loadList()
            }
        }

        function listReloaded()
        {
            if(imageOfGalleryNextImageEffectModel !== undefined && imageOfGalleryNextImageEffectModel !== null)
            {
                imageOfGalleryNextImageEffectModel.listReloaded.disconnect(listReloaded)
                imageOfGalleryNextImageEffectCurrentIndex = imageOfGalleryNextImageEffectModel.currentIndex
            }
        }
    }

    Action {
        id: addNextImageEffect
        text: qsTr("&Add next image effects")
        onTriggered: {
            if(imageOfGalleryNextImageEffectModel !== undefined && imageOfGalleryNextImageEffectModel !== null)
            {
                var pointsModel = images_of_gallery.model.currentImagePoints()
                var pointIndex = imageOfGalleryPointIndex >= 0 ? imageOfGalleryPointIndex : images_of_gallery.model.currentImagePoints().getSize() - 1

                imagePointEffectEditDialog.fieldNew = true
                imagePointEffectEditDialog.fieldImagePoint = pointsModel.itemAt(pointIndex)
                if(imagePointEffectEditDialog.fieldImagePoint == null)
                {
                    return
                }
                var imagePointEffectModel = imagePointEffectEditDialog.fieldImagePoint.ipEffect
                imagePointEffectEditDialog.fieldImagePointEffectModel = imagePointEffectModel
                imagePointEffectEditDialog.fieldImagePointEffect = imagePointEffectModel.createItem()
                imagePointEffectEditDialog.fieldDuration = 1000
                imagePointEffectEditDialog.open()
            }
        }
    }

    Action {
        id: editNextImageEffect
        text: qsTr("&Edit next image effects")
        onTriggered: {
            if(imageOfGalleryNextImageEffectModel !== undefined && imageOfGalleryNextImageEffectModel !== null && imageOfGalleryNextImageEffectCurrentIndex >= 0)
            {
                var pointsModel = images_of_gallery.model.currentImagePoints()
                var pointIndex = imageOfGalleryPointIndex >= 0 ? imageOfGalleryPointIndex : images_of_gallery.model.currentImagePoints().getSize() - 1

                imagePointEffectEditDialog.fieldNew = false
                imagePointEffectEditDialog.fieldImagePoint = pointsModel.itemAt(pointIndex)
                if(imagePointEffectEditDialog.fieldImagePoint == null)
                {
                    return
                }
                var imagePointEffectModel = imagePointEffectEditDialog.fieldImagePoint.ipEffect
                imagePointEffectEditDialog.fieldImagePointEffectModel = imagePointEffectModel
                imagePointEffectEditDialog.fieldImagePointEffect = imagePointEffectModel.itemAt(imageOfGalleryNextImageEffectCurrentIndex)
                imagePointEffectEditDialog.fieldDuration = imagePointEffectEditDialog.fieldImagePointEffect.imagePointEffectDuration
                imagePointEffectEditDialog.open()
            }
        }
    }

    Action {
        id: removeNextImageEffect
        text: qsTr("Remove next image effects")
        onTriggered: {
            if(imageOfGalleryNextImageEffectModel !== undefined && imageOfGalleryNextImageEffectModel !== null && imageOfGalleryNextImageEffectCurrentIndex >= 0)
            {
                imageOfGalleryNextImageEffectModel.itemDeleted.connect(itemDeleted)
                imageOfGalleryNextImageEffectModel.delItem(imageOfGalleryNextImageEffectCurrentIndex)
            }
        }

        function itemDeleted()
        {
            if(imageOfGalleryNextImageEffectModel !== undefined && imageOfGalleryNextImageEffectModel !== null && imageOfGalleryNextImageEffectCurrentIndex >= 0)
            {
                imageOfGalleryNextImageEffectModel.itemDeleted.disconnect(itemDeleted)
                imageOfGalleryNextImageEffectCurrentIndex = -1
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
            MenuItem { action: refreshNextImageEffect }
            MenuItem { action: addNextImageEffect }
            MenuItem { action: editNextImageEffect }
            MenuItem { action: removeNextImageEffect }
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
        id: refreshEffect
        text: qsTr("Re&fresh effects")
        onTriggered: {
            effectCurrentIndex = -1
            effectsListBusyIndicator.visible = true
            effectsListBusyIndicator.running = true
            effectModel.listReloaded.connect(effectModelListReloaded)
            effectModel.loadList()
        }

        function effectModelListReloaded()
        {
            effectsListBusyIndicator.visible = false
            effectsListBusyIndicator.running = false
            effectCurrentIndex = effectModel.currentIndex
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
            if(effectCurrentIndex >= 0)
            {
                effectEditDialog.fieldEffect = effectModel.getCurrentItem()
                effectEditDialog.fieldNewItem = false
                effectEditDialog.open()
            }
        }
    }

    Action {
        id: removeEffect
        text: qsTr("&Remove effect")
        onTriggered: {
            if(effectCurrentIndex >= 0)
            {
                effectModel.delItem(effectCurrentIndex)
            }
        }
    }

    Action {
        id: refreshShaders
        text: qsTr("Re&fresh shaders")
        onTriggered: {
            //shaderEditDialog
            if(effectShadersCurrentModel !== undefined && effectShadersCurrentModel !== null)
            {
                effectShaderCurrentIndex = -1
                effectShadersCurrentModel.listReloaded.connect(effectShadersCurrentModelListReloaded)
                effectShadersCurrentModel.loadList()
            }
        }

        function effectShadersCurrentModelListReloaded()
        {
            if(effectShadersCurrentModel !== undefined && effectShadersCurrentModel !== null)
            {
                effectShadersCurrentModel.listReloaded.disconnect(effectShadersCurrentModelListReloaded)
                effectShaderCurrentIndex = effectShadersCurrentModel.currentIndex
            }
        }
    }

    Action {
        id: addNewShader
        text: qsTr("&Add new shader")
        onTriggered: {
            if(effectShadersCurrentModel !== undefined && effectShadersCurrentModel !== null)
            {
                var newEffectShader = effectShadersCurrentModel.createItem()
                var newShader = newEffectShader.effectShaderShader.createItem()

                shaderEditDialog.fieldEffectShader = newEffectShader
                shaderEditDialog.fieldShader = newShader
                shaderEditDialog.fieldNewItem = true
                shaderEditDialog.open()
            }
        }
    }

    Action {
        id: addExistingShader
        text: qsTr("Add &existing shader")
        onTriggered: {
            if(effectShadersCurrentModel !== undefined && effectShadersCurrentModel !== null)
            {
                chooseShaderDialog.open()
            }
        }
    }

    Action {
        id: editShaderInfo
        text: qsTr("Edit shader &info")
        onTriggered: {
            if(effectShadersCurrentModel !== undefined && effectShadersCurrentModel !== null)
            {
                if(effectShadersCurrentModel.effectShaders.size() > 0 && effectShaderCurrentIndex >= 0)
                {
                    var effectShader = effectShadersCurrentModel
                    var shader = effectShadersCurrentModel.effectShaders.getCurrentItem()
                    shaderEditDialog.fieldEffectShader = effectShader
                    shaderEditDialog.fieldShader = shader
                    shaderEditDialog.fieldNewItem = false
                    shaderEditDialog.open()
                }
            }
        }
    }

    Action {
        id: removeShader
        text: qsTr("&Remove shader")
        onTriggered: {
            if(effectShaderCurrentIndex >= 0 && effectShadersCurrentModel !== undefined && effectShadersCurrentModel !== null)
            {
                effectShadersCurrentModel.delItem(effectShaderCurrentIndex)
            }
        }
    }

    Action {
        id: refreshEffectArguments
        text: qsTr("&Refresh effect arguments")
        onTriggered: {
            if(effectModel.currentIndex >= 0 && effectModel.currentItem !== undefined && effectModel.currentItem !== null)
            {
                refreshEffectsArgumentsDialog.fieldEffect = effectModel.currentItem
                refreshEffectsArgumentsDialog.connect()
                refreshEffectsArgumentsDialog.init()
                if(refreshEffectsArgumentsDialog.fieldEffect.startRefreshArguments())
                {
                    refreshEffectsArgumentsDialog.open()
                }
            }
        }
    }

    Action {
        id: refreshArgumentSet
        text: qsTr("Re&fresh argument set")
        onTriggered: {
            if(effectArgumentSetsCurrentModel !== undefined && effectArgumentSetsCurrentModel !== null)
            {
                effectArgumentSetsCurrentIndex = -1
                effectArgumentSetsListBusyIndicator.visible = true
                effectArgumentSetsListBusyIndicator.running = true
                effectArgumentSetsCurrentModel.listReloaded.connect(listReloaded)
                effectArgumentSetsCurrentModel.loadList()
            }
        }

        function listReloaded()
        {
            if(effectArgumentSetsCurrentModel !== undefined && effectArgumentSetsCurrentModel !== null)
            {
                effectArgumentSetsCurrentModel.listReloaded.disconnect(listReloaded)
                effectArgumentSetsListBusyIndicator.visible = false
                effectArgumentSetsListBusyIndicator.running = false
                effectArgumentSetsCurrentIndex = effectArgumentSetsCurrentModel.currentIndex
            }
        }
    }

    Action {
        id: addArgumentSet
        text: qsTr("&Add argument set")
        onTriggered: {
            if(effectArgumentSetsCurrentModel !== undefined && effectArgumentSetsCurrentModel !== null)
            {
                effectArgumentSetEditDialog.fieldNew = true
                effectArgumentSetEditDialog.fieldEffectArgumentSet = effectArgumentSetsCurrentModel.createItem()
                effectArgumentSetEditDialog.fieldEffectArgumentSet.effectArgSetEffectId = effectModel.getCurrentItem().effectId
                effectArgumentSetEditDialog.open()
            }
        }
    }

    Action {
        id: editArgumentSet
        text: qsTr("Edit argument &set")
        onTriggered: {
            if(effectArgumentSetsCurrentModel !== undefined && effectArgumentSetsCurrentModel !== null && effectArgumentSetsCurrentIndex >= 0)
            {
                effectArgumentSetEditDialog.fieldNew = false
                effectArgumentSetEditDialog.fieldEffectArgumentSet = effectArgumentSetsCurrentModel.itemAt(effectArgumentSetsCurrentIndex)
                effectArgumentSetEditDialog.open()
            }
        }
    }

    Action {
        id: removeArgumentSet
        text: qsTr("Remove argument set")
        onTriggered: {
            if(effectArgumentSetsCurrentModel !== undefined && effectArgumentSetsCurrentModel !== null && effectArgumentSetsCurrentIndex >= 0)
            {
                effectArgumentSetsCurrentModel.delItem(effectArgumentSetsCurrentIndex)
            }
        }
    }

    Action {
        id: refreshArgumentOfArgumentSet
        text: qsTr("Refresh argument of argument set")
        onTriggered: {
            if(effectArgumentSetValuesCurrentModel !== undefined && effectArgumentSetValuesCurrentModel !== null)
            {
                effectArgumentSetValuesCurrentIndex = -1
                effectArgumentSetValuesList.model = 0
                effectArgumentSetValuesCurrentModel.listReloaded.connect(listReloaded)
                effectArgumentSetValuesCurrentModel.loadList()
            }
        }

        function listReloaded()
        {
            if(effectArgumentSetValuesCurrentModel !== undefined && effectArgumentSetValuesCurrentModel !== null)
            {
                effectArgumentSetValuesCurrentModel.listReloaded.disconnect(listReloaded)
                effectArgumentSetValuesList.model = effectArgumentSetValuesCurrentModel
                effectArgumentSetValuesCurrentIndex = effectArgumentSetValuesCurrentModel.currentIndex
            }
        }
    }

    Action {
        id: addArgumentOfArgumentSet
        text: qsTr("Add a&rgument of argument set")
        onTriggered: {
            if(effectArgumentSetsCurrentModel !== undefined && effectArgumentSetsCurrentModel !== null && effectArgumentSetsCurrentIndex >= 0 && effectArgumentSetValuesCurrentModel !== undefined && effectArgumentSetValuesCurrentModel !== null && effectCurrentIndex >= 0)
            {
                effectArgumentValueEditDialog.fieldNew = true
                var effectArgumentSetItem = effectArgumentSetsCurrentModel.itemAt(effectArgumentSetsCurrentIndex)
                var effectArgumentValueItem = effectArgumentSetValuesCurrentModel.createItem()
                chooseEffectArgumentDialog.fieldEffectArgumentSet = effectArgumentSetItem
                chooseEffectArgumentDialog.fieldEffectArgumentValue = effectArgumentValueItem
                chooseEffectArgumentDialog.fieldExceptIds = effectArgumentSetItem.argValuesOfSetIdList()
                chooseEffectArgumentDialog.effectArgModel = effectModel.currentItem.effectArgs
                chooseEffectArgumentDialog.open()
            }
        }
    }

    Action {
        id: editArgumentOfArgumentSet
        text: qsTr("&Edit argument of argument set")
        onTriggered: {
            if(effectArgumentSetsCurrentModel !== undefined && effectArgumentSetsCurrentModel !== null && effectArgumentSetsCurrentIndex >= 0 && effectArgumentSetValuesCurrentModel !== undefined && effectArgumentSetValuesCurrentModel !== null && effectArgumentSetValuesCurrentIndex >= 0)
            {
                effectArgumentValueEditDialog.fieldNew = false
                effectArgumentValueEditDialog.fieldEffectArgumentSet = effectArgumentSetsCurrentModel.itemAt(effectArgumentSetsCurrentIndex)
                effectArgumentValueEditDialog.fieldEffectArgumentValue = effectArgumentSetValuesCurrentModel.itemAt(effectArgumentSetValuesCurrentIndex)
                effectArgumentValueEditDialog.open()
            }
        }
    }

    Action {
        id: removeArgumentOfArgumentSet
        text: qsTr("Remove argument of argument set")
        onTriggered: {
            if(effectArgumentSetValuesCurrentModel !== undefined && effectArgumentSetValuesCurrentModel !== null && effectArgumentSetValuesCurrentIndex >= 0)
            {
                effectArgumentSetValuesCurrentModel.delItem(effectArgumentSetValuesCurrentIndex)
            }
        }
    }

    Action {
        id: demoSetImageFrom
        text: qsTr("Choose image to play demo &from it")
        onTriggered: {
            chooseImageDialog.imageChoosed.connect(imageChoosed)
            chooseImageDialog.open()
        }

        function imageChoosed()
        {
            chooseImageDialog.imageChoosed.disconnect(imageChoosed)
            if(chooseImageDialog.fieldImage !== undefined && chooseImageDialog.fieldImage !== null) {
                demoImageFrom = [chooseImageDialog.fieldImage.imageSource, chooseImageDialog.fieldImage.imageHash]
            }
        }
    }

    Action {
        id: demoSetImageTo
        text: qsTr("Choose image to play demo &to it")
        onTriggered: {
            chooseImageDialog.imageChoosed.connect(imageChoosed)
            chooseImageDialog.open()
        }
        function imageChoosed()
        {
            chooseImageDialog.imageChoosed.disconnect(imageChoosed)
            if(chooseImageDialog.fieldImage !== undefined && chooseImageDialog.fieldImage !== null) {
                demoImageTo = [chooseImageDialog.fieldImage.imageSource, chooseImageDialog.fieldImage.imageHash]
            }
        }
    }

    Action {
        id: playEffectDemo
        text: qsTr("&Play effect demo")
        onTriggered: {
            if(effectModel.getCurrentItem() !== undefined && effectModel.getCurrentItem() !== null && effectArgumentSetsCurrentModel !== undefined && effectArgumentSetsCurrentModel !== null && effectArgumentSetsCurrentIndex >= 0)
            {
                var effect = effectModel.getCurrentItem()
                var effectArgSet = effectArgumentSetsCurrentModel.itemAt(effectArgumentSetsCurrentIndex)
                effectImageDemo.effect = effect
                effectImageDemo.argumentSet = effectArgSet
                numberAnimationForward.easing.type = easingTypeModel.findItemById(effectArgSet.effectArgSetEasingId) !== null ? GalleryFunctions.easingTypeFromStr(easingTypeModel.findItemById(effectArgSet.effectArgSetEasingId).easingTypeType) : Easing.Linear
            }
            else
            {
                effectImageDemo.effect = null
                effectImageDemo.argumentSet = null
                numberAnimationForward.easing.type = Easing.Linear
            }
            effectImageDemo.updateEffects()
            effectDemoImageAnimationForward.running = true
        }
    }

    Action {
        id: playCurrentEffectDemo
        text: qsTr("Play current effect demo")
        onTriggered: {
            effectDemoImageAnimationForward.running = true
        }
    }

    Action {
        id: reversePlayEffectDemo
        text: qsTr("&Play reverse effect demo")
        onTriggered: {
            if(effectModel.getCurrentItem() !== undefined && effectModel.getCurrentItem() !== null && effectArgumentSetsCurrentModel !== undefined && effectArgumentSetsCurrentModel !== null && effectArgumentSetsCurrentIndex >= 0)
            {
                var effect = effectModel.getCurrentItem()
                var effectArgSet = effectArgumentSetsCurrentModel.itemAt(effectArgumentSetsCurrentIndex)
                effectImageDemo.effect = effect
                effectImageDemo.argumentSet = effectArgSet
                numberAnimationBackward.easing.type = easingTypeModel.findItemById(effectArgSet.effectArgSetEasingId) !== null ? GalleryFunctions.easingTypeFromStr(easingTypeModel.findItemById(effectArgSet.effectArgSetEasingId).easingTypeType) : Easing.Linear
            }
            else
            {
                effectImageDemo.effect = null
                effectImageDemo.argumentSet = null
                numberAnimationBackward.easing.type = Easing.Linear
            }
            effectImageDemo.updateEffects()
            effectDemoImageAnimationBackward.running = true
        }
    }

    Action {
        id: reverseCurrentEffectDemo
        text: qsTr("Play reverse current effect demo")
        onTriggered: {
            effectDemoImageAnimationBackward.running = true
        }
    }

    Action {
        id: defaultEffectDemo
        text: qsTr("&Default effect demo")
        onTriggered: {
            effectDemoImageAnimationForward.running = false
            effectDemoImageAnimationBackward.running = false
            effectImageDemo.effect = null
            effectImageDemo.argumentSet = null
            effectImageDemo.t = 0.5
            numberAnimationBackward.easing.type = Easing.Linear
            effectImageDemo.updateEffects()
        }
    }

    Action {
        id: stopEffectDemo
        text: qsTr("&Stop effect demo")
        onTriggered: {
            effectDemoImageAnimationForward.running = false
            effectDemoImageAnimationBackward.running = false
        }
    }

    function testEffectDemo(t)
    {
        effectDemoImageAnimationBackward.running = false
        effectDemoImageAnimationBackward.running = false
        effectImageDemo.t = t
    }

    Action {
        id: testEffect00Demo
        text: qsTr("Test current effect with t = 0.&0")
        onTriggered: {
            testEffectDemo(0.0)
        }
    }

    Action {
        id: testEffect01Demo
        text: qsTr("Test current effect with t = 0.&1")
        onTriggered: {
            testEffectDemo(0.1)
        }
    }

    Action {
        id: testEffect02Demo
        text: qsTr("Test current effect with t = 0.&2")
        onTriggered: {
            testEffectDemo(0.2)
        }
    }

    Action {
        id: testEffect03Demo
        text: qsTr("Test current effect with t = 0.&3")
        onTriggered: {
            testEffectDemo(0.3)
        }
    }

    Action {
        id: testEffect04Demo
        text: qsTr("Test current effect with t = 0.&4")
        onTriggered: {
            testEffectDemo(0.4)
        }
    }

    Action {
        id: testEffect05Demo
        text: qsTr("Test current effect with t = 0.&5")
        onTriggered: {
            testEffectDemo(0.5)
        }
    }

    Action {
        id: testEffect06Demo
        text: qsTr("Test current effect with t = 0.&6")
        onTriggered: {
            testEffectDemo(0.6)
        }
    }

    Action {
        id: testEffect07Demo
        text: qsTr("Test current effect with t = 0.&7")
        onTriggered: {
            testEffectDemo(0.7)
        }
    }

    Action {
        id: testEffect08Demo
        text: qsTr("Test current effect with t = 0.&8")
        onTriggered: {
            testEffectDemo(0.8)
        }
    }

    Action {
        id: testEffect09Demo
        text: qsTr("Test current effect with t = 0.&9")
        onTriggered: {
            testEffectDemo(0.9)
        }
    }

    Action {
        id: testEffect10Demo
        text: qsTr("Test current effect with t = 1.0")
        onTriggered: {
            testEffectDemo(1.0)
        }
    }

    MenuBar {
        id: effectsMenuBar
        AutoSizeMenu {
            title: qsTr("&Effects")
            MenuItem { action: refreshEffect }
            MenuItem { action: addEffect }
            MenuItem { action: editEffect }
            MenuItem { action: removeEffect }
        }
        AutoSizeMenu {
            title: qsTr("&Shaders")
            MenuItem { action: refreshShaders }
            MenuItem { action: addNewShader }
            MenuItem { action: addExistingShader }
            MenuItem { action: editShaderInfo }
            MenuItem { action: removeShader }
        }
        AutoSizeMenu {
            title: qsTr("&Arguments")
            MenuItem { action: refreshEffectArguments }
        }
        AutoSizeMenu {
            title: qsTr("Argument Se&ts")
            MenuItem { action: refreshArgumentSet }
            MenuItem { action: refreshArgumentOfArgumentSet }
            MenuItem { action: addArgumentSet }
            MenuItem { action: editArgumentSet }
            MenuItem { action: addArgumentOfArgumentSet }
            MenuItem { action: editArgumentOfArgumentSet }
            MenuItem { action: removeArgumentSet }
            MenuItem { action: removeArgumentOfArgumentSet }
        }
        AutoSizeMenu {
            title: qsTr("&Demo")
            MenuItem { action: demoSetImageFrom }
            MenuItem { action: demoSetImageTo }
            MenuItem { action: playEffectDemo }
            MenuItem { action: reversePlayEffectDemo }
            MenuItem { action: playCurrentEffectDemo }
            MenuItem { action: reverseCurrentEffectDemo }
            MenuItem { action: stopEffectDemo }
            MenuItem { action: defaultEffectDemo }
            MenuItem { action: testEffect00Demo }
            MenuItem { action: testEffect01Demo }
            MenuItem { action: testEffect02Demo }
            MenuItem { action: testEffect03Demo }
            MenuItem { action: testEffect04Demo }
            MenuItem { action: testEffect05Demo }
            MenuItem { action: testEffect06Demo }
            MenuItem { action: testEffect07Demo }
            MenuItem { action: testEffect08Demo }
            MenuItem { action: testEffect09Demo }
            MenuItem { action: testEffect10Demo }
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
                                            id: imageOfGalleryNextImageTabNextImage
                                            anchors.fill: parent
                                            visible: imageOfGalleryPointIndex >= 0

                                            Row {
                                                Column{
                                                    Text {
                                                        id: imageOfGalleryNextImageText
                                                        visible: imageOfGalleryPointIndex >= 0
                                                        width: (slitViewPaneImageInfo.height - imageOfGalleryInfoBar.height) * Constants.aspectX / Constants.aspectY
                                                        text: Constants.notANextImagePoint
                                                        wrapMode: Text.WordWrap
                                                    }

                                                    Image {
                                                        id: imageOfGalleryNextImageNextImage
                                                        height: slitViewPaneImageInfo.height - imageOfGalleryInfoBar.height - imageOfGalleryNextImageText.height
                                                        width: (slitViewPaneImageInfo.height - imageOfGalleryInfoBar.height) * Constants.aspectX / Constants.aspectY
                                                        visible: imageOfGalleryPointIndex >= 0
                                                        fillMode: Image.PreserveAspectFit
                                                        source: Constants.noImage
                                                    }
                                                }

                                                Rectangle {
                                                    id: imageOfGalleryNextImageEffectRect
                                                    visible: imageOfGalleryPointIndex >= 0
                                                    width: imageOfGalleryNextImageTabNextImage.width - imageOfGalleryNextImageNextImage.width
                                                    height: imageOfGalleryNextImageTabNextImage.height

                                                    ListView {
                                                        id: imageOfGalleryNextImageEffectList
                                                        anchors.fill: parent
                                                        visible: imageOfGalleryPointIndex >= 0
                                                        clip: true
                                                        model: 0
                                                        delegate: imageOfGalleryNextImageEffectListItem
                                                        highlight: imageOfGalleryNextImageEffectListItemHighlight
                                                        highlightFollowsCurrentItem: false
                                                    }
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
                        SplitView.minimumWidth: Constants.leftSideBarWidth / 2
                        SplitView.maximumWidth: Constants.leftSideBarWidth * 2
                        height: parent.height

                        ListView {
                            id: effectsList

                            anchors.fill: parent
                            spacing: Constants.effectsListViewSpacing
                            clip: true
                            model: effectModel
                            delegate: effectItem
                            highlight: effectItemHighlight
                            highlightFollowsCurrentItem: false
                            z: 0.0

                            BusyIndicator {
                                id: effectsListBusyIndicator
                                anchors.centerIn: parent
                                visible: false
                                running: false
                                z: 1.0
                            }
                        }
                    }
                    Rectangle{
                        id: splitEffectsInfo

                        SplitView.preferredWidth: parent.width - (splitEffects.width + Constants.leftSideBarWidth)
                        SplitView.minimumWidth: Constants.leftSideBarWidth / 2
                        SplitView.maximumWidth: parent.width - (splitEffects.width + Constants.leftSideBarWidth / 2)
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
                                Column {
                                    Label {
                                        text: qsTr("Effect name : ")
                                        padding: Constants.effectInfoPadding
                                    }
                                    Text
                                    {
                                        id: effectInfoCommonName
                                        width: splitEffectsInfo.width
                                        text: qsTr("")
                                        wrapMode: Text.WordWrap
                                        padding: Constants.effectInfoPadding
                                    }
                                    Label {
                                        text: qsTr("Effect description : ")
                                        padding: Constants.effectInfoPadding
                                    }
                                    Text
                                    {
                                        id: effectInfoCommonDescription
                                        width: splitEffectsInfo.width
                                        text: qsTr("")
                                        wrapMode: Text.WordWrap
                                        padding: Constants.effectInfoPadding
                                    }
                                }
                            }
                            Item {
                                id: effectInfoShaders
                                ListView {
                                    id: effectShadersList

                                    anchors.fill: parent
                                    spacing: Constants.effectsListViewSpacing
                                    clip: true
                                    model: 0
                                    delegate: effectShaderItem
                                    highlight: effectShaderItemHighlight
                                    highlightFollowsCurrentItem: false
                                    z: 0.0

                                    BusyIndicator {
                                        id: effectShadersListBusyIndicator
                                        anchors.centerIn: parent
                                        visible: false
                                        running: false
                                        z: 1.0
                                    }
                                }
                            }
                            Item {
                                id: effectInfoArguments
                                ListView {
                                    id: effectArgumentsList

                                    anchors.fill: parent
                                    spacing: Constants.effectsListViewSpacing
                                    clip: true
                                    model: 0
                                    delegate: effectArgumentsItem
                                    highlight: effectArgumentsItemHighlight
                                    highlightFollowsCurrentItem: false
                                    z: 0.0

                                    BusyIndicator {
                                        id: effectArgumentsListBusyIndicator
                                        anchors.centerIn: parent
                                        visible: false
                                        running: false
                                        z: 1.0
                                    }
                                }
                            }
                            Item {
                                id: effectInfoArgumentSets

                                SplitView {
                                    id: slitEffectArgumentSetsAndEffectArgumentValues
                                    anchors.fill: parent
                                    orientation: Qt.Vertical

                                    Rectangle {
                                        width: effectInfoArgumentSets.width
                                        SplitView.minimumHeight: effectInfoArgumentSets.height / 4
                                        SplitView.maximumHeight: effectInfoArgumentSets.height * 3 / 4
                                        SplitView.preferredHeight: effectInfoArgumentSets.height / 2
                                        ListView {
                                            id: effectArgumentSetsList

                                            anchors.fill: parent
                                            spacing: Constants.effectsListViewSpacing
                                            clip: true
                                            model: 0
                                            delegate: effectArgumentSetsItem
                                            highlight: effectArgumentSetsItemHighlight
                                            highlightFollowsCurrentItem: false
                                            z: 0.0

                                            BusyIndicator {
                                                id: effectArgumentSetsListBusyIndicator
                                                anchors.centerIn: parent
                                                visible: false
                                                running: false
                                                z: 1.0
                                            }
                                        }
                                    }
                                    Rectangle {
                                        width: effectInfoArgumentSets.width
                                        SplitView.minimumHeight: effectInfoArgumentSets.height / 4
                                        SplitView.maximumHeight: effectInfoArgumentSets.height * 3 / 4
                                        SplitView.preferredHeight: effectInfoArgumentSets.height/2
                                        ListView {
                                            id: effectArgumentSetValuesList

                                            anchors.fill: parent
                                            spacing: Constants.effectsListViewSpacing
                                            clip: true
                                            model: 0
                                            delegate: effectArgumentSetValuesItem
                                            highlight: effectArgumentSetValuesItemHighlight
                                            highlightFollowsCurrentItem: false
                                        }
                                    }
                                }
                            }
                        }
                    }
                    Rectangle{
                        id: splitEffectsDemo
                        height: parent.height

                        property var effectDemoImageAnimationDuration: 3000
                        ScrollView {
                            anchors.fill: parent
                            clip:true

                            Column {
                                Rectangle {
                                    id: effectImageDemoClipper
                                    width: splitEffectsDemo.width
                                    height: splitEffectsDemo.width * Constants.aspectY / Constants.aspectX
                                    color: "transparent"
                                    clip: true

                                    QuizImage {
                                        id: effectImageDemo
                                        width: splitEffectsDemo.width
                                        height: splitEffectsDemo.width * Constants.aspectY / Constants.aspectX
                                        visible: true
                                        clip: true
                                        fromImage: [Constants.noImage, Constants.noImageHash]
                                        toImage: [Constants.noImage, Constants.noImageHash]
                                        t: 0.5

                                        SequentialAnimation {
                                            id: effectDemoImageAnimationForward
                                            running: false

                                            NumberAnimation { id: numberAnimationForward; target: effectImageDemo; property: "t"; from: 0.0; to: 1.0; duration: splitEffectsDemo.effectDemoImageAnimationDuration; easing.type: Easing.Linear }
                                        }

                                        SequentialAnimation {
                                            id: effectDemoImageAnimationBackward
                                            running: false

                                            NumberAnimation { id: numberAnimationBackward;  target: effectImageDemo; property: "t"; from: 1.0; to: 0.0; duration: splitEffectsDemo.effectDemoImageAnimationDuration; easing.type: Easing.Linear }
                                        }

                                        Connections {
                                            target: effectDemoImageAnimationForward

                                            function onFinished()
                                            {
                                                console.log("effectDemoImageAnimationForward.finished()");
                                            }
                                        }

                                        Connections {
                                            target: effectDemoImageAnimationBackward

                                            function onFinished()
                                            {
                                                console.log("effectDemoImageAnimationBackward.finished()");
                                            }
                                        }
                                    }
                                }

                                Image {
                                    id: effectDemoImageFrom
                                    width: splitEffectsDemo.width
                                    height: splitEffectsDemo.width * Constants.aspectY / Constants.aspectX
                                    fillMode: Image.PreserveAspectFit
                                    source: Constants.noImage
                                }

                                Image {
                                    id: effectDemoImageTo
                                    width: splitEffectsDemo.width
                                    height: splitEffectsDemo.width * Constants.aspectY / Constants.aspectX
                                    fillMode: Image.PreserveAspectFit
                                    source: Constants.noImage
                                }
                            }
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
        Column {
            id: effectItemRect

            FontMetrics{
                id: effectItemFontMetrics
                font: effectItemHeaderText.font
            }

            width: effectsList.width

            property bool showFullDescription: false

            Text {
                id: effectItemHeaderText
                width: effectsList.width
                wrapMode: Text.WordWrap
                text: effectName
                padding: Constants.effectsListHeaderPadding

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
                        showFullDescription = !showFullDescription
                    }

                    AutoSizeMenu {
                        id: effectItemMenu
                        MenuItem { action: refreshEffect }
                        MenuItem { action: addEffect }
                        MenuItem { action: editEffect }
                        MenuItem { action: removeEffect }
                    }
                }
            }

            Text {
                id: effectItemDescriptionText
                width: effectsList.width
                wrapMode: Text.WordWrap
                text: showFullDescription ? mastactva.leftDoubleCR(effectDescription) : mastactva.readMore(effectDescription, Constants.effectsListReadMoreLength, qsTr(" ..."))

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
                        showFullDescription = !showFullDescription
                    }
                }
            }
        }
    }

    Component {
        id: effectItemHighlight

        Rectangle {
            SystemPalette {
                id: effectItemHighlightPallete
                colorGroup: SystemPalette.Active
            }

            border.color: effectItemHighlightPallete.highlight
            border.width: 2
            radius: 5
            y: (effectsList.currentItem !== undefined && effectsList.currentItem !== null) ? effectsList.currentItem.y : 0
            x: (effectsList.currentItem !== undefined && effectsList.currentItem !== null) ? effectsList.currentItem.x : 0
            width: (effectsList.currentItem !== undefined && effectsList.currentItem !== null) ? effectsList.currentItem.width : 0
            height: (effectsList.currentItem !== undefined && effectsList.currentItem !== null) ? effectsList.currentItem.height : 0
        }
    }

    Component {
        id: effectShaderItem

        MouseArea {
            width: childrenRect.width
            height: childrenRect.height

            acceptedButtons: Qt.LeftButton | Qt.RightButton

            property var shaderItem: effectShaderShader.currentItem
            property bool showFullDescription: false

            Connections {
                target: effectShaderShader

                function onListReloaded()
                {
                    shaderItem = effectShaderShader.currentItem
                }
            }

            onClicked:
            {
                if (mouse.button === Qt.RightButton)
                {
                    effectShaderItemMenu.popup()
                }
                else
                {
                    effectShaderCurrentIndex = index
                    mouse.accepted = false
                }
            }

            onPressAndHold: {
                if (mouse.source === Qt.MouseEventNotSynthesized)
                    effectShaderItemMenu.popup()
            }

            onDoubleClicked: {
                showFullDescription = !showFullDescription
            }

            AutoSizeMenu {
                id: effectShaderItemMenu
                MenuItem { action: refreshShaders }
                MenuItem { action: addNewShader }
                MenuItem { action: addExistingShader }
                MenuItem { action: editShaderInfo }
                MenuItem { action: removeShader }
            }

            Column {
                id: effectShaderItemRect
                width: effectShadersList.width

                FontMetrics{
                    id: effectShaderItemFontMetrics
                    font: effectShaderItemType.font
                }

                Row {
                    padding: Constants.effectShaderListHeaderPadding
                    Label {
                        id: effectShaderItemTypeLabel
                        text: qsTr("Type : ")
                    }
                    Text {
                        id: effectShaderItemType
                        width: effectShadersList.width - effectShaderItemTypeLabel.width
                        text: shaderItem !== undefined && shaderItem !== null  && shaderTypeModel.findItemById(shaderItem.shaderTypeId) !== null ? shaderTypeModel.findItemById(shaderItem.shaderTypeId).shaderTypeType : ""
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.effectShaderListHeaderPadding
                    Label {
                        id: effectShaderItemFilenameLabel
                        text: qsTr("Filename : ")
                    }

                    Text {
                        id: effectShaderItemFilename
                        width: effectShadersList.width - effectShaderItemFilenameLabel.width
                        text: shaderItem !== undefined && shaderItem !== null ? shaderItem.shaderFilename : ""
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.effectShaderListHeaderPadding
                    Label {
                        id: effectShaderItemHashLabel
                        text: qsTr("Hash : ")
                    }

                    Text {
                        id: effectShaderItemHash
                        width: effectShadersList.width - effectShaderItemHashLabel.width
                        text: shaderItem !== undefined && shaderItem !== null  ? shaderItem.shaderHash : ""
                        wrapMode: Text.Wrap
                    }
                }

                Text {
                    id: effectShaderItemDescriptionText
                    width: effectShadersList.width
                    wrapMode: Text.WordWrap
                    text: showFullDescription ? mastactva.leftDoubleCR(shaderItem !== undefined && shaderItem !== null  ? shaderItem.shaderDescription : "") : mastactva.readMore(shaderItem !== undefined && shaderItem !== null  ? shaderItem.shaderDescription : "", Constants.effectsListReadMoreLength, qsTr(" ..."))
                }
            }
        }
    }

    Component {
        id: effectShaderItemHighlight

        Rectangle {
            SystemPalette {
                id: effectShaderItemHighlightPallete
                colorGroup: SystemPalette.Active
            }

            border.color: effectShaderItemHighlightPallete.highlight
            border.width: 2
            radius: 5
            y: (effectShadersList.currentItem !== undefined && effectShadersList.currentItem !== null) ? effectShadersList.currentItem.y : 0
            x: (effectShadersList.currentItem !== undefined && effectShadersList.currentItem !== null) ? effectShadersList.currentItem.x : 0
            width: (effectShadersList.currentItem !== undefined && effectShadersList.currentItem !== null) ? effectShadersList.currentItem.width : 0
            height: (effectShadersList.currentItem !== undefined && effectShadersList.currentItem !== null) ? effectShadersList.currentItem.height : 0
        }
    }

    Component {
        id: effectArgumentsItem

        MouseArea {
            width: childrenRect.width
            height: childrenRect.height

            acceptedButtons: Qt.LeftButton | Qt.RightButton

            property bool showFullDescription: false

            onClicked:
            {
                if (mouse.button === Qt.RightButton)
                {
                    effectArgumentsItemMenu.popup()
                }
                else
                {
                    effectArgumentsCurrentIndex = index
                    mouse.accepted = false
                }
            }

            onPressAndHold: {
                if (mouse.source === Qt.MouseEventNotSynthesized)
                    effectArgumentsItemMenu.popup()
            }

            onDoubleClicked: {
                showFullDescription = !showFullDescription
            }

            AutoSizeMenu {
                id: effectArgumentsItemMenu
                MenuItem { action: refreshEffectArguments }
            }

            Column {
                id: effectArgumentsItemRect
                width: effectArgumentsList.width

                FontMetrics{
                    id: effectArgumentsItemFontMetrics
                    font: effectArgumentsItemType.font
                }

                Row {
                    padding: Constants.effectShaderListHeaderPadding
                    Label {
                        id: effectArgumentsItemTypeLabel
                        text: qsTr("Type : ")
                    }
                    Text {
                        id: effectArgumentsItemType
                        width: effectArgumentsList.width - effectArgumentsItemTypeLabel.width
                        text: shaderArgTypeModel.findItemById(effectArgArgTypeId) !== null ? shaderArgTypeModel.findItemById(effectArgArgTypeId).shaderArgTypeType : ""
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.effectShaderListHeaderPadding
                    Label {
                        id: effectArgumentsItemNameLabel
                        text: qsTr("Name : ")
                    }

                    Text {
                        id: effectArgumentsItemName
                        width: effectArgumentsList.width - effectArgumentsItemNameLabel.width
                        text: effectArgName
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.effectShaderListHeaderPadding
                    Label {
                        id: effectArgumentsItemDefaultValueLabel
                        text: qsTr("Default value : ")
                    }

                    Text {
                        id: effectArgumentsDefaultValue
                        width: effectArgumentsList.width - effectArgumentsItemDefaultValueLabel.width
                        text: effectArgDefaultValue
                        wrapMode: Text.Wrap
                    }
                }

                Text {
                    id: effectArgumentsItemDescriptionText
                    width: effectArgumentsList.width
                    wrapMode: Text.WordWrap
                    text: showFullDescription ? mastactva.leftDoubleCR(effectArgDescription) : mastactva.readMore(effectArgDescription, Constants.effectsListReadMoreLength, qsTr(" ..."))
                }
            }
        }
    }

    Component {
        id: effectArgumentsItemHighlight

        Rectangle {
            SystemPalette {
                id: effectArgumentsItemHighlightPallete
                colorGroup: SystemPalette.Active
            }

            border.color: effectArgumentsItemHighlightPallete.highlight
            border.width: 2
            radius: 5
            y: (effectArgumentsList.currentItem !== undefined && effectArgumentsList.currentItem !== null) ? effectArgumentsList.currentItem.y : 0
            x: (effectArgumentsList.currentItem !== undefined && effectArgumentsList.currentItem !== null) ? effectArgumentsList.currentItem.x : 0
            width: (effectArgumentsList.currentItem !== undefined && effectArgumentsList.currentItem !== null) ? effectArgumentsList.currentItem.width : 0
            height: (effectArgumentsList.currentItem !== undefined && effectArgumentsList.currentItem !== null) ? effectArgumentsList.currentItem.height : 0
        }
    }

    Component {
        id: effectArgumentSetsItem

        MouseArea {
            width: childrenRect.width
            height: childrenRect.height

            acceptedButtons: Qt.LeftButton | Qt.RightButton

            property bool showFullDescription: false

            onClicked:
            {
                if (mouse.button === Qt.RightButton)
                {
                    effectArgumentSetsItemMenu.popup()
                }
                else
                {
                    effectArgumentSetsCurrentIndex = index
                    mouse.accepted = false
                }
            }

            onPressAndHold: {
                if (mouse.source === Qt.MouseEventNotSynthesized)
                    effectArgumentSetsItemMenu.popup()
            }

            onDoubleClicked: {
                showFullDescription = !showFullDescription
            }

            AutoSizeMenu {
                id: effectArgumentSetsItemMenu
                MenuItem { action: addArgumentSet }
                MenuItem { action: editArgumentSet }
                MenuItem { action: removeArgumentSet }
            }

            Column {
                id: effectArgumentSetsItemRect
                width: effectArgumentSetsList.width

                FontMetrics{
                    id: effectArgumentSetsItemFontMetrics
                    font: effectArgumentSetsItemType.font
                }

                Row {
                    padding: Constants.effectShaderListHeaderPadding
                    Label {
                        id: effectArgumentSetsItemTypeLabel
                        text: qsTr("Easing type : ")
                    }
                    Text {
                        id: effectArgumentSetsItemType
                        width: effectArgumentSetsList.width - effectArgumentSetsItemTypeLabel.width
                        text: easingTypeModel.findItemById(effectArgSetEasingId) !== null ? easingTypeModel.findItemById(effectArgSetEasingId).easingTypeType : ""
                        wrapMode: Text.Wrap
                    }
                }

                Text {
                    id: effectArgumentSetsItemDescriptionText
                    width: effectArgumentSetsList.width
                    wrapMode: Text.WordWrap
                    text: showFullDescription ? mastactva.leftDoubleCR(effectArgSetDescription) : mastactva.readMore(effectArgSetDescription, Constants.effectsListReadMoreLength, qsTr(" ..."))
                }
            }
        }
    }

    Component {
        id: effectArgumentSetsItemHighlight

        Rectangle {
            SystemPalette {
                id: effectArgumentSetsItemHighlightPallete
                colorGroup: SystemPalette.Active
            }

            border.color: effectArgumentSetsItemHighlightPallete.highlight
            border.width: 2
            radius: 5
            y: (effectArgumentSetsList.currentItem !== undefined && effectArgumentSetsList.currentItem !== null) ? effectArgumentSetsList.currentItem.y : 0
            x: (effectArgumentSetsList.currentItem !== undefined && effectArgumentSetsList.currentItem !== null) ? effectArgumentSetsList.currentItem.x : 0
            width: (effectArgumentSetsList.currentItem !== undefined && effectArgumentSetsList.currentItem !== null) ? effectArgumentSetsList.currentItem.width : 0
            height: (effectArgumentSetsList.currentItem !== undefined && effectArgumentSetsList.currentItem !== null) ? effectArgumentSetsList.currentItem.height : 0
        }
    }

    Component {
        id: effectArgumentSetValuesItem

        MouseArea {
            width: childrenRect.width
            height: childrenRect.height

            acceptedButtons: Qt.LeftButton | Qt.RightButton

            property bool showFullDescription: false

            onClicked:
            {
                if (mouse.button === Qt.RightButton)
                {
                    effectArgumentSetValuesItemMenu.popup()
                }
                else
                {
                    effectArgumentSetValuesCurrentIndex = index
                    mouse.accepted = false
                }
            }

            onPressAndHold: {
                if (mouse.source === Qt.MouseEventNotSynthesized)
                    effectArgumentSetValuesItemMenu.popup()
            }

            onDoubleClicked: {
                showFullDescription = !showFullDescription
            }

            AutoSizeMenu {
                id: effectArgumentSetValuesItemMenu
                MenuItem { action: addArgumentOfArgumentSet }
                MenuItem { action: editArgumentOfArgumentSet }
                MenuItem { action: removeArgumentOfArgumentSet }
            }

            Connections {
                target: effectArgValueArg
                function onListReloaded()
                {
                    effectArgumentSetValuesItemArgType.text = shaderArgTypeModel.findItemById(effectArgValueArg.currentItem.effectArgArgTypeId).shaderArgTypeType
                    effectArgumentSetValuesItemArgName.text = effectArgValueArg.currentItem.effectArgName
                    effectArgumentSetValuesItemArgDefaultValue.text = effectArgValueArg.currentItem.effectArgDefaultValue
                }
            }

            Column {
                id: effectArgumentSetValuesItemRect
                width: effectArgumentSetValuesList.width

                Row {
                    padding: Constants.effectShaderListHeaderPadding
                    Label {
                        id: effectArgumentSetValuesItemArgTypeLabel
                        text: qsTr("Argument type : ")
                    }
                    Text {
                        id: effectArgumentSetValuesItemArgType
                        width: effectArgumentSetValuesList.width - effectArgumentSetValuesItemArgTypeLabel.width
                        text: effectArgValueArg.isListLoaded() && effectArgValueArg.currentItem !== null ? shaderArgTypeModel.findItemById(effectArgValueArg.currentItem.effectArgArgTypeId) !== null ? shaderArgTypeModel.findItemById(effectArgValueArg.currentItem.effectArgArgTypeId).shaderArgTypeType : "" : ""
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.effectShaderListHeaderPadding
                    Label {
                        id: effectArgumentSetValuesItemArgNameLabel
                        text: qsTr("Argument name : ")
                    }
                    Text {
                        id: effectArgumentSetValuesItemArgName
                        width: effectArgumentSetValuesList.width - effectArgumentSetValuesItemArgNameLabel.width
                        text: effectArgValueArg.isListLoaded() && effectArgValueArg.currentItem !== null ? effectArgValueArg.currentItem.effectArgName : ""
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.effectShaderListHeaderPadding
                    Label {
                        id: effectArgumentSetValuesItemArgDefaultValueLabel
                        text: qsTr("Argument default value : ")
                    }
                    Text {
                        id: effectArgumentSetValuesItemArgDefaultValue
                        width: effectArgumentSetValuesList.width - effectArgumentSetValuesItemArgDefaultValueLabel.width
                        text: effectArgValueArg.isListLoaded() && effectArgValueArg.currentItem !== null ? effectArgValueArg.currentItem.effectArgDefaultValue : ""
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.effectShaderListHeaderPadding
                    Label {
                        id: effectArgumentSetValuesItemValueLabel
                        text: qsTr("Value : ")
                    }
                    Text {
                        id: effectArgumentSetValuesItemValue
                        width: effectArgumentSetValuesList.width - effectArgumentSetValuesItemValueLabel.width
                        text: effectArgValueValue
                        wrapMode: Text.Wrap
                    }
                }

                Text {
                    id: effectArgumentSetValuesItemDescriptionText
                    width: effectArgumentSetValuesList.width
                    wrapMode: Text.WordWrap
                    text: showFullDescription ? mastactva.leftDoubleCR(effectArgValueDescription) : mastactva.readMore(effectArgValueDescription, Constants.effectsListReadMoreLength, qsTr(" ..."))
                }
            }
        }
    }

    Component {
        id: effectArgumentSetValuesItemHighlight

        Rectangle {
            SystemPalette {
                id: effectArgumentSetValuesItemHighlightPallete
                colorGroup: SystemPalette.Active
            }

            border.color: effectArgumentSetValuesItemHighlightPallete.highlight
            border.width: 2
            radius: 5
            y: (effectArgumentSetValuesList.currentItem !== undefined && effectArgumentSetValuesList.currentItem !== null) ? effectArgumentSetValuesList.currentItem.y : 0
            x: (effectArgumentSetValuesList.currentItem !== undefined && effectArgumentSetValuesList.currentItem !== null) ? effectArgumentSetValuesList.currentItem.x : 0
            width: (effectArgumentSetValuesList.currentItem !== undefined && effectArgumentSetValuesList.currentItem !== null) ? effectArgumentSetValuesList.currentItem.width : 0
            height: (effectArgumentSetValuesList.currentItem !== undefined && effectArgumentSetValuesList.currentItem !== null) ? effectArgumentSetValuesList.currentItem.height : 0
        }
    }

    Component {
        id: imageOfGalleryNextImageEffectListItem

        MouseArea {
            width: childrenRect.width
            height: childrenRect.height

            acceptedButtons: Qt.LeftButton | Qt.RightButton

            property bool showFullDescription: false

            onClicked:
            {
                if (mouse.button === Qt.RightButton)
                {
                    imageOfGalleryNextImageEffectListItemMenu.popup()
                }
                else
                {
                    imageOfGalleryNextImageEffectCurrentIndex = index
                    mouse.accepted = false
                }
            }

            onPressAndHold: {
                if (mouse.source === Qt.MouseEventNotSynthesized)
                    imageOfGalleryNextImageEffectListItemMenu.popup()
            }

            onDoubleClicked: {
                showFullDescription = !showFullDescription
            }

            AutoSizeMenu {
                id: imageOfGalleryNextImageEffectListItemMenu
                MenuItem { action: refreshNextImageEffect }
                MenuItem { action: addNextImageEffect }
                MenuItem { action: editNextImageEffect }
                MenuItem { action: removeNextImageEffect }
            }

            Connections {
                target: imagePointEffectEffect
                function onListReloaded()
                {
                    imageOfGalleryNextImageEffectListItemEffectName.text = imagePointEffectEffect !== undefined && imagePointEffectEffect !== null && imagePointEffectEffect.isListLoaded() && imagePointEffectEffect.currentItem !== null ? imagePointEffectEffect.currentItem.effectName : ""
                    imageOfGalleryNextImageEffectListItemDescription.text = imagePointEffectEffect !== undefined && imagePointEffectEffect !== null && imagePointEffectEffect.isListLoaded() && imagePointEffectEffect.currentItem !== null ? showFullDescription ? mastactva.leftDoubleCR(imagePointEffectEffect.currentItem.effectDescription) : mastactva.readMore(imagePointEffectEffect.currentItem.effectDescription, Constants.effectsListReadMoreLength, qsTr(" ...")) : ""
                }
            }

            Connections {
                target: imagePointEffectArgSet
                function onListReloaded()
                {
                    imageOfGalleryNextImageEffectListItemEasing.text = imagePointEffectArgSet !== undefined && imagePointEffectArgSet !== null && imagePointEffectArgSet.isListLoaded() && imagePointEffectArgSet.currentItem !== null && easingTypeModel.findItemById(imagePointEffectArgSet.currentItem.effectArgSetEasingId) !== null ? easingTypeModel.findItemById(imagePointEffectArgSet.currentItem.effectArgSetEasingId).easingTypeType : ""
                    imageOfGalleryNextImageEffectListItemArgSetDescription.text = imagePointEffectArgSet !== undefined && imagePointEffectArgSet !== null && imagePointEffectArgSet.isListLoaded() && imagePointEffectArgSet.currentItem !== null ? showFullDescription ? mastactva.leftDoubleCR(imagePointEffectArgSet.currentItem.effectArgSetDescription) : mastactva.readMore(imagePointEffectArgSet.currentItem.effectArgSetDescription, Constants.effectsListReadMoreLength, qsTr(" ...")) : ""
                }
            }

            Column {
                id: imageOfGalleryNextImageEffectListItemRect
                width: imageOfGalleryNextImageEffectList.width

                Row {
                    padding: Constants.effectShaderListHeaderPadding
                    Label {
                        id: imageOfGalleryNextImageEffectListItemEffectNameLabel
                        text: qsTr("Effect name : ")
                    }
                    Text {
                        id: imageOfGalleryNextImageEffectListItemEffectName
                        width: imageOfGalleryNextImageEffectList.width - imageOfGalleryNextImageEffectListItemEffectNameLabel.width
                        text: imagePointEffectEffect !== undefined && imagePointEffectEffect !== null && imagePointEffectEffect.isListLoaded() && imagePointEffectEffect.currentItem !== null ? imagePointEffectEffect.currentItem.effectName : ""
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.effectShaderListHeaderPadding
                    Label {
                        id: imageOfGalleryNextImageEffectListItemDescriptionLabel
                        text: qsTr("Effect description : ")
                    }
                    Text {
                        id: imageOfGalleryNextImageEffectListItemDescription
                        width: imageOfGalleryNextImageEffectList.width - imageOfGalleryNextImageEffectListItemDescriptionLabel.width
                        text: imagePointEffectEffect !== undefined && imagePointEffectEffect !== null && imagePointEffectEffect.isListLoaded() && imagePointEffectEffect.currentItem !== null ? showFullDescription ? mastactva.leftDoubleCR(imagePointEffectEffect.currentItem.effectDescription) : mastactva.readMore(imagePointEffectEffect.currentItem.effectDescription, Constants.effectsListReadMoreLength, qsTr(" ...")) : ""
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.effectShaderListHeaderPadding
                    Label {
                        id: imageOfGalleryNextImageEffectListItemEasingLabel
                        text: qsTr("Effect argument set easing : ")
                    }
                    Text {
                        id: imageOfGalleryNextImageEffectListItemEasing
                        width: imageOfGalleryNextImageEffectList.width - imageOfGalleryNextImageEffectListItemEasingLabel.width
                        text: imagePointEffectArgSet !== undefined && imagePointEffectArgSet !== null && imagePointEffectArgSet.isListLoaded() && imagePointEffectArgSet.currentItem !== null && easingTypeModel.findItemById(imagePointEffectArgSet.currentItem.effectArgSetEasingId) !== null ? easingTypeModel.findItemById(imagePointEffectArgSet.currentItem.effectArgSetEasingId).easingTypeType : ""
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.effectShaderListHeaderPadding
                    Label {
                        id: imageOfGalleryNextImageEffectListItemArgSetDescriptionLabel
                        text: qsTr("Effect argument set description : ")
                    }
                    Text {
                        id: imageOfGalleryNextImageEffectListItemArgSetDescription
                        width: imageOfGalleryNextImageEffectList.width - imageOfGalleryNextImageEffectListItemArgSetDescriptionLabel.width
                        text: imagePointEffectArgSet !== undefined && imagePointEffectArgSet !== null && imagePointEffectArgSet.isListLoaded() && imagePointEffectArgSet.currentItem !== null ? showFullDescription ? mastactva.leftDoubleCR(imagePointEffectArgSet.currentItem.effectArgSetDescription) : mastactva.readMore(imagePointEffectArgSet.currentItem.effectArgSetDescription, Constants.effectsListReadMoreLength, qsTr(" ...")) : ""
                        wrapMode: Text.Wrap
                    }
                }
            }
        }
    }

    Component {
        id: imageOfGalleryNextImageEffectListItemHighlight

        Rectangle {
            SystemPalette {
                id: imageOfGalleryNextImageEffectListItemHighlightPallete
                colorGroup: SystemPalette.Active
            }

            border.color: imageOfGalleryNextImageEffectListItemHighlightPallete.highlight
            border.width: 2
            radius: 5
            y: (imageOfGalleryNextImageEffectList.currentItem !== undefined && imageOfGalleryNextImageEffectList.currentItem !== null) ? imageOfGalleryNextImageEffectList.currentItem.y : 0
            x: (imageOfGalleryNextImageEffectList.currentItem !== undefined && imageOfGalleryNextImageEffectList.currentItem !== null) ? imageOfGalleryNextImageEffectList.currentItem.x : 0
            width: (imageOfGalleryNextImageEffectList.currentItem !== undefined && imageOfGalleryNextImageEffectList.currentItem !== null) ? imageOfGalleryNextImageEffectList.currentItem.width : 0
            height: (imageOfGalleryNextImageEffectList.currentItem !== undefined && imageOfGalleryNextImageEffectList.currentItem !== null) ? imageOfGalleryNextImageEffectList.currentItem.height : 0
        }
    }

}

