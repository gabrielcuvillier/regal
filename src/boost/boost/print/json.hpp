/*
 * Copyright (c) 2010-2012 NVIDIA Corporation
 * Copyright (c) 2010-2012 Nigel Stewart
 * All rights reserved.
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * boost::print is a library for string conversion.
 * boost::print is not currently a part of boost.
 */

#ifndef BOOST_PRINT_JSON_HPP
#define BOOST_PRINT_JSON_HPP

#if defined(_MSC_VER) && (_MSC_VER>=1200)
#pragma once
#endif

#include <cstring>
#include <string>

#include "interface.hpp"

// See: http://www.json.org/

namespace boost { namespace print { namespace detail { namespace json {

struct pair_null
{
  pair_null(const char *str) : _string(str) {}
  pair_null &operator=(const pair_null &other) { _string = other._string; return *this; }
  const char * _string;
};

template<typename T>
struct pair
{
  pair(const char *str, const T &val) : _string(str), _value(val) {}
  pair<T> &operator=(const pair<T> &other) { _string = other._string; _value = other._value; return *this; }
  const char * const  _string;
  T                   _value;
};

template<typename T>
struct pair_array
{
  pair_array(const char *str, const T *val, const size_t size) : _string(str), _value(val), _size(size) {}
  pair<T> &operator=(const pair<T> &other) { _string = other._string; _value = other._value; _size = other._size; return *this; }
  const char *_string;
  const T    *_value;
  size_t      _size;
};

template<typename P>
struct member
{
  member(const P &pair, bool comma = true) : _pair(pair), _comma(comma) {}
  member<P> &operator=(const member<P> &other) { _pair = other._pair; _comma = other._comma; return *this; }
  P        _pair;
  bool     _comma;
};

// Format according to string escapes for \n etc

inline size_t length(const char *str)
{
  size_t len = 2;
  for (const char *i = str; *i; ++i)
    switch (*i)
    {
      case '\"':
      case '\\':
      case '/':
      case '\b':
      case '\f':
      case '\n':
      case '\r':
      case '\t':
        len += 1;
      default:
        len += 1;
    }
  return len;
}

template<typename Iterator>
inline void write(Iterator &i, const char *val)
{
  boost::print::extend::write(i,"\"");
  for (const char *j = val; *j; ++j)
    switch (*j)
    {
      case '\"': boost::print::extend::write(i,"\\\""); break;
      case '\\': boost::print::extend::write(i,"\\\\"); break;
      case '/' : boost::print::extend::write(i,"\\/");  break;
      case '\b': boost::print::extend::write(i,"\\b");  break;
      case '\f': boost::print::extend::write(i,"\\f");  break;
      case '\n': boost::print::extend::write(i,"\\n");  break;
      case '\r': boost::print::extend::write(i,"\\r");  break;
      case '\t': boost::print::extend::write(i,"\\t");  break;
      default:   boost::print::extend::write(i,*j);     break;
    }
  boost::print::extend::write(i,"\"");
}

}}}}

namespace boost { namespace print { namespace json {

// null

boost::print::detail::json::pair_null                 pair(const char *str)                               { return boost::print::detail::json::pair_null(str);  }

boost::print::detail::json::pair<bool>                pair(const char *str, const bool val)               { return boost::print::detail::json::pair<bool>(str,val); }

boost::print::detail::json::pair<unsigned short    >  pair(const char *str, const unsigned short     val) { return boost::print::detail::json::pair<unsigned short>(str,val); }
boost::print::detail::json::pair<unsigned int      >  pair(const char *str, const unsigned int       val) { return boost::print::detail::json::pair<unsigned int>(str,val); }
boost::print::detail::json::pair<unsigned long     >  pair(const char *str, const unsigned long      val) { return boost::print::detail::json::pair<unsigned long>(str,val); }
boost::print::detail::json::pair<unsigned long long>  pair(const char *str, const unsigned long long val) { return boost::print::detail::json::pair<unsigned long long>(str,val); }

boost::print::detail::json::pair<signed short>        pair(const char *str, const signed short       val) { return boost::print::detail::json::pair<signed short>(str,val); }
boost::print::detail::json::pair<signed int>          pair(const char *str, const signed int         val) { return boost::print::detail::json::pair<signed int>(str,val); }
boost::print::detail::json::pair<signed long>         pair(const char *str, const signed long        val) { return boost::print::detail::json::pair<signed long>(str,val); }
boost::print::detail::json::pair<signed long long>    pair(const char *str, const signed long long   val) { return boost::print::detail::json::pair<signed long long>(str,val); }

boost::print::detail::json::pair<float>               pair(const char *str, const float  val)             { return boost::print::detail::json::pair<float>(str,val); }
boost::print::detail::json::pair<double>              pair(const char *str, const double val)             { return boost::print::detail::json::pair<double>(str,val); }

boost::print::detail::json::pair<const char       *>  pair(const char *str, const char        *val)       { return boost::print::detail::json::pair<const char *>(str,val); }
boost::print::detail::json::pair<std::string>         pair(const char *str, const std::string &val)       { return boost::print::detail::json::pair<std::string>(str,val); }

// array

template<typename T>
detail::json::pair_array<T>
pair(const char *str, const T *value, const std::size_t size)
{
  return boost::print::detail::json::pair_array<T>(str,value,size);
}

// array

#if 0
template<typename T>
detail::json::pair_array<T>
iterator_pair(const char *str, const T &begin, const T &end)
{
  return boost::print::detail::json::pair_array<T>(str,value,size);
}
#endif

template<typename P>
boost::print::detail::json::member<P>
member(const P &pair, const bool comma = true)
{
  return boost::print::detail::json::member<P>(pair,comma);
}

}}}

