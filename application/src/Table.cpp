#include "Table.h"

application::Table::Table(int posX, int posY, int w, int h, SDL_Color color, int cols, int rows, const std::string& font, int fontSize) :
                          Widget(posX, posY, w, h, color), 
                          m_columns(cols),  m_rows(rows), 
                          m_slotW(w / cols),  m_slotH(h / rows), 
                          m_font(font), m_fontSize(fontSize) 
{
    
    application::TextLine::loadFont(font, fontSize);
    
    // Initialize as a rows x columns matrix
    m_table.resize(m_rows);
    for (int j = 0; j < m_rows; j++) {
        m_table[j].resize(m_columns);
        for (int i = 0; i < m_columns; i++) {
            SDL_Rect dstRect = { 
                m_transform.x + i * m_slotW, 
                m_transform.y + j * m_slotH,  // Fixed: .y instead of .h
                m_slotW, 
                m_slotH 
            };
            m_table[j][i].setWidth(dstRect.w);
            m_table[j][i].setHeight(dstRect.h);
            m_table[j][i].setPosition({dstRect.x, dstRect.y});
            m_table[j][i].useFont(m_font, m_fontSize);
        }
    }
}

void application::Table::render(core::Renderer &renderer, const SDL_Rect *srcRect, const SDL_Rect *dstRect) {
    for (int j = 0; j < m_rows; j++) {
        for (int i = 0; i < m_columns; i++) {
            SDL_Rect dstRect = { 
                m_transform.x + i * m_slotW, 
                m_transform.y + j * m_slotH,  // Fixed: .y instead of .h
                m_slotW, 
                m_slotH 
            };
            
            if (!(i == 0 && j == 0) && (i == 0 || j == 0)) {
                renderer.drawRect(dstRect, SDL_Color{0x25, 0x25, 0x25, 0xFF});
            }
            
            renderer.drawRectBorder(dstRect, SDL_Color{255, 255, 255, 255});
            m_table[j][i].render(renderer);
        }
    }
}

std::string application::Table::getText(const vector2i& coord) {
    if (!isInBounds(coord)) {
        return "";
    }
    // Access as [row][column]
    return m_table[coord.y][coord.x].getText();
}

void application::Table::setText(const vector2i &coord, const std::string &text) {
    if (!isInBounds(coord)) {
        return;
    }
    // Access as [row][column]
    m_table[coord.y][coord.x].clear();
    m_table[coord.y][coord.x].appendText(text, true);
}

vector2i application::Table::getSize() {
    return vector2i{m_columns, m_rows};
}

bool application::Table::isInBounds(const vector2i& coord) {
    return coord.x >= 0 && coord.x < m_columns && coord.y >= 0 && coord.y < m_rows;
}
