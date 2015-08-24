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

#include "whtmltxt.h"
#include <Wt/Utils>

using namespace WtCommons;
using namespace Wt;

WHTMLTxt::WHTMLTxt(WContainerWidget* parent) : WText(parent)
{
}

WHTMLTxt::WHTMLTxt(const WString& text, WFlags< Utils::HtmlEncodingFlag > flag, WContainerWidget* parent)
  : WText(parent)
{
  encodeText(text, flag);
}

WHTMLTxt::WHTMLTxt(const std::string& text, WFlags< Utils::HtmlEncodingFlag > flag, WContainerWidget* parent): WText(parent)
{
  encodeText(text, flag);
}


WHTMLTxt* WHTMLTxt::encodeText(const std::string& textToEncode, WFlags<Utils::HtmlEncodingFlag> flag)
{
  return encodeText(WString::fromUTF8(textToEncode), flag);
}

WHTMLTxt* WHTMLTxt::encodeText(const WString& textToEncode, WFlags< Utils::HtmlEncodingFlag > flag)
{
  setText(Utils::htmlEncode(textToEncode, flag));
  return this;
}