//
// boost::print::extend namespace for implementing length and write per supported type
//

namespace boost { namespace print { namespace extend {

using boost::print::detail::json::pair_null;
using boost::print::detail::json::pair;
using boost::print::detail::json::pair_array;
using boost::print::detail::json::member;

inline size_t length(const pair_null &val) { return length(val._string) + 3 + 4; }

template<typename T>
inline size_t length(const pair<T> &val) { return length(val._string) + 3 + length(val._value); }

template<>
inline size_t length(const pair<const char *> &val) { return length(val._string) + 3 + boost::print::detail::json::length(val._value); }

template<>
inline size_t length(const pair<std::string> &val) { return length(val._string) + 3 + boost::print::detail::json::length(val._value.c_str()); }

template<typename T>
inline size_t length(const pair_array<T> &val)
{
  size_t len = length(val._string) + 5;
  if (val._size)
  {
    len += length(val._value[0]);
    for (size_t j=1; j<val._size; ++j)
      len += length(val._value[j]) + 2;
  }
  return len;
}

template<>
inline size_t length(const pair_array<const char *> &val)
{
  size_t len = length(val._string) + 5;
  if (val._size)
  {
    len += boost::print::detail::json::length(val._value[0]);
    for (size_t j=1; j<val._size; ++j)
      len += boost::print::detail::json::length(val._value[j]) + 2;
  }
  return len;
}

template<>
inline size_t length(const pair_array<std::string> &val)
{
  size_t len = length(val._string) + 5;
  if (val._size)
  {
    len += boost::print::detail::json::length(val._value[0].c_str());
    for (size_t j=1; j<val._size; ++j)
      len += boost::print::detail::json::length(val._value[j].c_str()) + 2;
  }
  return len;
}

template<typename P>
inline size_t length(const member<P> &pair)
{
  return length(pair._pair) + (pair._comma ? 2 : 1);
}

/////////////////////

template<typename Iterator>
inline void write(Iterator &i, const pair_null &val)
{
  write(i,val._string," : null");
}

template<typename Iterator, typename T>
inline void write(Iterator &i, const pair<T> &val)
{
  write(i,val._string," : ",val._value);
}

template<typename Iterator>
inline void write(Iterator &i, const pair<const char *> &val)
{
  write(i,val._string," : ");
  boost::print::detail::json::write(i,val._value);
}

template<typename Iterator>
inline void write(Iterator &i, const pair<std::string> &val)
{
  write(i,val._string," : ");
  boost::print::detail::json::write(i,val._value.c_str());
}

template<typename Iterator, typename T>
inline void write(Iterator &i, const pair_array<T> &val)
{
  write(i,val._string);
  write(i," : [");
  if (val._size)
  {
    write(i,val._value[0]);
    for (size_t j=1; j<val._size; ++j)
    {
      write(i,", ");
      write(i,val._value[j]);
    }
  }
  write(i,"]");
}

template<typename Iterator>
inline void write(Iterator &i, const pair_array<const char *> &val)
{
  write(i,val._string);
  write(i," : [");
  if (val._size)
  {
    boost::print::detail::json::write(i,val._value[0]);
    for (size_t j=1; j<val._size; ++j)
    {
      write(i,", ");
      boost::print::detail::json::write(i,val._value[j]);
    }
  }
  write(i,"]");
}

template<typename Iterator>
inline void write(Iterator &i, const pair_array<std::string> &val)
{
  write(i,val._string);
  write(i," : [");
  if (val._size)
  {
    boost::print::detail::json::write(i,val._value[0].c_str());
    for (size_t j=1; j<val._size; ++j)
    {
      write(i,", ");
      boost::print::detail::json::write(i,val._value[j].c_str());
    }
  }
  write(i,"]");
}

template<typename Iterator, typename P>
inline void write(Iterator &i, const member<P> &val)
{
  write(i,val._pair);
  write(i,val._comma ? ",\n" : "\n");
}

}}}

#endif
