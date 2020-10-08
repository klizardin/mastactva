import QtQuick 2.12
import QtQuick.Controls 2.14
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.15
import MastactvaQuizEditor 1.0
import "GalleryFunctions.js" as GalleryFunctions
import Mastactva 1.0


ApplicationWindow {
    id: root
    visible: true
    width: Constants.width
    height: Constants.height
    title: qsTr("Mastactva Quiz Editor")

    property alias galleryCurrentIndex: mastactva.galleryIndex
    property alias galleryImagesCurrentIndex: mastactva.imageOfGalleryIndex
    property alias galleryCurrentId: mastactva.galleryId
    property alias galleryImagesCurrentId: mastactva.imageOfGalleryId
    property alias imageOfGalleryDescriptionIndex: mastactva.imageOfGalleryDescriptionIndex
    property alias imageOfGalleryDescriptionId: mastactva.imageOfGalleryDescriptionId
    property alias imageOfGalleryPointIndex: mastactva.imageOfGalleryPointIndex
    property bool showImagePoints: false

    MastactvaAPI {
        id: mastactva
    }

    GalleryEditDialog {
        id: createNewGalleryDialog

        onOpened: {
            fieldId = -1
            fieldDescription = ""
            fieldKeywords = ""
            fieldCreated = "now()"
            fieldPointsToPass = 1.0
        }

        onAccepted: {
            mastactva.createNewGallery(fieldDescription, fieldKeywords, fieldPointsToPass)
            mastactva.onNewGalleryCreated.connect(onNewGalleryCreated)
        }

        function onNewGalleryCreated()
        {
            mastactva.reloadGalleriesModel()
            mastactva.galleryReloaded.connect(onGalleryReloaded)
        }

        function onGalleryReloaded()
        {
            galleries.currentIndex = galleryCurrentIndex;
        }

        onRejected: {
        }
    }

    GalleryEditDialog {
        id: editCurrentGalleryDialog

        property int oldId: -1
        property string oldDescription: ""
        property string oldKeywords: ""
        property string oldCreated: ""
        property string oldPointsToPass: "1.0"

        onOpened: {
            oldId = fieldId
            oldDescription = fieldDescription
            oldKeywords = fieldKeywords
            oldCreated = fieldCreated
            oldPointsToPass = fieldPointsToPass
        }

        onAccepted: {
            mastactva.editGallery(fieldId, fieldDescription, fieldKeywords, fieldCreated, fieldPointsToPass)
            mastactva.onGalleryEdited.connect(onNewGalleryCreated)
        }

        function onNewGalleryCreated()
        {
            mastactva.reloadGalleriesModel()
            mastactva.galleryReloaded.connect(onGalleryReloaded)
        }

        function onGalleryReloaded()
        {
            galleries.currentIndex = galleryCurrentIndex;
        }

        function setFields(item)
        {
            fieldId = item.id
            fieldDescription = item.description
            fieldKeywords = item.keywords
            fieldCreated = item.created
            fieldPointsToPass = item.pointsToPass
        }

        onRejected: {
            fieldId = oldId
            fieldDescription = oldDescription
            fieldKeywords = oldKeywords
            fieldCreated = oldCreated
            fieldPointsToPass = oldPointsToPass
        }
    }

    ImageEditDialog {
        id: addImageDialog

        property int currentGalleryId: -1

        onOpened: {
            fieldImageFileName = ""
            fieldTopImage = false
        }

        onAccepted: {
            mastactva.addImage(currentGalleryId, fieldImageFileName, fieldTopImage)
            mastactva.onImageAdded.connect(onImageAdded)
        }

        function onImageAdded()
        {
            mastactva.reloadGalleriesModel()
            mastactva.reloadAllImagesOfGalleryViewModel()
            mastactva.galleryReloaded.connect(onGalleryReloaded)
            mastactva.imagesOfGalleryReloaded.connect(onGalleryImagesReloaded)
        }

        function onGalleryReloaded()
        {
            galleries.currentIndex = galleryCurrentIndex;
        }

        function onGalleryImagesReloaded()
        {
            galleries.images_of_gallery = galleryImagesCurrentIndex;
        }
    }

    DescriptionEditDialog {
        id: createNewDescriptionDialog

        onOpened: {
            fieldDescription = ""
            fieldFrom = mastactva.nowJsonStr()
        }

        onAccepted: {
            mastactva.newDescription(fieldDescription, fieldFrom)
            mastactva.onNewDescriptionAdded.connect(onNewDescriptionAdded)
        }

        function onNewDescriptionAdded()
        {
            mastactva.refreshDescriptions();
        }
    }

    DescriptionEditDialog {
        id: editDescriptionDialog

        property int oldDescriptionIndex: -1

        onOpened: {
            var descr = mastactva.getCurrentDescription()
            if(descr !== undefined) {
                fieldId = descr.id
                fieldImageId = descr.imageId
                fieldDescription = descr.descriptionStr
                fieldFrom = descr.fromDateTime
            } else {
                fieldId = -1
                fieldImageId = -1
                fieldDescription = ""
                fieldFrom = mastactva.nowJsonStr()
            }
            oldDescriptionIndex = imageOfGalleryDescriptionIndex
        }

        onAccepted: {
            mastactva.editDescription(fieldId, fieldImageId, fieldDescription, fieldFrom)
            mastactva.onDescriptionEdited.connect(onDescriptionEdited)
        }

        function onDescriptionEdited()
        {
            mastactva.refreshDescriptions();
            mastactva.onRefreshDescriptions.connect(onRefreshDescriptions)
        }

        function onRefreshDescriptions()
        {
            imageOfGalleryDescriptionIndex = oldDescriptionIndex
        }
    }

    Action {
        id: refreshGalleriesModel
        text: qsTr("&Refresh Galleries")
        onTriggered: mastactva.reloadGalleriesModel()
    }

    Action {
        id: refreshAllGalleryImagesModel
        text: qsTr("Refresh &All Images")
        onTriggered: mastactva.reloadAllImagesOfGalleryViewModel()
    }

    Action {
        id: createNewGallery
        text: qsTr("&Create New Gallery")
        onTriggered: {
            createNewGalleryDialog.open()
        }
    }

    Action {
        id: editCurrentGallery
        text: qsTr("&Edit Current Gallery")
        onTriggered: {
            editCurrentGalleryDialog.open()
        }
    }

    Action {
        id: addGalleryImage
        text: qsTr("Add &Image To Current Gallery")
        onTriggered: {
            addImageDialog.open()
        }
    }

    Action {
        id: removeCurrentImageOfGallery
        text: qsTr("&Remove Current Image")
        onTriggered: {
            mastactva.removeCurrentImage()
            mastactva.imageOfGalleryRemoved.connect(onImageOfGalleryRemoved)
        }

        function onImageOfGalleryRemoved()
        {
            mastactva.reloadGalleriesModel()
            mastactva.galleryReloaded.connect(onGalleryReloaded)
            mastactva.reloadAllImagesOfGalleryViewModel()
            mastactva.imagesOfGalleryReloaded.connect(onGalleryImagesReloaded)
        }

        function onGalleryReloaded()
        {
            galleries.currentIndex = galleryCurrentIndex;
        }

        function onGalleryImagesReloaded()
        {
            galleries.currentIndex = -1;
        }
    }

    Action {
        id: showImagePointsAction
        text: qsTr("Show image &points")
        onTriggered: {
            showImagePoints = !showImagePoints
        }
    }

    Action {
        id: imageOfGalleryCreateDescription
        text: qsTr("&New Description")
        onTriggered: {
            createNewDescriptionDialog.open()
        }
    }

    Action {
        id: imageOfGalleryEditDescription
        text: qsTr("&Edit Description")
        onTriggered: {
            editDescriptionDialog.open()
        }
    }

    Action {
        id: imageOfGalleryDeleteDescription
        text: qsTr("&Delete Description")
        onTriggered: {
            mastactva.removeCurrentDescription()
            mastactva.onDescriptionDeleted.connect(onDescriptionDeleted)
        }

        function onDescriptionDeleted()
        {
            imageOfGalleryDescriptionIndex = -1;
            mastactva.refreshDescriptions();
        }
    }

    menuBar: MenuBar {
        Menu {
            title: qsTr("Galleries")
            MenuItem { action: refreshGalleriesModel }
            MenuItem { action: createNewGallery }
            MenuItem { action: editCurrentGallery }
            MenuItem { action : addGalleryImage }
        }
        Menu {
            title: qsTr("All Images of Gallery")
            MenuItem { action: refreshAllGalleryImagesModel }
            MenuItem { action: removeCurrentImageOfGallery }
            MenuItem {
                action: showImagePointsAction
                checkable: true
                checked: showImagePoints
            }
        }
        Menu {
            title: qsTr("Description")
            MenuItem { action: imageOfGalleryCreateDescription }
            MenuItem { action: imageOfGalleryEditDescription }
            MenuItem { action: imageOfGalleryDeleteDescription }
        }
    }

    SplitView {
        id: slitGalleriesAndImagesOfGallery
        anchors.fill: parent
        orientation: Qt.Horizontal

        Rectangle{

            id: splitViewPaneGallery

            SplitView.preferredWidth: Constants.leftSideBarWidth
            SplitView.minimumWidth: Constants.leftSideBarWidth/2
            SplitView.maximumWidth: Constants.leftSideBarWidth*2
            height: parent.height

            ListView {
                id: galleries

                anchors.fill: parent
                spacing: Constants.galleriesListViewSpacing
                clip: true

                model: GalleryEditViewModel {
                    objectName: "GalleryModel"
                }

                delegate: gallery
            }
        }

        Rectangle{

            id: slitViewPaneImageOfGallery
            SplitView.preferredWidth: root.width - Constants.leftSideBarWidth
            height: parent.height

            SplitView {
                id: slitImagesOfGalleryAndImageInfo
                anchors.fill: parent
                orientation: Qt.Vertical

                Rectangle{

                    id: slitViewPaneImagesOfGallery

                    SplitView.preferredHeight: Constants.height
                    SplitView.minimumHeight: Constants.height/2

                    ListView {
                        id: images_of_gallery

                        anchors.fill: parent
                        orientation: ListView.Horizontal
                        clip: true
                        spacing: Constants. imagesOfGalleryListViewSpacing

                        model: GalleryImagesModel {
                            objectName: "AllGalleryImagesModel"
                            galleryViewImages: false
                            galleryId: -1
                        }

                        delegate : gallery_image

                        Connections{
                            target: images_of_gallery
                            function onCurrentIndexChanged()
                            {
                                if(imageOfGalleryDescriptionIndex == -1)
                                {
                                    imageOfGalleryNextImageNextImage.source = Constants.noImage;
                                }
                            }
                        }

                    }
                }

                Rectangle {

                    id: slitViewPaneImageInfo
                    SplitView.preferredHeight: parent.height - Constants.height

                    TabBar {
                        id: imageOfGalleryInfoBar
                        anchors.top: parent.top
                        width: parent.width
                        TabButton {
                            text: qsTr("Description")
                        }
                        TabButton {
                            text: qsTr("Next image")
                        }
                        TabButton {
                            text: qsTr("Question")
                        }
                    }

                    StackLayout {
                        anchors.top: imageOfGalleryInfoBar.bottom
                        width: slitViewPaneImageOfGallery.width
                        height: slitViewPaneImageInfo.height - imageOfGalleryInfoBar.height
                        currentIndex: imageOfGalleryInfoBar.currentIndex
                        Item {
                            id: imageOfGalleryDescriptionTab
                            ListView {
                                id: imageOfGalleryDescriptionListView
                                anchors.fill: parent
                                clip: true
                                model: DescriptionModel {
                                    objectName: "ImageOfGalleryDescriptionModel"
                                    imageID: -1
                                }
                                delegate : imageOfGalleryDescriptionListViewItem
                                highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
                            }
                        }
                        Item {
                            id: imageOfGalleryNextImageTab
                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.left
                                anchors.fill: parent
                                visible: imageOfGalleryPointIndex === -1
                                text: qsTr("<Select image point>")
                            }
                            Image {
                                height: slitViewPaneImageInfo.height - imageOfGalleryInfoBar.height
                                width: (slitViewPaneImageInfo.height - imageOfGalleryInfoBar.height) * Constants.aspectX / Constants.aspectY
                                id: imageOfGalleryNextImageNextImage
                                visible: imageOfGalleryPointIndex >= 0
                                fillMode: Image.PreserveAspectFit
                                source: Constants.noImage
                            }
                        }
                        Item {
                            id: imageOfGalleryQuestionTab
                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.left
                                anchors.fill: parent
                                visible: imageOfGalleryPointIndex === -1
                                text: qsTr("<Select image point>")
                            }
                            Item {
                                anchors.fill: parent
                                Column{
                                    anchors.left: parent.left

                                    Rectangle{
                                        width: imageOfGalleryQuestionTab.width
                                        height: (imageOfGalleryQuestionTab.height * 2) / 5

                                        TextArea {
                                            id: imageOfGalleryQuestionText
                                            anchors.fill: parent
                                            readOnly: true
                                            text: qsTr("<Question>")
                                        }
                                    }

                                    Rectangle{
                                        width: imageOfGalleryQuestionTab.width
                                        height: (imageOfGalleryQuestionTab.height * 3) / 5

                                        ListView {
                                            anchors.fill: parent
                                            id: imageOfGalleryQuestionAnswersListView
                                            clip: true
                                            highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
                                            model: QuestionAnswersModel {
                                                objectName: "ImageOfGalleryQuestionAnswersModel"
                                                questionId: -1
                                            }

                                            delegate: imageOfGalleryQuestionAnswersListViewItem
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Component {
        id: gallery

        Column {
            SystemPalette {
                id: galleryItemPallete
                colorGroup: SystemPalette.Active
            }

            property int gallery_index: index
            property int galleryPaneWidth: splitViewPaneGallery.width

            Rectangle {
                width: galleryPaneWidth
                height: (galleryPaneWidth / Constants.aspectX) * Constants.aspectY
                border.width: 2
                border.color: galleryCurrentIndex === gallery_index ? galleryItemPallete.highlight : galleryItemPallete.window

                SwipeView {
                    id: gallery_images

                    x: Constants.galleryImageSpacing / 2
                    y: Constants.galleryImageSpacing / 2
                    width: (galleryPaneWidth - Constants.galleryImageSpacing)
                    height: ((galleryPaneWidth - Constants.galleryImageSpacing) / Constants.aspectX) * Constants.aspectY
                    clip: true

                    Repeater {
                        id: gallery_image_repeater
                        model: images
                        Image {
                            id: image_of_gallery_view
                            width: (galleryPaneWidth - Constants.galleryImageSpacing)
                            height: ((galleryPaneWidth - Constants.galleryImageSpacing) / Constants.aspectX) * Constants.aspectY
                            source: image_source
                            fillMode: Image.PreserveAspectFit

                            MouseArea {
                                anchors.fill: parent
                                acceptedButtons: Qt.LeftButton | Qt.RightButton

                                onClicked:
                                {
                                    if (mouse.button === Qt.RightButton)
                                    {
                                        galleryContextMenu.popup()
                                    }
                                    else
                                    {
                                        galleryCurrentIndex = gallery_index
                                        galleries.currentIndex = galleryCurrentIndex
                                        galleryImagesCurrentIndex = -1
                                        images_of_gallery.currentIndex = galleryImagesCurrentIndex
                                        imageOfGalleryDescriptionListView.model.imageID = -1
                                        imageOfGalleryDescriptionIndex = -1
                                        images_of_gallery.model.galleryIndex = galleryCurrentIndex
                                        galleries.model.currentIndex = galleryCurrentIndex
                                        editCurrentGalleryDialog.setFields(galleries.model.getCurrentItem())
                                        addImageDialog.currentGalleryId = galleries.model.currentId
                                        images_of_gallery.model.galleryCurrentItem
                                        mouse.accepted = false
                                    }
                                }

                                onPressAndHold: {
                                    if (mouse.source === Qt.MouseEventNotSynthesized)
                                        galleryContextMenu.popup()
                                }

                                Menu {
                                    id: galleryContextMenu
                                    MenuItem { action: refreshGalleriesModel }
                                    MenuItem { action: createNewGallery }
                                    MenuItem { action: editCurrentGallery }
                                    MenuItem { action : addGalleryImage }
                                }
                            }
                        }
                    }
                }

            }


            PageIndicator {
               x:(galleryPaneWidth-width)/2
               height: Constants.leftSideBarPageIndicatorHeight
               currentIndex: gallery_images.currentIndex
               count: gallery_images.count
            }

            Text {
                id : gallery_description
                text: GalleryFunctions.description_first_part(description)
                x: (galleryPaneWidth-width)/2
                width: galleryPaneWidth - (Constants.leftSideBarWidth - Constants.leftSideBarTextWidth)
                wrapMode: Text.WordWrap
            }
        }
    }

    Component {
        id: gallery_image

        Rectangle {
            id: gallery_image_rect

            property int imageOfGallery_index: index
            property int imageOfGalleryHeight: slitViewPaneImagesOfGallery.height

            SystemPalette {
                id: imageOfGalleryItemPallete
                colorGroup: SystemPalette.Active
            }

            width: (imageOfGalleryHeight * Constants.aspectX) / Constants.aspectY
            height: imageOfGalleryHeight
            z: 0
            border.width: 2
            border.color: galleryImagesCurrentIndex === imageOfGallery_index ? imageOfGalleryItemPallete.highlight : imageOfGalleryItemPallete.window

            Image  {
                id: image_of_gallery
                x: Constants.imageOfGalleryImageSpacing / 2
                y: Constants.imageOfGalleryImageSpacing / 2
                width: ((imageOfGalleryHeight - Constants.imageOfGalleryImageSpacing) * Constants.aspectX) / Constants.aspectY
                height: (imageOfGalleryHeight - Constants.imageOfGalleryImageSpacing)
                source: image_source
                fillMode: Image.PreserveAspectFit
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton

                    onClicked:
                    {
                        if (mouse.button === Qt.RightButton)
                        {
                            imagesOfGalleryContextMenu.popup()
                        }
                        else
                        {
                            galleryImagesCurrentIndex = imageOfGallery_index
                            images_of_gallery.currentIndex = galleryImagesCurrentIndex
                            imageOfGalleryDescriptionListView.model.imageID = galleryImagesCurrentId
                            imageOfGalleryDescriptionIndex = -1
                            mouse.accepted = false
                        }
                    }

                    onPressAndHold: {
                        if (mouse.source === Qt.MouseEventNotSynthesized)
                            imagesOfGalleryContextMenu.popup()
                    }

                    Menu {
                        id: imagesOfGalleryContextMenu
                        MenuItem { action: refreshAllGalleryImagesModel }
                        MenuItem { action: removeCurrentImageOfGallery }
                        MenuItem {
                            action: showImagePointsAction
                            checkable: true
                            checked: showImagePoints
                        }
                    }
                }
                onStatusChanged: {
                    loadVoronoyDiagram(image_points, status);
                }
            }

            Connections {
                target: image_points
                function onImagePointsLoaded() {
                    loadVoronoyDiagram(image_points, image_of_gallery.status);
                }
            }

            function loadVoronoyDiagram(imagePoints, imageStatus)
            {
                if(imagePoints !== undefined && imagePoints !== null && !imagePoints.isEmpty() && imageStatus === Image.Ready)
                {
                    Qt.createQmlObject(
                                   "import MastactvaQuizEditor 1.0
                                    VoronoyDiagram {
                                        x: (image_of_gallery.width - image_of_gallery.paintedWidth)/2 + image_of_gallery.x
                                        y: (image_of_gallery.height - image_of_gallery.paintedHeight)/2 + image_of_gallery.y
                                        width: image_of_gallery.paintedWidth
                                        height: image_of_gallery.paintedHeight
                                        visible: showImagePoints
                                        opacity: 0.5
                                        z: 0.5
                                        model: image_points
                                        color: \"#000080\"
                                        clip: true
                                    }",
                                   gallery_image_rect,
                                   "voronoyDiagram")
                }
            }

            Item {
                anchors.fill: parent
                visible: showImagePoints
                opacity: 0.5
                z: 1.0
                Repeater {
                    id: imagePointsRepeater
                    model: image_points
                    Image {
                        id: pointImage

                        x: ptX() - Constants.pointImageSize/2
                        y: ptY() - Constants.pointImageSize/2
                        width: Constants.pointImageSize
                        height: Constants.pointImageSize

                        source: imageOfGalleryPointIndex === index ? Constants.activePoint : Constants.inactivePoint
                        function ptX()
                        {
                            return (gallery_image_rect.width - image_of_gallery.paintedWidth)/2 + (imagePoint_x * image_of_gallery.paintedWidth) + image_of_gallery.x
                        }

                        function ptY()
                        {
                            return (gallery_image_rect.height - image_of_gallery.paintedHeight)/2 + (imagePoint_y * image_of_gallery.paintedHeight) + image_of_gallery.y
                        }

                        property string nextImage: Constants.noImage
                        property int questionId: -1
                        property string questionText: "<Question>"

                        Connections {
                            target: imagePoint_toNextImage
                            function onImagePointToImageLoaded()
                            {
                                nextImage = imagePoint_toNextImage.imageSource
                                //imageOfGalleryNextImageNextImage.source
                            }
                        }

                        Connections {
                            target: imagePoint_toQuestion
                            function onImagePointToQuestionLoaded()
                            {
                                questionId = imagePoint_toQuestion.questionId
                            }

                            function onImagePointToQuestionTextLoaded()
                            {
                                questionText = imagePoint_toQuestion.question
                            }
                        }

                        MouseArea {
                            anchors.fill: parent

                            onClicked:
                            {
                                if (mouse.button === Qt.RightButton)
                                {
                                }
                                else
                                {
                                    if(imageOfGallery_index == galleryImagesCurrentIndex)
                                    {
                                        if(imageOfGalleryPointIndex !== -1)
                                        {
                                            imagePointsRepeater.itemAt(imageOfGalleryPointIndex).source = Constants.inactivePoint
                                        }

                                        imageOfGalleryPointIndex = index;
                                        //console.log("imageOfGalleryPointIndex = ", imageOfGalleryPointIndex);
                                        pointImage.source = Constants.activePoint
                                        imageOfGalleryNextImageNextImage.source = pointImage.nextImage
                                        imageOfGalleryQuestionAnswersListView.model.questionId = questionId
                                        imageOfGalleryQuestionText.text = questionText
                                    }
                                    mouse.accepted = false;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Component {
        id: imageOfGalleryDescriptionListViewItem
        Item {

            property int imageOfGalleryDescriptionWidth: slitViewPaneImageOfGallery.width

            width: imageOfGalleryDescriptionWidth
            height: Constants.descriptionRowHeight
            Column {
                TextArea {
                    focus: true
                    readOnly: true
                    width: imageOfGalleryDescriptionWidth
                    height: Constants.descriptionRowHeight*4/5
                    text: GalleryFunctions.description_first_part(description_description)
                }
                Text { text: qsTr("<b>From : </b>") + description_fromDateTime }
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton

                onClicked:
                {
                    if (mouse.button === Qt.RightButton)
                    {
                        imageOfGalleryDescriptionListViewItemMenu.popup()
                    }
                    else
                    {
                        imageOfGalleryDescriptionListView.currentIndex = index
                        imageOfGalleryDescriptionIndex = index
                        mouse.accepted = false
                    }
                }

                onPressAndHold: {
                    if (mouse.source === Qt.MouseEventNotSynthesized)
                        imageOfGalleryDescriptionListViewItemMenu.popup()
                }

                Menu {
                    id: imageOfGalleryDescriptionListViewItemMenu
                    MenuItem { action: imageOfGalleryCreateDescription }
                    MenuItem { action: imageOfGalleryEditDescription }
                    MenuItem { action: imageOfGalleryDeleteDescription }
                }
            }
        }
    }

    Component {
        id: imageOfGalleryQuestionAnswersListViewItem
        Item {
            width: imageOfGalleryQuestionAnswersListView.width
            height: 40 // TODO use text metrics
            Row {
                Text { text: questionAnswer_text }
                Text { text: qsTr(" ( points = ") +  questionAnswer_pointsToPass + qsTr(")")}
            }
            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton

                onClicked:
                {
                    if (mouse.button === Qt.RightButton)
                    {
                        imageOfGalleryQuestionAnswersListViewItemMenu.popup()
                    }
                    else
                    {
                        imageOfGalleryQuestionAnswersListView.currentIndex = index
                        mouse.accepted = false
                    }
                }

                onPressAndHold: {
                    if (mouse.source === Qt.MouseEventNotSynthesized)
                        imageOfGalleryQuestionAnswersListViewItemMenu.popup()
                }

                Menu {
                    id: imageOfGalleryQuestionAnswersListViewItemMenu
                    MenuItem { action: imageOfGalleryCreateDescription }
                    MenuItem { action: imageOfGalleryEditDescription }
                    MenuItem { action: imageOfGalleryDeleteDescription }
                }
            }
        }
    }
}

