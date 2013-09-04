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

#include "dbmigrationmanager.h"
#include "dbmigrationmanager_p.h"
#include <Wt/Dbo/Session>
#include <Wt/Dbo/Transaction>
#include <boost/thread.hpp>
using namespace WtCommons;
using namespace WtCommonsPrivate;
using namespace std;

namespace dbo = Wt::Dbo;

DbMigrationManagerPrivate::DbMigrationManagerPrivate( DbMigrationManager *q, dbo::Session &session, dbo::SqlConnection *connection, const MigrationsList &migrations, const std::string &tablename )
  : q( q ), session( session ), connection(connection), migrations( migrations ), tablename( tablename )
{
}
DbMigrationManagerPrivate::~DbMigrationManagerPrivate()
{
}

void DbMigrationManagerPrivate::apply()
{
  try
  {
    session.createTables();
    dbo::Transaction t( session );

    session.add( new DboMigration {migrations.size()} );
  }
  catch
    ( ... )
  {
  }

  int migrationId = -1;
  {
    dbo::Transaction t( session );
    dbo::ptr<DboMigration> currentMigration;

    try
    {
      currentMigration = session.find<DboMigration>().orderBy( "migration_id DESC" ).limit( 1 ).resultValue();
    }
    catch
      ( ... ) {}
    if(currentMigration)
      migrationId = currentMigration->migrationId;
  }
  for(int nextMigrationId = migrationId +1; nextMigrationId < migrations.size(); nextMigrationId++ )
  {
    dbo::Transaction t(session);
    cerr  << ": Applying migration id: " << nextMigrationId << endl;
    for( auto migration : migrations[nextMigrationId] )
    {
      migration( t , connection);
    }

    session.add( new DboMigration {nextMigrationId} );
  }
}

DbMigrationManager::DbMigrationManager( dbo::Session &session, dbo::SqlConnection *connection, const MigrationsList &migrations, const std::string &tablename )
  : d {new DbMigrationManagerPrivate( this, session, connection, migrations, tablename )}
{
  session.mapClass<DboMigration>( tablename.c_str() );
}

DbMigrationManager::~DbMigrationManager()
{
  d->apply();
}
