/*
 * Copyright 2013 Marco Gulino <email>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "compositeresource.h"
#include "compositeresource_p.h"

#include <Wt/Http/Request>
#include <Wt/Http/Response>
#include <Wt/WServer>

using namespace Wt;
using namespace std;
using namespace WtCommonsPrivate;

CompositeResourcePrivate::CompositeResourcePrivate(CompositeResource* q) : q(q)
{
}
CompositeResourcePrivate::~CompositeResourcePrivate()
{
}

CompositeResource::~CompositeResource()
{
    delete d;
}

CompositeResource* CompositeResource::add(string pathInfo, WResource* resource)
{
  d->resources[pathInfo] = resource;
  return this;
}


void CompositeResource::handleRequest(const Http::Request& request, Http::Response& response)
{
  log("notice") << "Got pathInfo: " << request.pathInfo();
  auto subResource = d->resources[request.pathInfo()];
  if(!subResource) {
    response.setStatus(404);
    response.out() << "Not Found.";
    return;
  }
  log("notice") << "Resource " << request.pathInfo() << " found, handling output";
  subResource->handleRequest(request, response);
}

CompositeResource::CompositeResource(WObject* parent)
   : WResource(parent), d(new CompositeResourcePrivate(this))
{
}
