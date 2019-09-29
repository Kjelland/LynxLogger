import QtQuick 2.7
import QtQuick.Window 2.2
import backend 1.1
import QtQuick.Controls 2.12

Item
{
    id:mains
    visible: true
    width: 1000
    height: 600

    Row
    {
        id: connectRow
        anchors.right: parent.right
        anchors.rightMargin: 0
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

        Switch {
            id: imuInitDone
            text: qsTr("Gyro Init done")
            checked: backEnd.sta & (1<<4)
            checkable: false
        }

        Switch {
            id: useYawButton
            text: qsTr("Enable Yaw Compentation")
            onClicked: backEnd.enableYawButtonClicked(!(backEnd.sta & (1<<2)))
            checked: backEnd.sta & (1<<2)
        }

        Switch {
            id: element
            text: qsTr("Gyro Compensation")
            onClicked: backEnd.gyroConnectButtonClicked(!(backEnd.sta & (1<<3)))
            checked: backEnd.sta & (1<<3)
        }

        Button{
            id:autoscale
            onClicked: scopeView.autoscale(true)
        }


    }
    BackEnd
    {
        id: backEnd


        onGetData: {
            switch (variable)
            {
            case BackEnd.Ex:
                dialX.value = input
                break;

            case BackEnd.Ey:
                dialY.value = input
                break;

            case BackEnd.Ez:
                dialZ.value = input
                break;
            case BackEnd.Eroll:
                dialRoll.value = input
                break;
            case BackEnd.Epitch:
                dialPitch.value = input
                break;
            case BackEnd.Eyaw:
                dialYaw.value = input
                break;
            }
        }

        onClearPortList:
        {
            portListModel.clear()
            portListModel.append({ text: qsTr("Select port") })
        }
        onAddPort: portListModel.append({ text: portName })
    }



    ControlPanel {
        id: controlPanel
        x: 10
        y: 10
        width: 150
        height: 200
        anchors.bottomMargin: 0
        anchors.top: connectRow.bottom
        anchors.topMargin: 0
        anchors.bottom: dialRow.top
        anchors.left: parent.left
        anchors.leftMargin: 0
        //![1]

        onSampleChanged: {
            scopeView.nMin = count;
        }
        onSeriesTypeChanged: scopeView.changeSeriesType(type);
        onRefreshRateChanged: scopeView.changeRefreshRate(rate);
        onAntialiasingEnabled: scopeView.antialiasing = enabled;
        onOpenGlChanged: {
            scopeView.openGL = enabled;
        }
    }


    ScopeView {



        id: scopeView
        x: 1000
        y: 0
        anchors.top: connectRow.bottom
        anchors.bottom: dialRow.top
        anchors.right: parent.right
        anchors.left: controlPanel.right
        height: mains.height
        anchors.bottomMargin: 0
        anchors.topMargin: 0

        onOpenGLSupportedChanged: {
            if (!openGLSupported) {
                controlPanel.openGLButton.enabled = false
                controlPanel.openGLButton.currentSelection = 0
            }
        }
    }

    Row {
        id: dialRow
        y: 20
        width: 400
        height: 100
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        MyDial{
            id:dialX
            variable: BackEnd.Ex
            label: "X"
            from: -2
            to:2

        }
        MyDial{
            id:dialY
            variable: BackEnd.Ey
            label: "Y"
            from: -2
            to:2

        }
        MyDial{
            id:dialZ
            variable: BackEnd.Ez
            label: "Z"
            from: -2
            to:2

        }
        MyDial{
            id:dialRoll
            variable: BackEnd.Eroll
            label: "R"
            from: -30
            to:30

        }
        MyDial{
            id:dialPitch
            variable: BackEnd.Epitch
            label: "P"
            from: -30
            to:30
        }
        MyDial{
            id:dialYaw
            variable: BackEnd.Eyaw
            label: "W"
            from: -30
            to:30
        }

        //            Dial
        //            {
        //                id:setX
        //                from: -180
        //                to:180
        //                value: 0
        //                onPressedChanged:
        //                {

        //                    if (!pressed)
        //                    {
        //                        backEnd.setData(value,BackEnd.Ex)
        //                    }

        //                }
        //            }
    }

    Column {
        id: mruRow
        x: 440
        y: 20
        width: 200
        height: 79
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0

        Label {
            id: labelPitch
            text: qsTr("Pitch: ") + backEnd.pitch.toFixed(2) + qsTr(" degrees")
            horizontalAlignment: Text.AlignRight
            font.pointSize: 15
        }

        Label {
            id: labelRoll
            text: qsTr("Roll: ") + backEnd.roll.toFixed(2) + qsTr(" degrees")
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: 15
        }

        Label {
            id: labelYaw
            text: qsTr("Yaw: ") + backEnd.yaw.toFixed(2) + qsTr(" degrees")
            font.pointSize: 15
        }
    }








}









/*##^## Designer {
    D{i:1;anchors_x:380;anchors_y:20}D{i:12;anchors_x:420}
}
 ##^##*/
