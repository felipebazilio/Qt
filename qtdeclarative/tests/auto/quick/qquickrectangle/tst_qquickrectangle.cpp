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
#include <qtest.h>
#include <QtTest/QSignalSpy>

#include <QtGui/qscreen.h>
#include <QtQml/qqmlengine.h>
#include <QtQml/qqmlcomponent.h>
#include <QtQuick/qquickview.h>
#include <private/qquickitem_p.h>
#include <private/qquickrectangle_p.h>

#include "../../shared/util.h"

class tst_qquickrectangle : public QQmlDataTest
{
    Q_OBJECT
public:
    tst_qquickrectangle();

private slots:
    void color();
    void gradient();
    void gradient_border();
    void gradient_separate();
    void gradient_multiple();
    void antialiasing();

private:
    QQmlEngine engine;
};

tst_qquickrectangle::tst_qquickrectangle()
{
}

void tst_qquickrectangle::color()
{
    if (QGuiApplication::primaryScreen()->depth() < 24)
        QSKIP("This test does not work at display depths < 24");

    QQuickView view;
    view.setSource(testFileUrl("color.qml"));
    view.show();

    QVERIFY(QTest::qWaitForWindowExposed(&view));

    QImage image = view.grabWindow();
    QVERIFY(image.pixel(0,0) == QColor("#020202").rgba());
}

void tst_qquickrectangle::gradient()
{
    QQmlComponent component(&engine, testFileUrl("gradient.qml"));
    QQuickRectangle *rect = qobject_cast<QQuickRectangle*>(component.create());
    QVERIFY(rect);

    QQuickGradient *grad = rect->gradient();
    QVERIFY(grad);

    QQmlListProperty<QQuickGradientStop> stops = grad->stops();
    QCOMPARE(stops.count(&stops), 2);
    QCOMPARE(stops.at(&stops, 0)->position(), 0.0);
    QCOMPARE(stops.at(&stops, 0)->color(), QColor("gray"));
    QCOMPARE(stops.at(&stops, 1)->position(), 1.0);
    QCOMPARE(stops.at(&stops, 1)->color(), QColor("white"));

    QGradientStops gradientStops = grad->gradientStops();
    QCOMPARE(gradientStops.count(), 2);
    QCOMPARE(gradientStops.at(0).first, 0.0);
    QCOMPARE(gradientStops.at(0).second, QColor("gray"));
    QCOMPARE(gradientStops.at(1).first, 1.0);
    QCOMPARE(gradientStops.at(1).second, QColor("white"));

    QMetaObject::invokeMethod(rect, "resetGradient");

    grad = rect->gradient();
    QVERIFY(!grad);

    delete rect;
}

void tst_qquickrectangle::gradient_border()
{
    QQuickView view;
    view.setSource(testFileUrl("gradient-border.qml"));
    view.show();

    QVERIFY(QTest::qWaitForWindowExposed(&view));
}

// A gradient not defined inline with the Rectangle using it should still change
// that Rectangle.
void tst_qquickrectangle::gradient_separate()
{
    QQuickView view;
    view.setSource(testFileUrl("gradient-separate.qml"));
    view.show();

    QVERIFY(QTest::qWaitForWindowExposed(&view));

    QQuickRectangle *rect = qobject_cast<QQuickRectangle*>(view.rootObject());
    QVERIFY(rect);

    // Start off clean
    QQuickItemPrivate *rectPriv = QQuickItemPrivate::get(rect);
    bool isDirty = rectPriv->dirtyAttributes & QQuickItemPrivate::Content;
    QVERIFY(!isDirty);

    QMetaObject::invokeMethod(rect, "changeGradient");

    // Changing the gradient should have scheduled an update of the item.
    isDirty = rectPriv->dirtyAttributes & QQuickItemPrivate::Content;
    QVERIFY(isDirty);
}

// When a gradient is changed, every Rectangle connected to it must update.
void tst_qquickrectangle::gradient_multiple()
{
    QQuickView view;
    view.setSource(testFileUrl("gradient-multiple.qml"));
    view.show();

    QVERIFY(QTest::qWaitForWindowExposed(&view));

    QQuickRectangle *firstRect = qobject_cast<QQuickRectangle*>(view.rootObject()->property("firstRectangle").value<QObject*>());
    QQuickRectangle *secondRect = qobject_cast<QQuickRectangle*>(view.rootObject()->property("secondRectangle").value<QObject*>());
    QVERIFY(firstRect);
    QVERIFY(secondRect);

    // Start off clean
    QQuickItemPrivate *firstRectPriv = QQuickItemPrivate::get(firstRect);
    QQuickItemPrivate *secondRectPriv = QQuickItemPrivate::get(secondRect);
    bool firstIsDirty = firstRectPriv->dirtyAttributes & QQuickItemPrivate::Content;
    bool secondIsDirty = secondRectPriv->dirtyAttributes & QQuickItemPrivate::Content;
    QVERIFY(!firstIsDirty);
    QVERIFY(!secondIsDirty);

    QMetaObject::invokeMethod(view.rootObject(), "changeGradient");

    // Changing the gradient should have scheduled an update of both items
    firstIsDirty = firstRectPriv->dirtyAttributes & QQuickItemPrivate::Content;
    secondIsDirty = secondRectPriv->dirtyAttributes & QQuickItemPrivate::Content;
    QVERIFY(firstIsDirty);
    QVERIFY(secondIsDirty);
}

void tst_qquickrectangle::antialiasing()
{
    QQmlComponent component(&engine);
    component.setData("import QtQuick 2.0\n Rectangle {}", QUrl());
    QScopedPointer<QObject> object(component.create());
    QQuickRectangle *rect = qobject_cast<QQuickRectangle *>(object.data());
    QVERIFY(rect);

    QSignalSpy spy(rect, SIGNAL(antialiasingChanged(bool)));

    QCOMPARE(rect->antialiasing(), false);

    rect->setAntialiasing(true);
    QCOMPARE(rect->antialiasing(), true);
    QCOMPARE(spy.count(), 1);

    rect->setAntialiasing(true);
    QCOMPARE(spy.count(), 1);

    rect->resetAntialiasing();
    QCOMPARE(rect->antialiasing(), false);
    QCOMPARE(spy.count(), 2);

    rect->setRadius(5);
    QCOMPARE(rect->antialiasing(), true);
    QCOMPARE(spy.count(), 3);

    rect->resetAntialiasing();
    QCOMPARE(rect->antialiasing(), true);
    QCOMPARE(spy.count(), 3);

    rect->setRadius(0);
    QCOMPARE(rect->antialiasing(), false);
    QCOMPARE(spy.count(), 4);

    rect->resetAntialiasing();
    QCOMPARE(rect->antialiasing(), false);
    QCOMPARE(spy.count(), 4);

    rect->setRadius(5);
    QCOMPARE(rect->antialiasing(), true);
    QCOMPARE(spy.count(), 5);

    rect->resetAntialiasing();
    QCOMPARE(rect->antialiasing(), true);
    QCOMPARE(spy.count(), 5);

    rect->setAntialiasing(false);
    QCOMPARE(rect->antialiasing(), false);
    QCOMPARE(spy.count(), 6);

    rect->resetAntialiasing();
    QCOMPARE(rect->antialiasing(), true);
    QCOMPARE(spy.count(), 7);
}

QTEST_MAIN(tst_qquickrectangle)

#include "tst_qquickrectangle.moc"
