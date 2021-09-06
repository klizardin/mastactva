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
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15
import MastactvaQuizEditor 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


Dialog {
    id: nextImageEditDialog
    modal: true

    property int fieldGalleryId: -1
    property string fieldImageSource: Constants.noImage
    property int fieldImageId: -1
    property int fieldNextImageCurrentIndex: -1
    property int fieldNextImageId: -1
    property string fieldNextImageSource: Constants.noImage
    property var fieldPointsModel: undefined
    property int fieldPointIndex: -1

    title: qsTr("Edit Next Image")
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    Connections {
        target: nextImageEditDialog

        function onFieldNextImageCurrentIndexChanged()
        {
            nextImagesList.model.currentIndex = fieldNextImageCurrentIndex
            var img = nextImagesList.model.currentItem()
            if(img !== undefined && img !== null)
            {
                fieldNextImageId = img.id
                fieldNextImageSource = img.source
            }
            else
            {
                fieldNextImageId = -1
                fieldNextImageSource = Constants.noImage
            }
        }

        function onFieldNextImageSourceChanged()
        {
            nextImage.source = fieldNextImageSource
        }

        function onFieldPointsModelChanged()
        {
            imageRect.loadVoronoyDiagram(fieldPointsModel, imageOfGallery.status);
        }
    }


    ScrollView {
        anchors.fill: parent
        clip:true

        ColumnLayout {
            CheckBox {
                id: showImagePoints
                text: qsTr("Show Image Points")
                checked: true
            }

            CheckBox {
                id: showImageVoronoyDiagram
                text: qsTr("Show Voronoy Diagram")
                checked: true
            }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                width: Constants.smallImageWidth * 2
                height: Constants.smallImageHeight

                Rectangle
                {
                    id: imageRect
                    width: Constants.smallImageWidth
                    height: Constants.smallImageHeight

                    property var voronoyDiagramDlg: undefined

                    function loadVoronoyDiagram(imagePoints, imageStatus)
                    {
                        if(imagePoints !== undefined && imagePoints !== null && !imagePoints.isEmpty() && imageStatus === Image.Ready)
                        {
                            if(voronoyDiagramDlg === undefined)
                            {
                                voronoyDiagramDlg = Qt.createQmlObject(
                                               "import MastactvaQuizEditor 1.0
                                                VoronoyDiagram {
                                                    x: (imageOfGallery.width - imageOfGallery.paintedWidth)/2 + imageOfGallery.x
                                                    y: (imageOfGallery.height - imageOfGallery.paintedHeight)/2 + imageOfGallery.y
                                                    width: imageOfGallery.paintedWidth
                                                    height: imageOfGallery.paintedHeight
                                                    visible: showImageVoronoyDiagram.checked
                                                    opacity: 0.5
                                                    z: 0.5
                                                    model: fieldPointsModel
                                                    color: \"#000080\"
                                                    clip: true
                                                }",
                                               imageRect,
                                               "voronoyDiagramDlg")
                            }
                            else
                            {
                                voronoyDiagramDlg.x = (imageOfGallery.width - imageOfGallery.paintedWidth)/2 + imageOfGallery.x
                                voronoyDiagramDlg.y = (imageOfGallery.height - imageOfGallery.paintedHeight)/2 + imageOfGallery.y
                                voronoyDiagramDlg.width = imageOfGallery.paintedWidth
                                voronoyDiagramDlg.height = imageOfGallery.paintedHeight
                                voronoyDiagramDlg.modelUpdated()
                            }
                        }
                    }

                    Image {
                        id: imageOfGallery
                        source: fieldImageSource
                        anchors.fill: parent
                        fillMode: Image.PreserveAspectFit
                        z: 0.0

                        onStatusChanged: {
                            imageRect.loadVoronoyDiagram(fieldPointsModel, status);
                        }
                    }


                    Item {
                        id: imagePoints
                        anchors.fill: parent
                        visible: showImagePoints.checked
                        opacity: 0.5
                        z: 1.0
                        Repeater {
                            id: imagePointsRepeater
                            model: fieldPointsModel
                            Image {
                                id: pointImage

                                x: ptX() - Constants.pointImageSize/4
                                y: ptY() - Constants.pointImageSize/4
                                width: Constants.pointImageSize/2
                                height: Constants.pointImageSize/2
                                source: fieldPointIndex === index ? Constants.activePoint : Constants.inactivePoint

                                function ptX()
                                {
                                    return (imageRect.width - imageOfGallery.paintedWidth)/2 + (imagePoint_x * imageOfGallery.paintedWidth) + imageOfGallery.x
                                }

                                function ptY()
                                {
                                    return (imageRect.height - imageOfGallery.paintedHeight)/2 + (imagePoint_y * imageOfGallery.paintedHeight) + imageOfGallery.y
                                }
                            }
                        }
                    }
                }
                Rectangle {
                    id: nextImageRect
                    width: Constants.smallImageWidth
                    height: Constants.smallImageHeight

                    Image {
                        id: nextImage
                        source: fieldNextImageSource
                        anchors.fill: parent
                        fillMode: Image.PreserveAspectFit
                    }
                }
            }
            Rectangle {
                id: nextImagesListRect
                Layout.alignment: Qt.AlignHCenter
                width: Constants.smallImageWidth
                height: Constants.smallImageHeight * 2

                ListView {
                    id: nextImagesList

                    anchors.fill: parent
                    spacing: Constants.galleriesListViewSpacing
                    clip: true

                    model: GalleryImagesModel {
                        objectName: "GalleryNextImagesModel"
                        galleryViewImages: false
                        galleryId: fieldGalleryId
                    }

                    delegate: nextImageListItemComponent
                }
            }
        }
    }

    standardButtons: Dialog.Cancel | Dialog.Save

    Component {
        id: nextImageListItemComponent

        Rectangle {
            id: nextImageListItemComponentRect

            property int imageOfGalleryIndex: index

            SystemPalette {
                id: nextImageListItemComponentPallete
                colorGroup: SystemPalette.Active
            }

            width: Constants.smallImageWidth
            height: (Constants.smallImageWidth * Constants.aspectY) / Constants.aspectX
            border.width: 2
            border.color: fieldNextImageCurrentIndex === imageOfGalleryIndex ? nextImageListItemComponentPallete.highlight : nextImageListItemComponentPallete.window

            Image  {
                id: nextImage
                x: Constants.imageOfGalleryImageSpacing2 / 2
                y: Constants.imageOfGalleryImageSpacing2 / 2
                width: (Constants.smallImageWidth - Constants.imageOfGalleryImageSpacing2)
                height: ((Constants.smallImageWidth - Constants.imageOfGalleryImageSpacing2) * Constants.aspectY) / Constants.aspectX
                source: image_source
                fillMode: Image.PreserveAspectFit

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        fieldNextImageCurrentIndex = imageOfGalleryIndex
                    }
                }
            }
        }
    }
}
