import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.15
import MastactvaQuiz 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


Page {
    id: galleryAllImagesPage
    width: Constants.pageWidth
    height: Constants.pageHeight

    title: qsTr("Help")

    property string helpText: qsTr("")
    property string helpForPage: qsTr("")
    property bool hasDescription: false
    property bool hasCrossPage: false
    property string crossPageName: qsTr("")
    property var crossPage: undefined
    property bool hasHelpPage: false
    property string helpPageText: qsTr("")

    signal viewed()

    Rectangle
    {
        anchors.fill: parent

        Column {

            Text {
                id: helpHeader
                padding: Constants.questionPadding
                text: qsTr("Help for page : ")
            }

            Text {
                id: helpTextCtrl
                x: Constants.questionPadding
                width: questionPage.width - Constants.questionPadding * 2
                wrapMode: Text.WordWrap
            }

            Button {
                width: Constants.pageWidth
                text: qsTr("Close")
                Layout.alignment: Qt.AlignVCenter
                onClicked: {
                    viewed()
                }
            }
        }
    }

    function init()
    {
        helpHeader.text = qsTr("Help for page : ") + helpForPage
        helpTextCtrl.text = helpText
    }
}
