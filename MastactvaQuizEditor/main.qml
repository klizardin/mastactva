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

    FileDialog {
        id: uploadGalleryImageDialog
        title: qsTr("Please choose image to upload to the server")
        folder: shortcuts.pictures
        nameFilters: [ "Image files (*.jpg *.png)" ]
        selectExisting: true
        selectMultiple: false

        onAccepted: {
            // TODO: start image upload
        }

        onRejected: {
            // TODO: add no image upload reaction
        }
    }

    Dialog {
        id : editGallery
        modal: true

        title: qsTr("Edit Gallery Info")
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2

        FontMetrics{
            id: editGalleryDescriptionFontMetrics
            font: editGalleryDescription.font
        }
        FontMetrics{
            id: editGalleryKeywordsFontMetrics
            font: editGalleryKeywords.font
        }

        ScrollView {
            anchors.fill: parent
            clip:true

            ColumnLayout {
                RowLayout {
                    Label {
                        text: qsTr("id :")
                    }
                    Text {
                        id: editGalleryID
                        text: qsTr("<id>")
                    }
                }
                Label {
                    text: qsTr("description :")
                }
                TextArea {
                    id: editGalleryDescription
                    placeholderText: qsTr("Enter the gallery`s description")
                    focus: true
                    KeyNavigation.priority: KeyNavigation.BeforeItem
                    KeyNavigation.tab: editGalleryKeywords
                }
                Label {
                    text: qsTr("Keywords :")
                }
                TextArea {
                    id: editGalleryKeywords
                    placeholderText: qsTr("Enter gallery`s keywords")
                    focus: true
                    KeyNavigation.priority: KeyNavigation.BeforeItem
                    KeyNavigation.backtab: editGalleryDescription
                    KeyNavigation.tab: editGalleryPointsToPass
                }
                RowLayout {
                    Label {
                        text: qsTr("created :")
                    }
                    Text {
                        id: editGalleryCreated
                        text: qsTr("<now()>")
                    }
                }
                RowLayout {
                    Label {
                        text: qsTr("Points to pass")
                    }
                    TextField {
                        id: editGalleryPointsToPass
                        placeholderText: qsTr("Enter points to pass number")
                        focus: true
                    }
                }
            }
        }

        standardButtons: Dialog.Cancel | Dialog.Save

        onAccepted: {
            // TODO: save gallery action
        }
        onRejected: {
            // TODO: add cancel action
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
        id: testEditGalleryDialog
        text: qsTr("Edit Gallery Dialog Test")
        onTriggered: editGallery.open()
    }

    menuBar: MenuBar {
        Menu {
            title: qsTr("Galleries")
            MenuItem { action: refreshGalleriesModel }
        }
        Menu {
            title: qsTr("All Images of Gallery")
            MenuItem { action: refreshAllGalleryImagesModel }
        }
        Menu {
            title: qsTr("Test")
            MenuItem { action: testEditGalleryDialog }
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
                        source: image
                        fillMode: Image.PreserveAspectFit

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
            width: (Constants.height * Constants.aspectX) / Constants.aspectY
            height: Constants.height
            source: image
            fillMode: Image.PreserveAspectFit
        }
    }
}

