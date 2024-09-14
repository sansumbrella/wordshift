//
//  CascadeLogger.h
//  WordShift
//
//  Created by David Wicks on 5/3/13.
//
//

namespace cascade
{
  // Pipes messages over to NSLog, so we can get logfiles from devices
  void CascadeLog( const std::string &str );
}