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
    property var userStepModel: undefined
    property alias galleryModel: galleries.model
    property var currentImage: undefined
    property bool hasDescription: false
    property bool hasCrossPage: false

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
                                    var ip = currentImage.imagePoints
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

            Text {
                id: galleryPoints
                x: (galleryPage.width - width)/2
                width: galleryPage.width - (Constants.leftSideBarWidth - Constants.leftSideBarTextWidth)
                wrapMode: Text.WordWrap
                text: getPointsText()
            }

            Connections {
                target: galleryStatistics

                function onListReloaded()
                {
                    galleryPoints.text = getPointsText()
                }
            }

            function getPointsText()
            {
                var gs = galleryStatistics
                if(gs.isListLoaded() && !galleryStatistics.isEmpty())
                {
                    return pointsToPass + qsTr("/") + galleryStatistics.getCurrentItem().ugsPoints
                }
                else
                {
                    return pointsToPass + qsTr("/") + qsTr("...")
                }
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
