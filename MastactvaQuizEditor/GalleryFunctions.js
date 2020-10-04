.pragma library
.import QtQuick 2.0 as QtQuickModuleImportedInJS

function description_first_part(str) {
    return str.substring(0, 128);
}

/*var voronoyDiagramParams = null;
var voronoyDiagramComponent = null;
var voronoyDiagramParent = null;

function createVoronoyDiagram(parent, params) {
    voronoyDiagramParent = parent;
    voronoyDiagramParams = params;
    voronoyDiagramComponent = Qt.createComponent("VoronoyDiagram.qml");
    console.log("voronoyDiagramComponent = ", voronoyDiagramComponent, " voronoyDiagramParams = ", voronoyDiagramParams);
    if (voronoyDiagramComponent.status === QtQuickModuleImportedInJS.Component.Ready
            || voronoyDiagramComponent.status === QtQuickModuleImportedInJS.Component.Error) {
        finishVoronoyDiagramCreation();
    } else {
        voronoyDiagramComponent.statusChanged.connect(finishVoronoyDiagramCreation);
    }
}

function finishVoronoyDiagramCreation()
{
    console.log("finishVoronoyDiagramCreation()");
    if (voronoyDiagramComponent.status === QtQuickModuleImportedInJS.Component.Ready) {
        var voronoyDiagram = voronoyDiagramComponent.createObject(voronoyDiagramParent, voronoyDiagramParams);
        if (voronoyDiagram === null) {
            console.log("Error creating diagram");
        }
    } else if (voronoyDiagramComponent.status === QtQuickModuleImportedInJS.Component.Error) {
        console.log("Error loading component:", voronoyDiagramComponent.errorString());
    }
}*/
