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

#ifndef COMPOSITERESOURCE_H
#define COMPOSITERESOURCE_H

#include <Wt/WResource>

namespace WtCommonsPrivate {
  class CompositeResourcePrivate;
}

class CompositeResource : public Wt::WResource
{
public:
    ~CompositeResource();
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
    CompositeResource(Wt::WObject* parent = 0);
    CompositeResource *add(std::string pathInfo, Wt::WResource *resource);
private:
  WtCommonsPrivate::CompositeResourcePrivate* const d;
};

#endif // COMPOSITERESOURCE_H
