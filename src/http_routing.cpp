//
// Created by Pat Le Cat on 15/07/2024.
//

#include "http_routing.hpp"
// #include <boost/system/result.hpp>

namespace whz {
    http_routing::http_routing(const std::string& basedomain) {
        this->_basedomain = basedomain;
        this->_page_pathResources.reserve(100); // Assume we will have 100 pages
    }
/*
http_routing::http_routing();

http_routing::~http_routing();
*/
/**
 * @brief add a new path resource to the routing table. Don't add existing paths
 * @param path as std::string
 * @return bool true if adding went well, false if not
 */
    bool http_routing::addPathResource(const std::string&path) {
        bool bRet = false;
        boost::system::result <boost::url_view> const uRes = boost::urls::parse_uri(path);
        if (!uRes) {
            this->_qlogger.error(fmt::format("Routing Error: Path is not valid {}", uRes.error().message()));
            //LOG_ERROR("Routing Error: Path is not valid {}", uRes.error().message());
            std::cerr << "Routing Error: Path is not valid " << uRes.error().message()
                      << std::endl;
            return bRet;
        }
        std::string sPath = uRes.value().path();
        std::string::size_type pos = sPath.rfind("/");
        if (pos == std::string::npos) {
            this->_qlogger.error("Routing Error: No '/' found in the path");
            //LOG_ERROR("Routing Error: No '/' found in the path");
            std::cerr << "Routing Error: No '/' found in the path" << std::endl;
            return bRet;
        }
        if (pos == sPath.size() - 1) {
            this->_qlogger.info("'/' found at the end of the path = is a path-page");
            std::cout << "'/' found at the end of the path = is a path-page" << std::endl;
            std::string::size_type spos = sPath.rfind("/", sPath.size() - 1); // find the second last slash /
            std::string sPathname = sPath.substr(spos + 1, pos); // extract the part after the second-last
            // and the last slash / ,it's the page-path
            this->_page_pathResources.insert(std::make_pair(sPathname, sPath)); // There's no filename only a path
            bRet = true;
        } else {
            this->_qlogger.info("'/' found in the middle of the path = is a path-file");
            //std::cout << "'/' found in the middle of the path = is a path-file" << std::endl;
            std::string sfilename = sPath.substr(pos + 1,
                                                 sPath.size()); // the part after the last slash / is the filename
            this->_page_pathResources.insert(std::make_pair(sfilename, sPath));
            bRet = true;
        }
        return bRet;
    }

    bool http_routing::addPathResource(std::filesystem::path path) {
        return this->addPathResource(path.string());
    }

/**
 * @brief Find a path in the routing table and get the relative path with it don't use full paths only the last part/page or filename
 * @param path as std::string
 * @return std::optional<MMPathlist> containing the path and the relative path or empty if not found
 */
    std::optional <MMPathlist> http_routing::findPath(const std::string&fpath) {
        if (!fpath.empty()) {
            // Find all occurrences of fpath in this->_page_pathResources and insert them into a new unordered_multimap and return it
            MMPathlist mmpl;
            for (auto&it: this->_page_pathResources) {
                if (it.first.compare(fpath) == 0) { // 0 if equal
                    mmpl.insert(it);
                }
            }
            return mmpl;
        } else {
            this->_qlogger.error(fmt::format("Routing Error: path is empty or could not be found: {}", fpath));
            //LOG_ERROR("Routing Error: path is empty or could not be found: {}", fpath);
            std::cerr << "Routing Error: path is empty or could not be found: " << fpath << std::endl;
            return std::nullopt;
        }
    }

} // namespace WHZ