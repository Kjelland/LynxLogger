/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Charts module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.12
import QtCharts 2.3

//![1]

ChartView {
    id: chartView
    animationOptions: ChartView.NoAnimation
    theme: ChartView.ChartThemeLight
    antialiasing: true
    property var setMaxAxis: 0
    property var setMinAxis: 0
    property bool openGL: true
    property bool openGLSupported: true
    property var xaxis: new Date()
    property var xaxis_min: new Date()
    property var nMin : 1
    onOpenGLChanged: {
        if (openGLSupported) {
            series("signal 1").useOpenGL = openGL;
            series("signal 2").useOpenGL = openGL;
        }
    }
    Component.onCompleted: {
        if (!series("signal 1").useOpenGL) {
            openGLSupported = false
            openGL = false
        }
    }

    Rectangle{
    id: rubberBandRec1
    border.color: "black"
    border.width: 1
    opacity: 0.3
    visible: false
    transform: Scale { origin.x: 0; origin.y: 0; yScale: -1}
    }
    Rectangle{
    id: rubberBandRec2
    border.color: "black"
    border.width: 1
    opacity: 0.3
    visible: false
    transform: Scale { origin.x: 0; origin.y: 0; yScale: -1; xScale: -1}
    }
    Rectangle{
    id: rubberBandRec3
    border.color: "black"
    border.width: 1
    opacity: 0.3
    visible: false
    transform: Scale { origin.x: 0; origin.y: 0; xScale: -1}
    }
    Rectangle{
    id: rubberBandRec4
    border.color: "black"
    border.width: 1
    opacity: 0.3
    visible: false
    }
    MouseArea{
        property bool hasBeenPressedAndHoled: false
        id:mouseArea

        hoverEnabled: true
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onPressed: {
            rubberBandRec1.x = mouseX; rubberBandRec1.y = mouseY; rubberBandRec1.visible = true;
            rubberBandRec2.x = mouseX; rubberBandRec2.y = mouseY; rubberBandRec2.visible = true;
            rubberBandRec3.x = mouseX; rubberBandRec3.y = mouseY; rubberBandRec3.visible = true;
            rubberBandRec4.x = mouseX; rubberBandRec4.y = mouseY; rubberBandRec4.visible = true;
            console.log("pressed");
            hasBeenPressedAndHoled=true

        }

        onMouseXChanged: {
        rubberBandRec1.width = mouseX - rubberBandRec1.x;
        rubberBandRec2.width = rubberBandRec2.x-mouseX;
        rubberBandRec3.width = rubberBandRec3.x-mouseX;
        rubberBandRec4.width = mouseX - rubberBandRec4.x;
        }
        onMouseYChanged: {
        rubberBandRec1.height = rubberBandRec1.y - mouseY;
        rubberBandRec2.height = rubberBandRec2.y - mouseY;
        rubberBandRec3.height = mouseY - rubberBandRec3.y;
        rubberBandRec4.height = mouseY - rubberBandRec4.y;
        }
        onReleased: {
            if(hasBeenPressedAndHoled)
            {
                var x = rubberBandRec4.x-(rubberBandRec4.width<0)*Math.abs(rubberBandRec4.width);
                var y = rubberBandRec4.y-(rubberBandRec4.height<0)*Math.abs(rubberBandRec4.height);
                if (Math.abs(rubberBandRec4.width*rubberBandRec4.height)>100)
                chartView.zoomIn(Qt.rect(x, y, Math.abs(rubberBandRec4.width),
                Math.abs(rubberBandRec4.height)));
                rubberBandRec1.visible = false;
                rubberBandRec2.visible = false;
                rubberBandRec3.visible = false;
                rubberBandRec4.visible = false;

            }
hasBeenPressedAndHoled=false

        }
        onClicked: {
            if(mouse.button === Qt.RightButton)
            {
                chartView.zoomReset();
               console.log("Right button used");
           }
//            if(mouse.button === Qt.LeftButton)
//            {
//                chartView.zoomIn();
//               console.log("Left button used");
//           }

        }

    }

    PinchArea{
                id: pa

                anchors.fill: parent
                onPinchUpdated: {
                     console.log ("pich clicked")
                    chartView.zoomReset();
//                    var center_x = pinch.center.x
//                    var center_y = pinch.center.y
//                    var width_zoom = height/pinch.scale;
//                    var height_zoom = width/pinch.scale;
//                    var r = Qt.rect(center_x-width_zoom/2, center_y - height_zoom/2, width_zoom, height_zoom)
//                    chartView.zoomIn(r)
                }

            }
    ValueAxis {
        id: axisY1
        min: -1
        max: 1
    }

    ValueAxis {
        id: axisY2
        min: -10
        max: 10
    }
    DateTimeAxis {
        id:axisX
        //format: "mm hh mm ss"
        format: "hh:mm:ss"
        tickCount: 5

       min:xaxis_min
       max:xaxis


    }

    LineSeries {
        id: lineSeries1
        name: "signal 1"
        axisX: axisX
        axisY: axisY1
        useOpenGL: chartView.openGL
    }
    LineSeries {
        id: lineSeries2
        name: "signal 2"
        axisX: axisX
        axisYRight: axisY2
        useOpenGL: chartView.openGL
    }
//![1]

    //![2]
    Timer {
        id: refreshTimer
        interval: 1 / 60 * 1000 // 60 Hz
        running: true
        repeat: true
        onTriggered: {
            backend.update(chartView.series(0),0);
            backend.update(chartView.series(1),1);
            if(!chartView.isZoomed())
            {
            xaxis=new Date()
            var today = new Date();
            var ONE_HOUR =  nMin * 60 * 1000; /* ms */
            today.setDate(today.getDate()-ONE_HOUR)


            xaxis_min=new Date(new Date() - ONE_HOUR)
            }
        }
    }
    //![2]

    //![3]
    function autoscale(enabled)
    {
        chartView.zoomReset();
        axisY1.max=backend.getMax()*1.05
        axisY1.min=backend.getMin()*1.05
        axisY2.max=backend.getMax()*1.05
        axisY2.min=backend.getMin()*1.05
    }

    function changeSeriesType(type) {
        chartView.removeAllSeries();

        // Create two new series of the correct type. Axis x is the same for both of the series,
        // but the series have their own y-axes to make it possible to control the y-offset
        // of the "signal sources".
        if (type == "line") {
            var series1 = chartView.createSeries(ChartView.SeriesTypeLine, "signal 1",
                                                 axisX, axisY1);
            series1.useOpenGL = chartView.openGL

            var series2 = chartView.createSeries(ChartView.SeriesTypeLine, "signal 2",
                                                 axisX, axisY2);
            series2.useOpenGL = chartView.openGL
        } else {
            var series1 = chartView.createSeries(ChartView.SeriesTypeScatter, "signal 1",
                                                 axisX, axisY1);
            series1.markerSize = 2;
            series1.borderColor = "transparent";
            series1.useOpenGL = chartView.openGL

            var series2 = chartView.createSeries(ChartView.SeriesTypeScatter, "signal 2",
                                                 axisX, axisY2);
            series2.markerSize = 2;
            series2.borderColor = "transparent";
            series2.useOpenGL = chartView.openGL
        }
    }

//    function createAxis(min, max) {
//        // The following creates a ValueAxis object that can be then set as a x or y axis for a series
//        return Qt.createQmlObject("import QtQuick 2.0; import QtCharts 2.0; ValueAxis { min: "
//                                  + min + "; max: " + max + " }", chartView);
//    }
    //![3]

    function setAnimations(enabled) {
        if (enabled)
            chartView.animationOptions = ChartView.SeriesAnimations;
        else
            chartView.animationOptions = ChartView.NoAnimation;
    }

    function changeRefreshRate(rate) {
        refreshTimer.interval = 1 / Number(rate) * 1000;
    }
}
