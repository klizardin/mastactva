import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.1
import MastactvaQuizEditor 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


ApplicationWindow {
    id: root
    visible: true
    width: Constants.width
    height: Constants.height
    title: qsTr("Mastactva Quiz Editor")

    property alias galleryCurrentIndex: mastactva.galleryIndex
    property alias galleryImagesCurrentIndex: mastactva.imageOfGalleryIndex
    property bool showImagePoints: false

    MastactvaAPI {
        id: mastactva
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
            mastactva.createNewGallery(fieldDescription, fieldKeywords, fieldPointsToPass)
            mastactva.onNewGalleryCreated.connect(onNewGalleryCreated)
        }

        function onNewGalleryCreated()
        {
            mastactva.reloadGalleriesModel()
            mastactva.galleryReloaded.connect(onGalleryReloaded)
        }

        function onGalleryReloaded()
        {
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
            mastactva.editGallery(fieldId, fieldDescription, fieldKeywords, fieldCreated, fieldPointsToPass)
            mastactva.onGalleryEdited.connect(onNewGalleryCreated)
        }

        function onNewGalleryCreated()
        {
            mastactva.reloadGalleriesModel()
            mastactva.galleryReloaded.connect(onGalleryReloaded)
        }

        function onGalleryReloaded()
        {
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
            fieldImageFileName = ""
            fieldTopImage = false
        }

        onAccepted: {
            mastactva.addImage(currentGalleryId, fieldImageFileName, fieldTopImage)
            mastactva.onImageAdded.connect(onImageAdded)
        }

        function onImageAdded()
        {
            mastactva.reloadGalleriesModel()
            mastactva.reloadAllImagesOfGalleryViewModel()
            mastactva.galleryReloaded.connect(onGalleryReloaded)
            mastactva.imagesOfGalleryReloaded.connect(onGalleryImagesReloaded)
        }

        function onGalleryReloaded()
        {
            galleries.currentIndex = galleryCurrentIndex;
        }

        function onGalleryImagesReloaded()
        {
            galleries.images_of_gallery = galleryImagesCurrentIndex;
        }
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
            editCurrentGalleryDialog.open()
        }
    }

    Action {
        id: addGalleryImage
        text: qsTr("Add &Image To Current Gallery")
        onTriggered: {
            addImageDialog.open()
        }
    }

    Action {
        id: removeCurrentImageOfGallery
        text: qsTr("&Remove Current Image")
        onTriggered: {
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
        id: showImagePointsAction
        text: qsTr("Show image &points")
        onTriggered: {
            showImagePoints = !showImagePoints
        }
    }

    menuBar: MenuBar {
        Menu {
            title: qsTr("Galleries")
            MenuItem { action: refreshGalleriesModel }
            MenuItem { action: createNewGallery }
            MenuItem { action: editCurrentGallery }
            MenuItem { action : addGalleryImage }
        }
        Menu {
            title: qsTr("All Images of Gallery")
            MenuItem { action: refreshAllGalleryImagesModel }
            MenuItem { action: removeCurrentImageOfGallery }
            MenuItem {
                action: showImagePointsAction
                checkable: true
                checked: showImagePoints
            }
        }
        Menu {
            title: qsTr("Test")
            MenuItem {  }
        }
    }

    Row {
        width : parent.width
        height: parent.height

        Rectangle{

            width: Constants.leftSideBarWidth
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

        Rectangle {
            width: Constants.leftSideBarWidthSpace
            height: parent.height
        }

        Rectangle{

            width: root.width - Constants.leftSideBarWidth
            height: parent.height

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

            Rectangle {
                width: Constants.leftSideBarWidth
                height: (Constants.leftSideBarWidth / Constants.aspectX) * Constants.aspectY
                border.width: 2
                border.color: galleryCurrentIndex === gallery_index ? galleryItemPallete.highlight : galleryItemPallete.window

                SwipeView {
                    id: gallery_images

                    x: Constants.galleryImageSpacing / 2
                    y: Constants.galleryImageSpacing / 2
                    width: (Constants.leftSideBarWidth - Constants.galleryImageSpacing)
                    height: ((Constants.leftSideBarWidth - Constants.galleryImageSpacing) / Constants.aspectX) * Constants.aspectY
                    clip: true

                    Repeater {
                        id: gallery_image
                        model: images
                        Image {
                            id: image_of_gallery_view
                            width: (Constants.leftSideBarWidth - Constants.galleryImageSpacing)
                            height: ((Constants.leftSideBarWidth - Constants.galleryImageSpacing) / Constants.aspectX) * Constants.aspectY
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
                                        galleryImagesCurrentIndex = -1
                                        images_of_gallery.model.galleryIndex = galleryCurrentIndex
                                        galleries.model.currentIndex = galleryCurrentIndex
                                        editCurrentGalleryDialog.setFields(galleries.model.getCurrentItem())
                                        addImageDialog.currentGalleryId = galleries.model.currentId
                                        images_of_gallery.model.galleryCurrentItem
                                        mouse.accepted = false
                                    }
                                }

                                onPressAndHold: {
                                    if (mouse.source === Qt.MouseEventNotSynthesized)
                                        galleryContextMenu.popup()
                                }

                                Menu {
                                    id: galleryContextMenu
                                    MenuItem { action: refreshGalleriesModel }
                                    MenuItem { action: createNewGallery }
                                    MenuItem { action: editCurrentGallery }
                                    MenuItem { action : addGalleryImage }
                                }
                            }
                        }
                    }
                }

            }


            PageIndicator {
               x:(Constants.leftSideBarWidth-width)/2
               height: Constants.leftSideBarPageIndicatorHeight
               currentIndex: gallery_images.currentIndex
               count: gallery_images.count
            }

            Text {
                id : gallery_description
                text: GalleryFunctions.description_first_part(description)
                x: (Constants.leftSideBarWidth-width)/2
                width: Constants.leftSideBarTextWidth
                wrapMode: Text.WordWrap
            }
        }
    }

    Component {
        id: gallery_image

        Rectangle {

            property int imageOfGallery_index: index

            SystemPalette {
                id: imageOfGalleryItemPallete
                colorGroup: SystemPalette.Active
            }

            width: (Constants.height * Constants.aspectX) / Constants.aspectY
            height: Constants.height
            z: 0
            border.width: 2
            border.color: galleryImagesCurrentIndex === imageOfGallery_index ? imageOfGalleryItemPallete.highlight : imageOfGalleryItemPallete.window

            Image  {
                id: image_of_gallery
                x: Constants.imageOfGalleryImageSpacing / 2
                y: Constants.imageOfGalleryImageSpacing / 2
                width: ((Constants.height - Constants.imageOfGalleryImageSpacing) * Constants.aspectX) / Constants.aspectY
                height: (Constants.height - Constants.imageOfGalleryImageSpacing)
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
                            galleryImagesCurrentIndex = imageOfGallery_index
                            mouse.accepted = false
                        }
                    }

                    onPressAndHold: {
                        if (mouse.source === Qt.MouseEventNotSynthesized)
                            imagesOfGalleryContextMenu.popup()
                    }

                    Menu {
                        id: imagesOfGalleryContextMenu
                        MenuItem { action: refreshAllGalleryImagesModel }
                        MenuItem { action: removeCurrentImageOfGallery }
                        MenuItem {
                            action: showImagePointsAction
                            checkable: true
                            checked: showImagePoints
                        }
                    }
                }
            }

            /*VoronoyDiagramItem {
                anchors.fill: parent
                id: voronoyDiagram
                visible: showImagePoints
                opacity: 0.5
                z: 0.5
                model: image_points
            }*/

            /*CustomPaintedItem {
                anchors.fill: parent
                id: testFBODraw
                visible: showImagePoints
                opacity: 0.5
                z: 0.5
            }*/

            FBORenderer {
                anchors.fill: parent
                id: testFBODraw
                visible: showImagePoints
                opacity: 0.5
                z: 0.5
                model: image_points
            }

            Item {
                anchors.fill: parent
                visible: showImagePoints
                opacity: 0.5
                z: 1.0
                Repeater {
                    model: image_points
                    Canvas {
                        id: canvas
                        anchors.fill: parent

                        onPaint: {
                            var ctx = canvas.getContext("2d")
                            var ptx = width - image_of_gallery.paintedWidth + (imagePoint_x * image_of_gallery.paintedWidth)
                            var pty = height - image_of_gallery.paintedHeight + (imagePoint_y * image_of_gallery.paintedHeight)
                            ctx.fillStyle = "#000080"
                            ctx.ellipse(ptx - 5, pty - 5, 10, 10)
                            ctx.fill()
                        }
                    }
                }
            }
        }
    }
}

