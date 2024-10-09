//
// Created by Pat Le Cat on 15/07/2024.
//

#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <optional>
#include <filesystem>
//#include "fmt/core.h"
//#include "tree.hh"
//#include "tree_util.hh"
// Adding the URL parser library
#include <boost/url.hpp>
#include "whz_quill_wrapper.hpp"


namespace whz {

using  MMPathlist = std::unordered_multimap<std::string, std::string>;

class http_routing {
public:
    explicit http_routing(
     const std::string& basedomain); /// Constructor, takes a string of the base-domain: e.g. "www.basedomain.ch"
  virtual ~http_routing() = default;

  bool addPathResource(const std::string& path); /// Add a new relative path resource to the routing table. Don't add existing or absolute paths
  bool addPathResource(std::filesystem::path path); /// Dito
  std::string getBaseDomain() const { return _basedomain; };
  std::optional<MMPathlist> findPath(const std::string& path); /// Find a path in the routing table and get the relative path with it



//  void printMe(std::ostream& oss) {
//    /// Use cout, cerr, or a file stream to print out the text
//    oss << fmt::format("I am a http_routing object at address: {}\n", _basedomain);
//    std::ostringstream osc;
//    if (osc.str() == "I am a http_routing object at address: Hello\n") {
//      osc << fmt::format("Hello, I am a wchar-http_routing object at address: {}\n", _basedomain);
//    }
//  }

protected:

private:
  std::string _basedomain;
  MMPathlist _page_pathResources;   /// Contains the page/file names and the relative filepath they reside in
  whz::whz_qlogger _qlogger;
};

} // namespace WHZ


/*
 * Example HTTP paths could be:
 * https://www.rootdomain.com/api/v1/users
 * /api/v1/users/1?fields=id,name,email
 * /api/v1/users/1/posts
 * /blog/news/2024/07/15
 * /blog/my-thoughts/october-2024
 */
