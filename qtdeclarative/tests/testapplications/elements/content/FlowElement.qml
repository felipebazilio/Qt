/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
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
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0

Item {
    id: flowelementtest
    anchors.fill: parent
    property string testtext: ""

    Rectangle { anchors.fill: flowelement; color: "lightsteelblue"; radius: 5 }

    Flow {
        id: flowelement
        height: 110; width: 110
        spacing: 5; flow: Flow.LeftToRight
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 15
        Rectangle { id: gr; color: "green"; height: 50; width: 50; border.color: "gray"; border.width: 3; opacity: .9; radius: 5; clip: true
            Text { anchors.centerIn: parent; text: "1" }
        }
        Rectangle { id: re; color: "red"; height: 50; width: 50; border.color: "gray"; border.width: 3; opacity: .9; radius: 5; clip: true
            Text { anchors.centerIn: parent; text: "2" }
        }
        Rectangle { id: bl; color: "blue"; height: 50; width: 50; border.color: "gray"; border.width: 3; opacity: .9; radius: 5; clip: true; visible: false
            Text { anchors.centerIn: parent; text: "3" }
        }
        Rectangle { id: bk; color: "black"; height: 50; width: 50; border.color: "gray"; border.width: 3; opacity: .9; radius: 5; clip: true
            Text { anchors.centerIn: parent; text: "4"; color: "lightgray" }
        }

        move: Transition { NumberAnimation { properties: "x"; duration: 500; easing.type: Easing.OutBounce } }
        add: Transition { NumberAnimation { properties: "x"; duration: 500; easing.type: Easing.OutBounce } }

    }

    SystemTestHelp { id: helpbubble; visible: statenum != 0
        anchors { top: parent.top; horizontalCenter: parent.horizontalCenter; topMargin: 50 }
    }
    BugPanel { id: bugpanel }

    states: [
        State { name: "start"; when: statenum == 1
            PropertyChanges { target: flowelementtest
                testtext: "This is a Flow element. At present it should be showing three rectangles - green, red and black.\n"+
                "Next, let's add a rectangle to the Flow - it should slide in from the left and the black rectangle should move to give it space" }
        },
        State { name: "added"; when: statenum == 2
            PropertyChanges { target: bl; visible: true }
            PropertyChanges { target: flowelementtest
                testtext: "Flow should now be showing four rectangles - green, red, blue and black.\n"+
                "Next let's change the direction of the flow to vertical." }
        },
        State { name: "vertical"; when: statenum == 3
            PropertyChanges { target: bl; visible: true }
            PropertyChanges { target: flowelement; flow: Flow.TopToBottom }
            PropertyChanges { target: flowelementtest
                testtext: "Flow should now be showing four rectangles - green, blue, red and black.\n"+
                "Next, let's flip the layout direction to RightToLeft." }
        },
        State { name: "rtlvertical"; when: statenum == 4
            PropertyChanges { target: bl; visible: true }
            PropertyChanges { target: flowelement; flow: Flow.TopToBottom; layoutDirection: Qt.RightToLeft }
            PropertyChanges { target: flowelementtest
                testtext: "Flow should now be showing the four rectangles aligned to the right and in a column order flowing to the left"+
                "Advance to restart the test." }
        }
    ]
}
