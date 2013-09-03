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

  dbo::Transaction t( session );
  dbo::ptr<DboMigration> currentMigration;

  try
  {
    currentMigration = session.find<DboMigration>().orderBy( "migration_id DESC" ).limit( 1 ).resultValue();
  }
  catch
    ( ... ) {}

  for( int migrationId = currentMigration ? currentMigration->migrationId : -1; migrationId < migrations.size()-1; migrationId++ )
  {
    for( auto migration : migrations[migrationId] )
    {
      cerr << "Applying migration id: " << migrationId << endl;
      migration( t , connection);
    }

    session.add( new DboMigration {migrationId} );
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
