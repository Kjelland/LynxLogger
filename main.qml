import QtQuick 2.7
import QtQuick.Window 2.2
//import backend 1.1
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

Item
{
    id:mains
    visible: true
    width: 1000
    height: 600
    Material.theme: Material.Light
        Material.accent: Material.Purple


    Row
    {
        id: connectRow
        width: 300
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.top: parent.top
        anchors.topMargin: 0
        spacing: 5

        ComboBox
        {
            width: 200
            id: portComboBox
            // currentIndex: -1
            palette {
                button: "white"

                buttonText: "black"
            }
            model:
                ListModel
            {
                id: portListModel
                ListElement{text: qsTr("Select port")}
            }

            onActivated:
            {
                // console.log("Activated")
                if (currentIndex > 0)
                {
                    backEnd.portSelected(portComboBox.currentIndex - 1)
                    connectButton.enabled = true
                }
                else
                    connectButton.enabled = false
            }

            onPressedChanged:
            {
                if (pressed)
                {
                    backEnd.refreshPortList()
                    if (portListModel.count < 2)
                        connectButton.enabled = false
                    // console.log("Pressed")
                }
            }
        }

        Button
        {
            width: 100
            id: connectButton
            enabled: false
            text: qsTr("Connect")

            palette {
                button: "white"

                buttonText: "black"
            }

            onClicked:
            {
                backEnd.connectButtonClicked()
                if (backEnd.uartConnected())
                {
                    text = qsTr("Disconnect")
                    portComboBox.enabled = false
                }
                else
                {
                    text = qsTr("Connect")
                    portComboBox.enabled = true
                }
            }
        }


    }
//    BackEnd
//    {
//        id: backEnd


//        onGetData: {
//            switch (variable)
//            {
//            case BackEnd.Ex:
//                dialX.value = input
//                break;

//            case BackEnd.Ey:
//                dialY.value = input
//                break;

//            case BackEnd.Ez:
//                dialZ.value = input
//                break;
//            case BackEnd.Eroll:
//                dialRoll.value = input
//                break;
//            case BackEnd.Epitch:
//                dialPitch.value = input
//                break;
//            case BackEnd.Eyaw:
//                dialYaw.value = input
//                break;
//            }
//        }

//        onClearPortList:
//        {
//            console.log(("clear"));
//            portListModel.clear()
//            portListModel.append({ text: qsTr("Select port") })
//        }
//        onAddPort: portListModel.append({ text: portName })
//    }


    Row {
        id: scopeSetting
        height: 50
        layoutDirection: Qt.RightToLeft
        anchors.left: connectRow.right
        anchors.leftMargin: 100
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 50

        IconButton{
            id:verticalScaleButton
            filename:"icons8-resize-vertical-50"
            tooltip: "Resize Vertical"
            onClicked: scopeView.autoscale()
        }
        IconButton{
            id:horizontalScaleButton
            filename:"icons8-resize-horizontal-50"
            tooltip: "Resize Horizontal"
            onClicked: scopeView.resizeHorizontal()
        }
        IconButton{
            id:expandButton
            filename:"icons8-expand-50"
            tooltip: "Zoom all"
            onClicked: {
                scopeView.resizeHorizontal()
                scopeView.autoscale()
            }
        }
        IconButton{
            id:linePlot
            filename:"icons8-plot-50"
            tooltip: "Line Plot"
            onClicked: scopeView.changeSeriesType("line");
        }
        IconButton{
            id:scatterPlot
            filename:"icons8-scatter-plot-50"
            tooltip: "Scatter Plot"
            onClicked: scopeView.changeSeriesType("scatter");
        }
        IconButton{
            id:saveToFile
            filename:"icons8-save-50"
            tooltip: "Save to CSV"
            onClicked: backend.saveToTextfile()
        }
        IconButton{
            id:pauseChartRefresh
            filename:"icons8-pause-50"
            tooltip: "Pause"
            onClicked: {
                console.log("pause")
                backend.pauseChartviewRefresh()
            }
        }
        IconButton{
            id:resumeChartRefresh
            filename:"icons8-play-50"
            tooltip: "Play"
            onClicked:{
                console.log("resume")
                backend.resumeChartviewRefresh()
            }
        }
        IconButton{
            id:loadFile
            filename:"icons8-folder-50"
            tooltip: "Load File"
            onClicked:{
                backend.readFromTextfile("")
            }
        }
    }

    ScopeView {



        id: scopeView
        x: 1000
        y: 0
        z: -1
        anchors.top: connectRow.bottom
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottomMargin: 0
        anchors.topMargin: 0

//        onOpenGLSupportedChanged: {
//            if (!openGLSupported) {
//                controlPanel.openGLButton.enabled = false
//                controlPanel.openGLButton.currentSelection = 0
//            }
//        }
    }

}


/*##^## Designer {
    D{i:1;anchors_x:380;anchors_y:20}D{i:11;anchors_height:600;anchors_x:420}D{i:8;anchors_width:709;anchors_x:204;anchors_y:55}
D{i:14;anchors_height:600;anchors_x:420}
}
 ##^##*/
