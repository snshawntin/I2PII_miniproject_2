#include "TextBox.hpp"
#include "Engine/Resources.hpp"
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

#include <iostream>

using namespace Engine;

TextBox::TextBox(float x, float y, float w, float h, int fontsize) :
    ImageButton("win/TextBoxBG.png", "win/TextBoxBG.png", x, y, w, h),
    font_size(fontsize) { ; }

void TextBox::Draw() const {
    ImageButton::Draw();
    al_draw_text(Resources::GetInstance().GetFont("pirulen.ttf", font_size).get(), al_map_rgb(0, 0, 0), Position.x + 20, Position.y + 5, 0, text.c_str());
}

void TextBox::OnKeyDown(int keyCode) {
    if (keyCode == ALLEGRO_KEY_BACKSPACE && !text.empty()){
        text.pop_back();
    }
    else if(text.size() >= 9){
        return;
    }
    else if (keyCode >= ALLEGRO_KEY_A && keyCode <= ALLEGRO_KEY_Z){
        text += 'a' + (keyCode - ALLEGRO_KEY_A);
    }
    else if(keyCode == ALLEGRO_KEY_SPACE){
        text += ' ';
    }
    else if(keyCode == ALLEGRO_KEY_MINUS){
        text += '-';
    }
    //you can add more char support here.....
}

//& now, this is not useful.
void TextBox::OnMouseDown(int button, int mx, int my) {
    // focused = (mx >= Position.x && mx <= Position.x + Size.x && my >= Position.y && my <= Position.y + Size.y);
    ImageButton::OnMouseDown(button, mx, my);
}