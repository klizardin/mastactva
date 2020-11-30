pragma Singleton
import QtQuick 2.6

QtObject {
    readonly property int loginWidth: 480
    readonly property int loginHeight: 360
    readonly property int width: 640
    readonly property int height: 480
    readonly property int smallImageWidth: 320
    readonly property int smallImageHeight: 240
    readonly property int leftSideBarWidth: 250
    readonly property int leftSideBarWidthSpace: 5
    readonly property int leftSideBarTextWidth: 220
    readonly property int leftSideBarPageIndicatorHeight: 30
    readonly property int galleriesListViewSpacing: 10
    readonly property int effectsListViewSpacing: 10
    readonly property int effectsListReadMoreLength: 256
    readonly property int effectsListHeaderPadding: 10
    readonly property int effectShaderListHeaderPadding: 10
    readonly property int effectInfoPadding: 10
    readonly property int warningDialogWidth: 360
    readonly property int warningDialogHeight: 480
    readonly property int warningDialogPadding: 10
    readonly property int minConstValueWidth: 150
    readonly property int minConstValueHeight: 75
    readonly property int effectShaderTypeListSpacing: 5
    readonly property int imagesOfGalleryListViewSpacing: 10
    readonly property int galleryImageSpacing: 10
    readonly property int imageOfGalleryImageSpacing: 10
    readonly property int imageOfGalleryImageSpacing2: 4
    readonly property int aspectX: 3
    readonly property int aspectY: 4
    readonly property int descriptionRowHeight: 100
    readonly property int logingItemsSpacing: 10
    readonly property string activePoint: "qrc:///resources/selectedPoint.png"
    readonly property string inactivePoint: "qrc:///resources/point.png"
    readonly property int pointImageSize: 32
    readonly property string noImage: "qrc:///resources/no-image.png"
    readonly property string notAQuestionPoint: qsTr("Not a point to question")
    readonly property string notANextImagePoint: qsTr("Not a point to next image")
    readonly property string aNextImagePoint: qsTr("Point to next image")
    readonly property string selectImagePoint : qsTr("Select image point")
}
