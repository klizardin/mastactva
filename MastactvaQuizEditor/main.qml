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

    //Row {
    //    width : parent.width
    //    height: parent.height

       //ScrollView {
       //     id: scroll_galleries

       //     anchors.fill: parent
       //     width: 210
       //     height: parent.height

            ListView {
                id: galleries

                anchors.fill: parent
                //width: 210
                //height: parent.height

                model: GalleryEditViewModel {
                    objectName: "GalleryModel"
                }

                delegate: gallery
            }
        //}
        //ListView {
        //    id: images_of_gallery

        //    width: 640 - 210
        //    height : parent.height

        //    model: GalleryImagesModel {
        //        objectName: "AllGalleryImagesModel"
        //        galleryViewImages: false
        //        galleryId: galleries.model[galleries.currentIndex].galleryid
        //    }

        //    delegate : Image  {
        //        id: image_of_gallery
        //        width: (480 * 3) / 4
        //        height: 480
        //        source: image
        //        fillMode: Image.Stretch
        //    }
        //}
    //}

    Component {
        id: gallery

        Column {
            //width: 210

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
}

