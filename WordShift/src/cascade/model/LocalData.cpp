//
//  CascadePreferences.cpp
//  WordShift
//
//  Created by David Wicks on 5/9/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "LocalData.h"
#include "CascadeLogger.h"
#include <fstream>
#include <boost/uuid/sha1.hpp>

using namespace cascade;
using namespace cinder;
using namespace std;

// Crypto definitions
typedef unsigned int SHA1Digest[5];
typedef char SHA1Hash[20];

inline void getSHA1Digest( const std::string &str, SHA1Digest &digest )
{
  boost::uuids::detail::sha1 sha1;
  sha1.process_bytes( str.data(), str.size() );
  sha1.get_digest( digest );
}

inline void digestToHash( SHA1Digest digest, SHA1Hash &hash )
{ // thanks to: https://gist.github.com/jhasse/990731 for sample of using boost::sha1
  for(int i = 0; i < 5; ++i)
  {
    const char* tmp = reinterpret_cast<char*>(digest);
    hash[i*4] =   tmp[i*4+3];
    hash[i*4+1] = tmp[i*4+2];
    hash[i*4+2] = tmp[i*4+1];
    hash[i*4+3] = tmp[i*4];
  }
}

inline string hashToString( SHA1Hash hash )
{
  stringstream ss;
  ss << std::hex;
  for(int i = 0; i < 20; ++i)
  {
    ss  << ((hash[i] & 0x000000F0) >> 4)
    <<  (hash[i] & 0x0000000F);
  }
  return ss.str();
}

inline bool hashesMatch( SHA1Hash lhs, SHA1Hash rhs )
{
  for( int i = 0; i < 20; ++i )
  {
    if( lhs[i] != rhs[i] ){ return false; }
  }
  return true;
}

void LocalData::write(const ci::fs::path &data_path)
{
  mData.write( data_path );
}

void LocalData::read(const ci::fs::path &data_path, const ci::JsonTree &default_data)
{
  if( fs::exists( data_path ) )
  { // load data
    ifstream input;
    input.open( data_path.string(), ios::binary );
    std::stringstream data_buffer;
    data_buffer << input.rdbuf();
    input.close();
    std::string data( data_buffer.str() );

    try
    {
      mData = JsonTree{ data };
    } catch( std::exception &exc )
    { // error parsing data, use default
      CascadeLog( "Error parsing LocalData: " + string(exc.what()) );
      mData = default_data;
    }
  }
  else
  { // No files to load, use default
    mData = default_data;
  }
}

void HashedLocalData::write(const ci::fs::path &data_path)
{
  fs::path hash_path = data_path.string() + "-hash";
  // data to save
  string serialized_data = mData.serialize();
  // generate hash from data
  SHA1Digest  digest;
  SHA1Hash    hash;
  getSHA1Digest( salt() + serialized_data + pepper(), digest );
  digestToHash( digest, hash );

  ofstream out;
  // write data
  out.open( data_path.string(), ios::binary | ios::trunc );
  out << serialized_data;
  out.close();
  // write hash
  out.open( hash_path.string(), ios::binary );
  out.write( hash, 20 );
  out.close();
}

void HashedLocalData::read(const ci::fs::path &data_path, const ci::JsonTree &default_data)
{
  fs::path hash_path = data_path.string() + "-hash";
  if( fs::exists( data_path ) && fs::exists( hash_path ) )
  { // load data
    ifstream input;
    input.open( data_path.string(), ios::binary );
    std::stringstream data_buffer;
    data_buffer << input.rdbuf();
    input.close();
    std::string data( data_buffer.str() );
    // load hash
    SHA1Hash saved_hash;
    input.open( hash_path.string(), ios::binary );
    input.read( saved_hash, 20 );
    input.close();
    // compute hash of loaded data
    SHA1Digest  digest;
    SHA1Hash    data_hash;
    getSHA1Digest( salt() + data + pepper(), digest );
    digestToHash( digest, data_hash );
    
    if( hashesMatch( saved_hash, data_hash ) )
    { // Hashes match, parse the JSON
      try
      { // unlikely to fail here, but we maay have written malformed json before
        mData = JsonTree{ data };
      } catch( std::exception &exc )
      { // error parsing data, use default
        CascadeLog( "Error parsing HashedLocalData: " + string(exc.what()) );
        mData = default_data;
      }
    }
    else
    { // the data on disk doesn't match the saved hash
      CascadeLog( "File changed on disk (Hashes don't match)" );
      CascadeLog( "Loaded: " + hashToString( saved_hash ) );
      CascadeLog( "Calced: " + hashToString( data_hash ) );
      mData = default_data;
    }
  }
  else
  { // No files to load, use default
    mData = default_data;
  }
}
