pragma Singleton
import QtQuick 2.6

QtObject {
    readonly property int width: 640
    readonly property int height: 480
    readonly property string noImage: "qrc:///resources/no-image.png"
    readonly property string noImageResource: ":/resources/no-image.png"
}
