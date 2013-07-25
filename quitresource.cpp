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

#include "quitresource_p.h"
#include <Wt/Http/Response>
#include <Wt/WServer>
#include <thread>
#include <boost/thread.hpp>
#include <../../opt/wt/include/Wt/WIOService>

using namespace std;
using namespace Wt;
using namespace WtCommons;
using namespace WtCommonsPrivate;

QuitResourcePrivate::QuitResourcePrivate(std::string password, ShutdownCondition condition, QuitResource* q)
  : password(password), condition(condition), q(q)
{
}

QuitResourcePrivate::~QuitResourcePrivate()
{
}

QuitResource::~QuitResource()
{
    delete d;
}

void QuitResource::handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response)
{
  if(d->password.empty()) {
    WServer::instance()->log("warning") << "Quit resource called but no password configured, returning 404" << d->password;
    response.setStatus(404);
    response.out() << "404 Not Found";
    return;
  }
  auto password = request.getParameter("pwd");
  if(!password || d->password != *password) {
    response.setStatus(403);
    response.out() << "403 Forbidden";
    return;
  }
  boost::thread t([=] {
    do
      this_thread::sleep_for(chrono::milliseconds(500));
    while(!d->condition());
    WServer::instance()->log("notice") << "Server stopping as requested.";
    WServer::instance()->stop();
    while(WServer::instance()->isRunning())
      this_thread::sleep_for(chrono::milliseconds(500));
    if(d->restart_argc > -1)
      WServer::restart(d->restart_argc, d->restart_argv, d->restart_envp);
    exit(0);
  });
  response.setStatus(200);
  response.out() << "Server stop scheduled";
}

QuitResource* QuitResource::setRestart(int argc, char** argv, char** envp)
{
  d->restart_argc = argc;
  d->restart_argv = argv;
  d->restart_envp = envp;
  return this;
}


QuitResource::QuitResource(std::string password, ShutdownCondition condition, WObject* parent)
    : d(new QuitResourcePrivate(password, condition, this))
{
}
