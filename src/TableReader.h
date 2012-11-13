// $Id: TableReader.h,v 1.4 2009/05/01 16:27:53 sunil Exp $
//--------------------------------------------------------------------------
// File:   TableReader.h
// Module:  simx
// Author:  Sunil Thulasidasan
// Created: Dec 15 2008
//
//	Generic 2-column table reader
//      Format of the table is:
//
//      + owner_id_1  
//      key value
//      key value 
//      ....
//      + owner_id_2
//      key value... 
//      
//      NOTE: operator ">>" must be defined for owner_id, key and value types.
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------



#ifndef NISAC_SIMX_TABLE_READER_H
#define NISAC_SIMX_TABLE_READER_H

#include <map>
#include "boost/shared_ptr.hpp"
#include "simx/logger.h"
#include "simx/readers.h"
#include <fstream>

namespace simx

{
  template<typename OwnerType>
    bool processAllEntries( const OwnerType& )
    {
      return true;
    }
  
  template< typename OwnerType, typename MapType >
  class TableReader
  {
  public:

    TableReader();
    //~TableReader();
    bool ProcessAllEntries ( const OwnerType& );
    void LoadTable( std::ifstream&, std::map< OwnerType, boost::shared_ptr<MapType> >&,
		    bool (*)( const  OwnerType& ) = &processAllEntries<OwnerType> );// &(TableReader< OwnerType, MapType>::ProcessAllEntries ) );
    void TableReadError(std::ifstream& );

  private:
    void readUntilNextEntry( std::ifstream& ifs );
    bool fMoreData;

  };

  //will skip white space, comments etc.
  template< typename OwnerType, typename MapType > 
    void TableReader<OwnerType, MapType>::readUntilNextEntry( std::ifstream& ifs )
  {
    std::string line;
    while( ! ifs.eof() )
      {
	while( ifs.peek() == '\t' || ifs.peek() == ' ' )
	  ifs.get();
	if ( ifs.eof() )
	  break;
	char c = ifs.peek();
	if ( c == '\n' )
	  {
	    ifs.get();
	    continue;
	  }
	if ( c == '#' )
	  {
	    getline( ifs, line );
	    continue;
	  }
	//if c is something else, assume a valid line here
	break;
      } 
  }
  


  template< typename OwnerType, typename MapType >
  TableReader<OwnerType, MapType>::TableReader() {}

  template< typename OwnerType, typename MapType >
    void TableReader<OwnerType, MapType>::LoadTable( std::ifstream& ifs, 
						   std::map< OwnerType,
						     boost::shared_ptr<MapType> >& tmap,
						     bool ( *processEntry) ( const OwnerType& ) )
  {
    char  dummy;
    std::string line;
    OwnerType curr_entry;
    bool foundEntry = false;
    boost::shared_ptr<MapType> curr_map_ptr;
    typename MapType::key_type key;
    typename MapType::mapped_type value;
    int i = 0;
    fMoreData = true;
    for (;;)
      {
	readUntilNextEntry( ifs );
	if ( ifs.eof() )
	  break;
	if ( ifs.peek() == '+' )
	  {
	    ifs >> dummy >> curr_entry;

	    if ( processEntry( curr_entry ) )
	      {
		simx::Logger::debug3() << "Table Reader: Processed " << i++ << " entries " << std::endl;
		foundEntry = true;
		typename std::map< OwnerType, boost::shared_ptr<MapType> >::const_iterator
		  iter = tmap.find( curr_entry );
		if ( iter == tmap.end() )
		  {
		    simx::Logger::debug3() << "Inserting entry for " << curr_entry << std::endl;
		    boost::shared_ptr<MapType> map_ptr( new MapType );
		    tmap.insert( make_pair( curr_entry, map_ptr ));
		    curr_map_ptr = map_ptr;
		  }
		else //entry for this entity already exists
		  {
		    curr_map_ptr = iter->second;
		  }
	      }
	    else // do not process this entry
	      {
		foundEntry = false;
	      }
	  }
	else // not a new entry, assume it is a map entry
	  {
	    if ( foundEntry )
	      {
		ifs >> key >> value;
		curr_map_ptr->insert( make_pair( key, value ));
	      }
	    else //skip line
	      {
		getline( ifs, line );
	      }
	  }
	if ( ifs.fail() || ifs.rdstate() )
	  TableReadError( ifs );
      }    
  }
  
  
  //TODO: better error message
  template< typename OwnerType, typename MapType >
  void TableReader<OwnerType, MapType>::TableReadError( std::ifstream& ifs)
  {

    simx::Logger::failure("Error while reading Table");
  }
		
} //namespace

#endif				   
