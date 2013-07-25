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

#ifndef QUITRESOURCEPRIVATE_H
#define QUITRESOURCEPRIVATE_H

#include "quitresource.h"

namespace WtCommons { class QuitResource;
namespace WtCommonsPrivate {
class QuitResourcePrivate
{
public:
  QuitResourcePrivate(std::string password, ShutdownCondition condition, WtCommons::QuitResource* q);
  virtual ~QuitResourcePrivate();
  std::string password;
  ShutdownCondition condition;
  WtCommons::QuitResource* const q;
  int restart_argc{-1};
  char** restart_argv;
  char** restart_envp;
};
}
}
#endif // QUITRESOURCEPRIVATE_H
