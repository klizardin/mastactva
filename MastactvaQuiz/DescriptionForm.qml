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
import QtQuick.Layouts 1.15
import MastactvaQuiz 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


Page {
    id: descriptionPage
    width: Constants.pageWidth
    height: Constants.pageHeight

    title: qsTr("Description")

    property var mastactvaAPI: undefined
    property bool hasDescription: false
    property bool hasCrossPage: false
    property string descriptionTextArg: qsTr("")
    property string imageSource: Constants.noImage
    property int galleryId: -1
    property int imageId: -1
    property int descriptionId: -1
    property bool hasHelpPage: true
    property string helpPageText: qsTr("Help for the description page. " +
                                       "At the page you can see a description of the image if any. " +
                                       "Description can help you learn more about image. " +
                                       "Also description can help you answer the questions of the quiz. ")

    signal closeDescriptionPage()

    Rectangle {
        anchors.fill: parent

        ScrollView {
            anchors.fill: parent

            Column {
                Image {
                    id: imageOfDescription
                    width: Constants.pageWidth
                    height: (Constants.pageWidth / Constants.aspectX) * Constants.aspectY * 0.5
                    source: imageSource
                    fillMode: Image.PreserveAspectFit
                }

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
    }

    function init()
    {
        // log view description
        if(!mastactvaAPI.isAndroidFullscreen())
        {
            var userStepD = userStepModel.createItem()
            userStepD.usGalleryId = galleryId
            userStepD.usImageId = imageId
            userStepD.usDescriptionId = descriptionId
            userStepD.usT = mastactvaAPI.now()
            userStepModel.addItem(userStepD)
        }

        imageOfDescription.source = imageSource
        descriptionText.text = descriptionTextArg
    }

    function closePage()
    {
        closeDescriptionPage()
    }
}
