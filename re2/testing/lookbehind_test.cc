// Copyright 2024 The RE2 Authors.  All Rights Reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Test lookbehind and lookahead assertions

#include <string>

#include "gtest/gtest.h"
#include "absl/base/macros.h"
#include "re2/regexp.h"

namespace re2 {

// Test basic parsing of lookbehind patterns
TEST(LookbehindTest, ParseNegativeLookbehind) {
  Regexp::ParseFlags flags = Regexp::LikePerl;
  RegexpStatus status;
  
  // Test negative lookbehind parsing
  Regexp* re = Regexp::Parse("(?<!\\d)test", flags, &status);
  ASSERT_TRUE(re != NULL) << "Failed to parse negative lookbehind: " 
                          << status.Text();
  EXPECT_EQ(kRegexpConcat, re->op());
  
  // First part should be the lookbehind
  ASSERT_GT(re->nsub(), 0);
  Regexp* lb = re->sub()[0];
  EXPECT_EQ(kRegexpLookBehindNegative, lb->op());
  
  re->Decref();
}

TEST(LookbehindTest, ParsePositiveLookbehind) {
  Regexp::ParseFlags flags = Regexp::LikePerl;
  RegexpStatus status;
  
  // Test positive lookbehind parsing
  Regexp* re = Regexp::Parse("(?<=foo)bar", flags, &status);
  ASSERT_TRUE(re != NULL) << "Failed to parse positive lookbehind: " 
                          << status.Text();
  EXPECT_EQ(kRegexpConcat, re->op());
  
  // First part should be the lookbehind
  ASSERT_GT(re->nsub(), 0);
  Regexp* lb = re->sub()[0];
  EXPECT_EQ(kRegexpLookBehindPositive, lb->op());
  
  re->Decref();
}

TEST(LookbehindTest, ParseNegativeLookahead) {
  Regexp::ParseFlags flags = Regexp::LikePerl;
  RegexpStatus status;
  
  // Test negative lookahead parsing
  Regexp* re = Regexp::Parse("test(?!\\d)", flags, &status);
  ASSERT_TRUE(re != NULL) << "Failed to parse negative lookahead: " 
                          << status.Text();
  EXPECT_EQ(kRegexpConcat, re->op());
  
  re->Decref();
}

TEST(LookbehindTest, ParsePositiveLookahead) {
  Regexp::ParseFlags flags = Regexp::LikePerl;
  RegexpStatus status;
  
  // Test positive lookahead parsing
  Regexp* re = Regexp::Parse("foo(?=bar)", flags, &status);
  ASSERT_TRUE(re != NULL) << "Failed to parse positive lookahead: " 
                          << status.Text();
  EXPECT_EQ(kRegexpConcat, re->op());
  
  re->Decref();
}

TEST(LookbehindTest, ComplexPattern) {
  Regexp::ParseFlags flags = Regexp::LikePerl;
  RegexpStatus status;
  
  // Test more complex pattern with lookbehind
  Regexp* re = Regexp::Parse("(?<!@)email", flags, &status);
  ASSERT_TRUE(re != NULL) << "Failed to parse complex pattern: " 
                          << status.Text();
  
  re->Decref();
}

TEST(LookbehindTest, ToString) {
  Regexp::ParseFlags flags = Regexp::LikePerl;
  RegexpStatus status;
  
  // Test that ToString works correctly
  Regexp* re = Regexp::Parse("(?<!\\d)test", flags, &status);
  ASSERT_TRUE(re != NULL);
  
  std::string str = re->ToString();
  // The string should contain the lookbehind syntax
  EXPECT_TRUE(str.find("(?<!") != std::string::npos) 
      << "ToString should preserve lookbehind syntax, got: " << str;
  
  re->Decref();
}

TEST(LookbehindTest, Simplify) {
  Regexp::ParseFlags flags = Regexp::LikePerl;
  RegexpStatus status;
  
  // Test that Simplify works with lookbehind
  Regexp* re = Regexp::Parse("(?<!foo)bar", flags, &status);
  ASSERT_TRUE(re != NULL);
  
  Regexp* simplified = re->Simplify();
  ASSERT_TRUE(simplified != NULL);
  
  simplified->Decref();
  re->Decref();
}

}  // namespace re2
