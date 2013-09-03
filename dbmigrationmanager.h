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

#ifndef DBMIGRATIONMANAGER_H
#define DBMIGRATIONMANAGER_H
#include <map>
#include <functional>
#include <vector>
#include <list>
#include <memory>
#define QUERY(...) #__VA_ARGS__

namespace WtCommonsPrivate
{
    class DbMigrationManagerPrivate;
}
namespace Wt
{
  namespace Dbo
  {
    class Session;
    class Transaction;
    class SqlConnection;
  }
}
namespace WtCommons
{
    typedef std::function<void ( Wt::Dbo::Transaction &, Wt::Dbo::SqlConnection* ) > Migration;
    typedef std::list<Migration> Migrations;
    typedef std::vector<Migrations> MigrationsList;

    class DbMigrationManager
    {
      public:
        DbMigrationManager( Wt::Dbo::Session &session, Wt::Dbo::SqlConnection *connection, const MigrationsList &migrations, const std::string &tablename = "wt_migrations" );
        ~DbMigrationManager();

      private:
        const std::unique_ptr<::WtCommonsPrivate::DbMigrationManagerPrivate> d;
    };
}
#endif // DBMIGRATIONMANAGER_H

class Session;

class Session;
