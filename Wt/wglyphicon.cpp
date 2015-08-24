/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2014  Marco Gulino <email>
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

#include "wglyphicon.h"
#include <boost/format.hpp>
using namespace std;
using namespace Wt;
using namespace WtCommons;

WGlyphicon::~WGlyphicon()
{

}

WGlyphicon::WGlyphicon(const std::string& iconName, Wt::WContainerWidget* parent)
  : WContainerWidget(parent)
{
  setInline(true);
  addStyleClass("glyphicon");
  addStyleClass(iconName);
}

WGlyphicon *WGlyphicon::color(const string& color)
{
  setAttributeValue("style", (boost::format("color: %s") % color).str() );
  return this;
}
