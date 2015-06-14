/*
 * Copyright (c) 2013 "Marco Gulino <marco.gulino@gmail.com>"
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

#ifndef COOKIESLAWDISCLAIMER_H
#define COOKIESLAWDISCLAIMER_H

#include <Wt/WCompositeWidget>
#include <memory>
#include <functional>
namespace WtCommons {
class CookiesLawDisclaimer : public Wt::WCompositeWidget
{
public:
  ~CookiesLawDisclaimer();
  static void checkOrCreate(Wt::WContainerWidget *container, const std::function<void()> &runOnAccepted = []{} );
private:
    CookiesLawDisclaimer(Wt::WContainerWidget* parent, const std::function<void()> &runOnAccepted);
};
}
#endif // COOKIESLAWDISCLAIMER_H
