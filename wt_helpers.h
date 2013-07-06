/***********************************************************************
Copyright (c) 2013 "Marco Gulino <marco.gulino@gmail.com>"

This file is part of Pandorica: https://github.com/GuLinux/Pandorica

Pandorica is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details (included the COPYING file).

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/


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
#include <Wt/WConfig.h>
#include <Wt/WPanel>
#include <Wt/WContainerWidget>

#define wLog (wApp ? wApp->log("notice") : Wt::WServer::instance()->log("notice"))
#define JS(...) #__VA_ARGS__
#define HTML(...) #__VA_ARGS__

namespace WtCommons {

inline void setHeaderCollapsible(Wt::WPanel *panel) {
  if(std::string{"3.3.0"} != WT_VERSION_STR) return;
  panel->titleBarWidget()->clicked().connect([=](Wt::WMouseEvent) { panel->setCollapsed(!panel->isCollapsed()); });
}

  
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
  
  
  template<typename C>
  WW &onClick(C listener) {
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
  WW &setIcon(const Wt::WLink &link) {
    widget->setIcon(link);
    return *this;
  }
  WW &setImage(Wt::WImage *image) {
    widget->setImage(image);
    return *this;
  }
  WW &resize(const Wt::WLength &w, const Wt::WLength &h) {
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
}
#endif