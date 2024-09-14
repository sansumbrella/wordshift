//
//  CascadeLogger.m
//  WordShift
//
//  Created by David Wicks on 5/3/13.
//
//

#import "CascadeLogger.h"

void cascade::CascadeLog(const std::string &str)
{
  NSString *message = [NSString stringWithCString:str.c_str() encoding:[NSString defaultCStringEncoding]];
  NSLog( @"%@", message );
}
