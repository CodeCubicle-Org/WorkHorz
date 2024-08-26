//
// Created by Pat Le Cat on 30 Jul 2024.
//

#pragma once

namespace WHZ {

    enum class ResourceType {
        CSS,        /// Files with .css extension
        HTML,       /// Files with .html extension
        WHZT,       /// Files with .whzt extension, HTML templates for WHZ
        JS,         /// Files with .js extension
        IMAGE_PNG,  /// Files with .png extension
        IMAGE_JPG,  /// Files with .jpg extension
        UNKNOWN     /// Files with an extension unknown to WHZ
    };

} // WHZ
