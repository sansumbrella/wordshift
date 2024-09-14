//
//  CascadePreferences.h
//  WordShift
//
//  Created by David Wicks on 5/9/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "cinder/Json.h"

namespace cascade
{
  /**
  JSON data that is backed by a file.
  */
  class LocalData
  {
  public:
    //! Add data to store
    void          appendData( const ci::JsonTree &data ){ mData.pushBack( data ); }
    template<typename T>
    void          appendData( const std::string &key, T value ){ mData.pushBack( ci::JsonTree( key, value ) ); }
    //! Access stored data by reference
    ci::JsonTree& getData( const std::string &data ){ return mData.getChild( data ); }
    template<typename T>
    T             getDataValue( const std::string &data ){ return mData.getChild( data ).getValue<T>(); }
    //! reads the local data from file; falls back to default_data if there is an error (or file was edited)
    void          read( const ci::fs::path &data_path, const ci::JsonTree &default_data );
    //! writes local data to file (usually call whenever you lose app focus)
    void          write( const ci::fs::path &data_path );
  private:
    ci::JsonTree  mData;
  };

  /**
  JSON data that is backed by a file and protected with a hash.
  Hash is checked to ensure no editing of the saved file is allowed.
  */
  class HashedLocalData
  {
  public:
    void          appendData( const ci::JsonTree &data ){ mData.pushBack( data ); }
    template<typename T>
    void          appendData( const std::string &key, T value ){ mData.pushBack( ci::JsonTree( key, value ) ); }
    void          setData( const ci::JsonTree &data ){ mData = data; }
    //! get a JSON element by reference
    ci::JsonTree& getData( const std::string &data ){ return mData.getChild( data ); }
    template<typename T>
    T             getDataValue( const std::string &data ){ return mData.getChild( data ).getValue<T>(); }
    //! reads the local data from file; falls back to default_data if there is an error (or file was edited)
    void          read( const ci::fs::path &data_path, const ci::JsonTree &default_data );
    //! writes local data to file (usually call whenever you lose app focus)
    void          write( const ci::fs::path &data_path );
  private:
    ci::JsonTree  mData;
    // Change salt and pepper with every application this is used int
    static std::string salt(){ return   "bHoem!@#%asdf23"; }
    static std::string pepper(){ return "crunchy!23F!@#A"; }
  };
}
