import QtQuick 2.12
import QtQuick.Controls 2.5
import org.mastactva 1.0
import "GalleryFunctions.js" as GalleryFunctions


ApplicationWindow {
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

    ScrollView {
        anchors.fill: parent
        width: parent.width

        ListView {
            id: galleries
            width: parent.width
            model: GalleryEditViewModel {
                objectName: "GalleryModel"
            }
            delegate: Item {
                id: gallery
                SwipeView {
                    id: gallery_images
                    width: galleries.width
                    height: 200
                    Repeater {
                        model: images
                        Image {
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
}
