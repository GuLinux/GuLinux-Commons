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

#include "migratedbo.h"
#include "migratedbo_p.h"
#include <Wt/Dbo/Session>
#include <Wt/Dbo/Transaction>
#include <boost/thread.hpp>
#include <boost/format.hpp>
#ifdef HAVE_MYSQL
#include <Wt/Dbo/backend/MySQL>
#endif
#ifdef HAVE_POSTGRES
#include <Wt/Dbo/backend/Postgres>
#endif
#include <typeindex>
using namespace WtCommons;
using namespace WtCommonsPrivate;
using namespace std;
using namespace Wt;
namespace dbo = Wt::Dbo;

DboMigration::DboMigration( Migration migration, const string &migrationName, const string &whenCreated )
  : _migration( migration ), _migrationName( migrationName ), _whenCreated( WDateTime::fromString( whenCreated, "yyyy-MM-dd HH:mm:ss" ).toPosixTime() )
{
  cerr << "Created migration: " << migrationName << endl;
}


MigrateDboPrivate::MigrateDboPrivate( MigrateDbo *q, dbo::Session &session, dbo::SqlConnection *connection, const Migrations &migrations, const string &tablename )
  : q( q ), session( session ), connection( connection ), migrations( migrations ), tablename( tablename )
{
}
MigrateDboPrivate::~MigrateDboPrivate()
{
}

namespace WtCommonsPrivate
{
  struct TemporarlyDisableForeignKeysSupport
  {
    TemporarlyDisableForeignKeysSupport( Dbo::SqlConnection *connection );
    ~TemporarlyDisableForeignKeysSupport();
    Dbo::SqlConnection *connection;
    int wasEnabledInFirstPlace;
  };
}

TemporarlyDisableForeignKeysSupport::TemporarlyDisableForeignKeysSupport( Dbo::SqlConnection *connection ) : connection(connection)
{
  unique_ptr<Dbo::SqlStatement> getForeignKeysEnabledStatement { connection->prepareStatement( "PRAGMA foreign_keys" ) };
  getForeignKeysEnabledStatement->execute();

  if( getForeignKeysEnabledStatement->nextRow() )
    getForeignKeysEnabledStatement->getResult( 0, &wasEnabledInFirstPlace );

  if( wasEnabledInFirstPlace )
  {
    cerr << "Disabling Foreign Keys Support" << endl;
    unique_ptr<Dbo::SqlStatement> disableForeignKeysStatement { connection->prepareStatement( "PRAGMA foreign_keys = OFF" ) };
    disableForeignKeysStatement->execute();
  }
}

TemporarlyDisableForeignKeysSupport::~TemporarlyDisableForeignKeysSupport()
{
  if( wasEnabledInFirstPlace )
  {
    cerr << "Enabling back Foreign Keys Support" << endl;
    unique_ptr<Dbo::SqlStatement> enableForeignKeysStatement { connection->prepareStatement( "PRAGMA foreign_keys = ON" ) };
    enableForeignKeysStatement->execute();
  }
}


void MigrateDboPrivate::apply()
{
  cerr << "Applying migrations" << endl;

  try
  {
    session.createTables();
    dbo::Transaction t( session );
    session.add( new DboMigration(*migrations.front().get()) );
    t.commit();
  }
  catch
    ( ... )
  {
    cerr << "Tables already found" << endl;
  }

  int migrationIndex = -1;
  {
    dbo::Transaction t( session );
    dbo::ptr<DboMigration> currentMigration;

    try
    {
      currentMigration = session.find<DboMigration>().orderBy( "id ASC" ).limit( 1 ).resultValue();
    }
    catch
      ( ... ) {}

    if( currentMigration ) {
      migrationIndex = currentMigration->migrationIndex();
      cerr << "Last migration found: " << migrationIndex << ", '" << currentMigration->name() << "', created: " << currentMigration->whenCreated() << ", applied: " << currentMigration->whenApplied() << endl;
    }
  }

  for( int nextMigrationId = migrationIndex + 1; nextMigrationId < migrations.size(); nextMigrationId++ )
  {
    dbo::Transaction t( session );
    migrations[nextMigrationId]->apply( t, connection, nextMigrationId );
  }
}

