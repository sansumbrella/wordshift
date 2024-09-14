//
//  WordEvaluator.h
//  WordShift
//
//  Created by David Wicks on 4/19/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once

namespace cascade
{
  class WordEvaluator
  {
  public:
    WordEvaluator();
    ~WordEvaluator();
    uint32_t getWordValue( const std::string &word, int num_matches );
  private:
    std::map<const char, uint32_t>    mLetterValues;
    std::map<size_t, float>           mLengthMultipliers;
    std::map<int, float>              mMatchCountMultipliers;
  };
}
