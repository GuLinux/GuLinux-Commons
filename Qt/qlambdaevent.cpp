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

#include "qlambdaevent.h"

using namespace std;

class QLambdaEvent::Private {
public:
  Private(const function<void()>& process_event, const function< void()> &after_processed, QLambdaEvent *q);
  function<void()> process_event;
  function< void()> after_processed;
private:
  QLambdaEvent *q;
};

QEvent::Type QLambdaEvent::type = static_cast<QEvent::Type>(QEvent::User + 1);

QLambdaEvent::Private::Private(const function<void()>& process_event, const function< void()> &after_processed, QLambdaEvent* q) 
  : process_event{process_event}, after_processed{after_processed}, q{q}
{

}


QLambdaEvent::~QLambdaEvent()
{
  if(d->after_processed)
    d->after_processed();
}

QLambdaEvent::QLambdaEvent(const function<void()>& process_event, const function< void()> &after_processed): QEvent(type), dptr(process_event, after_processed, this)
{
}


void QLambdaEvent::run()
{
  if(d->process_event)
    d->process_event();
  accept();
}
