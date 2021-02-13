import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15
import MastactvaQuizEditor 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


Dialog {
    id : chooseImageDialog
    modal: true

    title: qsTr("Choose image")

    property var fieldImage: undefined
    property var galleryModel: undefined
    property var allImagesOfGalleryModel: undefined
    property var mastactva: undefined

    property int galleryListWidth: Constants.smallDialogWidth / 2
    property int galleryListHeight: Constants.smallDialogHeight
    property int imagesListWidth: Constants.smallDialogWidth / 2
    property int imagesListHeight: Constants.smallDialogHeight / 2
    property int descriptionWidth: Constants.smallDialogWidth / 2
    property int descriptionHeight: Constants.smallDialogHeight / 2

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    Row
    {
        Rectangle {
            id: galleryListRect
            width: galleryListWidth
            height: galleryListHeight

            ListView {
                id: galleryList
                anchors.fill: parent
                clip: true
                spacing: Constants.smallListSmallSpacing
                model: galleryModel
                delegate: galleryItem
                highlight: galleryItemHighlight
                highlightFollowsCurrentItem: false
            }
        }

        Column {
            Rectangle {
                id: imagesListRect
                width: imagesListWidth
                height: imagesListHeight

                ListView {
                    id: imagesList
                    anchors.fill: parent
                    clip: true
                    spacing: Constants.smallListSmallSpacing
                    model: allImagesOfGalleryModel
                    delegate: imageItem
                    highlight: imageItemHighlight
                    highlightFollowsCurrentItem: false
                }
            }
            Rectangle {
                id: descriptionRect
                width: descriptionWidth
                height: descriptionHeight

                Text {
                    id: description
                    anchors.fill: parent
                    width: descriptionWidth
                    height: descriptionHeight
                    clip: true
                    wrapMode: Text.Wrap
                }
            }
        }
    }

    function init()
    {
        galleryModel.loadList()
    }

    standardButtons: Dialog.Cancel | Dialog.Save

    Component {
        id: galleryItem

        MouseArea {
            width: galleryListWidth
            height: childrenRect.height
            acceptedButtons: Qt.LeftButton | Qt.RightButton

            property bool showFullDescription: false

            onClicked:
            {
                galleryList.currentIndex = index
                galleryModel.currentIndex = index
                mouse.accepted = false
            }

            onDoubleClicked: {
                showFullDescription = !showFullDescription
            }

            Column {
                width: galleryListWidth

                FontMetrics{
                    id: galleryItemFontMetrics
                    font: galleryItemCreated.font
                }

                Row {
                    Label {
                        id: galleryItemCreatedLabel
                        text: qsTr("Created : ")
                    }

                    Text {
                        id: galleryItemCreated
                        width: galleryListWidth - galleryItemCreatedLabel
                        text: mastactva.dateTimeToUserStr(created)
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    Label {
                        id: galleryItemPointsToPassLabel
                        text: qsTr("Points to pass : ")
                    }

                    Text {
                        id: galleryItemPointsToPass
                        width: galleryListWidth - galleryItemPointsToPassLabel.width
                        text: pointsToPass
                        wrapMode: Text.Wrap
                    }
                }

                Row {
                    Label {
                        id: galleryItemKeywordsLabel
                        text: qsTr("Keywords : ")
                    }

                    Text {
                        id: galleryItemKeywords
                        width: galleryListWidth - galleryItemKeywordsLabel.width
                        text: showFullDescription ? mastactva.leftDoubleCR(keywords) : mastactva.readMore(keywords, Constants.smallListReadMoreLength, qsTr(" ..."))
                        wrapMode: Text.Wrap
                    }
                }

                Text {
                    id: galleryItemDescriptionText
                    width: galleryListWidth
                    text: showFullDescription ? mastactva.leftDoubleCR(description) : mastactva.readMore(description, Constants.smallListReadMoreLength, qsTr(" ..."))
                    wrapMode: Text.WordWrap
                }
            }
        }
    }

    Component {
        id: galleryItemHighlight

        Rectangle {
            SystemPalette {
                id: galleryItemHighlightPallete
                colorGroup: SystemPalette.Active
            }

            border.color: galleryItemHighlightPallete.highlight
            border.width: 2
            radius: 5
            y: (galleryList.currentItem !== undefined && galleryList.currentItem !== null) ? galleryList.currentItem.y : 0
            x: (galleryList.currentItem !== undefined && galleryList.currentItem !== null) ? galleryList.currentItem.x : 0
            width: (galleryList.currentItem !== undefined && galleryList.currentItem !== null) ? galleryList.currentItem.width : 0
            height: (galleryList.currentItem !== undefined && galleryList.currentItem !== null) ? galleryList.currentItem.height : 0
        }
    }

    Component {
        id: imageItem

        MouseArea {
            width: imagesListWidth
            height: childrenRect.height
            acceptedButtons: Qt.LeftButton | Qt.RightButton

            property bool showFullDescription: false

            onClicked:
            {
                imagesList.currentIndex = index
                allImagesOfGalleryModel.currentIndex = index
                fieldImage = allImagesOfGalleryModel.currentItem
                description.text = imageDescription.currentItem !== null ? mastactva.leftDoubleCR(imageDescription.currentItem.idDescriptionText) : ""
                mouse.accepted = false
            }

            onDoubleClicked: {
                showFullDescription = !showFullDescription
            }

            Column {
                width: imagesListWidth

                FontMetrics{
                    id: imageItemFontMetrics
                    font: imageItemCreated.font
                }

                Image {
                    id: image
                    width: imagesListWidth
                    height: imagesListWidth * Constants.aspectY / Constants.aspectX
                    fillMode: Image.PreserveAspectFit
                    source: imageSource
                }

                Row {
                    Label {
                        id: imageItemCreatedLabel
                        text: qsTr("Created : ")
                    }

                    Text {
                        id: imageItemCreated
                        width: imagesListWidth - imageItemCreatedLabel.width
                        text: mastactva.dateTimeToUserStr(imageCreated)
                        wrapMode: Text.Wrap
                    }
                }

                Text {
                    id: imageItemDescriptionText
                    width: imagesListWidth
                    text: imageDescription.currentItem !== null ? showFullDescription ? mastactva.leftDoubleCR(imageDescription.currentItem.idDescriptionText) : mastactva.readMore(imageDescription.currentItem.idDescriptionText, Constants.smallListReadMoreLength, qsTr(" ...")) : ""
                    wrapMode: Text.WordWrap
                }

                Connections {
                    target: imageDescription

                    function onListReloaded()
                    {
                        imageItemDescriptionText.text = imageDescription.currentItem !== null ? showFullDescription ? mastactva.leftDoubleCR(imageDescription.currentItem.idDescriptionText) : mastactva.readMore(imageDescription.currentItem.idDescriptionText, Constants.smallListReadMoreLength, qsTr(" ...")) : ""
                        if(index === imagesList.currentIndex)
                        {
                            description.text = imageDescription.currentItem !== null ? mastactva.leftDoubleCR(imageDescription.currentItem.idDescriptionText) : ""
                        }
                    }
                }
            }
        }
    }

    Component {
        id: imageItemHighlight

        Rectangle {
            SystemPalette {
                id: imageItemHighlightPallete
                colorGroup: SystemPalette.Active
            }

            border.color: imageItemHighlightPallete.highlight
            border.width: 2
            radius: 5
            y: (imagesList.currentItem !== undefined && imagesList.currentItem !== null) ? imagesList.currentItem.y : 0
            x: (imagesList.currentItem !== undefined && imagesList.currentItem !== null) ? imagesList.currentItem.x : 0
            width: (imagesList.currentItem !== undefined && imagesList.currentItem !== null) ? imagesList.currentItem.width : 0
            height: (imagesList.currentItem !== undefined && imagesList.currentItem !== null) ? imagesList.currentItem.height : 0
        }
    }
}
