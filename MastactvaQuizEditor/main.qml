import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.1
import org.mastactva 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


ApplicationWindow {
    id: root
    visible: true
    width: Constants.width
    height: Constants.height
    title: qsTr("Mastactva Quiz Editor")

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
        text: qsTr("&Create New Gallery Dialog")
        onTriggered: {
            createNewGalleryDialog.open()
        }
    }

    Action {
        id: editCurrentGallery
        text: qsTr("&Edit Current Gallery Dialog")
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

                model: GalleryEditViewModel {
                    objectName: "GalleryModel"
                }

                delegate: gallery
            }
        }

        Rectangle{

            width: root.width - Constants.leftSideBarWidth
            height: parent.height

            ListView {
                id: images_of_gallery

                anchors.fill: parent
                orientation: ListView.Horizontal
                clip: true

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
            property int gallery_index: index

            SwipeView {
                id: gallery_images

                width: Constants.leftSideBarWidth
                height: (Constants.leftSideBarWidth / Constants.aspectX) * Constants.aspectY
                clip: true

                Repeater {
                    id: gallery_image
                    model: images
                    Image {
                        id: image_of_gallery_view
                        width: Constants.leftSideBarWidth
                        height: (Constants.leftSideBarWidth / Constants.aspectX) * Constants.aspectY
                        source: image
                        fillMode: Image.PreserveAspectFit

                        MouseArea {
                            anchors.fill: parent
                            onClicked:
                            {
                                images_of_gallery.model.galleryIndex = gallery_index
                                galleries.model.currentIndex = gallery_index
                                editCurrentGalleryDialog.setFields(galleries.model.getCurrentItem())
                                addImageDialog.currentGalleryId = galleries.model.currentId
                                images_of_gallery.model.galleryCurrentItem
                                mouse.accepted = false
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

        Image  {
            id: image_of_gallery
            width: (Constants.height * Constants.aspectX) / Constants.aspectY
            height: Constants.height
            source: image
            fillMode: Image.PreserveAspectFit
        }
    }
}