void DboMigration::apply( Dbo::Transaction &transaction, Dbo::SqlConnection *connection, int64_t migrationIndex )
{
  _transaction = &transaction;
  _connection = connection;
  _migrationIndex = migrationIndex;
  _whenApplied = WDateTime::currentDateTime().toPosixTime() ;
  cerr << "Applying migration " << migrationIndex << ": " << _migrationName << endl;
  _migration( transaction, *this );
  transaction.session().add( new DboMigration(*this) );
}

DboMigration::~DboMigration()
{
}


void DboMigration::execute( const string &statement, const vector< string > &args, DboMigration::DboType dboType )
{
  if( ! _transaction )
  {
    throw runtime_error( "Transaction not initialized; exiting" );
  }

  static map<type_index, DboType> dboTypes
  {
#ifdef HAVE_SQLITE3
    { typeid( Wt::Dbo::backend::Sqlite3 ), Sqlite3 },
#endif
#ifdef HAVE_POSTGRES
    { typeid( Wt::Dbo::backend::Postgres ), PostgreSQL },
#endif
#ifdef HAVE_MYSQL
    { typeid( Wt::Dbo::backend::MySQL ), MySQL },
#endif
  };

  if( !( dboType & dboTypes[typeid( *_connection )] ) )
    return;

  boost::format query( statement );

  for( auto arg : args )
    query % arg;

  cerr << "executing query: " << query.str() << endl;
  _transaction->session().execute( query.str() );
}


MigrateDbo::MigrateDbo( dbo::Session &session, dbo::SqlConnection *connection, const Migrations &migrations, const string &tablename )
  : d {new MigrateDboPrivate( this, session, connection, migrations, tablename )}
{
  session.mapClass<DboMigration>( tablename.c_str() );
}

MigrateDbo::~MigrateDbo()
{
  d->apply();
}

void DboMigration::renameTable( const string &tableName, const string &newTableName )
{
  static string renameTableStatement = "ALTER TABLE \"%s\" RENAME TO \"%s\"";
  execute( renameTableStatement, {tableName, newTableName} );
}

DboMigration::CreateTable DboMigration::createTable( const string &tableName )
{
  return CreateTable {this, tableName};
}

DboMigration::CreateTable::CreateTable( DboMigration *dboMigration, const string &tableName )
  : dboMigration( dboMigration ), tableName( tableName )
{
}


DboMigration::CreateTable &DboMigration::CreateTable::primaryKey( const string &primaryKeyName )
{
  primaryKeys.push_back( primaryKeyName );
}

DboMigration::CreateTable::~CreateTable()
{
  if( _copied || _run )
    return;

  _run = true;
  static string createTableTemplate = "CREATE TABLE \"%s\" (%s);";
  vector<string> columnsValues;
  transform( columns.begin(), columns.end(), back_insert_iterator<vector<string>>( columnsValues ), []( Column p )
  {
    return ( boost::format( "\"%s\" %s %s" ) % p.name % p.type % p.defaultValue ).str();
  } );

  if( primaryKeys.size() > 0 )
  {
    transform( primaryKeys.begin(), primaryKeys.end(), primaryKeys.begin(), []( const string & s )
    {
      return "\"" + s + "\"";
    } );
    columnsValues.push_back( ( boost::format( "PRIMARY KEY (%s)" ) % boost::join( primaryKeys, ", " ) ).str() );
  }

  dboMigration->execute( createTableTemplate, {tableName, boost::join( columnsValues, ", " )} );
}


DboMigration::CreateTable::CreateTable( const DboMigration::CreateTable &other )
  : dboMigration( other.dboMigration ), tableName( other.tableName ), columns( other.columns ), primaryKeys( other.primaryKeys ), _run( other._run )
{
  const_cast<CreateTable &>( other )._copied = true;
}

void DboMigration::dropTable( const string &tableName )
{
  static string dropStatement = "DROP TABLE \"%s\"";
  execute( dropStatement, {tableName} );
}

