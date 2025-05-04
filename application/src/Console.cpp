#include "Console.h"

application::Console::Console(int posX, int posY, int w, int h, SDL_Color color) :
                              Widget(posX, posY, w, h, color),
                              m_activeLine(posX, posY, w, 20, {255, 255, 255, 255}, false),
                              m_thisWidgetSelected(false),
                              m_gdbAttached(false),
                              m_locked(false),
                              m_renderStartLine(0)
{
    m_activeLine.useFont("JetBrainsMono-Medium.ttf", 14);
    m_activeLine.appendText("> ", true);
}

void application::Console::handleEvents(const core::InputManager &inputMngr)
{
    if (m_gdbAttached) {
        std::string out = m_gdb->getTargetOutput();

        if (!out.empty()) {
            printToConsole(out);
        }
        
    }


    vector2i mousePos = inputMngr.getMousePosition();

    // Check for scrolling
    if (isMouseInsideTransform(mousePos) && inputMngr.getMouseWheelScroll() != 0 && m_lines.size() > 0) {
        m_renderStartLine -= inputMngr.getMouseWheelScroll();

        if (m_renderStartLine < 0)
            m_renderStartLine = 0;
        else if (m_renderStartLine >= m_lines.size())
            m_renderStartLine = m_lines.size() - 1;
    }

    if (inputMngr.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (!isMouseInsideTransform(mousePos))
            m_thisWidgetSelected = false;
        else
            m_thisWidgetSelected = true;
    }

    if (!m_thisWidgetSelected)
        return;

    std::string input = inputMngr.getPressedKey();

    if (input == "\n") {
        if (m_locked)
            return;

        // TODO: Remove lines below. The console SHOULD NOT send commands to gdb.
        if (m_gdbAttached) { // send commands to gdb / target
            m_gdb->sendTargetInput(m_activeLine.getText().substr(2));
        }
        
        m_lines.push_back(m_activeLine);
        m_activeLine.clear();

        m_activeLine.appendText("> ", true);

        m_renderStartLine++;
    }
    else if (input == "\b") {
        if (m_activeLine.getLenght() > 2)
            m_activeLine.editText(m_activeLine.getLenght() - 1, m_activeLine.getLenght(), "");
    }
    else {
        m_activeLine.appendText(input, true);
    }
}

void application::Console::render(core::Renderer &renderer, const SDL_Rect *srcRect, const SDL_Rect *dstRect)
{
    Widget::render(renderer);

    // Calculate the maximum number of lines that can fit into the console's height
    int lineHeight = m_activeLine.getHeight();
    int maxLines = m_transform.h / lineHeight;
    
    // Ensure m_renderStartLine doesn't go beyond the total number of lines
    int totalLines = m_lines.size();
    m_renderStartLine = std::min(m_renderStartLine, std::max(0, totalLines - (maxLines - 1)));
    
    // Render the lines that fit into the console's height
    for (int i = m_renderStartLine; 
         i < totalLines && (i - m_renderStartLine) < maxLines - 1; 
         i++) 
    {
        SDL_Rect lineDstRect = {
            m_transform.x,
            m_transform.y + (i - m_renderStartLine) * lineHeight,
            m_lines[i].getWidth(),
            lineHeight
        };
        m_lines[i].render(renderer, nullptr, &lineDstRect);
    }

    // Render the active line at the bottom
    SDL_Rect activeLineDstRect = {
        m_transform.x,
        m_transform.y + (maxLines - 1) * lineHeight,
        m_activeLine.getWidth(),
        lineHeight
    };
    m_activeLine.render(renderer, nullptr, &activeLineDstRect);
    
}

void application::Console::attachGDB(const std::shared_ptr<GDBController> &gdb)
{
    m_gdb = gdb;
    m_gdbAttached = true;
}

void application::Console::detachGDB()
{
    m_gdb.reset();
    m_gdbAttached = false;
}

std::string application::Console::getLastInput(size_t n)
{
    if (n == 0 || m_lines.empty()) return "";

    // Determine the start index to get the last n strings
    size_t start = m_lines.size() > n ? m_lines.size() - n : 0;

    std::ostringstream result;
    for (size_t i = start; i < m_lines.size(); ++i) {
        result << m_lines[i].getText().substr(2);
        if (i != m_lines.size() - 1) {
            result << "\n"; // Add space between strings
        }
    }

    return result.str();
}

