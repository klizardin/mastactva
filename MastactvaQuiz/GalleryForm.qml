/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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
    property bool hasHelpPage: true
    property string helpPageText: qsTr("Help for the galleries page. " +
                                       "At the page you can choose gallery to play quiz in. " +
                                       "Also you can view you playing statistics here for every gallery. ")

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

            property int currentGalleryIndex : index

            Rectangle {
                width: galleryPage.width
                height: (galleryPage.width / Constants.galleryAspectX) * Constants.galleryAspectY

                SwipeView {
                    id: galleryImagesSwipeView

                    x: Constants.galleryImageSpacing / 2
                    y: Constants.galleryImageSpacing / 2
                    width: (galleryPage.width - Constants.galleryImageSpacing)
                    height: ((galleryPage.width - Constants.galleryImageSpacing) / Constants.galleryAspectX) * Constants.galleryAspectY
                    clip: true

                    Repeater {
                        id: gallerySwipeViewImageRepeater
                        model: images
                        Image {
                            id: imageOfGallery
                            width: (galleryPage.width - Constants.galleryImageSpacing)
                            height: ((galleryPage.width - Constants.galleryImageSpacing) / Constants.galleryAspectX) * Constants.galleryAspectY
                            source: localImageSource
                            fillMode: Image.PreserveAspectFit

                            MouseArea {
                                anchors.fill: parent
                                acceptedButtons: Qt.LeftButton | Qt.RightButton

                                onClicked:
                                {
                                    galleryModel.currentIndex = currentGalleryIndex
                                    //console.log("currentGalleryIndex =",currentGalleryIndex)
                                    gallerySwipeViewImageRepeater.model.currentIndex = index
                                    currentImage = gallerySwipeViewImageRepeater.model.getCurrentItem()
                                    var ip = currentImage.imagePoints
                                    galleryPage.startQuiz(currentImage)
                                    mouse.accepted = false
                                }
                            }

                            Connections {
                                target: images.itemAt(model.index)
                                function onLocalImageSourceChanged()
                                {
                                    source = localImageSource
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

                    onClicked: {
                        showFullDescription = !showFullDescription;
                    }
                }
            }
        }
    }
}
