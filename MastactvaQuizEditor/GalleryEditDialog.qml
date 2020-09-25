import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.1

Dialog {
    id : editGallery
    modal: true

    title: qsTr("Edit Gallery Info")
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    property alias fieldId: editGalleryID.text
    property alias fieldDescription: editGalleryDescription.text
    property alias fieldKeywords: editGalleryKeywords.text
    property alias fieldCreated: editGalleryCreated.text
    property alias fieldPointsToPass: editGalleryPointsToPass.text

    FontMetrics{
        id: editGalleryDescriptionFontMetrics
        font: editGalleryDescription.font
    }
    FontMetrics{
        id: editGalleryKeywordsFontMetrics
        font: editGalleryKeywords.font
    }

    ScrollView {
        anchors.fill: parent
        clip:true

        ColumnLayout {
            RowLayout {
                Label {
                    text: qsTr("Id : ")
                }
                Text {
                    id: editGalleryID
                    text: editGallery.fieldId
                }
            }
            Label {
                text: qsTr("Description : ")
            }
            TextArea {
                id: editGalleryDescription
                placeholderText: qsTr("Enter the gallery`s description")
                text: editGallery.fieldDescription
                focus: true
                KeyNavigation.priority: KeyNavigation.BeforeItem
                KeyNavigation.tab: editGalleryKeywords
            }
            Label {
                text: qsTr("Keywords : ")
            }
            TextArea {
                id: editGalleryKeywords
                placeholderText: qsTr("Enter gallery`s keywords")
                text: editGallery.fieldKeywords
                focus: true
                KeyNavigation.priority: KeyNavigation.BeforeItem
                KeyNavigation.backtab: editGalleryDescription
                KeyNavigation.tab: editGalleryPointsToPass
            }
            RowLayout {
                Label {
                    text: qsTr("Created : ")
                }
                Text {
                    id: editGalleryCreated
                    text: editGallery.fieldCreated
                }
            }
            RowLayout {
                Label {
                    text: qsTr("Points to pass level : ")
                }
                TextField {
                    id: editGalleryPointsToPass
                    placeholderText: qsTr("Enter points to pass number")
                    text: editGallery.fieldPointsToPass
                    focus: true
                }
            }
        }
    }

    standardButtons: Dialog.Cancel | Dialog.Save
}
