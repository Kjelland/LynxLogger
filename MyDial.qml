import QtQuick 2.0
import QtQuick.Controls 2.12


Item {
    id:item1
    property int sizeH: 100
    property int sizeW: 100
    property string label: "-"
    property int variable: 0
    property var value: 0
    property var from: -2
    property var to: 2

    implicitHeight: sizeH
    implicitWidth: sizeW
    Dial{

        value: item1.value

        implicitHeight: sizeH
        implicitWidth: sizeW

        from:item1.from
        to:item1.to
        onPressedChanged:
        {

            if (!pressed)
            {
                backEnd.setData(value,variable)
            }

        }

        Text {
            id: name
            text: qsTr(label)
            anchors.centerIn: parent
            font.pointSize: 16
        }

    }

}
