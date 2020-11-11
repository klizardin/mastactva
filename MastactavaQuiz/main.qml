import QtQuick 2.12
import QtQuick.Controls 2.5
import MastactvaQuiz 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


ApplicationWindow {
    id: window
    width: Constants.width
    height: Constants.height
    visible: true
    title: qsTr("Mastactva Quiz")

    NetAPI {
        id: netAPI
        objectName: "NetAPI"
    }

    MastactvaAPI {
        id: mastactvaAPI
        objectName: "MastactvaAPI"
    }

    QuizUserModel {
        id: quizUserModel
        objectName: "QuizUserModel"
        layoutQMLName: "QuizUserModel"
        layoutIdField: "deviceid"
    }

    GalleryModel {
        id: galleryModel
        objectName: "GalleryModel"
        layoutQMLName: "GalleryModel"
        layoutIdField: "id"
        currentRef: "deviceid"
        jsonParamsGet: true
    }



    function initGalleryModel()
    {
        galleryModel.setLayoutRef("deviceid", "QuizUserModel", "deviceid")
    }

    Connections {
        target: netAPI

        function onInitialized()
        {
            galleryPage.netAPI = netAPI
            quizPage.netAPI = netAPI
            descriptionPage.netAPI = netAPI
            galleryPage.galleryModel = galleryModel
            galleryPage.mastactvaAPI = mastactvaAPI

            initGalleryModel()
            quizUserModel.loadList()
        }
    }

    GalleryForm {
        id: galleryPage
        galleryModel: galleryModel
    }

    QuizForm {
        id: quizPage
    }

    DescriptionForm {
        id: descriptionPage
    }


    header: ToolBar {
        contentHeight: toolButton.implicitHeight

        ToolButton {
            id: toolButton
            text: stackView.depth > 1 ? "\u25C0" : "\u2630"
            font.pixelSize: Qt.application.font.pixelSize * 1.6
            onClicked: {
                if (stackView.depth > 1) {
                    stackView.pop()
                } else {
                    drawer.open()
                }
            }
        }

        Label {
            text: stackView.currentItem.title
            anchors.centerIn: parent
        }
    }

    Drawer {
        id: drawer
        width: window.width * 0.66
        height: window.height

        Column {
            anchors.fill: parent

            ItemDelegate {
                text: qsTr("Quiz")
                width: parent.width
                onClicked: {
                    stackView.push(quizPage)
                    drawer.close()
                }
            }
            ItemDelegate {
                text: qsTr("Description")
                width: parent.width
                onClicked: {
                    stackView.push(descriptionPage)
                    drawer.close()
                }
            }
        }
    }

    StackView {
        id: stackView
        initialItem: galleryPage
        anchors.fill: parent
    }
}
