import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.15
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


Page {
    id: descriptionPage
    width: Constants.pageWidth
    height: Constants.pageHeight

    title: qsTr("Description")

    property var netAPI: undefined
    property string descriptionTextArg: qsTr("")
    property bool hasDescription: false
    property int galleryId: -1
    property int imageId: -1
    property int descriptionId: -1

    signal closeDescriptionPage()

    Rectangle {
        anchors.fill: parent

        Column {
            Text {
                id: descriptionText
                text: qsTr("")
                x: Constants.questionPadding
                width: Constants.pageWidth - Constants.questionPadding * 2
                wrapMode: Text.WordWrap
            }
            Button {
                text: qsTr("Close")
                width: Constants.pageWidth
                Layout.alignment: Qt.AlignVCenter
                onClicked: {
                    closePage()
                }
            }
        }
    }

    function init()
    {
        // log view description
        var userStep = userStepModel.createItem()
        userStep.usGalleryId = galleryId
        userStep.usImageId = imageId
        userStep.usDescriptionId = descriptionId
        userStep.usT = mastactvaAPI.now()
        userStepModel.addItem(userStep)

        descriptionText.text = descriptionTextArg
    }

    function closePage()
    {
        closeDescriptionPage()
    }
}
