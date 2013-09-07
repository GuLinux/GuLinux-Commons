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
#include <string>
#include <list>
#include <memory>
#include <Wt/WDateTime>
#include <Wt/WDate>
#include <Wt/WTime>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/Types>
#include <Wt/Dbo/ptr>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#ifndef IN_KDEVELOP_PARSER
#define CREATE_MIGRATION(migrationName, creationTime, applyFunction) WtCommons::DboMigration * migrationName () { \
    auto executeMigration = [](Wt::Dbo::Transaction & t, WtCommons::DboMigration &m) applyFunction  ; \
    return new WtCommons::DboMigration( executeMigration, __func__, creationTime); \
  }
#else
#define CREATE_MIGRATION(a, b)
#endif

#include <Wt/Dbo/backend/Sqlite3>

namespace WtCommonsPrivate
{
  class MigrateDboPrivate;
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
  class DboMigration;
  typedef std::function<void ( Wt::Dbo::Transaction &, WtCommons::DboMigration & ) > Migration;

  class DboMigration
  {
    public:
      struct CreateTable
      {
        struct Column {
          std::string name;
          std::string type;
          std::string defaultValue;
        };
          CreateTable( const CreateTable &other );
          CreateTable(DboMigration *dboMigration, const std::string &tableName );
          DboMigration * const dboMigration ;
          std::string tableName;
          std::vector<Column> columns;
          std::vector<std::string> primaryKeys;
          template<typename T>
          CreateTable &column( std::string name )
          {
            columns.push_back( {name, dboMigration->sqlType<T>()} );
            return *this;
          }
          template<typename T>
          CreateTable &column( std::string name, T defaultValue )
          {
            columns.push_back( {name, dboMigration->sqlType<T>(), "default " + boost::lexical_cast<std::string>(defaultValue)} );
            return *this;
          }
          CreateTable &primaryKey( const std::string &primaryKeyName );
          ~CreateTable();
        private:
          bool _copied = false;
          bool _run = false;
      };
      DboMigration() = default;
      DboMigration( Migration migration, const std::string &migrationName, const std::string &whenCreated );
      template<class Action>
      void persist( Action &a )
      {
        Wt::Dbo::id( a, _migrationId, "migration_id" );
        Wt::Dbo::field( a, _migrationName, "migration_name" );
        Wt::Dbo::field( a, _whenApplied, "when_applied" );
        Wt::Dbo::field( a, _whenCreated, "when_created" );
      }
      int64_t migrationId() const
      {
        return _migrationId;
      }
      template<typename T>
      std::string sqlType( int size = -1 ) const
      {
        return Wt::Dbo::sql_value_traits<T>().type( _connection, -1 );
      }
      void execute( const std::string &statement, const std::vector<std::string> &args );
      void apply( Wt::Dbo::Transaction &transaction, Wt::Dbo::SqlConnection *connection, int64_t migrationId );
      CreateTable createTable( const std::string &tableName );

      template<typename ColumnType>
      void addColumn( const std::string &tableName, const std::string &columnName, const ColumnType &defaultValue )
      {
        static std::string addColumnStatement = "ALTER TABLE \"%s\" ADD COLUMN \"%s\" %s default %s";
        execute( addColumnStatement, {tableName, columnName, sqlType<ColumnType>(), defaultValue} );
      }
      template<typename ColumnType>
      void addColumn( const std::string &tableName, const std::string &columnName )
      {
        static std::string addColumnStatement = "ALTER TABLE \"%s\" ADD COLUMN \"%s\" %s";
        execute( addColumnStatement, {tableName, columnName, sqlType<ColumnType>()} );
      }
      template<typename ColumnType>
      void modifyColumn( const std::string &tableName, const std::string &columnName )
      {
        static std::string modifyColumnStatementPgSql = "ALTER TABLE \"%s\" ALTER COLUMN \"%s\" %s";

        if( typeid( *_connection ) == typeid( Wt::Dbo::backend::Sqlite3 ) )
        {
        }

        execute( modifyColumnStatementPgSql, {tableName, columnName, sqlType<ColumnType>()} );
      }

      void renameTable( const std::string &tableName, const std::string &newTableName );
      void renameColumn( const std::string &tableName, const std::string &columnName, const std::string &newColumnName );
      void removeColumn( const std::string &tableName, const std::string &columnName );
      void dropTable( const std::string &tableName );
    private:
      int64_t _migrationId;
      boost::posix_time::ptime _whenApplied;
      boost::posix_time::ptime _whenCreated;
      Migration _migration;
      std::string _migrationName;
      Wt::Dbo::Transaction *_transaction = 0;
      Wt::Dbo::SqlConnection *_connection = 0;
  };

  typedef std::vector<std::shared_ptr<DboMigration>> Migrations;
  class MigrateDbo
  {
    public:
      MigrateDbo( Wt::Dbo::Session &session, Wt::Dbo::SqlConnection *connection, const Migrations &migrations, const std::string &tablename = "wt_migrations" );
      ~MigrateDbo();
    private:
      const std::unique_ptr< ::WtCommonsPrivate::MigrateDboPrivate > d;
  };
}
#endif // DBMIGRATIONMANAGER_H

class Session;

class Session;
