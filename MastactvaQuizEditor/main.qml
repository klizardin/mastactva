/*
    Copyright 2021

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

import QtQuick 2.12
import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15
//import Qt.labs.platform 1.1
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
    property var effectObjectsCurrentModel: undefined
    property int effectObjectsCurrentIndex: -1
    property var effectObjectArtefactsCurrentModel: undefined
    property int effectObjectArtefactsCurrentIndex: -1
    property var effectObjectArtefactArgsCurrentModel: undefined
    property int effectObjectArtefactArgsCurrentIndex: -1
    property var effectArgumentsCurrentModel: undefined
    property int effectArgumentsCurrentIndex: -1
    property var effectArgumentSetsCurrentModel: undefined
    property int effectArgumentSetsCurrentIndex: -1
    property var effectArgumentSetValuesCurrentModel: undefined
    property int effectArgumentSetValuesCurrentIndex: -1
    property int artefactCurrentIndex: -1
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
                var effectObjectsModel = effect.effectObjects
                if(effectObjectsModel.isListLoaded())
                {
                    effectObjectsListBusyIndicator.visible = false
                    effectObjectsListBusyIndicator.running = false
                    effectObjectsCurrentModel = effectObjectsModel
                    effectObjectsList.model = effectObjectsModel
                    effectObjectsCurrentIndex = effectObjectsCurrentModel.size() > 0 ? effectObjectsCurrentModel.currentIndex : -1
                }
                else
                {
                    effectObjectsListBusyIndicator.visible = true
                    effectObjectsListBusyIndicator.running = true
                    effectObjectsList.model = 0
                    effectObjectsModel.listReloaded.connect(effectObjectsListReloaded)
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
                effectObjectsCurrentModel = undefined
                effectObjectsList.model = 0
                effectObjectsCurrentIndex = -1
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

        function effectObjectsListReloaded()
        {
            effectObjectsListBusyIndicator.visible = false
            effectObjectsListBusyIndicator.running = false

            var effect = effectModel.getCurrentItem()
            var effectObjectsModel = effect.effectObjects
            effectObjectsModel.listReloaded.disconnect(effectObjectsListReloaded)
            if(effectObjectsModel.isListLoaded())
            {
                effectObjectsList.model = effectObjectsModel
                effectObjectsCurrentModel = effectObjectsModel
                effectObjectsList.currentIndex = effectObjectsCurrentModel.currentIndex
            }
            else
            {
                effectObjectsList.model = 0
                effectObjectsCurrentModel = undefined
            }
        }

        function effectArgumentsListReloaded()
        {
            effectArgumentsListBusyIndicator.visible = false
            effectArgumentsListBusyIndicator.running = false

            var effect = effectModel.getCurrentItem()
            if(effect !== undefined && effect !== null)
            {
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
            if(effect !== undefined && effect !== null)
            {
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

        function clearEffectObjectArtefacsCurrent()
        {
            effectObjectArtefactsList.model = 0
            effectObjectArtefactsCurrentModel = undefined
            effectObjectArtefactsCurrentIndex = -1
        }

        function onEffectObjectsCurrentIndexChanged()
        {
            effectObjectsList.currentIndex = effectObjectsCurrentIndex
            if(effectObjectsCurrentModel !== undefined && effectObjectsCurrentModel !== null)
            {
                effectObjectsCurrentModel.currentIndex = effectObjectsCurrentIndex
                var effectObjectItem = effectObjectsCurrentModel.currentItem
                if(effectObjectItem !== null && effectObjectItem !== undefined)
                {
                    var effectObjectArtefacsModel = effectObjectItem.effectObjectsObjectArtefacts
                    if(effectObjectArtefacsModel !== null && effectObjectArtefacsModel !== undefined)
                    {
                        clearEffectObjectArtefacsCurrent()
                        effectObjectArtefactsCurrentModel = effectObjectArtefacsModel
                        if(effectObjectArtefacsModel.isListLoaded())
                        {
                            effectObjectArtefactsList.model = effectObjectArtefactsCurrentModel
                            effectObjectArtefactsCurrentIndex = effectObjectArtefactsCurrentModel.currentIndex
                        }
                        else
                        {
                            effectObjectArtefacsModel.listReloaded.connect(effectObjectArtefacsModelRelistLoaded)
                        }
                        return
                    }
                }
            }
            clearEffectObjectArtefacsCurrent()
        }

        function effectObjectArtefacsModelRelistLoaded()
        {
            if(effectObjectsCurrentModel !== undefined && effectObjectsCurrentModel !== null)
            {
                var effectObjectItem = effectObjectsCurrentModel.currentItem
                if(effectObjectItem !== null && effectObjectItem !== undefined)
                {
                    var effectObjectArtefacsModel = effectObjectItem.effectObjectsObjectArtefacts
                    if(effectObjectArtefacsModel !== null && effectObjectArtefacsModel !== undefined)
                    {
                        effectObjectArtefacsModel.listReloaded.disconnect(effectObjectArtefacsModelRelistLoaded)
                        if(effectObjectArtefacsModel.isListLoaded())
                        {
                            clearEffectObjectArtefacsCurrent()
                            effectObjectArtefactsCurrentModel = effectObjectArtefacsModel
                            effectObjectArtefactsList.model = effectObjectArtefactsCurrentModel
                            effectObjectArtefactsCurrentIndex = effectObjectArtefactsCurrentModel.currentIndex
                            return
                        }
                    }
                }
            }
            clearEffectObjectArtefacsCurrent()
        }

        function clearEffectObjectArtefacArgsCurrent()
        {
            effectObjectArtefactArgsList.model = 0
            effectObjectArtefactArgsCurrentModel = undefined
            effectObjectArtefactArgsCurrentIndex = -1
        }

        function onEffectObjectArtefactsCurrentIndexChanged()
        {
            effectObjectArtefactsList.currentIndex = effectObjectArtefactsCurrentIndex
            if(effectObjectArtefactsCurrentModel !== null && effectObjectArtefactsCurrentModel !== undefined)
            {
                effectObjectArtefactsCurrentModel.currentIndex = effectObjectArtefactsCurrentIndex
                var effectObjectArtefact = effectObjectArtefactsCurrentModel.currentItem
                if(effectObjectArtefact !== null && effectObjectArtefact !== undefined)
                {
                    if(effectObjectArtefact.objectArtefactArtefact.size() > 0)
                    {
                        effectObjectArtefact.objectArtefactArtefact.currentIndex = 0
                        var effectObjectArtefacArgsModel = effectObjectArtefact.objectArtefactArtefact.currentItem.artefactArtefactArg
                        if(effectObjectArtefacArgsModel !== null && effectObjectArtefacArgsModel !== undefined)
                        {
                            clearEffectObjectArtefacArgsCurrent()
                            effectObjectArtefactArgsCurrentModel = effectObjectArtefacArgsModel
                            if(effectObjectArtefacArgsModel.isListLoaded())
                            {
                                effectObjectArtefactArgsList.model = effectObjectArtefactArgsCurrentModel
                                effectObjectArtefactArgsCurrentIndex = effectObjectArtefactArgsCurrentModel.currentIndex
                            }
                            else
                            {
                                effectObjectArtefacArgsModel.listReloaded.connect(effectObjectArtefacArgsModelRelistLoaded)
                            }
                            return
                        }
                    }
                }
            }
            clearEffectObjectArtefacArgsCurrent()
        }

        function effectObjectArtefacArgsModelRelistLoaded()
        {
            if(effectObjectArtefactsCurrentModel !== null && effectObjectArtefactsCurrentModel !== undefined)
            {
                var effectObjectArtefact = effectObjectArtefactsCurrentModel.currentItem
                if(effectObjectArtefact !== null && effectObjectArtefact !== undefined)
                {
                    if(effectObjectArtefact.objectArtefactArtefact.size() > 0)
                    {
                        effectObjectArtefact.objectArtefactArtefact.currentIndex = 0
                        var effectObjectArtefacArgsModel = effectObjectArtefact.objectArtefactArtefact.currentItem.artefactArtefactArg
                        if(effectObjectArtefacArgsModel !== null && effectObjectArtefacArgsModel !== undefined)
                        {
                            effectObjectArtefacArgsModel.listReloaded.disconnect(effectObjectArtefacArgsModelRelistLoaded)
                            if(effectObjectArtefacArgsModel.isListLoaded())
                            {
                                clearEffectObjectArtefacArgsCurrent()
                                effectObjectArtefactArgsCurrentModel = effectObjectArtefacArgsModel
                                effectObjectArtefactArgsList.model = effectObjectArtefactArgsCurrentModel
                                effectObjectArtefactArgsCurrentIndex = effectObjectArtefactArgsCurrentModel.currentIndex
                                return;
                            }
                        }
                    }
                }
            }
            clearEffectObjectArtefacArgsCurrent()
        }

        function onEffectObjectArtefactArgsCurrentIndexChanged()
        {
            effectObjectArtefactArgsList.currentIndex = effectObjectArtefactArgsCurrentIndex
            if(effectObjectArtefactArgsCurrentModel !== null && effectObjectArtefactArgsCurrentModel !== undefined)
            {
                effectObjectArtefactArgsCurrentModel.currentIndex = effectObjectArtefactArgsCurrentIndex
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
            if(effectArgumentSetsCurrentModel !== undefined && effectArgumentSetsCurrentModel !== null && effectArgumentSetsCurrentModel.currentItem !== null)
            {
                effectArgumentSetsCurrentModel.currentIndex = effectArgumentSetsCurrentIndex
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

                    effectArgumentSetValuesListBusyIndicator.visible = true
                    effectArgumentSetValuesListBusyIndicator.running = true

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
            effectArgumentSetValuesListBusyIndicator.visible = false
            effectArgumentSetValuesListBusyIndicator.running = false

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

        function onArtefactCurrentIndexChanged()
        {
            artefactsList.currentIndex = artefactCurrentIndex
            artefactModel.currentIndex = artefactCurrentIndex
            var artefact = artefactModel.currentItem
            setArtefactInfo(artefact)
        }
    }

    function setArtefactInfo(artefact)
    {
        if(artefact === undefined || artefact === null)
        {
            return
        }

        artefactInfo.currentIndex = -1

        if(artefact.isShader())
        {
            artefactInfo.currentIndex = 0
            shaderArtefactInfoShaderText.text = mastactva.getFileText(artefact.artefactFilename)
            shaderArtefactInfoShaderText.visible = true
        }
        else
        {
            shaderArtefactInfoShaderText.text = ""
            shaderArtefactInfoShaderText.visible = false
        }

        if(artefact.isTexture())
        {
            artefactInfo.currentIndex = 1
            textureArtefactInfoImage.visible = true
            textureArtefactInfoImage.source = artefact.artefactFilename
        }
        else
        {
            textureArtefactInfoImage.visible = false
        }

        if(artefact.isJson())
        {
            artefactInfo.currentIndex = 2
            jsonArtefactInfoJsonText.text = mastactva.getFileText(artefact.artefactFilename)
            jsonArtefactInfoJsonText.visible = true
        }
        else
        {
            jsonArtefactInfoJsonText.text = ""
            jsonArtefactInfoJsonText.visible = false
        }

        if(artefact.isObj3d())
        {
            artefactInfo.currentIndex = 3
            obj3dArtefactInfoObj3dText.text = mastactva.getFileText(artefact.artefactFilename)
            obj3dArtefactInfoObj3dText.visible = true
        }
        else
        {
            obj3dArtefactInfoObj3dText.text = ""
            obj3dArtefactInfoObj3dText.visible = false
        }

        if(artefact.isLua())
        {
            artefactInfo.currentIndex = 4
            luaArtefactInfoLuaText.text = mastactva.getFileText(artefact.artefactFilename)
            luaArtefactInfoLuaText.visible = true
        }
        else
        {
            luaArtefactInfoLuaText.text = ""
            luaArtefactInfoLuaText.visible = false
        }

        // artefactInfo.currentIndex {-1, [0..4]}
        //shaderArtefactInfo, shaderArtefactInfoShaderText
        //textureArtefactInfo, textureArtefactInfoImage
        //jsonArtefactInfo, jsonArtefactInfoJsonText
        //obj3dArtefactInfo, obj3dArtefactInfoObj3dText
        //luaArtefactInfo, luaArtefactInfoLuaText
    }

    function setArtefactInfoFromFile(artefact, fileUrl)
    {
        if(artefact === undefined || artefact === null)
        {
            return
        }

        artefactInfo.currentIndex = -1

        if(artefact.isShader())
        {
            artefactInfo.currentIndex = 0
            shaderArtefactInfoShaderText.text = mastactva.getLocalFileText(fileUrl)
            shaderArtefactInfoShaderText.visible = true
        }
        else
        {
            shaderArtefactInfoShaderText.text = ""
            shaderArtefactInfoShaderText.visible = false
        }

        if(artefact.isTexture())
        {
            artefactInfo.currentIndex = 1
            textureArtefactInfoImage.visible = true
            textureArtefactInfoImage.source = fileUrl
        }
        else
        {
            textureArtefactInfoImage.visible = false
        }

        if(artefact.isJson())
        {
            artefactInfo.currentIndex = 2
            jsonArtefactInfoJsonText.text = mastactva.getLocalFileText(fileUrl)
            jsonArtefactInfoJsonText.visible = true
        }
        else
        {
            jsonArtefactInfoJsonText.text = ""
            jsonArtefactInfoJsonText.visible = false
        }

        if(artefact.isObj3d())
        {
            artefactInfo.currentIndex = 3
            obj3dArtefactInfoObj3dText.text = mastactva.getLocalFileText(fileUrl)
            obj3dArtefactInfoObj3dText.visible = true
        }
        else
        {
            obj3dArtefactInfoObj3dText.text = ""
            obj3dArtefactInfoObj3dText.visible = false
        }

        if(artefact.isLua())
        {
            artefactInfo.currentIndex = 4
            luaArtefactInfoLuaText.text = mastactva.getLocalFileText(fileUrl)
            luaArtefactInfoLuaText.visible = true
        }
        else
        {
            luaArtefactInfoLuaText.text = ""
            luaArtefactInfoLuaText.visible = false
        }
    }

    function createArtefactInfoTmpFile(artefact)
    {
        if(artefact === undefined || artefact === null)
        {
            return undefined
        }

        var text = ""
        var fileName = undefined

        if(artefact.isShader())
        {
            text = shaderArtefactInfoShaderText.text
        }
        else if(artefact.isTexture())
        {
            fileName = textureArtefactInfoImage.source
        }
        else if(artefact.isJson())
        {
            text = jsonArtefactInfoJsonText.text
        }
        else if(artefact.isObj3d())
        {
            text = obj3dArtefactInfoObj3dText.text
        }
        else if(artefact.isLua())
        {
            text = luaArtefactInfoLuaText.text
        }

        if(fileName === undefined)
        {
            fileName = mastactva.createTempFile(artefact.artefactFilename, text)
            if(fileName === "")
            {
                fileName = undefined
            }
        }

        return fileName
    }

    function getArtefactInfoFileFilter(artefact)
    {
        if(artefact === undefined || artefact === null)
        {
            return undefined
        }
        return artefact.getFileFilter()
    }

    MastactvaAPI {
        id: mastactva
        objectName: "MastactvaAPI"
    }

    // constant models
    ArtefactTypeModel {
        id: artefactTypeModel
        objectName: "ArtefactTypeModel"
        layoutQMLName: "ArtefactTypeModel"
        layoutIdField: "id"
        jsonParamsGet: false
        autoCreateChildrenModels: false
    }

    ArtefactArgTypeModel {
        id: artefactArgTypeModel
        objectName: "ArtefactArgTypeModel"
        layoutQMLName: "ArtefactArgTypeModel"
        layoutIdField: "id"
        jsonParamsGet: false
        autoCreateChildrenModels: false
    }

    ArtefactArgStorageModel {
        id: artefactArgStorageModel
        objectName: "ArtefactArgStorageModel"
        layoutQMLName: "ArtefactArgStorageModel"
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

    ArtefactModel {
        id: artefactModel
        objectName: "ArtefactModel"
        layoutQMLName: "ArtefactModel"
        layoutIdField: "id"
        jsonParamsGet: false
        autoCreateChildrenModels: true
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

    EffectObjectsModel {
        id: effectObjectsModel
        objectName: "EffectObjectsModel"
        layoutQMLName: "EffectObjectsModel"
        layoutIdField: "id"
        currentRef: ""
        jsonParamsGet: false
        autoCreateChildrenModels: true
    }

    ObjectInfoModel {
        id: objectInfoModel
        objectName: "ObjectInfoModel"
        layoutQMLName: "ObjectInfoModel"
        layoutIdField: "id"
        currentRef: ""
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
            artefactTypeModel.loadList()
            artefactArgTypeModel.loadList()
            artefactArgStorageModel.loadList()
            easingTypeModel.loadList()
            artefactModel.loadList()
            artefactEditDialog.mastactva = mastactva
            artefactEditDialog.artefactTypeModel = artefactTypeModel
            chooseObjectInfoDialog.objectInfoModel = objectInfoModel
            chooseEffectObjectArtefactDialog.artefactTypeModel = artefactTypeModel
            chooseEffectObjectArtefactDialog.artefactModel = artefactModel
            artefactArgEditDialog.artefactArgTypeModel = artefactArgTypeModel
            artefactArgEditDialog.artefactArgStorageModel = artefactArgStorageModel
            effectArgumentSetEditDialog.easingTypeModel = easingTypeModel
            effectArgumentValueEditDialog.artefactArgTypeModel = artefactArgTypeModel
            chooseEffectArgumentDialog.artefactArgTypeModel = artefactArgTypeModel
            chooseImageDialog.galleryModel = galleryModel
            chooseImageDialog.allImagesOfGalleryModel = allImagesOfGalleryModel
            chooseImageDialog.mastactva = mastactva
            imagePointEffectEditDialog.effectModel = effectModel
            effectObjectEditDialog.mastactva = mastactva
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
            effectsList.model = effectModel.size() > 0 ? effectModel : 0
            var newIndex = effectModel.size() > 0 ? effectModel.currentIndex : -1
            effectsList.currentIndex = newIndex
            effectCurrentIndex = newIndex
        }

        function onItemDeleted()
        {
            effectsList.model = effectModel.size() > 0 ? effectModel : 0
            effectCurrentIndex = -1
        }

        function onError(errorCode, description)
        {
            showModelError(errorCode, description)
        }
    }

    Connections {
        target: artefactModel

        function onListReloaded()
        {
            artefactsListBusyIndicator.visible = false
            artefactsListBusyIndicator.running = false
            artefactsList.model = artefactModel.size() > 0 ? artefactModel : 0
            var newIndex = artefactModel.size() > 0 ? artefactModel.currentIndex : -1
            artefactsList.currentIndex = newIndex
            artefactCurrentIndex = newIndex
        }

        function onItemDeleted()
        {
            artefactsList.model = artefactModel.size() > 0 ? artefactModel : 0
            artefactCurrentIndex = -1
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
            if(fieldNewItem)
            {
                fieldEffect.effectCreated = mastactva.now()
            }
        }

        onAccepted: {
            update()
            if(fieldNewItem)
            {
                effectModel.itemAdded.connect(effectModelItemAdded)
                effectModel.addItem(fieldEffect)
            }
            else
            {
                effectModel.setItem(effectCurrentIndex, fieldEffect)
                fieldEffect = undefined
            }
        }

        onRejected: {
            fieldEffect = undefined
        }

        function effectModelItemAdded()
        {
            effectModel.itemAdded.disconnect(effectModelItemAdded)
            effectModel.selectItemByAppId(effectModel.getItemAppId(fieldEffect))
            fieldEffect = undefined
            if(effectModel.currentItem === null) { return; }
            if(effectModel.currentItem.isChildrenLoaded())
            {
                effectModel.currentItem.addDefaultObject()
            }
            else
            {
                effectModel.currentItem.childrenLoaded.connect(effectItemChildrenLoaded)
            }
        }

        function effectItemChildrenLoaded()
        {
            if(effectModel.currentItem === null) { return; }
            effectModel.currentItem.childrenLoaded.disconnect(effectItemChildrenLoaded)
            effectModel.currentItem.addDefaultObject()
        }
    }

    EffectObjectEditDialog {
        id: effectObjectEditDialog

        onOpened: {
            init()
        }

        onAccepted: {
            update()
            if(validState())
            {
                if(fieldNewItem)
                {
                    fieldEffectObjectInfo.effectObjectInfoCreated = mastactva.now()
                    var objectInfoModel = fieldEffectObject.effectObjectsObjectInfo
                    if(objectInfoModel !== null && objectInfoModel !== undefined)
                    {
                        objectInfoModel.itemAdded.connect(effectObjectInfoAdded)
                        objectInfoModel.addItem(fieldEffectObjectInfo)
                    }
                    else
                    {
                        clear()
                    }
                }
                else
                {
                    var itemIndex = fieldEffectObject.effectObjectsObjectInfo.indexOfItem(fieldEffectObjectInfo)
                    if(itemIndex >= 0)
                    {
                        fieldEffectObject.effectObjectsObjectInfo.itemSet.connect(effectObjectInfoSet)
                        fieldEffectObject.effectObjectsObjectInfo.setItem(itemIndex, fieldEffectObjectInfo)
                    }
                    else
                    {
                        clear()
                    }
                }
            }
            else
            {
                clear()
            }
        }

        onRejected: {
            clear()
        }

        function validModel()
        {
            return effectObjectsCurrentModel !== undefined && effectObjectsCurrentModel !== null
        }

        function validModelAndPosition()
        {
            return validModel() && effectObjectsCurrentIndex >= 0
        }

        function validState()
        {
            var validEffectObject = fieldEffectObject !== null && fieldEffectObject !== undefined
            var validEffectObjectInfo = fieldEffectObjectInfo !== null && fieldEffectObjectInfo !== undefined
            var validArgs = validEffectObject && validEffectObjectInfo
            return validArgs && validModel()
        }

        function createNew()
        {
            clear()
            if(!validModel())
            {
                return
            }
            fieldNewItem = true
            fieldEffectObject = effectObjectsCurrentModel.createItem()
            var objectInfoModel = fieldEffectObject.effectObjectsObjectInfo
            fieldEffectObjectInfo = objectInfoModel.createItem()
            if(validState())
            {
                open()
            }
        }

        function editCurrent()
        {
            clear()
            if(!validModelAndPosition())
            {
                return;
            }
            fieldNewItem = false
            fieldEffectObject = effectObjectsCurrentModel.currentItem
            fieldEffectObjectInfo = fieldEffectObject.effectObjectsObjectInfo.currentItem
            if(validState())
            {
                open()
            }
        }

        function clear()
        {
            fieldEffectObject = undefined
            fieldEffectObjectInfo = undefined
            fieldNewItem = false
        }

        function effectObjectProcess()
        {
            if(validState())
            {
                if(fieldNewItem)
                {
                    effectObjectsCurrentModel.itemAdded.connect(effectObjectAdded)
                    effectObjectsCurrentModel.addItem(fieldEffectObject);
                }
                else
                {
                    var itemIndex = effectObjectsCurrentModel.indexOfItem(fieldEffectObject)
                    if(itemIndex >= 0)
                    {
                        effectObjectsCurrentModel.itemSet.connect(effectObjectSet)
                        var index = effectObjectsCurrentModel.indexOfItem(fieldEffectObject)
                        effectObjectsCurrentModel.setItem(index, fieldEffectObject);
                    }
                    else
                    {
                        clear();
                    }
                }
            }
            else
            {
                clear();
            }
        }

        function effectObjectInfoAdded()
        {
            if(validState())
            {
                fieldEffectObject.effectObjectsObjectInfo.itemAdded.disconnect(effectObjectInfoAdded)

                if(fieldEffectObject.updateObjectInfoId(fieldEffectObjectInfo))
                {
                    effectObjectProcess()
                }
                else
                {
                    clear();
                }
            }
            else
            {
                clear();
            }
        }

        function effectObjectAdded()
        {
            if(validState())
            {
                effectObjectsCurrentModel.itemAdded.disconnect(effectObjectAdded)
                var itemIndex = effectObjectsCurrentModel.indexOfItem(fieldEffectObject)
                effectObjectsCurrentIndex = itemIndex
            }
            clear();
        }

        function effectObjectInfoSet()
        {
            if(validState())
            {
                fieldEffectObject.effectObjectsObjectInfo.itemSet.disconnect(effectObjectInfoSet)
                effectObjectProcess()
            }
            else
            {
                clear();
            }
        }

        function effectObjectSet()
        {
            clear();
        }
    }

    ChooseObjectInfoDialog {
        id: chooseObjectInfoDialog

        // private:
        property var newEffectObject: undefined

        onOpened: {
            clear()
            init()
        }

        onAccepted: {
            update()
            processEffectObject()
        }

        // public:
        function choose()
        {
            open()
        }

        // private:
        function isValid()
        {
            var validEffectObject = fieldObjectInfo !== null && fieldObjectInfo !== undefined
            var validModel = effectObjectsCurrentModel !== undefined && effectObjectsCurrentModel !== null
            return validEffectObject && validModel;
        }

        function clear()
        {
            fieldObjectInfo = undefined
            newEffectObject = undefined
            fieldEditEffectObjectStepValue = -1
        }

        function processEffectObject()
        {
            if(isValid())
            {
                effectObjectsCurrentModel.listReloaded.connect(effectObjectCopied)
                var args = {"effect":effectModel.currentItem.effectId , "source":fieldObjectInfo.effectObjectInfoId, "step_index":fieldEditEffectObjectStepValue}
                effectObjectsCurrentModel.procedure("copy_from_effect_object", args)
            }
            else
            {
                clear()
            }
        }

        function effectObjectCopied()
        {
            if(isValid())
            {
                effectObjectsCurrentModel.listReloaded.disconnect(effectObjectCopied)
            }
            clear()
        }
    }

    ArtefactEditDialog {
        id: artefactEditDialog

        onOpened: {
            init()
        }

        onAccepted: {
            update()
            if(validState())
            {
                if(fieldNewItem)
                {
                    fieldArtefact.artefactCreated = mastactva.now()
                    fieldObjectArtefact.objectArtefactArtefact.itemAdded.connect(artefactAdded)
                    fieldObjectArtefact.objectArtefactArtefact.addItem(fieldArtefact)
                }
                else
                {
                    var itemIndex = fieldObjectArtefact.objectArtefactArtefact.indexOfItem(fieldArtefact)
                    if(itemIndex >= 0)
                    {
                        fieldObjectArtefact.objectArtefactArtefact.itemSet.connect(artefactSet)
                        fieldObjectArtefact.objectArtefactArtefact.setItem(itemIndex, fieldArtefact)
                    }
                    else
                    {
                        clear()
                    }
                }
            }
            else
            {
                clear()
            }
        }

        onRejected: {
            clear()
        }

        function validModel()
        {
            return effectObjectArtefactsCurrentModel !== undefined && effectObjectArtefactsCurrentModel !== null
        }

        function validModelAndPosition()
        {
            return validModel() && effectObjectArtefactsCurrentIndex >= 0
        }

        function validState()
        {
            var validEffectObjectArtefact = fieldObjectArtefact !== null && fieldObjectArtefact !== undefined
            var validArtefact = fieldArtefact !== null && fieldArtefact !== undefined
            var validArgs = validEffectObjectArtefact && validArtefact
            var validEffectObjectsModel = effectObjectsCurrentModel !== undefined && effectObjectsCurrentModel !== null
            var objectInfoModel = effectObjectsCurrentModel.currentItem.effectObjectsObjectInfo
            var validObjectInfoModel = objectInfoModel !== undefined && objectInfoModel !== null
            validObjectInfoModel = validObjectInfoModel && objectInfoModel.currentItem !== null
            return validArgs && validModel() && validEffectObjectsModel && validObjectInfoModel
        }

        function createNew()
        {
            clear()
            if(!validModel())
            {
                return
            }
            fieldNewItem = true
            fieldObjectArtefact = effectObjectArtefactsCurrentModel.createItem()
            fieldArtefact = fieldObjectArtefact.objectArtefactArtefact.createItem()
            if(validState())
            {
                open()
            }
        }

        function editCurrent()
        {
            clear()
            if(!validModelAndPosition())
            {
                return;
            }
            fieldNewItem = false
            fieldObjectArtefact = effectObjectArtefactsCurrentModel.currentItem
            fieldArtefact = fieldObjectArtefact.objectArtefactArtefact.currentItem
            if(validState())
            {
                open()
            }
        }

        function clear()
        {
            fieldObjectArtefact = undefined
            fieldArtefact = undefined
            fieldNewItem = false
        }

        function artefactAdded()
        {
            if(validState())
            {
                artefactModel.itemAdded.connect(artefactAdded)
                objectArtefactProcess()
            }
            else
            {
                clear();
            }
        }

        function artefactSet()
        {
            if(validState())
            {
                fieldObjectArtefact.objectArtefactArtefact.itemSet.disconnect(artefactSet)
                objectArtefactProcess()
            }
            else
            {
                clear();
            }
        }

        function objectArtefactProcess()
        {
            if(validState())
            {
                if(fieldNewItem)
                {
                    if(fieldObjectArtefact.updateArtefactId(fieldArtefact))
                    {
                        var currentObjectInfoId = effectObjectsCurrentModel.currentItem.effectObjectsObjectInfo.currentItem.effectObjectInfoId
                        fieldObjectArtefact.updateObjectInfoId(currentObjectInfoId)

                        effectObjectArtefactsCurrentModel.itemAdded.connect(objectArtefactAdded)
                        effectObjectArtefactsCurrentModel.addItem(fieldObjectArtefact)
                    }
                    else
                    {
                        clear()
                    }
                }
                else
                {
                    var itemIndex = effectObjectArtefactsCurrentModel.indexOfItem(fieldObjectArtefact)
                    if(itemIndex >= 0)
                    {
                        effectObjectArtefactsCurrentModel.itemSet.connect(objectArtefactSet)
                        effectObjectArtefactsCurrentModel.setItem(itemIndex, fieldObjectArtefact)
                    }
                    else
                    {
                        clear()
                    }
                }
            }
            else
            {
                clear();
            }
        }

        function objectArtefactAdded()
        {
            if(validState())
            {
                effectObjectArtefactsCurrentModel.itemAdded.disconnect(objectArtefactAdded)
            }
            clear()
        }

        function objectArtefactSet()
        {
            if(validState())
            {
                effectObjectArtefactsCurrentModel.itemSet.disconnect(objectArtefactSet)
            }
            clear()
        }
    }

    ChooseEffectObjectArtefactDialog {
        id: chooseEffectObjectArtefactDialog

        // private:
        property var newEffectObjectArtefact: undefined

        onOpened: {
            clear()
            init()
        }

        onAccepted: {
            if(isValid())
            {
                newEffectObjectArtefact = effectObjectArtefactsCurrentModel.createItem()
                var currentObjectInfoId = effectObjectsCurrentModel.currentItem.effectObjectsObjectInfo.currentItem.effectObjectInfoId
                newEffectObjectArtefact.updateObjectInfoId(currentObjectInfoId)
                newEffectObjectArtefact.setArtefactId(fieldArtefact.artefactId)

                effectObjectArtefactsCurrentModel.itemAdded.connect(effectObjectArtefactAdded)
                effectObjectArtefactsCurrentModel.addItem(newEffectObjectArtefact)
            }
            else
            {
                clear()
            }
        }

        // public:
        function choose()
        {
            open()
        }

        // private:
        function isValid()
        {
            var validEffectObject = fieldArtefact !== null && fieldArtefact !== undefined
            var validModel = effectObjectArtefactsCurrentModel !== undefined && effectObjectArtefactsCurrentModel !== null
            var validEffectObjectsModel = effectObjectsCurrentModel !== undefined && effectObjectsCurrentModel !== null
            var objectInfoModel = effectObjectsCurrentModel.currentItem.effectObjectsObjectInfo
            var validObjectInfoModel = objectInfoModel !== undefined && objectInfoModel !== null
            validObjectInfoModel = validObjectInfoModel && objectInfoModel.currentItem !== null
            return validEffectObject && validModel && validEffectObjectsModel && validObjectInfoModel;
        }

        function clear()
        {
            fieldArtefact = undefined
            newEffectObjectArtefact = undefined
        }

        function effectObjectArtefactAdded()
        {
            if(isValid())
            {
                effectObjectArtefactsCurrentModel.itemAdded.disconnect(effectObjectArtefactAdded)
                var index = effectObjectArtefactsCurrentModel.indexOfItem(newEffectObjectArtefact)
                effectObjectArtefactsCurrentIndex = index
            }
            clear()
        }
    }

    ArtefactArgEditDialog {
        id: artefactArgEditDialog

        onOpened: {
            init()
        }

        onAccepted: {
            update()
            if(validState())
            {
                if(fieldNewItem)
                {
                    var artefact = effectObjectArtefactsCurrentModel.currentItem.objectArtefactArtefact.currentItem
                    if(artefact !== undefined && artefact !== null)
                    {
                        fieldArtefactArg.updateArtefactId(artefact.artefactId)
                        effectObjectArtefactArgsCurrentModel.itemAdded.connect(artefactArgAdded)
                        effectObjectArtefactArgsCurrentModel.addItem(fieldArtefactArg)
                    }
                    else
                    {
                        clear()
                    }
                }
                else
                {
                    var itemIndex = effectObjectArtefactArgsCurrentModel.indexOfItem(fieldArtefactArg)
                    if(itemIndex >= 0)
                    {
                        effectObjectArtefactArgsCurrentModel.itemSet.connect(artefactArgSet)
                        effectObjectArtefactArgsCurrentModel.setItem(itemIndex, fieldArtefactArg)
                    }
                    else
                    {
                        clear()
                    }
                }
            }
            else
            {
                clear()
            }
        }

        onRejected: {
            clear()
        }

        function validModel()
        {
            var validObjectArtefactModel = effectObjectArtefactsCurrentModel !== undefined && effectObjectArtefactsCurrentModel !== null
            var validObjectArtefactModelItem = effectObjectArtefactsCurrentModel.currentItem !== null
            var validArtefactModel = effectObjectArtefactsCurrentModel.currentItem.objectArtefactArtefact !== undefined && effectObjectArtefactsCurrentModel.currentItem.objectArtefactArtefact !== null
            if(validArtefactModel)
            {
                effectObjectArtefactsCurrentModel.currentItem.objectArtefactArtefact.currentIndex = 0
            }
            var validArtefactModelItem = effectObjectArtefactsCurrentModel.currentItem.objectArtefactArtefact.currentItem !== null
            var validArgModel = effectObjectArtefactArgsCurrentModel !== undefined && effectObjectArtefactArgsCurrentModel !== null
            return validObjectArtefactModel && validObjectArtefactModelItem && validArtefactModel && validArtefactModelItem && validArgModel
        }

        function validModelAndPosition()
        {
            return validModel() && effectObjectArtefactArgsCurrentIndex >= 0
        }

        function validState()
        {
            var validEffectObjectArtefactArg = fieldArtefactArg !== null && fieldArtefactArg !== undefined
            return validEffectObjectArtefactArg && validModel()
        }

        function createNew()
        {
            clear()
            if(!validModel())
            {
                return
            }
            fieldNewItem = true
            fieldArtefactArg = effectObjectArtefactArgsCurrentModel.createItem()
            if(validState())
            {
                open()
            }
        }

        function editCurrent()
        {
            clear()
            if(!validModelAndPosition())
            {
                return;
            }
            fieldNewItem = false
            fieldArtefactArg = effectObjectArtefactArgsCurrentModel.currentItem
            if(validState())
            {
                open()
            }
        }

        function clear()
        {
            fieldArtefactArg = undefined
            fieldNewItem = false
        }

        function artefactArgAdded()
        {
            if(validState())
            {
                effectObjectArtefactArgsCurrentModel.itemAdded.disconnect(artefactArgAdded)
            }
            clear();
        }

        function artefactArgSet()
        {
            if(validState())
            {
                effectObjectArtefactArgsCurrentModel.itemSet.disconnect(artefactArgSet)
            }
            clear();
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
                    fieldEffectArgumentSet.effectArgSetCreated(mastactva.now())
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
            //fieldImagePointEffect.imagePointEffectStepIndex = 0
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

    LocalDataSet {
        id: localDataSet
        savePath: downloadGalleryLocalDataFileDialog.folder
    }

    FileDialog {
        id: downloadGalleryLocalDataFileDialog
        title: qsTr("Please choose root file to download local data in the folder of a root file")
        folder: localDataSet.savePath
        nameFilters: [ "All files (*.*)" ]
        selectExisting: false
        selectMultiple: false

        onAccepted: {
            localDataSet.savePath = fileUrl
            connect()
            // TODO: use separate dialog or rewrite possibility to close/stop download
            popupMessage.fieldPopupMessageShortText = qsTr("Please, wait downloading local data...")
            popupMessage.open()
            localDataSet.download()
        }

        onRejected: {
            // TODO: add implemention
        }

        function connect()
        {
            localDataSet.progress.connect(localDataSetProgress)
            localDataSet.downloaded.connect(localDataSetDownloaded)
        }

        function disconnect()
        {
            localDataSet.progress.disconnect(localDataSetProgress)
            localDataSet.downloaded.disconnect(localDataSetDownloaded)
        }

        function localDataSetProgress(p)
        {
            popupMessage.fieldPopupMessageDescriptionText = qsTr("Progress : ") + p + qsTr(" %")
        }

        function localDataSetDownloaded()
        {
            disconnect()
            popupMessage.fieldPopupMessageShortText = qsTr("Local data are  downloaded");
            popupMessage.fieldPopupMessageDescriptionText = qsTr("See local data in the folder : ") + localDataSet.savePath
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
            promptBeforeRemove()
        }

        function promptBeforeRemove()
        {
            if(imageOfGalleryNextImageEffectModel !== undefined && imageOfGalleryNextImageEffectModel !== null && imageOfGalleryNextImageEffectCurrentIndex >= 0)
            {
                var effect = imageOfGalleryNextImageEffectModel.itemAt(imageOfGalleryNextImageEffectCurrentIndex).imagePointEffectEffect
                var argSet = imageOfGalleryNextImageEffectModel.itemAt(imageOfGalleryNextImageEffectCurrentIndex).imagePointEffectArgSet
                if(effect !== undefined && effect !== null && effect.currentItem !== null && argSet !== undefined && argSet !== null && argSet.currentItem !== null)
                {
                    confirmDialog.confirmText = qsTr("Do you really want to remove image point effect?") + qsTr("<br/>") + qsTr("<br/>") + qsTr("Effect name : ") + effect.currentItem.effectName + qsTr("<br/>") + qsTr("Effect description : ") + effect.currentItem.effectDescription + qsTr("<br/>") + qsTr("Argument set : ") + argSet.currentItem.effectArgSetDescription + qsTr("<br/>");
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


    Action {
        id: downloadGalleryLocalData
        text: qsTr("&Download local data")
        onTriggered: {
            downloadGalleryLocalDataFileDialog.folder = localDataSet.savePath
            downloadGalleryLocalDataFileDialog.open()
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
            title: qsTr("&Local Data")
            MenuItem { action: downloadGalleryLocalData }
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
            promptBeforeRemove()
        }

        function promptBeforeRemove()
        {
            if(effectCurrentIndex >= 0)
            {
                var effect = effectModel.itemAt(effectCurrentIndex)
                if(effect !== undefined && effect !== null)
                {
                    confirmDialog.confirmText = qsTr("Do you really want to remove effect?") + qsTr("<br/>") + qsTr("<br/>") + qsTr("Effect name : ") + effect.effectName + qsTr("<br/>") + qsTr("Effect description : ") + effect.effectDescription + qsTr("<br/>");
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
            if(effectCurrentIndex >= 0)
            {
                effectModel.itemDeleted.connect(itemDeleted)
                effectModel.delItem(effectCurrentIndex)
            }
        }

        function itemDeleted()
        {
            effectCurrentIndex = -1
            effectModel.itemDeleted.disconnect(itemDeleted)
        }
    }

    function isEffectObjectsListValid()
    {
        return effectObjectsCurrentModel !== null;
    }

    Action {
        id: refreshEffectObjects
        text: qsTr("&Refresh")
        onTriggered: {
            if(isEffectObjectsListValid()) {
                effectObjectsCurrentIndex = -1
                effectObjectsCurrentModel.listReloaded.connect(onEffectObjectsListLoaded)
                effectObjectsListBusyIndicator.visible = true
                effectObjectsListBusyIndicator.running = true
                effectObjectsList.model = 0
                effectObjectsCurrentModel.loadList()
            }
        }

        function onEffectObjectsListLoaded()
        {
            if(isEffectObjectsListValid()) {
                effectObjectsCurrentModel.listReloaded.disconnect(onEffectObjectsListLoaded)
                effectObjectsListBusyIndicator.visible = false
                effectObjectsListBusyIndicator.running = false
                effectObjectsList.model = effectObjectsCurrentModel
                effectObjectsCurrentIndex = effectObjectsCurrentModel.currentIndex
            }
        }
    }

    Action {
        id: addNewEffectObject
        text: qsTr("Add &new")
        onTriggered: {
            effectObjectEditDialog.createNew()
        }
    }

    Action {
        id: addExistingEffectObject
        text: qsTr("Add &existing")
        onTriggered: {
            chooseObjectInfoDialog.choose()
        }
    }

    Action {
        id: editEffectObjectInfo
        text: qsTr("Edit &info")
        onTriggered: {
            effectObjectEditDialog.editCurrent()
        }
    }

    Action {
        id: removeEffectObject
        text: qsTr("Remove")
        onTriggered: {
            if(isValidPos())
            {
                effectObjectsCurrentModel.itemDeleted.connect(effectObjectsItemDeleted)
                effectObjectsCurrentModel.delItem(effectObjectsCurrentIndex)
                effectObjectsCurrentIndex = -1
            }
        }

        // private:
        function isValidModel()
        {
            var validModel = effectObjectsCurrentModel !== undefined && effectObjectsCurrentModel !== null
            return validModel
        }

        function isValidPos()
        {
            var validIndex = effectObjectsCurrentIndex >= 0
            return isValidModel() && validIndex
        }

        function effectObjectsItemDeleted()
        {
            if(isValidModel())
            {
                effectObjectsCurrentModel.itemDeleted.disconnect(effectObjectsItemDeleted)
            }
        }
    }

    function isEffectObjectArtefactsListValid()
    {
        return effectObjectArtefactsCurrentModel !== null;
    }

    Action {
        id: refreshEffectObjectArtefacts
        text: qsTr("&Refresh")
        onTriggered: {
            if(isEffectObjectArtefactsListValid())
            {
                effectObjectArtefactsCurrentIndex = -1
                effectObjectArtefactsCurrentModel.listReloaded.connect(onEffectObjectArtefactsListLoaded)
                effectObjectArtefactsListBusyIndicator.visible = true
                effectObjectArtefactsListBusyIndicator.running = true
                effectObjectArtefactsList.model = 0
                effectObjectArtefactsCurrentModel.loadList()
            }
        }

        function onEffectObjectArtefactsListLoaded()
        {
            if(isEffectObjectArtefactsListValid()) {
                effectObjectArtefactsCurrentModel.listReloaded.disconnect(onEffectObjectArtefactsListLoaded)
                effectObjectArtefactsListBusyIndicator.visible = false
                effectObjectArtefactsListBusyIndicator.running = false
                effectObjectArtefactsList.model = effectObjectArtefactsCurrentModel
                effectObjectArtefactsCurrentIndex = effectObjectArtefactsCurrentModel.currentIndex
            }
        }
    }

    Action {
        id: addNewEffectObjectArtefact
        text: qsTr("Add &new")
        onTriggered: {
            artefactEditDialog.createNew()
        }
    }

    Action {
        id: addExistingEffectObjectArtefact
        text: qsTr("Add &existing")
        onTriggered: {
            chooseEffectObjectArtefactDialog.choose()
        }
    }

    Action {
        id: editEffectObjectArtefact
        text: qsTr("&Edit")
        onTriggered: {
            artefactEditDialog.editCurrent()
        }
    }

    Action {
        id: removeEffectObjectArtefact
        text: qsTr("Remove")
        onTriggered: {
            if(isValidPos())
            {
                effectObjectArtefactsCurrentModel.itemDeleted.connect(effectObjectArtefactsItemDeleted)
                effectObjectArtefactsCurrentModel.delItem(effectObjectArtefactsCurrentIndex)
                effectObjectArtefactsCurrentIndex = -1
            }
        }

        // private:
        function isValidModel()
        {
            var validModel = effectObjectArtefactsCurrentModel !== undefined && effectObjectArtefactsCurrentModel !== null
            return validModel
        }

        function isValidPos()
        {
            var validIndex = effectObjectArtefactsCurrentIndex >= 0
            return isValidModel() && validIndex
        }

        function effectObjectArtefactsItemDeleted()
        {
            if(isValidModel())
            {
                effectObjectArtefactsCurrentModel.itemDeleted.disconnect(effectObjectArtefactsItemDeleted)
            }
        }
    }

    function refreshEffectObjectArtefactArgsFunc()
    {
        refreshEffectObjectArtefactArgs.refreshImpl()
    }

    Action {
        id: refreshEffectObjectArtefactArgs
        text: qsTr("&Refresh")
        onTriggered: {
            refreshEffectObjectArtefactArgsFunc()
        }

        function isValidModel()
        {
            return effectObjectArtefactArgsCurrentModel !== undefined && effectObjectArtefactArgsCurrentModel !== null
        }

        function refreshImpl()
        {
            if(isValidModel())
            {
                effectObjectArtefactArgsCurrentModel.listReloaded.connect(effectObjectArtefactArgsCurrentModelReloaded)
                effectObjectArtefactArgsList.model = 0
                effectObjectArtefactArgsCurrentIndex = -1
                effectObjectArtefactArgsListBusyIndicator.visible = true
                effectObjectArtefactArgsListBusyIndicator.running = true
                effectObjectArtefactArgsCurrentModel.loadList()
            }
        }

        function effectObjectArtefactArgsCurrentModelReloaded()
        {
            effectObjectArtefactArgsListBusyIndicator.visible = false
            effectObjectArtefactArgsListBusyIndicator.running = false
            if(isValidModel())
            {
                effectObjectArtefactArgsCurrentModel.listReloaded.connect(effectObjectArtefactArgsCurrentModelReloaded)
                effectObjectArtefactArgsList.model = effectObjectArtefactArgsCurrentModel
                effectObjectArtefactArgsCurrentIndex = effectObjectArtefactArgsCurrentModel.currentIndex
            }
        }
    }

    Action {
        id: addNewEffectObjectArtefactArg
        text: qsTr("Add &new")
        onTriggered: {
            artefactArgEditDialog.createNew()
        }
    }

    Action {
        id: addFromArtefactEffectObjectArtefactArg
        text: qsTr("Add from &artefact")
        onTriggered: {
            if(isValidPos())
            {
                var effectObjectArtefact = effectObjectArtefactsCurrentModel.currentItem
                if(effectObjectArtefact !== null && effectObjectArtefact !== undefined)
                {
                    var artefactModel = effectObjectArtefact.objectArtefactArtefact
                    if(artefactModel !== null && artefactModel !== undefined && artefactModel.isListLoaded())
                    {
                        var artefact = artefactModel.currentItem
                        if(artefact !== null && artefact !== undefined)
                        {
                            artefact.argumentsFromArtefactTextChanged.connect(argumentsFromArtefactTextChanged)
                            artefact.getArgumentsFromArtefactText()
                        }
                    }
                }
            }
        }

        // private:
        function isValidModel()
        {
            var validModel = effectObjectArtefactsCurrentModel !== undefined && effectObjectArtefactsCurrentModel !== null
            return validModel
        }

        function isValidPos()
        {
            var validIndex = effectObjectArtefactsCurrentIndex >= 0
            return isValidModel() && validIndex
        }

        function argumentsFromArtefactTextChanged()
        {
            if(isValidPos())
            {
                var effectObjectArtefact = effectObjectArtefactsCurrentModel.currentItem
                if(effectObjectArtefact !== null && effectObjectArtefact !== undefined)
                {
                    var artefactModel = effectObjectArtefact.objectArtefactArtefact
                    if(artefactModel !== null && artefactModel !== undefined && artefactModel.isListLoaded())
                    {
                        var artefact = artefactModel.currentItem
                        if(artefact !== null && artefact !== undefined)
                        {
                            artefact.argumentsFromArtefactTextChanged.disconnect(argumentsFromArtefactTextChanged)
                            refreshEffectObjectArtefactArgsFunc()
                        }
                    }
                }
            }
        }
    }

    Action {
        id: editEffectObjectArtefactArg
        text: qsTr("&Edit")
        onTriggered: {
            artefactArgEditDialog.editCurrent()
        }
    }

    Action {
        id: removeEffectObjectArtefactArg
        text: qsTr("Remove")
        onTriggered: {
            if(isValidPos())
            {
                effectObjectArtefactArgsCurrentModel.itemDeleted.connect(effectObjectArtefactArgsItemDeleted)
                effectObjectArtefactArgsCurrentModel.delItem(effectObjectArtefactArgsCurrentIndex)
                effectObjectArtefactArgsCurrentIndex = -1
            }
        }

        // private:
        function isValidModel()
        {
            var validModel = effectObjectArtefactArgsCurrentModel !== undefined && effectObjectArtefactArgsCurrentModel !== null
            return validModel
        }

        function isValidPos()
        {
            var validIndex = effectObjectArtefactArgsCurrentIndex >= 0
            return isValidModel() && validIndex
        }

        function effectObjectArtefactArgsItemDeleted()
        {
            if(isValidModel())
            {
                effectObjectArtefactArgsCurrentModel.itemDeleted.disconnect(effectObjectArtefactArgsItemDeleted)
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
                effectArgumentSetsList.currentIndex = -1
                effectArgumentSetsList.model = 0
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
                effectArgumentSetsList.model = effectArgumentSetsCurrentModel
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
            promptBeforeRemove()
        }

        function promptBeforeRemove()
        {
            if(effectArgumentSetsCurrentModel !== undefined && effectArgumentSetsCurrentModel !== null && effectArgumentSetsCurrentIndex >= 0)
            {
                var effectArgumentSets = effectArgumentSetsCurrentModel.itemAt(effectArgumentSetsCurrentIndex)
                if(effectArgumentSets !== undefined && effectArgumentSets !== null)
                {
                    confirmDialog.confirmText = qsTr("Do you really want to remove argument set?") + qsTr("<br/>") + qsTr("<br/>") + qsTr("Arguments set description : ") + effectArgumentSets.effectArgSetDescription + qsTr("<br/>");
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
            if(effectArgumentSetsCurrentModel !== undefined && effectArgumentSetsCurrentModel !== null && effectArgumentSetsCurrentIndex >= 0)
            {
                effectArgumentSetsCurrentModel.itemDeleted.connect(itemDeleted)
                effectArgumentSetsCurrentModel.delItem(effectArgumentSetsCurrentIndex)
            }
        }

        function itemDeleted()
        {
            effectArgumentSetsCurrentIndex = -1
            if(effectArgumentSetsCurrentModel !== undefined && effectArgumentSetsCurrentModel !== null)
            {
                effectArgumentSetsCurrentModel.itemDeleted.disconnect(itemDeleted)
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

                effectArgumentSetValuesListBusyIndicator.visible = true
                effectArgumentSetValuesListBusyIndicator.running = true

                effectArgumentSetValuesCurrentModel.listReloaded.connect(listReloaded)
                effectArgumentSetValuesCurrentModel.loadList()
            }
        }

        function listReloaded()
        {
            effectArgumentSetValuesListBusyIndicator.visible = false
            effectArgumentSetValuesListBusyIndicator.running = false

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
            promptBeforeRemove()
        }

        function promptBeforeRemove()
        {
            if(effectArgumentSetValuesCurrentModel !== undefined && effectArgumentSetValuesCurrentModel !== null && effectArgumentSetValuesCurrentIndex >= 0)
            {
                var effectArgumentSetValue = effectArgumentSetValuesCurrentModel.itemAt(effectArgumentSetValuesCurrentIndex)
                if(effectArgumentSetValue !== undefined && effectArgumentSetValue !== null)
                {
                    confirmDialog.confirmText = qsTr("Do you really want to remove argument set value?") + qsTr("<br/>") + qsTr("<br/>") + qsTr("Arguments set value description : ") + effectArgumentSetValue.effectArgValueDescription + qsTr("Value : ") + effectArgumentSetValue.effectArgValueValue + qsTr("<br/>");
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
            if(effectArgumentSetValuesCurrentModel !== undefined && effectArgumentSetValuesCurrentModel !== null && effectArgumentSetValuesCurrentIndex >= 0)
            {
                effectArgumentSetValuesCurrentModel.itemDeleted.connect(itemDeleted)
                effectArgumentSetValuesCurrentModel.delItem(effectArgumentSetValuesCurrentIndex)
            }
        }

        function itemDeleted()
        {
            effectArgumentSetValuesCurrentIndex = -1
            if(effectArgumentSetValuesCurrentModel !== undefined && effectArgumentSetValuesCurrentModel !== null)
            {
                effectArgumentSetValuesCurrentModel.itemDeleted.disconnect(itemDeleted)
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
            title: qsTr("&Objects")
            MenuItem { action: refreshEffectObjects }
            MenuItem { action: addNewEffectObject }
            MenuItem { action: addExistingEffectObject }
            MenuItem { action: editEffectObjectInfo }
            MenuItem { action: removeEffectObject }
        }
        AutoSizeMenu {
            title: qsTr("&Object's Artefacts")
            MenuItem { action: refreshEffectObjectArtefacts }
            MenuItem { action: addNewEffectObjectArtefact }
            MenuItem { action: addExistingEffectObjectArtefact }
            MenuItem { action: editEffectObjectArtefact }
            MenuItem { action: removeEffectObjectArtefact }
        }
        AutoSizeMenu {
            title: qsTr("&Object's Artefact's Args")
            MenuItem { action: refreshEffectObjectArtefactArgs }
            MenuItem { action: addNewEffectObjectArtefactArg }
            MenuItem { action: addFromArtefactEffectObjectArtefactArg }
            MenuItem { action: editEffectObjectArtefactArg }
            MenuItem { action: removeEffectObjectArtefactArg }
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

    Action {
        id: refreshArtefact
        text: qsTr("Refresh artifacts")
        onTriggered: {
            artefactModel.listReloaded.connect(artefactModelListReloaded)
            artefactsList.currentIndex = -1
            artefactsList.model = 0
            artefactsListBusyIndicator.visible = true
            artefactsListBusyIndicator.running = true
            artefactModel.loadList()
        }

        function artefactModelListReloaded()
        {
            artefactsListBusyIndicator.visible = false
            artefactsListBusyIndicator.running = false
            artefactModel.listReloaded.disconnect(artefactModelListReloaded)
            artefactsList.model = artefactModel
            artefactCurrentIndex = artefactModel.currentIndex
        }
    }

    FileDialog {
        id: loadArtefactFromFileDialog
        title: qsTr("Please choose an artefact file")
        folder: shortcuts.pictures
        nameFilters: [ "Shader files (*.vert *.vertex *.frag *.fragment *vsh *.fsh)","Texture files (*.png *.jpg *.jpeg)","Json data files (*.json)","3D Objext files (*.obj)","Lua script file (*.lua)" ]
        selectExisting: true
        selectMultiple: false

        property var artefact : undefined

        onAccepted: {
            setArtefactInfoFromFile(artefact, fileUrl)
        }

        onRejected: {
        }
    }

    Action {
        id: loadFromFileArtefact
        text: qsTr("Load from file artifact")

        onTriggered: {
            if(artefactModel !== undefined && artefactModel !== null)
            {
                var artefact = artefactModel.currentItem
                var fileFilter = getArtefactInfoFileFilter(artefact)
                loadArtefactFromFileDialog.artefact = artefact
                loadArtefactFromFileDialog.nameFilters = [ fileFilter ]
                loadArtefactFromFileDialog.open()
            }
        }
    }

    FileDialog {
        id: saveArtefactFromFileDialog
        title: qsTr("Please choose file to save artefact to")
        folder: shortcuts.pictures
        nameFilters: [ "Shader files (*.vert *.vertex *.frag *.fragment *vsh *.fsh)","Texture files (*.png *.jpg *.jpeg)","Json data files (*.json)","3D Objext files (*.obj)","Lua script file (*.lua)" ]
        selectExisting: false
        selectMultiple: false

        property string tmpFileName : ""

        onAccepted: {
            if(tmpFileName !== "")
            {
                mastactva.copyTmpFileTo(tmpFileName, fileUrl)
                mastactva.removeTmpFile(tmpFileName)
            }
        }

        onRejected: {
            if(tmpFileName !== "")
            {
                mastactva.removeTmpFile(tmpFileName)
            }
        }
    }

    Action {
        id: saveToFileArtefact
        text: qsTr("Save artifact to file")

        onTriggered: {
            if(artefactModel !== undefined && artefactModel !== null)
            {
                var artefact = artefactModel.currentItem
                var fileName = createArtefactInfoTmpFile(artefact)
                var fileFilter = getArtefactInfoFileFilter(artefact)
                if(fileName !== undefined)
                {
                    saveArtefactFromFileDialog.nameFilters = [ fileFilter ]
                    saveArtefactFromFileDialog.tmpFileName = fileName
                    saveArtefactFromFileDialog.open()
                }
            }
        }
    }

    Action {
        id: loadFromDBArtefact
        text: qsTr("Load from DB artifact")
        onTriggered: {
            if(artefactModel !== undefined && artefactModel !== null)
            {
                var artefact = artefactModel.currentItem
                setArtefactInfo(artefact)
            }
        }
    }

    Action {
        id: saveToDBArtefact
        text: qsTr("Save artifact to DB")
        onTriggered: {
            if(artefactModel !== undefined && artefactModel !== null)
            {
                var artefact = artefactModel.currentItem
                var fileName = createArtefactInfoTmpFile(artefact)
                if(fileName !== undefined)
                {
                    artefact.setArtefactFilenameLocalFile(fileName)
                    artefact.artefactHash = mastactva.calculateHash(fileName)

                    artefactModel.itemSet.connect(artefactModelItemSet)
                    var index = artefactModel.indexOfItem(artefact)
                    artefactModel.setItem(index, artefact)
                }
            }
        }

        function artefactModelItemSet()
        {
            artefactModel.itemSet.disconnect(artefactModelItemSet)
            if(artefactModel !== undefined && artefactModel !== null)
            {
                var artefact = artefactModel.currentItem
                artefact.fileDownloaded.connect(artefactFileDownloaded)
                artefact.downloadFile()
            }
        }

        function artefactFileDownloaded()
        {
            if(artefactModel !== undefined && artefactModel !== null)
            {
                var artefact = artefactModel.currentItem
                artefact.fileDownloaded.disconnect(artefactFileDownloaded)
                setArtefactInfo(artefact)
            }
        }
    }

    MenuBar {
        id: artefactsMenuBar
        AutoSizeMenu {
            title: qsTr("&Artefacts")
            MenuItem { action: refreshArtefact }
        }
        AutoSizeMenu {
            title: qsTr("&File")
            MenuItem { action: loadFromFileArtefact }
            MenuItem { action: saveToFileArtefact }
        }
        AutoSizeMenu {
            title: qsTr("&DB")
            MenuItem { action: loadFromDBArtefact }
            MenuItem { action: saveToDBArtefact }
        }
    }

    menuBar: appMode == 0
             ? galleriesMenuBar
             : appMode == 1
               ? effectsMenuBar
               : artefactsMenuBar

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
            TabButton {
                text: qsTr("Artefacts")
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
                                            if(imageOfGalleryDescriptionIndex === -1)
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
                            spacing: Constants.smallListViewSpacing
                            clip: true
                            model: 0
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
                                text: qsTr("Objects/Artefacts/Args")
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
                                        padding: Constants.smallViewPadding
                                    }
                                    Text
                                    {
                                        id: effectInfoCommonName
                                        width: splitEffectsInfo.width
                                        text: qsTr("")
                                        wrapMode: Text.WordWrap
                                        padding: Constants.smallViewPadding
                                    }
                                    Label {
                                        text: qsTr("Effect description : ")
                                        padding: Constants.smallViewPadding
                                    }
                                    Text
                                    {
                                        id: effectInfoCommonDescription
                                        width: splitEffectsInfo.width
                                        text: qsTr("")
                                        wrapMode: Text.WordWrap
                                        padding: Constants.smallViewPadding
                                    }
                                }
                            }
                            Item {
                                id: effectInfoObjectsArtefactsAndArguments
                                SplitView {
                                    id: slitEffectInfoObjectsArtefactsAndArguments
                                    anchors.fill: parent
                                    orientation: Qt.Vertical
                                    Rectangle {
                                        width: effectInfoObjectsArtefactsAndArguments.width

                                        SplitView.minimumHeight: effectInfoObjectsArtefactsAndArguments.height / 5
                                        SplitView.maximumHeight: effectInfoObjectsArtefactsAndArguments.height * 2 / 3
                                        SplitView.preferredHeight: effectInfoObjectsArtefactsAndArguments.height / 3

                                        ListView {
                                            id: effectObjectsList

                                            anchors.fill: parent
                                            spacing: Constants.smallListViewSpacing
                                            clip: true
                                            model: 0
                                            delegate: effectObjectsItem
                                            highlight: effectObjectsItemHighlight
                                            highlightFollowsCurrentItem: false
                                            z: 0.0

                                            BusyIndicator {
                                                id: effectObjectsListBusyIndicator
                                                anchors.centerIn: parent
                                                visible: false
                                                running: false
                                                z: 1.0
                                            }
                                        }
                                    }

                                    Rectangle {
                                        width: effectInfoObjectsArtefactsAndArguments.width

                                        SplitView.minimumHeight: effectInfoObjectsArtefactsAndArguments.height / 5
                                        SplitView.maximumHeight: effectInfoObjectsArtefactsAndArguments.height * 2 / 3
                                        SplitView.preferredHeight: effectInfoObjectsArtefactsAndArguments.height / 3

                                        ListView {
                                            id: effectObjectArtefactsList

                                            anchors.fill: parent
                                            spacing: Constants.smallListViewSpacing
                                            clip: true
                                            model: 0
                                            delegate: effectObjectArtefactsItem
                                            highlight: effectObjectArtefactsItemHighlight
                                            highlightFollowsCurrentItem: false
                                            z: 0.0

                                            BusyIndicator {
                                                id: effectObjectArtefactsListBusyIndicator
                                                anchors.centerIn: parent
                                                visible: false
                                                running: false
                                                z: 1.0
                                            }
                                        }
                                    }

                                    Rectangle {
                                        width: effectInfoObjectsArtefactsAndArguments.width
                                        SplitView.minimumHeight: effectInfoObjectsArtefactsAndArguments.height / 5
                                        SplitView.maximumHeight: effectInfoObjectsArtefactsAndArguments.height * 2 / 3
                                        SplitView.preferredHeight: effectInfoObjectsArtefactsAndArguments.height / 3

                                        ListView {
                                            id: effectObjectArtefactArgsList

                                            anchors.fill: parent
                                            spacing: Constants.smallListViewSpacing
                                            clip: true
                                            model: 0
                                            delegate: effectObjectArtefactArgsItem
                                            highlight: effectObjectArtefactArgsItemHighlight
                                            highlightFollowsCurrentItem: false
                                            z: 0.0

                                            BusyIndicator {
                                                id: effectObjectArtefactArgsListBusyIndicator
                                                anchors.centerIn: parent
                                                visible: false
                                                running: false
                                                z: 1.0
                                            }
                                        }
                                    }
                                }
                            }
                            Item {
                                id: effectInfoArguments
                                ListView {
                                    id: effectArgumentsList

                                    anchors.fill: parent
                                    spacing: Constants.smallListViewSpacing
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
                                            spacing: Constants.smallListViewSpacing
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
                                            spacing: Constants.smallListViewSpacing
                                            clip: true
                                            model: 0
                                            delegate: effectArgumentSetValuesItem
                                            highlight: effectArgumentSetValuesItemHighlight
                                            highlightFollowsCurrentItem: false
                                            z: 0.0

                                            BusyIndicator {
                                                id: effectArgumentSetValuesListBusyIndicator
                                                anchors.centerIn: parent
                                                visible: false
                                                running: false
                                                z: 1.0
                                            }
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
            Item {
                id: topicArtefacts
                SplitView {
                    id: slitArtefactsInfoDemo
                    anchors.fill: parent
                    orientation: Qt.Horizontal
                    Rectangle{
                        id: splitArtefactsList

                        SplitView.preferredWidth: Constants.leftSideBarWidth
                        SplitView.minimumWidth: Constants.leftSideBarWidth / 2
                        SplitView.maximumWidth: Constants.leftSideBarWidth * 2
                        height: parent.height

                        ListView {
                            id: artefactsList

                            anchors.fill: parent
                            spacing: Constants.smallListViewSpacing
                            clip: true
                            model: 0
                            delegate: artefactsItem
                            highlight: artefactsItemHighlight
                            highlightFollowsCurrentItem: false
                            z: 0.0

                            BusyIndicator {
                                id: artefactsListBusyIndicator
                                anchors.centerIn: parent
                                visible: false
                                running: false
                                z: 1.0
                            }
                        }
                    }
                    Rectangle{
                        id: splitArtefactInfo

                        SplitView.preferredWidth: parent.width - (splitEffects.width + Constants.leftSideBarWidth)
                        SplitView.minimumWidth: Constants.leftSideBarWidth / 2
                        SplitView.maximumWidth: parent.width - (splitEffects.width + Constants.leftSideBarWidth / 2)
                        height: parent.height
                        TabBar {
                            id: artefactTypeTabBar
                            anchors.top: parent.top
                            width: parent.width
                            TabButton {
                                text: qsTr("Shaders")
                            }
                            TabButton {
                                text: qsTr("Textures")
                            }
                            TabButton {
                                text: qsTr("JSON data")
                            }
                            TabButton {
                                text: qsTr("3D objects")
                            }
                            TabButton {
                                text: qsTr("Lua scripts")
                            }
                        }
                        StackLayout {
                            id: artefactInfo
                            anchors.top: artefactTypeTabBar.bottom
                            anchors.left: parent.left
                            width: parent.width
                            height: parent.height - effectInfoTabBar.height

                            Connections {
                                target: artefactTypeTabBar

                                function onCurrentIndexChanged()
                                {
                                    artefactInfo.currentIndex = artefactTypeTabBar.currentIndex
                                }
                            }

                            Item {
                                id: shaderArtefactInfo
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                ScrollView {
                                    id: shaderArtefactInfoShaderTextScroll
                                    anchors.top: parent.top
                                    anchors.left: parent.left
                                    width: artefactInfo.width
                                    height: artefactInfo.height
                                    clip:true
                                    TextArea {
                                        id: shaderArtefactInfoShaderText
                                        anchors.fill: parent
                                    }
                                }
                            }
                            Item {
                                id: textureArtefactInfo
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                Rectangle {
                                    id: textureArtefactInfoImageRect
                                    anchors.top: parent.top
                                    anchors.left: parent.left
                                    width: artefactInfo.width
                                    height: artefactInfo.height
                                    clip:true
                                    Image {
                                        id: textureArtefactInfoImage
                                        source: Constants.noImage
                                        fillMode: Image.PreserveAspectFit
                                        anchors.fill: parent
                                    }
                                }
                            }
                            Item {
                                id: jsonArtefactInfo
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                ScrollView {
                                    id: jsonArtefactInfoJsonTextScroll
                                    anchors.top: parent.top
                                    anchors.left: parent.left
                                    width: artefactInfo.width
                                    height: artefactInfo.height
                                    clip:true
                                    TextArea {
                                        id: jsonArtefactInfoJsonText
                                        anchors.fill: parent
                                    }
                                }
                            }
                            Item {
                                id: obj3dArtefactInfo
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                ScrollView {
                                    id: obj3dArtefactInfoObj3dTextScroll
                                    anchors.top: parent.top
                                    anchors.left: parent.left
                                    width: artefactInfo.width
                                    height: artefactInfo.height
                                    clip:true
                                    TextArea {
                                        id: obj3dArtefactInfoObj3dText
                                        anchors.fill: parent
                                    }
                                }
                            }
                            Item {
                                id: luaArtefactInfo
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                ScrollView {
                                    id: luaArtefactInfoLuaTextScroll
                                    anchors.top: parent.top
                                    anchors.left: parent.left
                                    width: artefactInfo.width
                                    height: artefactInfo.height
                                    clip:true
                                    TextArea {
                                        id: luaArtefactInfoLuaText
                                        anchors.fill: parent
                                    }
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
                                if(imageOfGalleryPointIndex === -1)
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
                                if(imageOfGallery_index === galleryImagesCurrentIndex)
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
                padding: Constants.smallListHeaderPadding

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
                text: showFullDescription ? mastactva.leftDoubleCR(effectDescription) : mastactva.readMore(effectDescription, Constants.smallListReadMoreLength, qsTr(" ..."))

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
        id: effectObjectsItem

        MouseArea {
            width: childrenRect.width
            height: childrenRect.height

            acceptedButtons: Qt.LeftButton | Qt.RightButton

            property var objecInfo: effectObjectsObjectInfo.currentItem
            property bool showFullDescription: false

            Connections {
                target: effectObjectsObjectInfo

                function onListReloaded()
                {
                    objecInfo = effectObjectsObjectInfo.currentItem
                }
            }

            onClicked:
            {
                if (mouse.button === Qt.RightButton)
                {
                    effectObjectsItemMenu.popup()
                }
                else
                {
                    effectObjectsCurrentIndex = index
                    mouse.accepted = false
                }
            }

            onPressAndHold: {
                if (mouse.source === Qt.MouseEventNotSynthesized)
                {
                    effectObjectsItemMenu.popup()
                }
            }

            onDoubleClicked: {
                showFullDescription = !showFullDescription
            }

            AutoSizeMenu {
                id: effectObjectsItemMenu
                MenuItem { action: refreshEffectObjects }
                MenuItem { action: addNewEffectObject }
                MenuItem { action: addExistingEffectObject }
                MenuItem { action: editEffectObjectInfo }
                MenuItem { action: removeEffectObject }
            }

            Column {
                id: effectObjectItemRect
                width: effectObjectsList.width

                FontMetrics{
                    id: effectObjectItemFontMetrics
                    font: effectObjectItemStepIndex.font
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: effectObjectItemStepIndexLabel
                        text: qsTr("Step : ")
                    }
                    Text {
                        id: effectObjectItemStepIndex
                        width: effectObjectsList.width - effectObjectItemStepIndexLabel.width
                        text: effectObjectsStepIndex
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: effectObjectItemNameLabel
                        text: qsTr("Name : ")
                    }
                    Text {
                        id: effectObjectItemName
                        width: effectObjectsList.width - effectObjectItemNameLabel.width
                        text: objecInfo !== undefined && objecInfo !== null ? objecInfo.effectObjectInfoName : ""
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: effectObjectItemProgrammerNameLabel
                        text: qsTr("Programmer name : ")
                    }
                    Text {
                        id: effectObjectItemProgrammerName
                        width: effectObjectsList.width - effectObjectItemProgrammerNameLabel.width
                        text: objecInfo !== undefined && objecInfo !== null ? objecInfo.effectObjectInfoProgrammerName : ""
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: effectObjectItemCreatedLabel
                        text: qsTr("Created : ")
                    }
                    Text {
                        id: effectObjectItemCreated
                        width: effectObjectsList.width - effectObjectItemCreatedLabel.width
                        text: objecInfo !== undefined && objecInfo !== null ? mastactva.dateTimeToUserStr(objecInfo.effectObjectInfoCreated) : ""
                        wrapMode: Text.Wrap
                    }
                }

                Text {
                    id: effectObjectItemDescriptionText
                    width: effectObjectsList.width
                    wrapMode: Text.WordWrap
                    text: objecInfo !== undefined && objecInfo !== null ? showFullDescription ? mastactva.leftDoubleCR(objecInfo.effectObjectInfoDescription ) : mastactva.readMore(objecInfo.effectObjectInfoDescription, Constants.smallListReadMoreLength, qsTr(" ..."))  : ""
                }
            }
        }
    }

    Component {
        id: effectObjectsItemHighlight

        Rectangle {
            SystemPalette {
                id: effectObjectItemHighlightPallete
                colorGroup: SystemPalette.Active
            }

            border.color: effectObjectItemHighlightPallete.highlight
            border.width: 2
            radius: 5
            y: (effectObjectsList.currentItem !== undefined && effectObjectsList.currentItem !== null) ? effectObjectsList.currentItem.y : 0
            x: (effectObjectsList.currentItem !== undefined && effectObjectsList.currentItem !== null) ? effectObjectsList.currentItem.x : 0
            width: (effectObjectsList.currentItem !== undefined && effectObjectsList.currentItem !== null) ? effectObjectsList.currentItem.width : 0
            height: (effectObjectsList.currentItem !== undefined && effectObjectsList.currentItem !== null) ? effectObjectsList.currentItem.height : 0
        }
    }

    Component {
        id: effectObjectArtefactsItem

        MouseArea {
            width: childrenRect.width
            height: childrenRect.height

            acceptedButtons: Qt.LeftButton | Qt.RightButton

            property var objecArtefact: objectArtefactArtefact.currentItem
            property bool showFullDescription: false

            Connections {
                target: objectArtefactArtefact

                function onListReloaded()
                {
                    objecArtefact = objectArtefactArtefact.currentItem
                }
            }

            onClicked:
            {
                if (mouse.button === Qt.RightButton)
                {
                    effectObjectArtefactsItemMenu.popup()
                }
                else
                {
                    effectObjectArtefactsCurrentIndex = index
                    mouse.accepted = false
                }
            }

            onPressAndHold: {
                if (mouse.source === Qt.MouseEventNotSynthesized)
                {
                    effectObjectArtefactsItemMenu.popup()
                }
            }

            onDoubleClicked: {
                showFullDescription = !showFullDescription
            }

            AutoSizeMenu {
                id: effectObjectArtefactsItemMenu
                MenuItem { action: refreshEffectObjectArtefacts }
                MenuItem { action: addNewEffectObjectArtefact }
                MenuItem { action: addExistingEffectObjectArtefact }
                MenuItem { action: editEffectObjectArtefact }
                MenuItem { action: removeEffectObjectArtefact }
            }

            Column {
                id: effectObjectArtefactItemRect
                width: effectObjectArtefactsList.width

                FontMetrics{
                    id: effectObjectArtefactItemFontMetrics
                    font: effectObjectArtefactItemStepIndex.font
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: effectObjectArtefactItemStepIndexLabel
                        text: qsTr("Step : ")
                    }
                    Text {
                        id: effectObjectArtefactItemStepIndex
                        width: effectObjectArtefactsList.width - effectObjectArtefactItemStepIndexLabel.width
                        text: objectArtefactStepIndex
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: effectObjectArtefactItemTypeLabel
                        text: qsTr("Type : ")
                    }
                    Text {
                        id: effectObjectArtefactItemType
                        width: effectObjectArtefactsList.width - effectObjectArtefactItemTypeLabel.width
                        text: objecArtefact !== undefined && objecArtefact !== null && artefactTypeModel.findItemById(objecArtefact.artefactTypeId) !== null ? artefactTypeModel.findItemById(objecArtefact.artefactTypeId).artefactTypeType : ""
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: effectObjectArtefactItemNameLabel
                        text: qsTr("Name : ")
                    }
                    Text {
                        id: effectObjectArtefactItemName
                        width: effectObjectArtefactsList.width - effectObjectArtefactItemNameLabel.width
                        text: objecArtefact !== undefined && objecArtefact !== null ? objecArtefact.artefactName : ""
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: effectObjectArtefactItemFileNameLabel
                        text: qsTr("File name : ")
                    }
                    Text {
                        id: effectObjectArtefactItemFileName
                        width: effectObjectArtefactsList.width - effectObjectArtefactItemFileNameLabel.width
                        text: objecArtefact !== undefined && objecArtefact !== null ? objecArtefact.artefactFilename : ""
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: effectObjectArtefactItemHashLabel
                        text: qsTr("Hash : ")
                    }
                    Text {
                        id: effectObjectArtefactItemHash
                        width: effectObjectArtefactsList.width - effectObjectArtefactItemHashLabel.width
                        text: objecArtefact !== undefined && objecArtefact !== null ? objecArtefact.artefactHash : ""
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: effectObjectArtefactItemCreatedLabel
                        text: qsTr("Create : ")
                    }
                    Text {
                        id: effectObjectArtefactItemCreated
                        width: effectObjectArtefactsList.width - effectObjectArtefactItemCreatedLabel.width
                        text: objecArtefact !== undefined && objecArtefact !== null ? mastactva.dateTimeToUserStr(objecArtefact.artefactCreated) : ""
                        wrapMode: Text.Wrap
                    }
                }

                Text {
                    id: effectObjectArtefactItemDescriptionText
                    width: effectObjectArtefactsList.width
                    wrapMode: Text.WordWrap
                    text: objecArtefact !== undefined && objecArtefact !== null ? showFullDescription ? mastactva.leftDoubleCR(objecArtefact.artefactDescription ) : mastactva.readMore(objecArtefact.artefactDescription, Constants.smallListReadMoreLength, qsTr(" ..."))  : ""
                }
            }
        }
    }

    Component {
        id: effectObjectArtefactsItemHighlight

        Rectangle {
            SystemPalette {
                id: effectObjectArtefactItemHighlightPallete
                colorGroup: SystemPalette.Active
            }

            border.color: effectObjectArtefactItemHighlightPallete.highlight
            border.width: 2
            radius: 5
            y: (effectObjectArtefactsList.currentItem !== undefined && effectObjectArtefactsList.currentItem !== null) ? effectObjectArtefactsList.currentItem.y : 0
            x: (effectObjectArtefactsList.currentItem !== undefined && effectObjectArtefactsList.currentItem !== null) ? effectObjectArtefactsList.currentItem.x : 0
            width: (effectObjectArtefactsList.currentItem !== undefined && effectObjectArtefactsList.currentItem !== null) ? effectObjectArtefactsList.currentItem.width : 0
            height: (effectObjectArtefactsList.currentItem !== undefined && effectObjectArtefactsList.currentItem !== null) ? effectObjectArtefactsList.currentItem.height : 0
        }
    }

    Component {
        id: artefactsItem

        MouseArea {
            width: childrenRect.width
            height: childrenRect.height

            acceptedButtons: Qt.LeftButton | Qt.RightButton

            property bool showFullDescription: false

            onClicked:
            {
                if (mouse.button === Qt.RightButton)
                {
                    //effectObjectArtefactsItemMenu.popup()
                }
                else
                {
                    artefactCurrentIndex = index
                    mouse.accepted = false
                }
            }

            onPressAndHold: {
                if (mouse.source === Qt.MouseEventNotSynthesized)
                {
                    effectObjectArtefactsItemMenu.popup()
                }
            }

            onDoubleClicked: {
                showFullDescription = !showFullDescription
            }

            /*AutoSizeMenu {
                id: effectObjectArtefactsItemMenu
                MenuItem { action: refreshEffectObjectArtefacts }
                MenuItem { action: addNewEffectObjectArtefact }
                MenuItem { action: addExistingEffectObjectArtefact }
                MenuItem { action: editEffectObjectArtefact }
                MenuItem { action: removeEffectObjectArtefact }
            }*/

            Column {
                id: artefactItemRect
                width: artefactsList.width

                FontMetrics{
                    id: artefactItemFontMetrics
                    font: artefactItemType.font
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: artefactItemTypeLabel
                        text: qsTr("Type : ")
                    }
                    Text {
                        id: artefactItemType
                        width: artefactsList.width - artefactItemTypeLabel.width
                        text: artefactTypeModel.findItemById(artefactTypeId) !== null ? artefactTypeModel.findItemById(artefactTypeId).artefactTypeType : ""
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: artefactItemNameLabel
                        text: qsTr("Name : ")
                    }
                    Text {
                        id: artefactItemName
                        width: artefactsList.width - artefactItemNameLabel.width
                        text: artefactName
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: artefactItemFileNameLabel
                        text: qsTr("File name : ")
                    }
                    Text {
                        id: artefactItemFileName
                        width: artefactsList.width - artefactItemFileNameLabel.width
                        text: artefactFilename
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: artefactItemHashLabel
                        text: qsTr("Hash : ")
                    }
                    Text {
                        id: artefactItemHash
                        width: artefactsList.width - artefactItemHashLabel.width
                        text: artefactHash
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: artefactItemCreatedLabel
                        text: qsTr("Create : ")
                    }
                    Text {
                        id: artefactItemCreated
                        width: artefactsList.width - artefactItemCreatedLabel.width
                        text: mastactva.dateTimeToUserStr(artefactCreated)
                        wrapMode: Text.Wrap
                    }
                }

                Text {
                    id: artefactItemDescriptionText
                    width: artefactsList.width
                    wrapMode: Text.WordWrap
                    text: showFullDescription ? mastactva.leftDoubleCR(artefactDescription ) : mastactva.readMore(artefactDescription, Constants.smallListReadMoreLength, qsTr(" ..."))
                }
            }
        }
    }

    Component {
        id: artefactsItemHighlight

        Rectangle {
            SystemPalette {
                id: artefactItemHighlightPallete
                colorGroup: SystemPalette.Active
            }

            border.color: artefactItemHighlightPallete.highlight
            border.width: 2
            radius: 5
            y: (artefactsList.currentItem !== undefined && artefactsList.currentItem !== null) ? artefactsList.currentItem.y : 0
            x: (artefactsList.currentItem !== undefined && artefactsList.currentItem !== null) ? artefactsList.currentItem.x : 0
            width: (artefactsList.currentItem !== undefined && artefactsList.currentItem !== null) ? artefactsList.currentItem.width : 0
            height: (artefactsList.currentItem !== undefined && artefactsList.currentItem !== null) ? artefactsList.currentItem.height : 0
        }
    }

    Component {
        id: effectObjectArtefactArgsItem

        MouseArea {
            width: childrenRect.width
            height: childrenRect.height

            acceptedButtons: Qt.LeftButton | Qt.RightButton

            property bool showFullDescription: false

            onClicked:
            {
                if (mouse.button === Qt.RightButton)
                {
                    effectObjectArtefactArgsItemMenu.popup()
                }
                else
                {
                    effectObjectArtefactArgsCurrentIndex = index
                    mouse.accepted = false
                }
            }

            onPressAndHold: {
                if (mouse.source === Qt.MouseEventNotSynthesized)
                {
                    effectObjectArtefactArgsItemMenu.popup()
                }
            }

            onDoubleClicked: {
                showFullDescription = !showFullDescription
            }

            AutoSizeMenu {
                id: effectObjectArtefactArgsItemMenu
                MenuItem { action: refreshEffectObjectArtefactArgs }
                MenuItem { action: addNewEffectObjectArtefactArg }
                MenuItem { action: addFromArtefactEffectObjectArtefactArg }
                MenuItem { action: editEffectObjectArtefactArg }
                MenuItem { action: removeEffectObjectArtefactArg }
            }

            Column {
                id: effectObjectArtefactArgItemRect
                width: effectObjectArtefactArgsList.width

                FontMetrics{
                    id: effectObjectArtefactItemFontMetrics
                    font: effectObjectArtefactArgNameIndex.font
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: effectObjectArtefactArgItemNameLabel
                        text: qsTr("Name : ")
                    }
                    Text {
                        id: effectObjectArtefactArgNameIndex
                        width: effectObjectArtefactArgsList.width - effectObjectArtefactArgItemNameLabel.width
                        text: artefactArgName
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: effectObjectArtefactArgItemTypeLabel
                        text: qsTr("Type : ")
                    }
                    Text {
                        id: effectObjectArtefactArgItemType
                        width: effectObjectArtefactArgsList.width - effectObjectArtefactArgItemTypeLabel.width
                        text: artefactArgTypeModel.findItemById(artefactArgArgTypeId) !== null ? artefactArgTypeModel.findItemById(artefactArgArgTypeId).artefactArgTypeType : ""
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: effectObjectArtefactArgItemStorageLabel
                        text: qsTr("Storage : ")
                    }
                    Text {
                        id: effectObjectArtefactArgItemStorage
                        width: effectObjectArtefactArgsList.width - effectObjectArtefactArgItemStorageLabel.width
                        text: artefactArgStorageModel.findItemById(artefactArgArgStorageId) !== null ? artefactArgStorageModel.findItemById(artefactArgArgStorageId).artefactArgStorageStorage : ""
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: effectObjectArtefactArgItemDefaultValueLabel
                        text: qsTr("Default value : ")
                    }
                    Text {
                        id: effectObjectArtefactArgDefaultValueIndex
                        width: effectObjectArtefactArgsList.width - effectObjectArtefactArgItemDefaultValueLabel.width
                        text: artefactArgDefaultValue
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: effectObjectArtefactArgItemCreatedLabel
                        text: qsTr("Created : ")
                    }
                    Text {
                        id: effectObjectArtefactArgCreatedIndex
                        width: effectObjectArtefactArgsList.width - effectObjectArtefactArgItemCreatedLabel.width
                        text: mastactva.dateTimeToUserStr(artefactArgCreated)
                        wrapMode: Text.Wrap
                    }
                }

                Text {
                    id: effectObjectArtefactArgItemDescriptionText
                    width: effectObjectArtefactArgsList.width
                    wrapMode: Text.WordWrap
                    text: showFullDescription ? mastactva.leftDoubleCR(artefactArgDescription) : mastactva.readMore(artefactArgDescription, Constants.smallListReadMoreLength, qsTr(" ..."))
                }
            }
        }
    }

    Component {
        id: effectObjectArtefactArgsItemHighlight

        Rectangle {
            SystemPalette {
                id: effectObjectArtefactArgItemHighlightPallete
                colorGroup: SystemPalette.Active
            }

            border.color: effectObjectArtefactArgItemHighlightPallete.highlight
            border.width: 2
            radius: 5
            y: (effectObjectArtefactArgsList.currentItem !== undefined && effectObjectArtefactArgsList.currentItem !== null) ? effectObjectArtefactArgsList.currentItem.y : 0
            x: (effectObjectArtefactArgsList.currentItem !== undefined && effectObjectArtefactArgsList.currentItem !== null) ? effectObjectArtefactArgsList.currentItem.x : 0
            width: (effectObjectArtefactArgsList.currentItem !== undefined && effectObjectArtefactArgsList.currentItem !== null) ? effectObjectArtefactArgsList.currentItem.width : 0
            height: (effectObjectArtefactArgsList.currentItem !== undefined && effectObjectArtefactArgsList.currentItem !== null) ? effectObjectArtefactArgsList.currentItem.height : 0
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
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: effectArgumentsItemObjectArtefactLabel
                        text: qsTr("Object artefact :")
                    }
                    Text {
                        id: effectArgumentsItemObjectArtefact
                        width: effectArgumentsList.width - effectArgumentsItemObjectArtefactLabel.width
                        text: effectArgObjectArtefactId
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: effectArgumentsItemTypeLabel
                        text: qsTr("Type : ")
                    }
                    Text {
                        id: effectArgumentsItemType
                        width: effectArgumentsList.width - effectArgumentsItemTypeLabel.width
                        text: artefactArgTypeModel.findItemById(effectArgArgTypeId) !== null ? artefactArgTypeModel.findItemById(effectArgArgTypeId).artefactArgTypeType : ""
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.smallListHeaderPadding
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
                    padding: Constants.smallListHeaderPadding
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
                    text: showFullDescription ? mastactva.leftDoubleCR(effectArgDescription) : mastactva.readMore(effectArgDescription, Constants.smallListReadMoreLength, qsTr(" ..."))
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
                    padding: Constants.smallListHeaderPadding
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
                    text: showFullDescription ? mastactva.leftDoubleCR(effectArgSetDescription) : mastactva.readMore(effectArgSetDescription, Constants.smallListReadMoreLength, qsTr(" ..."))
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
                    effectArgumentSetValuesItemArgType.text = artefactArgTypeModel.findItemById(effectArgValueArg.currentItem.effectArgArgTypeId).artefactArgTypeType
                    effectArgumentSetValuesItemArgName.text = effectArgValueArg.currentItem.effectArgName
                    effectArgumentSetValuesItemArgDefaultValue.text = effectArgValueArg.currentItem.effectArgDefaultValue
                }
            }

            Column {
                id: effectArgumentSetValuesItemRect
                width: effectArgumentSetValuesList.width

                Row {
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: effectArgumentSetValuesItemArgTypeLabel
                        text: qsTr("Argument type : ")
                    }
                    Text {
                        id: effectArgumentSetValuesItemArgType
                        width: effectArgumentSetValuesList.width - effectArgumentSetValuesItemArgTypeLabel.width
                        text: effectArgValueArg.isListLoaded() && effectArgValueArg.currentItem !== null ? artefactArgTypeModel.findItemById(effectArgValueArg.currentItem.effectArgArgTypeId) !== null ? artefactArgTypeModel.findItemById(effectArgValueArg.currentItem.effectArgArgTypeId).artefactArgTypeType : "" : ""
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.smallListHeaderPadding
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
                    padding: Constants.smallListHeaderPadding
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
                    padding: Constants.smallListHeaderPadding
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
                    text: showFullDescription ? mastactva.leftDoubleCR(effectArgValueDescription) : mastactva.readMore(effectArgValueDescription, Constants.smallListReadMoreLength, qsTr(" ..."))
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
                    imageOfGalleryNextImageEffectListItemDescription.text = imagePointEffectEffect !== undefined && imagePointEffectEffect !== null && imagePointEffectEffect.isListLoaded() && imagePointEffectEffect.currentItem !== null ? showFullDescription ? mastactva.leftDoubleCR(imagePointEffectEffect.currentItem.effectDescription) : mastactva.readMore(imagePointEffectEffect.currentItem.effectDescription, Constants.smallListReadMoreLength, qsTr(" ...")) : ""
                }
            }

            Connections {
                target: imagePointEffectArgSet
                function onListReloaded()
                {
                    imageOfGalleryNextImageEffectListItemEasing.text = imagePointEffectArgSet !== undefined && imagePointEffectArgSet !== null && imagePointEffectArgSet.isListLoaded() && imagePointEffectArgSet.currentItem !== null && easingTypeModel.findItemById(imagePointEffectArgSet.currentItem.effectArgSetEasingId) !== null ? easingTypeModel.findItemById(imagePointEffectArgSet.currentItem.effectArgSetEasingId).easingTypeType : ""
                    imageOfGalleryNextImageEffectListItemArgSetDescription.text = imagePointEffectArgSet !== undefined && imagePointEffectArgSet !== null && imagePointEffectArgSet.isListLoaded() && imagePointEffectArgSet.currentItem !== null ? showFullDescription ? mastactva.leftDoubleCR(imagePointEffectArgSet.currentItem.effectArgSetDescription) : mastactva.readMore(imagePointEffectArgSet.currentItem.effectArgSetDescription, Constants.smallListReadMoreLength, qsTr(" ...")) : ""
                }
            }

            Column {
                id: imageOfGalleryNextImageEffectListItemRect
                width: imageOfGalleryNextImageEffectList.width

                Row {
                    padding: Constants.smallListHeaderPadding
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
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: imageOfGalleryNextImageEffectListItemDescriptionLabel
                        text: qsTr("Effect description : ")
                    }
                    Text {
                        id: imageOfGalleryNextImageEffectListItemDescription
                        width: imageOfGalleryNextImageEffectList.width - imageOfGalleryNextImageEffectListItemDescriptionLabel.width
                        text: imagePointEffectEffect !== undefined && imagePointEffectEffect !== null && imagePointEffectEffect.isListLoaded() && imagePointEffectEffect.currentItem !== null ? showFullDescription ? mastactva.leftDoubleCR(imagePointEffectEffect.currentItem.effectDescription) : mastactva.readMore(imagePointEffectEffect.currentItem.effectDescription, Constants.smallListReadMoreLength, qsTr(" ...")) : ""
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    padding: Constants.smallListHeaderPadding
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
                    padding: Constants.smallListHeaderPadding
                    Label {
                        id: imageOfGalleryNextImageEffectListItemArgSetDescriptionLabel
                        text: qsTr("Effect argument set description : ")
                    }
                    Text {
                        id: imageOfGalleryNextImageEffectListItemArgSetDescription
                        width: imageOfGalleryNextImageEffectList.width - imageOfGalleryNextImageEffectListItemArgSetDescriptionLabel.width
                        text: imagePointEffectArgSet !== undefined && imagePointEffectArgSet !== null && imagePointEffectArgSet.isListLoaded() && imagePointEffectArgSet.currentItem !== null ? showFullDescription ? mastactva.leftDoubleCR(imagePointEffectArgSet.currentItem.effectArgSetDescription) : mastactva.readMore(imagePointEffectArgSet.currentItem.effectArgSetDescription, Constants.smallListReadMoreLength, qsTr(" ...")) : ""
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