void application::Console::printToConsole(const std::string &str)
{
    // Save the current user input to restore later
    std::string presentActiveText = m_activeLine.getText().substr(2);
    m_activeLine.clear();
    m_activeLine.appendText("> ", true);

    // Split the input string by newline characters
    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    bool isFirstLine = true;
    
    while ((pos = str.find('\n', prev)) != std::string::npos) {
        // Get the current line segment
        std::string line = str.substr(prev, pos - prev);
        
        // Add proper indentation for all lines except the first one
        if (!isFirstLine) {
            // Start with 2 spaces for new lines from '\n'
            m_activeLine.appendText("  ", true);
        }
        isFirstLine = false;
        
        // Process this line (might need to be wrapped)
        int addedUntil = m_activeLine.appendText(line);
        
        // Store the current line
        m_lines.push_back(m_activeLine);
        m_activeLine.clear();

        m_renderStartLine++;
        
        // If there's text that didn't fit in the current line, handle it
        if (addedUntil > 0 && addedUntil < static_cast<int>(line.length())) {
            std::string remainingText = line.substr(addedUntil);
            
            while (!remainingText.empty()) {
                // Add indentation for continuation lines
                std::string continuationLine = "  " + remainingText;
                
                // Try to append to a new active line
                int continuationAdded = m_activeLine.appendText(continuationLine);
                
                // Store this line
                m_lines.push_back(m_activeLine);
                m_activeLine.clear();

                m_renderStartLine++;
                
                // If everything fits or there was an error, we're done with this segment
                if (continuationAdded == 0 || continuationAdded == -1)
                    break;
                
                // The returned index is relative to the indented string
                int actualIndex = continuationAdded > 2 ? continuationAdded - 2 : 0;
                
                // Get the remaining text for the next iteration
                remainingText = remainingText.substr(actualIndex);
            }
        }
        
        // Move to the next line in the input string
        prev = pos + 1;
    }
    
    // Process the last part of the string (after the last newline or the entire string if no newlines)
    if (prev < str.length()) {
        std::string lastPart = str.substr(prev);
        
        // Add proper indentation if this isn't the first line
        if (!isFirstLine) {
            // Start with 2 spaces for new lines from '\n'
            m_activeLine.appendText("  ", true);
        }
        
        // Process this final segment similar to the existing logic
        int addedUntil = m_activeLine.appendText(lastPart);
        
        // If everything was added successfully or there was an error
        if (addedUntil == 0 || addedUntil == -1) {
            // Only add the final line if it's not empty
            if (!m_activeLine.getText().empty()) {
                m_lines.push_back(m_activeLine);
                m_activeLine.clear();

                m_renderStartLine++;
            }
        } else {
            // Store the current line
            m_lines.push_back(m_activeLine);
            m_activeLine.clear();

            m_renderStartLine++;
            
            // Process remaining text until everything fits
            std::string remainingText = lastPart.substr(addedUntil);
            
            while (!remainingText.empty()) {
                // Add indentation for continuation lines
                std::string line = "  " + remainingText;
                
                // Try to append to the new active line
                addedUntil = m_activeLine.appendText(line);
                
                // If everything fits, we're done
                if (addedUntil == 0 || addedUntil == -1)
                    break;
                
                // Otherwise, store this line and prepare for the next one
                m_lines.push_back(m_activeLine);
                m_activeLine.clear();

                m_renderStartLine++;
                
                // Adjust the index relative to the indented string
                int actualIndex = addedUntil > 2 ? addedUntil - 2 : 0;
                
                // Get the remaining text for the next iteration
                remainingText = remainingText.substr(actualIndex);
            }
            
            // Only add the final line if it's not empty
            if (!m_activeLine.getText().empty()) {
                m_lines.push_back(m_activeLine);
                m_activeLine.clear();

                m_renderStartLine++;
            }
        }
    }
    
    // Prepare a new command prompt with the previous user input
    m_activeLine.appendText("> ", true);
    m_activeLine.appendText(presentActiveText);
}

void application::Console::clearInputLine()
{
    m_activeLine.clear();
    m_activeLine.appendText("> ", true);
}

void application::Console::addDeltaTransform(int x, int y, int w, int h)
{
    Widget::addDeltaTransform(x, y, w, h);

    m_activeLine.addDeltaTransform(x, y, w, h);

    for (application::TextLine& tline : m_lines) {
        tline.addDeltaTransform(x, y, w, h);
    }
}

void application::Console::setWidth(int newW)
{
    Widget::setWidth(newW);

    m_activeLine.setWidth(newW);

    for (application::TextLine& tline : m_lines) {
        tline.setWidth(newW);
    }
}

bool application::Console::isMouseInsideTransform(vector2i mousePos)
{
    return (mousePos.x > m_transform.x && mousePos.x < m_transform.x + m_transform.w &&
            mousePos.y > m_transform.y && mousePos.y < m_transform.y + m_transform.h);
}
