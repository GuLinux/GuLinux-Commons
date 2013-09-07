/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2013  Marco Gulino <email>
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

#ifndef DBMIGRATIONMANAGERPRIVATE_H
#define DBMIGRATIONMANAGERPRIVATE_H

#include "migratedbo.h"
#include <stdint.h>
namespace WtCommonsPrivate
{
  class MigrateDboPrivate
  {
    public:
      MigrateDboPrivate( WtCommons::MigrateDbo *q, Wt::Dbo::Session &session, Wt::Dbo::SqlConnection *connection, const WtCommons::Migrations &migrations, const std::string &tablename );
      virtual ~MigrateDboPrivate();
      Wt::Dbo::Session &session;
      Wt::Dbo::SqlConnection *connection;
      WtCommons::Migrations migrations;
      const std::string tablename;
      void apply();
    private:
      WtCommons::MigrateDbo *const q;
  };
}
namespace Wt
{
  namespace Dbo
  {
    template<>
    struct dbo_traits<WtCommons::DboMigration> : public dbo_default_traits
    {
      static const char *surrogateIdField()
      {
        return 0;
      }
      static const char *versionField()
      {
        return 0;
      }
    };

  }
}
#endif // DBMIGRATIONMANAGERPRIVATE_H
