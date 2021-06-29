/**
* \file    StringUtils.h
* \brief
* \author  chi@spin-digital.com
* \date    15/12/2015
*
* Copyright:
* Spin Digital Video Technologies GmbH
* Einsteinufer 17
* 10587 Berlin
* Deutschland
*/

#pragma once 

#include <string>
#include <vector>
#include <istream>
#include <algorithm>

#define ARRAY_SIZE(array) (sizeof((array))/sizeof((array[0])))


template<typename T, typename P>
static std::istream& readStrToEnum(P map[], unsigned long mapLen, std::istream &in, T &val)
{
  std::string str;
  in >> str;

  for (int i = 0; i < mapLen; i++)
  {
    if (str == map[i].str)
    {
      val = map[i].value;
      goto found;
    }
  }
  /* not found */
  in.setstate(std::ios::failbit);
found:
  return in;
}

template <typename Separator>
static auto split_aux(const std::string& value, Separator&& separator) -> std::vector<std::string>
{
  std::vector<std::string> result;
  std::string::size_type p = 0;
  std::string::size_type q;
  while ((q = separator(value, p)) != std::string::npos)
  {
    result.emplace_back(value, p, q - p);
    p = q + 1;
  }
  result.emplace_back(value, p);
  return result;
}
static auto split(const std::string& value, char separator) -> std::vector<std::string>
{
  return split_aux(value,
    [=](const std::string& v, std::string::size_type p) {
    return v.find(separator, p);
  });
}

static bool is_digits(const std::string &str)
{
  return std::all_of(str.begin(), str.end(), ::isdigit); // C++11
}

static bool is_number(const std::string &str)
{
  if (str.at(0) == '-') {
    return std::all_of(std::next(str.begin()), str.end(), ::isdigit);
  }
  return is_digits(str);
}

static bool is_double(const std::string &str) {
  std::vector<std::string> fields = split(str, '.');
  if (fields.size() > 2 || fields.size() < 1) {
    return false;
  }
  if (fields.size() == 1) {
    return is_digits(str);
  }
  else {
    return (is_digits(fields[0]) && is_digits(fields[1]));
  }
}


static std::string ltrim(const std::string& str, const std::string &delimiter)
{
  size_t start = str.find_first_not_of(delimiter);
  return (start == std::string::npos) ? "" : str.substr(start);
}

static std::string rtrim(const std::string& str, const std::string& delimiter)
{
  size_t end = str.find_last_not_of(delimiter);
  return (end == std::string::npos) ? "" : str.substr(0, end + 1);
}

static std::string trim(const std::string& s, const std::string& delimiter)
{
  return rtrim(ltrim(s, delimiter), delimiter);
}


class LicenseException : public std::exception
{
};
