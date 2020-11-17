pragma Singleton
import QtQuick 2.6

QtObject {
    readonly property int width: 480
    readonly property int height: 640
    readonly property int pageWidth: 480
    readonly property int pageHeight: 580
    readonly property int answersHeight: 160
    readonly property int questionPadding: 10
    readonly property int smallImageWidth: 320
    readonly property int smallImageHeight: 240
    readonly property int leftSideBarWidth: 250
    readonly property int leftSideBarWidthSpace: 5
    readonly property int leftSideBarTextWidth: 220
    readonly property int leftSideBarPageIndicatorHeight: 30
    readonly property int galleriesListViewSpacing: 10
    readonly property int imagesOfGalleryListViewSpacing: 10
    readonly property int galleryImageSpacing: 10
    readonly property int imageOfGalleryImageSpacing: 10
    readonly property int imageOfGalleryImageSpacing2: 4
    readonly property int aspectX: 3
    readonly property int aspectY: 4
    readonly property int descriptionRowHeight: 100
    readonly property int logingItemsSpacing: 10
    readonly property int pointImageSize: 32
    readonly property string noImage: "qrc:///resources/no-image.png"
}
