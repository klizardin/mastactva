import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15
import MastactvaQuizEditor 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


Dialog {
    id : chooseShaderDialog
    modal: true

    title: qsTr("Choose image")

    property var fieldImage: undefined
    property var galleryModel: undefined
    property var allImagesOfGalleryModel: undefined
    property var mastactva: undefined

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    Column
    {
        Rectangle {
            id: shadersListRect
            width: Constants.smallDialogWidth
            height: Constants.smallDialogHeight

            ListView {
                id: galleryList
                anchors.fill: parent
                clip: true
                spacing: Constants.effectShaderTypeListSpacing
                model: galleryModel
                delegate: galleryItem
                highlight: galleryItemHighlight
                highlightFollowsCurrentItem: false
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
            width: Constants.smallDialogWidth
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
                width: Constants.smallDialogWidth

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
                        width: Constants.smallDialogWidth - galleryItemCreatedLabel
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
                        width: Constants.smallDialogWidth - galleryItemPointsToPassLabel.width
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
                        width: Constants.smallDialogWidth - galleryItemKeywordsLabel.width
                        text: showFullDescription ? mastactva.leftDoubleCR(keywords) : mastactva.readMore(keywords, Constants.effectsListReadMoreLength, qsTr(" ..."))
                        wrapMode: Text.Wrap
                    }
                }

                Text {
                    id: galleryItemDescriptionText
                    width: Constants.smallDialogWidth
                    text: showFullDescription ? mastactva.leftDoubleCR(description) : mastactva.readMore(description, Constants.effectsListReadMoreLength, qsTr(" ..."))
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
}
