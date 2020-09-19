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

        ScrollView {
            id: scroll_galleries
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            // anchors.left: parent.left
            width: parent.width/3

            ListView {
                id: galleries

                anchors.top: parent.top
                width: parent.width
                height: parent.height

                model: GalleryEditViewModel {
                    objectName: "GalleryModel"
                }

                delegate: Item {
                    id: gallery

                    width: parent.width
                    height: parent.height

                    SwipeView {
                        id: gallery_images

                        width: parent.width
                        height: (parent.width * 4) / 3

                        Repeater {
                            model: images
                            Image {
                                id: image_of_gallery_view
                                width: gallery_images.width
                                height: gallery_images.height
                                source: image
                                fillMode: Image.Stretch
                            }
                        }
                    }

                    Text {
                        id : gallery_description
                        anchors.top: gallery_images.bottom
                        text: GalleryFunctions.description_first_part(description)
                        width: (gallery_images.width*9)/10
                        x: gallery_images.width/10/2
                    }

                    PageIndicator {
                       anchors.top: gallery_description.bottom
                       x:(gallery_images.width-width)/2
                       currentIndex: gallery_images.currentIndex
                       count: gallery_images.count
                    }
                }
            }
        }
        ListView {
            id: images_of_gallery
            width: (parent.width * 2) /3
            height: parent.height
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            model: GalleryImagesModel {
                galleryViewImages: false
                objectName: "AllGalleryImagesModel"
            }

            delegate : Image  {
                id: image_of_gallery
                width: (parent.height * 3) / 4
                height: parent.height
                source: image
                fillMode: Image.Stretch
            }
        }
    }
}
