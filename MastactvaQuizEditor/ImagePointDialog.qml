import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15
import MastactvaQuizEditor 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


Dialog {
    id: imagePointDialog
    modal: true

    property string titleText: qsTr("Edit image point")
    property string imageSource: Constants.noImage
    property int imageId: -1
    property var pointsModel: undefined
    property int pointIndex: -1
    property real point_x: 0.5
    property real point_y: 0.5
    property real point_weight: 1.0
    property bool pointToNextImage: true
    property bool choosePointType: true

    title: titleText
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    function updatePointParams()
    {
        pointsModel.setCurrentPointParams(pointIndex, point_x, point_y, point_weight)
        voronoyDiagram.modelUpdated()
    }

    ScrollView {
        anchors.fill: parent
        clip:true

        ColumnLayout {
            Label {
                x: Constants.logingItemsSpacing
                text: qsTr("Image :")
            }

            Rectangle
            {
                id: imageRect
                Layout.alignment: Qt.AlignHCenter
                width: Constants.smallImageWidth
                height: Constants.smallImageHeight

                property bool dragPoint: false

                Image {
                    id: imageOfGallery
                    source: imageSource
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectFit

                    MouseArea {
                        x: (imageRect.width - imageOfGallery.paintedWidth)/2
                        y: (imageRect.height - imageOfGallery.paintedHeight)/2
                        width: imageOfGallery.paintedWidth
                        height: imageOfGallery.paintedHeight

                        onPositionChanged: {
                            if(imageRect.dragPoint) {
                                point_x = GalleryFunctions.clamp(mouseX / width, 0, 1)
                                point_y = GalleryFunctions.clamp(mouseY / height, 0, 1)
                                pointXTF.text = point_x
                                pointYTF.text = point_y
                                imagePointDialog.updatePointParams()
                            }
                        }
                        onReleased: {
                            imageRect.dragPoint = false
                            mouse.accepted = false
                        }
                    }
                }

                Item {
                    id: imagePoints
                    anchors.fill: parent
                    visible: true
                    opacity: 0.5
                    z: 1.0
                    Repeater {
                        id: imagePointsRepeater
                        model: pointsModel
                        Image {
                            id: pointImage

                            x: ptX() - Constants.pointImageSize/4
                            y: ptY() - Constants.pointImageSize/4
                            width: Constants.pointImageSize/2
                            height: Constants.pointImageSize/2
                            source: pointIndex === index ? Constants.activePoint : Constants.inactivePoint

                            function ptX()
                            {
                                return (imageRect.width - imageOfGallery.paintedWidth)/2 + (imagePoint_x * imageOfGallery.paintedWidth) + imageOfGallery.x
                            }

                            function ptY()
                            {
                                return (imageRect.height - imageOfGallery.paintedHeight)/2 + (imagePoint_y * imageOfGallery.paintedHeight) + imageOfGallery.y
                            }

                            MouseArea {
                                anchors.fill: parent

                                onPressed: {
                                    if(index === pointIndex)
                                    {
                                        imageRect.dragPoint = true
                                    }
                                    mouse.accepted = false
                                }
                                onReleased: {
                                    if(index === pointIndex)
                                    {
                                        imageRect.dragPoint = false
                                    }
                                    mouse.accepted = false
                                }

                                onCanceled: {
                                    if(index === pointIndex)
                                    {
                                        imageRect.dragPoint = false
                                    }
                                }
                            }
                        }
                    }
                }
                VoronoyDiagram {
                    id: voronoyDiagram
                    x: (imageOfGallery.width - imageOfGallery.paintedWidth)/2 + imageOfGallery.x
                    y: (imageOfGallery.height - imageOfGallery.paintedHeight)/2 + imageOfGallery.y
                    width: imageOfGallery.paintedWidth
                    height: imageOfGallery.paintedHeight
                    opacity: 0.5
                    visible: true
                    z: 0.5
                    model: pointsModel
                    color: "#000080"
                    clip: true
                }
            }
            Label {
                x: Constants.logingItemsSpacing
                text: qsTr("x :")
            }
            TextField {
                id: pointXTF
                x: Constants.logingItemsSpacing
                placeholderText: qsTr("<Input x coordinate of point [0..1]>")
                text: point_x
            }
            Label {
                x: Constants.logingItemsSpacing
                text: qsTr("y :")
            }
            TextField {
                id: pointYTF
                x: Constants.logingItemsSpacing
                placeholderText: qsTr("<Input y coordinate of point [0..1]>")
                text: point_y
            }
            Label {
                x: Constants.logingItemsSpacing
                text: qsTr("weight :")
            }
            TextField {
                id: pointWeightTF
                x: Constants.logingItemsSpacing
                placeholderText: qsTr("<Input y coordinate of point [0..1]>")
                text: point_weight
            }
            ColumnLayout {
                x: Constants.logingItemsSpacing
                visible: choosePointType
                RadioButton {
                    checked: pointToNextImage
                    text: qsTr("Point to next image")
                }
                RadioButton {
                    checked: !pointToNextImage
                    text: qsTr("Point to question")
                }
            }
            Button {
                Layout.alignment: Qt.AlignHCenter
                text: qsTr("Update")
                onClicked: {
                    imagePointDialog.updatePointParams()
                }
            }
        }
    }

    standardButtons: Dialog.Cancel | Dialog.Save
}
