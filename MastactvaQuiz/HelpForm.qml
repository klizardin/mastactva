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
    property bool needTextCaption: true
    property string defaultHelpText: qsTr("Help for Mastactave Quiz.\n\n " +
                                          "In the application you can view galleries. " +
                                          "You may answer the questions in galleries and get points for you answers. " +
                                          "From galleries page you can get into the quiz page. " +
                                          "Where you can search for images and questions of gallery by clicking at points at images. " +
                                          "When you click into an image in the quiz page you will step to a next image or to a question. " +
                                          "You goal is to answer all questions. " +
                                          "You may be able not to play quiz (if it is too hard for you) but you may simple view gallerie's " +
                                          "images by selecting 'Gallery' page in the quiz. " +
                                          "\n\nPlese click on page title to see help for a specific page. ")

    signal viewed()

    Rectangle
    {
        anchors.fill: parent

        ScrollView {
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
    }

    function init()
    {
        helpHeader.text = qsTr("Help for page : ") + helpForPage
        helpHeader.visible = needTextCaption
        helpTextCtrl.text = helpText
    }
}
