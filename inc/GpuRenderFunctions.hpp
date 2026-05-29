#pragma once

#include <graphics/Renderer.hpp>

static bool IGNORE_VIEW_ZOOM = true;

inline glm::vec2 getMouseGlobalPosition(const graphics::Renderer& renderer, const glm::vec2& mouse_position)
{
    /*
    const auto& view = renderer.getView();
    const auto& zoom = renderer.getZoom();
    const auto& window_size = renderer.getStandardWindowSize();

    glm::vec2 mid_screen {window_size.x * 0.5f, window_size.y * 0.5f};

    glm::vec2 result{ view + mid_screen + (mouse_position - mid_screen) / zoom };
    return result;
*/
    return {};
}

inline void drawRectangle(graphics::Renderer& screen, float x, float y, float width, float height, graphics::RenderType render_type, graphics::Color color, bool ignore_view_zoom = false)
{
    if (ignore_view_zoom)
        screen.setRenderMode(graphics::RenderMode::UI);
    screen.drawRectangle(x, y, width, height, color, render_type);
    if (ignore_view_zoom)
        screen.setRenderMode(graphics::RenderMode::WORLD);
}

inline void drawScaledSprite(graphics::Renderer& screen, const graphics::Sprite& sprite, float x, float y, float width, float height, bool ignore_view_zoom = false, graphics::Color color = graphics::Color::WHITE)
{
    if (ignore_view_zoom)
        screen.setRenderMode(graphics::RenderMode::UI);
    screen.drawSprite(sprite, x, y, width, height, 0.0f, SDL_FLIP_NONE, color);
    if (ignore_view_zoom) 
        screen.setRenderMode(graphics::RenderMode::WORLD);
}

inline void drawRotatedSprite(graphics::Renderer& screen, const graphics::Sprite& sprite, float x, float y, float width, float height, float angle,
    SDL_FlipMode flip_mode, bool ignore_view_zoom = false, graphics::Color color = graphics::Color::WHITE)
{
    if (ignore_view_zoom) 
        screen.setRenderMode(graphics::RenderMode::UI);
    screen.drawSprite(sprite, x, y, width, height, angle, flip_mode, color);
    if (ignore_view_zoom) 
        screen.setRenderMode(graphics::RenderMode::WORLD);
}

inline void printText(graphics::Renderer& screen, const graphics::Text& text, float x, float y, float w, float h, bool ignore_view_zoom = false)
{
    if (ignore_view_zoom) 
        screen.setRenderMode(graphics::RenderMode::UI);
    screen.drawText(text, x, y);
    if (ignore_view_zoom) 
        screen.setRenderMode(graphics::RenderMode::WORLD);
}

inline void printTextScaled(graphics::Renderer& screen, const graphics::Text& text, float x, float y, float scale_x, float scale_y, bool ignore_view_zoom = false)
{
    if (ignore_view_zoom) 
        screen.setRenderMode(graphics::RenderMode::UI);
    //glm::vec2 scaled_size = text.getTextSize({scale_x, scale_y});
    screen.drawText(text, x, y);
    if (ignore_view_zoom)
        screen.setRenderMode(graphics::RenderMode::WORLD);
}
