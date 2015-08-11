#pragma once

#ifndef _MR_UI_UTIL_H_
#define _MR_UI_UTIL_H_

#include <glm/glm.hpp>
#include <string>

namespace mr {

class UIUtil {
public:
    union Rect {
        glm::vec4 asVec;
        struct {
            float x;
            float y;
            float width;
            float height;
        };
    };

    inline static std::string RectToString(const UIUtil::Rect& rect) {
        return std::to_string(rect.x) + ", " + std::to_string(rect.y) + ", " + std::to_string(rect.width) + ", " + std::to_string(rect.height);
    }

    inline static bool IsPointIn(const UIUtil::Rect& rect, const glm::vec2& point) {
        if(rect.x > point.x) return false;
        if(rect.y > point.y) return false;
        if(rect.x+rect.width < point.x) return false;
        if(rect.y+rect.height < point.y) return false;
        return true;
    }
};

}

#endif // _MR_UI_UTIL_H_
