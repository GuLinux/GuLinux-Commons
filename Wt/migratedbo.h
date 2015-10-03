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
    std::string prettyFunction = __PRETTY_FUNCTION__;\
    boost::replace_all(prettyFunction, "WtCommons::DboMigration* ", "");\
    return new WtCommons::DboMigration( executeMigration, prettyFunction, creationTime); \
  }
#else
#define CREATE_MIGRATION(a, b)
#endif
#ifdef HAVE_SQLITE3
  #include <Wt/Dbo/backend/Sqlite3>
#endif
#include "c++/dptr.h"

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
  class MigrateDbo;
  class DboMigration;
  typedef std::function<void ( Wt::Dbo::Transaction &, WtCommons::DboMigration & ) > Migration;

  class DboMigration
  {
    public:
      enum DboType { Sqlite3 = 0x1, PostgreSQL = 0x2, MySQL=0x4, ALL=0x7 };
      template<typename T>
      struct DefaultValue
      {
        T defaultValue;
        DefaultValue<T>() = default;
        DefaultValue<T>( const T &defaultValue ) : defaultValue( defaultValue ) {}
        bool shouldUseDefaultValue = true;
        std::string sql() const
        {
          if( !shouldUseDefaultValue )
            return "";

          return "default " + boost::lexical_cast<std::string>( defaultValue );
        }
        static DefaultValue no()
        {
          DefaultValue<T> d;
          d.shouldUseDefaultValue = false;
          return d;
        }
      };
      struct CreateTable
      {
          struct Column
          {
            std::string name;
            std::string type;
            std::string defaultValue;
          };
          CreateTable( const CreateTable &other );
          CreateTable( DboMigration *dboMigration, const std::string &tableName );
          DboMigration *const dboMigration ;
          std::string tableName;
          std::vector<Column> columns;
          std::vector<std::string> primaryKeys;
          template<typename T>
          CreateTable &column( std::string name, DefaultValue<T> defaultValue = DefaultValue<T>::no() )
          {
            columns.push_back( {name, dboMigration->sqlType<T>(), defaultValue.sql()} );
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
      ~DboMigration();
      template<class Action>
      void persist( Action &a )
      {
        Wt::Dbo::field( a, _migrationIndex, "migration_index" );
        Wt::Dbo::field( a, _migrationName, "migration_name" );
        Wt::Dbo::field( a, _whenApplied, "when_applied" );
        Wt::Dbo::field( a, _whenCreated, "when_created" );
      }
      int64_t migrationIndex() const
      {
        return _migrationIndex;
      }
      template<typename T>
      std::string sqlType( int size = -1 ) const
      {
        return Wt::Dbo::sql_value_traits<T>().type( _connection, -1 );
      }
      void execute( const std::string &statement, const std::vector<std::string> &args, DboType dboType = ALL );
      void apply( Wt::Dbo::Transaction &transaction, Wt::Dbo::SqlConnection *connection, int64_t migrationIndex );
      CreateTable createTable( const std::string &tableName );

      template<typename ColumnType>
      void addColumn( const std::string &tableName, const std::string &columnName, DefaultValue<ColumnType> defaultValue = DefaultValue<ColumnType>::no() )
      {
        static std::string addColumnStatement = "ALTER TABLE \"%s\" ADD COLUMN \"%s\" %s %s";
        execute( addColumnStatement, {tableName, columnName, sqlType<ColumnType>(), defaultValue.sql()} );
      }
      
      template<typename ColumnType>
      void modifyColumn( const std::string &tableName, const std::string &columnName, DefaultValue<ColumnType> defaultValue = DefaultValue<ColumnType>::no() )
      {
        static std::string modifyColumnStatementPgSql = "ALTER TABLE \"%s\" ALTER COLUMN \"%s\" %s";
#ifdef HAVE_SQLITE3
        if( typeid( *_connection ) == typeid( Wt::Dbo::backend::Sqlite3 ) )
        {
          std::cerr << "Warning: Sqlite3 doesn't support MODIFY COLUMN statement; using workaround" << std::endl;
          std::string tempColName = columnName + "__temp";
          renameColumn( tableName, columnName, tempColName );
          addColumn<ColumnType>( tableName, columnName,  defaultValue );
          static std::string copyDataSql = "UPDATE \"%s\" set \"%s\" = \"%s\";";
          execute( copyDataSql, {tableName, columnName, tempColName} );
          removeColumn( tableName, tempColName );
          return;
        }
#endif

        execute( modifyColumnStatementPgSql, {tableName, columnName, sqlType<ColumnType>()} );
      }

      void renameTable( const std::string &tableName, const std::string &newTableName );
      void renameColumn( const std::string &tableName, const std::string &columnName, const std::string &newColumnName );
      void removeColumn( const std::string &tableName, const std::string &columnName );
      void dropTable( const std::string &tableName );

      boost::posix_time::ptime whenApplied() const { return _whenApplied; }
      boost::posix_time::ptime whenCreated() const { return _whenCreated; }
      std::string name() const { return _migrationName; }
    private:
      int64_t _migrationIndex;
      boost::posix_time::ptime _whenApplied;
      boost::posix_time::ptime _whenCreated;
      Migration _migration;
      std::string _migrationName;
      Wt::Dbo::Transaction *_transaction = 0;
      Wt::Dbo::SqlConnection *_connection = 0;
      friend class ::WtCommons::MigrateDbo;
  };

  typedef std::vector<std::shared_ptr<DboMigration>> Migrations;
  class MigrateDbo
  {
    public:
      MigrateDbo( Wt::Dbo::Session &session, Wt::Dbo::SqlConnection *connection, const Migrations &migrations, const std::string &tablename = "wt_migrations" );
      ~MigrateDbo();
    private:
      D_PTR
  };
}
#endif // DBMIGRATIONMANAGER_H

class Session;

class Session;
