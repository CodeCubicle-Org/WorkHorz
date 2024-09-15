//
// Created by Pat Le Cat on 10/09/2024.
//

#pragma once

#include <vector>
#include <string>
#include "whz_http_routing.hpp"


namespace whz {

    /**
     * @brief The renderer class composes all content parts for a page to serve back to the client. The content
     * substitution is done by user scripts/code like getting data from the DB. The users code has to call the renderer
     * when the page is ready to be served.
     *
     */
    class whz_renderer {
    public:
        whz_renderer() = default;
        ~whz_renderer() = default;

        bool render_page(whz::MMPathlist& page_to_render, std::vector<std::string>& page_partials);
        std::string getRenderedPageContent() const { return _rendered_page_content; };

    private:
        std::string _rendered_page_content;
    };


} // whz
