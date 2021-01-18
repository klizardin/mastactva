pragma Singleton
import QtQuick.Window 2.2
import QtQuick 2.6

QtObject {
    property int width: 480
    property int height: 640
    property int pageWidth: 480
    property int pageHeight: 580
    property int answersHeight: 160
    readonly property int questionPadding: 10
    property int smallImageWidth: 320
    property int smallImageHeight: 240
    property int leftSideBarWidth: 250
    readonly property int leftSideBarWidthSpace: 5
    property int leftSideBarTextWidth: 220
    readonly property int leftSideBarPageIndicatorHeight: 30
    readonly property int galleriesListViewSpacing: 10
    readonly property int imagesOfGalleryListViewSpacing: 10
    readonly property int galleryImageSpacing: 10
    readonly property int imageOfGalleryImageSpacing: 10
    readonly property int imageOfGalleryImageSpacing2: 4
    readonly property int aspectX: 3
    readonly property int aspectY: 4
    readonly property int galleryAspectX: 4
    readonly property int galleryAspectY: 3
    readonly property int descriptionRowHeight: 100
    readonly property int logingItemsSpacing: 10
    readonly property int pointImageSize: 32
    readonly property string noImage: "qrc:///resources/no-image.png"
    readonly property string noImageResource: ":/resources/no-image.png"
    property string noImageHash: ""
    property real animationSpeedNorm: 1000.0
    property real animationSpeedMin: 333.333333
    property real animationSpeedMax: 3000.0
    property real animationSpeedUpCoef: 0.8

    function init()
    {
        width = Screen.width
        height = Screen.height
        pageWidth = Screen.width
        pageHeight = Screen.height - 60
        answersHeight = Screen.height / 4
        smallImageWidth = Screen.width / 2
        smallImageHeight = Screen.height / 2
        leftSideBarWidth = Screen.width / 3
        leftSideBarTextWidth = leftSideBarWidth - 30
    }
}
