#ifndef PROJ_WT_HELPERS
#define PROJ_WT_HELPERS

#define _n1 Wt::NoClass
#define _n2 Wt::NoClass, _n1
#define _n3 Wt::NoClass, _n2
#define _n4 Wt::NoClass, _n3
#define _n5 Wt::NoClass, _n4
#define _n6 Wt::NoClass, _n5

#define wtr Wt::WString::tr

namespace Wt {
  class WLayout;
}

#include <Wt/WWidget>
#include <Wt/WEvent>
#include <functional>
#include <Wt/WServer>

#define wLog (wApp ? wApp->log("notice") : Wt::WServer::instance()->log("notice"))
#define JS(...) #__VA_ARGS__
#define HTML(...) #__VA_ARGS__

typedef std::function<void(Wt::WMouseEvent&)> MouseEventListener;

template<class WidgetType>
class WW {
public:
  WW(WidgetType *widget) :widget(widget) {}
  template<typename... Args>
  WW(Args... args) : WW{new WidgetType{args...} } {}

  WW &css(std::string css) {
    widget->setStyleClass(css);
    return *this;
  }
  
  WW &addCss(std::string css) {
    widget->addStyleClass(css);
    return *this;
  }
  
  WidgetType *get() {
    return widget;
  }
  
  WW &add(Wt::WWidget *newWidget) {
    widget->addWidget(newWidget);
    return *this;
  }
  
  WW &padding(const Wt::WLength& padding, Wt::WFlags<Wt::Side > sides = Wt::All) {
    widget->setPadding(padding, sides);
    return *this;
  }
  
template<class OtherType>  
  WW &add(WW<OtherType> &newWidget) {
    widget->addWidget(newWidget.get());
    return *this;
  }
  
  WW &onClick(MouseEventListener listener) {
    widget->clicked().connect(listener);
    return *this;
  }
  WW &setContentAlignment(Wt::WFlags<Wt::AlignmentFlag> contentAlignment) {
    widget->setContentAlignment(contentAlignment);
    return *this;
  }
  WW &setVerticalAlignment(Wt::AlignmentFlag alignment, const Wt::WLength & length = Wt::WLength()) {
    widget->setVerticalAlignment(alignment, length);
    return *this;
  }
  WW &setInline(bool isInline) {
    widget->setInline(isInline);
    return *this;
  }
  WW &resize(Wt::WLength &w, Wt::WLength &h) {
    widget->resize(w,h);
    return *this;
  }
  WW &setLayout(Wt::WLayout *layout) {
    widget->setLayout(layout);
    return *this;
  }
  WW &setAttribute(const std::string &attrName, const std::string &attrValue) {
    widget->setAttributeValue(attrName, attrValue);
    return *this;
  }
  WW &setMargin(const Wt::WLength& margin, Wt::WFlags<Wt::Side> sides = Wt::All) {
    widget->setMargin(margin, sides);
    return *this;
  }
  
  template<class T>
  WW &setText(T text) {
    widget->setText(text);
    return *this;
  }
  
  WW &setTextFormat(Wt::TextFormat format) {
    widget->setTextFormat(format);
    return *this;
  }
  
  operator WidgetType*() {
    return widget;
  }
   
  WW &setEnabled(bool enable) {
    widget->setEnabled(enable);
    return *this;
  }
   
  WW &enable() {
    widget->enable();
    return *this;
  }
   
  WW &disable() {
    widget->disable();
    return *this;
  }
private:
  WidgetType *widget;
};

//#define WW(WidgetType, ...) _WW<WidgetType>(new WidgetType(__VA_ARGS__) )
#endif