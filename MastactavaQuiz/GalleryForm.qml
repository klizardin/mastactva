import QtQuick 2.12
import QtQuick.Controls 2.5
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


Page {
    id: galleryPage
    width: Constants.pageWidth
    height: Constants.pageHeight

    title: qsTr("Galleries")

    property var mastactvaAPI: undefined
    property var netAPI: undefined
    property alias galleryModel: galleries.model
    property var currentImage: undefined

    signal startQuiz(var image)

    Rectangle {
        anchors.fill: parent

        ListView {
            id: galleries

            anchors.fill: parent
            spacing: Constants.galleriesListViewSpacing
            clip: true

            delegate: galleryItem
        }
    }

    Component {
        id: galleryItem

        Column {

            SystemPalette {
                id: galleryItemPallete
                colorGroup: SystemPalette.Active
            }

            Rectangle {
                width: galleryPage.width
                height: (galleryPage.width / Constants.aspectX) * Constants.aspectY

                SwipeView {
                    id: galleryImagesSwipeView

                    x: Constants.galleryImageSpacing / 2
                    y: Constants.galleryImageSpacing / 2
                    width: (galleryPage.width - Constants.galleryImageSpacing)
                    height: ((galleryPage.width - Constants.galleryImageSpacing) / Constants.aspectX) * Constants.aspectY
                    clip: true

                    Repeater {
                        id: gallerySwipeViewImageRepeater
                        model: images
                        Image {
                            id: imageOfGallery
                            width: (galleryPage.width - Constants.galleryImageSpacing)
                            height: ((galleryPage.width - Constants.galleryImageSpacing) / Constants.aspectX) * Constants.aspectY
                            source: imageSource
                            fillMode: Image.PreserveAspectFit

                            MouseArea {
                                anchors.fill: parent
                                acceptedButtons: Qt.LeftButton | Qt.RightButton

                                onClicked:
                                {
                                    mouse.accepted = false
                                }

                                onDoubleClicked:
                                {
                                    gallerySwipeViewImageRepeater.model.currentIndex = index
                                    currentImage = gallerySwipeViewImageRepeater.model.getCurrentItem()
                                    galleryPage.startQuiz(currentImage)
                                    mouse.accepted = false
                                }
                            }
                        }
                    }
                }
            }

            PageIndicator {
               x:(galleryPage.width-width)/2
               height: Constants.leftSideBarPageIndicatorHeight
               currentIndex: galleryImagesSwipeView.currentIndex
               count: galleryImagesSwipeView.count
            }

            property bool showFullDescription: false

            Text {
                id : gallery_description
                text: showFullDescription ? description : mastactvaAPI.readMore(description, 128, qsTr(" ..."))
                x: (galleryPage.width - width)/2
                width: galleryPage.width - (Constants.leftSideBarWidth - Constants.leftSideBarTextWidth)
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
}
