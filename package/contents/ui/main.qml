import QtQuick 2.0
import QtQuick.Layouts 1.1
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as Components
import org.kde.plasma.plasmoid 2.0

Item {
    id: main

    height: 50
    width: 200

    Layout.fillWidth: true

    // We don't want an icon when in panel.
    Plasmoid.preferredRepresentation: Plasmoid.fullRepresentation

    PlasmaCore.DataSource {
        id: dataSource
        engine: "i3ipc"
        connectedSources: ["workspace"]
        onNewData:{
            if(sourceName== "workspace"){
                label.text = formatWorkspaceStates(JSON.parse(data.state))
            }
        }

    }

    function formatWorkspaceStates(stateList) {
        var formattedWorkspaces = [];
        stateList.sort(function(a, b) { return a.num - b.num })
        for(var i = 0; i < stateList.length; i++) {
            var workspace = stateList[i];
            if (workspace.focused) {
                formattedWorkspaces.push("<font color='" +
                                         theme.buttonFocusColor +
                                         "'>[" + workspace.name + "]</font>")
            } else if (workspace.urgent) {
                formattedWorkspaces.push("<font color='red'>" + workspace.name + "!</font>")
            } else if (workspace.visible) {
                formattedWorkspaces.push("(" + workspace.name + ")")
            } else {
                formattedWorkspaces.push(workspace.name)
            }
        }
        return formattedWorkspaces.join(" ")
    }

    Components.Label  {
        id: label
        clip: true
        anchors.fill: parent
        textFormat: Text.StyledText

        // All this is needed to make the text fit itself vertically.
        fontSizeMode: Text.VerticalFit
        minimumPixelSize: 1
        height: 0
        width: 0
        font {
            pixelSize: 1024
            pointSize: 0
        }
    }
}
