#ifndef TEXTBOX_HPP
#define TEXTBOX_HPP

#include <string>
#include "UI/Component/ImageButton.hpp"

namespace Engine {
class TextBox : public ImageButton { // 改為繼承 ImageButton
public:
    std::string text;
    int font_size;
    TextBox(float x, float y, float w, float h, int fontsize);
    void Draw() const override;
    void OnKeyDown(int keyCode) override;
    void OnMouseDown(int button, int mx, int my) override;
};
}

#endif   // TEXTBOX_HPP