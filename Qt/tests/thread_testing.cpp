/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2015  <copyright holder> <email>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "Qt/functional.h"
#include <QtTest/QTest>

class ThreadTesting : public QObject {
    Q_OBJECT
private slots:
    void testThreadSimple();
    void testThreadWithCallback();
    void testThreadWithPreRunCallback();
    void testThreadWithCallbackWithResult();
    void testThreadWithPreRunCallbackWithResult();
};
using namespace GuLinux;
void ThreadTesting::testThreadSimple()
{
    int number = 1;
    auto current_thread_id = QThread::currentThreadId();
    Thread::Run<void>([&number, &current_thread_id]{ number++; current_thread_id = QThread::currentThreadId(); });
    QThread::msleep(100);
    QCOMPARE(number, 2);
    QVERIFY(current_thread_id != QThread::currentThreadId());
}

void ThreadTesting::testThreadWithCallback()
{
    int number = 1;
    bool stopped = false;
    Thread::Run<void>([&number]{ number++; }, [&stopped] {stopped = true; });
    while(!stopped)
        qApp->processEvents();
    QCOMPARE(number, 2);
}

void ThreadTesting::testThreadWithPreRunCallback()
{
    int number = 1;
    bool stopped = true;
    Thread::Run<void>([&number]{ number++; }, [&stopped] {stopped = true; }, [&stopped](Thread *) { stopped = false; } );
    while(!stopped)
        qApp->processEvents();
    QCOMPARE(number, 2);

}



void ThreadTesting::testThreadWithCallbackWithResult()
{
    int number = 1;
    bool stopped = false;
    Thread::Run<int>([]{ return 2;}, [&stopped, &number] (int result) { number = result; stopped = true; });
    while(!stopped)
        qApp->processEvents();
    QCOMPARE(number, 2);
}

void ThreadTesting::testThreadWithPreRunCallbackWithResult()
{
    int number = 1;
    bool stopped = true;
    Thread::Run<int>([]{ return 2;}, [&stopped, &number] (int result) { number = result; stopped = true; }, [&stopped](Thread*){ stopped = false; });
    while(!stopped)
        qApp->processEvents();
    QCOMPARE(number, 2);
}




QTEST_MAIN(ThreadTesting)

#include "thread_testing.moc"