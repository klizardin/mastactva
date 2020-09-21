import QtQuick 2.12
import QtQuick.Controls 2.5
import org.mastactva 1.0
import "GalleryFunctions.js" as GalleryFunctions


ApplicationWindow {
    id: root
    width: 640
    height: 480
    visible: true
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

            width: 210
            height: parent.height

            ListView {
                id: galleries

                anchors.fill: parent

                model: GalleryEditViewModel {
                    objectName: "GalleryModel"
                }

                delegate: gallery
            }
        }
        Rectangle{

            width: 640 - 210
            height: parent.height

            ListView {
                id: images_of_gallery

                anchors.fill: parent
                orientation: ListView.Horizontal
                clip: true

                model: GalleryImagesModel {
                    objectName: "AllGalleryImagesModel"
                    galleryViewImages: false
                    // galleryId: galleries.currentIndex
                    // galleryIndex: galleries.currentIndex
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

                width: 210
                height: (210 / 3) * 4
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
               x:(210-width)/2
               height: 20
               currentIndex: gallery_images.currentIndex
               count: gallery_images.count
            }

            Text {
                id : gallery_description
                text: GalleryFunctions.description_first_part(description)
                x: 5
                width: 200
                wrapMode: Text.WordWrap
            }
        }
    }

    Component {
        id: gallery_image

        Image  {
            id: image_of_gallery
            width: (480 * 3) / 4
            height: 480
            source: image
            fillMode: Image.Stretch
        }
    }
}

