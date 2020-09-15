import QtQuick 2.12
import QtQuick.Controls 2.5
import org.mastactva 1.0

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("Mastactva Quiz Editor")

    ScrollView {
        anchors.fill: parent

        ListView {
            id: galleries
            width: parent.width
            model: GalleryEditViewModel {}
            delegate: Item {
                id: gallery
                SwipeView {
                    id: gallery_images
                    width: galleries.width
                    height: 100
                    Repeater {
                        model: gallery.images
                        Image {
                            width: gallery_images.width
                            height: gallery_images.height
                            source: image
                            fillMode: Image.Stretch
                        }
                    }
                }
            }
        }
    }
}