void DboMigration::removeColumn( const string &tableName, const string &columnName )
{
  static string removeColumnStatement = "ALTER TABLE \"%s\" DROP COLUMN \"%s\"";
#ifdef HAVE_SQLITE3

  if( typeid( *_connection ) == typeid( Wt::Dbo::backend::Sqlite3 ) )
  {
    cerr << "Warning: Sqlite3 doesn't support DROP COLUMN statement; using workaround" << endl;
    static string getTableInfoSql = "pragma table_info(\"%s\");";
    TemporarlyDisableForeignKeysSupport disableForeignKeys( _connection );

    unique_ptr<Wt::Dbo::SqlStatement> getTableInfoStatement {_connection->prepareStatement( ( boost::format( getTableInfoSql ) % tableName ).str() ) };
    getTableInfoStatement->execute();
    vector<string> columnsToCopy;
    string tempTable = tableName + "_temp";
    {
      CreateTable table = createTable( tableName );

      while( getTableInfoStatement->nextRow() )
      {
        string colName, colType, defaultValue;
        short int isNotNull, isPrimaryKey;
        getTableInfoStatement->getResult( 1, &colName, 1024 );

        if( colName == columnName )
          continue;

        getTableInfoStatement->getResult( 2, &colType, 1024 );
        getTableInfoStatement->getResult( 3, &isNotNull );
        getTableInfoStatement->getResult( 4, &defaultValue, 1024 );
        getTableInfoStatement->getResult( 5, &isPrimaryKey );
        table.columns.push_back( CreateTable::Column { colName, colType + ( isNotNull ? " not null" : "" ) , defaultValue.empty() ? defaultValue : " default " + defaultValue } );
        columnsToCopy.push_back( "\"" + colName + "\"" );

        if( isPrimaryKey )
          table.primaryKey( colName );
      }

      renameTable( tableName, tempTable );
    }
    static string copyDataSql = "insert into \"%s\" select %s from \"%s\";";
    execute( copyDataSql, {tableName, boost::join( columnsToCopy, ", " ), tempTable} );
    dropTable( tempTable );
    return;
  }

#endif

  execute( removeColumnStatement, {tableName, columnName} );
}



void DboMigration::renameColumn( const string &tableName, const string &columnName, const string &newColumnName )
{
  static string renameColumnPgSqlStatement = "ALTER TABLE \"%s\" RENAME COLUMN \"%s\" TO \"%s\"";
#ifdef HAVE_SQLITE3

  if( typeid( *_connection ) == typeid( Wt::Dbo::backend::Sqlite3 ) )
  {
    cerr << "Warning: Sqlite3 doesn't support RENAME COLUMN statement; using workaround." << endl;
    TemporarlyDisableForeignKeysSupport disableForeignKeys( _connection );

    static string getTableCreationSql = "SELECT sql FROM sqlite_master WHERE type='table' AND name = \"%s\";";
    static string getTableInfoSql = "pragma table_info(\"%s\");";
    vector<string> tableColumns;
    unique_ptr<Wt::Dbo::SqlStatement> getTableInfoStatement { _connection->prepareStatement( ( boost::format( getTableInfoSql ) % tableName ).str() ) };
    getTableInfoStatement->execute();

    while( getTableInfoStatement->nextRow() )
    {
      string oldColumnName;
      getTableInfoStatement->getResult( 1, &oldColumnName, 1024 );
      tableColumns.push_back( oldColumnName == columnName ? "\"" + oldColumnName + "\" as \"" + newColumnName + "\"" : "\"" + oldColumnName + "\"" );
    }

    string creationScript = _transaction->session().query<string>( ( boost::format( getTableCreationSql ) % tableName ).str() );
    boost::replace_all( creationScript, columnName, newColumnName );
    string tempTable = tableName + "_temp";
    renameTable( tableName, tempTable );
    execute( creationScript, {} );
    static string copyDataSql = "insert into \"%s\" select %s from \"%s\";";
    execute( copyDataSql, {tableName, boost::join( tableColumns, ", " ), tempTable} );
    dropTable( tempTable );
    return;
  }

#endif

  execute( renameColumnPgSqlStatement, {tableName, columnName, newColumnName} );
}

