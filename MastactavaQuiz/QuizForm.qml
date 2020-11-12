import QtQuick 2.12
import QtQuick.Controls 2.5
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


Page {
    width: Constants.pageWidth
    height: Constants.pageHeight

    title: qsTr("Quiz")

    property var netAPI: undefined
    property var currentImage: undefined
    property alias currentImageSource: quizImage.source

    Image {
        id: quizImage
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit

        MouseArea {
            x: (quizImage.width - quizImage.paintedWidth) / 2
            y: (quizImage.height - quizImage.paintedHeight) / 2
            width: quizImage.paintedWidth
            height: quizImage.paintedHeight

            onClicked: {
                var x = mouseX / width
                var y = mouseY / height
                var nextImage = currentImage.imagePoints.nextImageByCoords(x, y)
                console.log("next image = ", nextImage)
                if(nextImage !== undefined && nextImage !== null)
                {
                    console.log("next image id = ", nextImage.iptniNextImage)
                }
            }
        }
    }
}
