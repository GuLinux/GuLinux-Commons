/*
 * Copyright (c) year Marco Gulino <marco.gulino@xpeppers.com>
 *
 * This file is part of Pandorica: https://github.com/GuLinux/Pandorica
 *
 * Pandorica is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details (included the COPYING file).
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef QUITRESOURCE_H
#define QUITRESOURCE_H

#include <Wt/WResource>
#include "c++/dptr.h"

namespace WtCommons {
typedef std::function<bool()> ShutdownCondition;

class QuitResource : public Wt::WResource
{
public:
    ~QuitResource();
    virtual void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);
    QuitResource(std::string password = std::string{}, ShutdownCondition condition = []{ return true; }, Wt::WObject* parent = 0);
    QuitResource *setRestart(int argc,char **argv, char **envp);
private:
    DPTR
};
}

#endif // QUITRESOURCE_H
