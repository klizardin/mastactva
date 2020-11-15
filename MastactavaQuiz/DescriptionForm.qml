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
        descriptionText.text = descriptionTextArg
    }

    function closePage()
    {
        closeDescriptionPage()
    }
}
