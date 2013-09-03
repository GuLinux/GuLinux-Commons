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

#include "dbmigrationmanager.h"
#include <stdint.h>
#include <Wt/WDateTime>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/Types>
#include <Wt/Dbo/ptr>
namespace WtCommonsPrivate
{
  class DboMigration
  {
    public:
      DboMigration() = default;
      DboMigration( int64_t migrationId, const Wt::WDateTime &when = Wt::WDateTime::currentDateTime() )
        : migrationId( migrationId ), _when( when.toPosixTime() ) {}
      int64_t migrationId;
      boost::posix_time::ptime _when;
      Wt::WDateTime when() const
      {
        return Wt::WDateTime::fromPosixTime( _when );
      }
      template<class Action>
      void persist( Action &a )
      {
        Wt::Dbo::id( a, migrationId, "migration_id" );
        Wt::Dbo::field( a, _when, "when" );
      }
  };


  class DbMigrationManagerPrivate
  {
    public:
      DbMigrationManagerPrivate( WtCommons::DbMigrationManager *q, Wt::Dbo::Session &session, Wt::Dbo::SqlConnection *connection, const WtCommons::MigrationsList &migrations, const std::string &tablename );
      virtual ~DbMigrationManagerPrivate();
      Wt::Dbo::Session &session;
      Wt::Dbo::SqlConnection *connection;
      WtCommons::MigrationsList migrations;
      const std::string tablename;
      void apply();
    private:
      WtCommons::DbMigrationManager *const q;
  };
}
namespace Wt
{
  namespace Dbo
  {
    template<>
    struct dbo_traits<WtCommonsPrivate::DboMigration> : public dbo_default_traits
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
