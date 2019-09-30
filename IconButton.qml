import QtQuick 2.0
import QtQuick.Controls 2.5

Button {
    property string filename: ""
    property string tooltip: ""
    property string pixelSize: "50"


    width: Number(pixelSize)
    height: Number(pixelSize)
    palette {
        button: "white"

        //buttonText: "white"
    }
    icon.width:Number(pixelSize)
    icon.height:Number(pixelSize)
    icon.name: "edit-cut"
    icon.source: "icons/"+filename+".png"
    hoverEnabled: true

    ToolTip.delay: 500
    ToolTip.timeout: 3000
    ToolTip.visible: hovered
    ToolTip.text: qsTr(tooltip)


}
