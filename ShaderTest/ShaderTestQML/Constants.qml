pragma Singleton
import QtQuick 2.6

QtObject {
    readonly property int width: 800
    readonly property int height: 600
    readonly property int shadersWidth: 400
    readonly property string noImage: "qrc:///resources/no-image.png"
    readonly property string noImageResource: ":/resources/no-image.png"
    property string noImageHash: ""
}
