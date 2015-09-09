#ifndef QT_COMMONS_FUNCTIONAL_H
#define QT_COMMONS_FUNCTIONAL_H
#include <QTimer>
#define F_PTR(class, name, ...) static_cast<void (class::*)(__VA_ARGS__)>(&class::name)
#include <QThread>
#include <functional>
#include <memory>

inline void timedLambda(int msec, std::function<void()> f, QObject *context) {
  QTimer *timer = new QTimer(context);
  timer->setSingleShot(true);
  QObject::connect(timer, &QTimer::timeout, context, [=] {f(); delete timer; });
  timer->start(msec);
}

class LambdaThread : public QThread {
Q_OBJECT
public:
  LambdaThread( const std::function<void()> &runnable, QObject* parent = 0) : runnable{runnable} {}
protected:
  virtual void run() { runnable(); }
private:
  std::function<void()> runnable;
};

template<typename T>
class LambdaFuture {
public:
   static void Run(const std::function<T()> &runnable, std::function<void(const T &t)> onFinish, QObject *context) {
    auto result = std::make_shared<T>();
    auto thread = new LambdaThread([=]{ *result = runnable(); });
    QObject::connect(thread, &QThread::finished, context, [=] { onFinish(*result); delete thread; }, Qt::QueuedConnection);
    thread->start();
  }
};

template<>
class LambdaFuture<void> {
public:
   static void Run(const std::function<void()> &runnable, std::function<void()> onFinish, QObject *context) {
    auto thread = new LambdaThread([=]{ runnable(); });
    QObject::connect(thread, &QThread::finished, context, [=] { onFinish(); delete thread; }, Qt::QueuedConnection);
    thread->start();
  }
};

#endif
