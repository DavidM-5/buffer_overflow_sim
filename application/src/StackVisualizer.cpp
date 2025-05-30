#include "StackVisualizer.h"

application::StackVisualizer::StackVisualizer(int posX, int posY, int w, int h, SDL_Color color, int slotsAmount, const std::string& font, int fontSize) : 
                                              Widget(posX, posY, w, h, color),
                                              m_slotsAmount(slotsAmount), m_slotHeight(h / slotsAmount),
                                              m_font(font),
                                              m_fontSize(fontSize),
                                              m_selectedBpSlot(-1)
{
    application::TextLine::loadFont(font, fontSize);

    m_bpText.useFont(font, fontSize);
    m_bpText.appendText("bp-> ", true);

    m_bpText.setHeight(m_slotHeight);
    m_bpText.setWidth(50);
}

void application::StackVisualizer::render(core::Renderer &renderer, const SDL_Rect* srcRect, const SDL_Rect* dstRect)
{   
    int startIdx = m_slots.size() > m_slotsAmount ? m_slots.size() - m_slotsAmount : 0;

    for (int i = 0; i < m_slotsAmount && (startIdx + i) < m_slots.size(); i++) {
        SDL_Rect dstRect = {m_transform.x,
                            m_transform.y + m_slotHeight * i,
                            m_slots[startIdx + i]->getWidth(),
                            m_slotHeight};

        if (startIdx + i == m_selectedBpSlot) {
            renderer.drawRect(dstRect, {0x71, 0x71, 0xD5, 0xFF});
            
            SDL_Rect bpDstRect = { dstRect.x - m_bpText.getWidth(),
                                   dstRect.y,
                                   m_bpText.getWidth(),
                                   m_bpText.getHeight() };
            m_bpText.render(renderer, nullptr, &bpDstRect);
        }

        m_slots[startIdx + i]->render(renderer, nullptr, &dstRect);
    }
    
    for (int i = 0; i < m_slotsAmount; i++) {
        SDL_Rect dstRect = {m_transform.x,
                            m_transform.y + m_slotHeight * i,
                            m_transform.w,
                            m_slotHeight};

        renderer.drawRectBorder(dstRect, m_mainColor);
    }
}

void application::StackVisualizer::push(const std::string& str)
{
    auto txt = std::make_unique<application::TextLine>(
                              m_transform.x, 
                              m_transform.y + m_slots.size() * m_slotHeight, 
                              m_transform.w, 
                              m_slotHeight,
                              m_mainColor
                              );

    txt->useFont(m_font, m_fontSize);
    txt->appendText(str);

    m_slots.push_back(std::move(txt));
}

void application::StackVisualizer::pop()
{
    if (m_slots.size() > 0) {
        m_slots.pop_back();
    }
}

void application::StackVisualizer::selectBPSlot(int slot)
{
    m_selectedBpSlot = slot;
}

void application::StackVisualizer::editSlot(int slotNum, const std::string &newText)
{
    if (slotNum < 0 || slotNum >= m_slots.size())
        return;
    
    m_slots[slotNum]->clear();
    m_slots[slotNum]->appendText(newText);
}

void application::StackVisualizer::clear()
{
    m_slots.clear();
}

void application::StackVisualizer::addDeltaTransform(int x, int y, int w, int h)
{
    m_transform.x += x;
    m_transform.y += y;
    m_transform.w += w;
    m_transform.h += h;

    m_slotHeight = m_transform.h / m_slotsAmount;

    for (auto& slot : m_slots) {
        slot->addDeltaTransform(x, y, 0, 0);
        slot->setWidth(m_transform.w);
        

        if (slot->getHeight() > m_slotHeight - 2) {
            slot->setHeight(m_slotHeight - 2);
            slot->useFont(m_font, slot->getFontSize() - 1);
        }
        else if (slot->getHeight() < m_slotHeight - 2) {
            slot->setHeight(m_slotHeight - 2);
            slot->useFont(m_font, slot->getFontSize() + 1);
        }
    }
}
