#ifndef QT_COMMONS_FUNCTIONAL_H
#define QT_COMMONS_FUNCTIONAL_H

#define F_PTR(class, name, ...) static_cast<void (class::*)(__VA_ARGS__)>(&class::name)

inline void timedLambda(int msec, std::function<void()> f, QObject *context) {
  QTimer *timer = new QTimer(context);
  timer->setSingleShot(true);
  QObject::connect(timer, &QTimer::timeout, context, [=] {f(); delete timer; });
  timer->start(msec);
}

#endif
