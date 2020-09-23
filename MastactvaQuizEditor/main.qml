import QtQuick 2.12
import QtQuick.Controls 2.5
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

    Action {
        id: refreshGalleriesModel
        text: qsTr("&Refresh Galleries")
        onTriggered: mastactva.reloadGalleriesModel()
    }

    menuBar: MenuBar {
        Menu {
            title: qsTr("Galleries")
            MenuItem { action: refreshGalleriesModel }
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
                height: (Constants.leftSideBarWidth / 3) * 4
                clip: true

                Repeater {
                    id: gallery_image
                    model: images
                    Image {
                        id: image_of_gallery_view
                        source: image
                        fillMode: Image.Stretch

                        MouseArea {
                            anchors.fill: parent
                            onClicked:
                            {
                                images_of_gallery.model.galleryIndex = gallery_index
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
            width: (Constants.height * 3) / 4
            height: Constants.height
            source: image
            fillMode: Image.Stretch
        }
    }
}

