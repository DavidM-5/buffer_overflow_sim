#include "Table.h"

application::Table::Table(int posX, int posY, int w, int h, SDL_Color color, int cols, int rows, const std::string& font, int fontSize) : 
                          Widget(posX, posY, w, h, color),
                          m_columns(cols), m_rows(rows),
                          m_slotH(h / rows), m_slotW(w / cols),
                          m_font(font), m_fontSize(fontSize)
{
    application::TextLine::loadFont(font, fontSize);

    m_table = std::vector<std::vector<application::TextLine>>(rows, std::vector<application::TextLine>(cols));

    for (int i = 0; i < m_columns; i++) {
        for (int j = 0; j < m_rows; j++) {
            SDL_Rect dstRect = { m_transform.x + i * m_slotW,
                                 m_transform.h + j * m_slotH,
                                 m_slotW,
                                 m_slotH };

            m_table[i][j].setWidth(dstRect.w);
            m_table[i][j].setHeight(dstRect.h);
            m_table[i][j].setPosition({dstRect.x, dstRect.y});

            m_table[i][j].useFont(m_font, m_fontSize);
        }
    }
}

void application::Table::render(core::Renderer &renderer, const SDL_Rect *srcRect, const SDL_Rect *dstRect)
{
    for (int i = 0; i < m_columns; i++) {
        for (int j = 0; j < m_rows; j++) {
            SDL_Rect DstRect = { m_transform.x + i * m_slotW,
                                 m_transform.h + j * m_slotH,
                                 m_slotW,
                                 m_slotH };

            renderer.drawRectBorder(DstRect, SDL_Color{255, 255, 255, 255});
            m_table[i][j].render(renderer);
        }
    }
}

std::string application::Table::getText(const vector2i& coord)
{
    if (!isInBounds(coord)) {
        return "";
    }

    return m_table[coord.x][coord.y].getText();
}

void application::Table::setText(const vector2i &coord, const std::string &text)
{
    if (!isInBounds(coord)) {
        return;
    }

    m_table[coord.x][coord.y].clear();
    m_table[coord.x][coord.y].appendText(text, true);
}

bool application::Table::isInBounds(const vector2i& coord)
{
    return coord.x >= 0 && coord.x < m_columns && coord.y >= 0 && coord.y < m_rows;
}
