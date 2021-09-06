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
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.1

Dialog {
    id : editImage
    modal: true

    title: qsTr("Edit Image")
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    property alias fieldImageFileName: imageFileName.text
    property alias fieldTopImage: topGalleryImage.checked
    property bool fieldCanChooseTopImage: true

    FileDialog {
        id: openImageDialog
        title: qsTr("Please choose image to upload to the server")
        folder: shortcuts.pictures
        nameFilters: [ "Image files (*.jpg *.jpeg *.png)" ]
        selectExisting: true
        selectMultiple: false

        onAccepted: {
            imageFileName.text = fileUrl
        }

        onRejected: {
            // TODO: add no image upload reaction
        }
    }

    ColumnLayout {
        RowLayout {
            Label {
                text: qsTr("Image file name : ")
            }
            TextField {
                id: imageFileName
                placeholderText: qsTr("Enter image file name")
                readOnly: true
            }

            Button {
                text: qsTr("Browse")
                onClicked: {
                    openImageDialog.open()
                }
            }
        }
        CheckBox {
            id: topGalleryImage
            text: qsTr("Top Gallery Image")
            enabled: fieldCanChooseTopImage
        }
    }

    standardButtons: Dialog.Cancel | Dialog.Save
}
