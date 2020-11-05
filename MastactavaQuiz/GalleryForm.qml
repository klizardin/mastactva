import QtQuick 2.12
import QtQuick.Controls 2.5
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


Page {
    id: galleryPage
    width: Constants.pageWidth
    height: Constants.pageHeight

    title: qsTr("Gallery")

    property var netAPI: undefined
    property var galleryModel: undefined

    ListView {
        id: galleries

        anchors.fill: parent
        spacing: Constants.galleriesListViewSpacing
        clip: true

        model: galleryModel
        delegate: galleryItem
    }

    Component {
        id: galleryItem

        Column {
            SystemPalette {
                id: galleryItemPallete
                colorGroup: SystemPalette.Active
            }

            Rectangle {
                width: Constants.pageWidth
                height: (Constants.pageWidth / Constants.aspectX) * Constants.aspectY

                SwipeView {
                    id: galleryImagesSwipeView

                    x: Constants.galleryImageSpacing / 2
                    y: Constants.galleryImageSpacing / 2
                    width: (Constants.pageWidth - Constants.galleryImageSpacing)
                    height: ((Constants.pageWidth - Constants.galleryImageSpacing) / Constants.aspectX) * Constants.aspectY
                    clip: true

                    Repeater {
                        id: gallerySwipeViewImageRepeater
                        model: images
                        Image {
                            id: imageOfGallery
                            width: (Constants.pageWidth - Constants.galleryImageSpacing)
                            height: ((Constants.pageWidth - Constants.galleryImageSpacing) / Constants.aspectX) * Constants.aspectY
                            source: filename
                            fillMode: Image.PreserveAspectFit

                            MouseArea {
                                anchors.fill: parent
                                acceptedButtons: Qt.LeftButton | Qt.RightButton

                                onClicked:
                                {
                                    mouse.accepted = false
                                }
                            }
                        }
                    }
                }
            }

            PageIndicator {
               x:(Constants.pageWidth-width)/2
               height: Constants.leftSideBarPageIndicatorHeight
               currentIndex: galleryImagesSwipeView.currentIndex
               count: galleryImagesSwipeView.count
            }

            Text {
                id : gallery_description
                text: GalleryFunctions.description_first_part(description)
                x: (Constants.pageWidth - width)/2
                width: Constants.pageWidth - (Constants.leftSideBarWidth - Constants.leftSideBarTextWidth)
                wrapMode: Text.WordWrap
            }
        }
    }
}
