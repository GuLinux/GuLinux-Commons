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

#ifndef QT_COMMONS_FUNCTIONAL_H
#define QT_COMMONS_FUNCTIONAL_H
#include <QTimer>
#define F_PTR(class, name, ...) static_cast<void (class::*)(__VA_ARGS__)>(&class::name)
#include <QThread>
#include <functional>
#include <memory>
namespace GuLinux {
inline void timedLambda(int msec, std::function<void()> f, QObject *context) {
  QTimer *timer = new QTimer(context);
  timer->setSingleShot(true);
  QObject::connect(timer, &QTimer::timeout, context, [=] {f(); delete timer; });
  timer->start(msec);
}

class Thread : public QThread {
Q_OBJECT
public:
 template<typename T> class Run;
protected:
  virtual void run() { runnable(); }
private:
  Thread( const std::function<void()> &runnable, QObject* parent = 0) : runnable{runnable} {}
  std::function<void()> runnable;
};

template<typename T>
class Thread::Run {
public:
    Run(const std::function<T()> &runnable, std::function<void(const T &t)> after_thread = [](const T &){}, std::function<void(Thread*)> before_thread = [](Thread*){}) {
    QObject *context = new QObject;
    auto result = std::make_shared<T>();
    auto thread = new Thread([=]{ *result = runnable(); });
    before_thread(thread);
    QObject::connect(thread, &QThread::finished, context, [=] { after_thread(*result); delete thread; delete context; }, Qt::QueuedConnection);
    thread->start();
  }
};

template<>
class Thread::Run<void> {
public:
    Run(const std::function<void()> &runnable, std::function<void()> after_thread = [](){}, std::function<void(Thread*)> before_thread = [](Thread*){}) {
    QObject *context = new QObject;
    auto thread = new Thread([=]{ runnable(); });
    before_thread(thread);
    QObject::connect(thread, &QThread::finished, context, [=] { after_thread(); delete thread; delete context; }, Qt::QueuedConnection);
    thread->start();
  }
};
}
#endif
