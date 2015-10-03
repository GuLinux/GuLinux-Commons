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

using namespace std;
using namespace Wt;
using namespace WtCommons;

QuitResource::Private::Private(std::string password, ShutdownCondition condition, QuitResource* q)
  : password(password), condition(condition), q(q)
{
}

QuitResource::Private::~Private()
{
}

QuitResource::~QuitResource()
{
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

  auto action = request.getParameter("action");
  if(action && *action == "status") {
      static map<Private::Status, string> statuses {
        {Private::Idle, "Idle"},
        {Private::WaitShutdown, "Waiting for Shutdown"},
        {Private::ShutDown, "Shutting down"},
      };
      response.setStatus(200);
      response.out() << "Status: " << statuses[d->status];
      return;
  }

  d->status = Private::WaitShutdown;
  boost::thread t([=] {
    do
      boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
    while(!d->condition());
    d->status = Private::ShutDown;
    WServer::instance()->log("notice") << "Server stopping as requested.";
    WServer::instance()->stop();
    while(WServer::instance()->isRunning())
      boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
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
    : WResource(parent), dptr(password, condition, this)
{
}
