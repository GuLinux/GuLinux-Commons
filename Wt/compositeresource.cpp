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




#include "compositeresource.h"
#include "compositeresource_p.h"

#include <Wt/Http/Request>
#include <Wt/Http/Response>
#include <Wt/WServer>

using namespace Wt;
using namespace std;
using namespace WtCommons;

CompositeResource::Private::Private(CompositeResource* q) : q(q)
{
}
CompositeResource::Private::~Private()
{
}

CompositeResource::~CompositeResource()
{
}

CompositeResource* CompositeResource::add(string pathInfo, WResource* resource)
{
  d->resources[pathInfo] = resource;
  return this;
}


void CompositeResource::handleRequest(const Http::Request& request, Http::Response& response)
{
  auto subResource = d->resources[request.pathInfo()];
  if(!subResource) {
    response.setStatus(404);
    response.out() << "Not Found.";
    return;
  }
  subResource->handleRequest(request, response);
}

CompositeResource::CompositeResource(WObject* parent)
   : WResource(parent), dptr(this)
{
}
