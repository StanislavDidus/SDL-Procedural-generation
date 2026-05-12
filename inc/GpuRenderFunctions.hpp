#pragma once

#include <graphics/GpuRenderer.hpp>

static bool IGNORE_VIEW_ZOOM = true;

inline glm::vec2 getMouseGlobalPosition(const graphics::GpuRenderer& renderer, const glm::vec2& mouse_position)
{
    const auto& view = renderer.getView();
    const auto& zoom = renderer.getZoom();
    const auto& window_size = renderer.getStandardWindowSize();

    glm::vec2 mid_screen {window_size.x * 0.5f, window_size.y * 0.5f};

    glm::vec2 result{ view + mid_screen + (mouse_position - mid_screen) / zoom };
    return result;
}

inline void drawRectangle(graphics::GpuRenderer& screen, float x, float y, float width, float height, graphics::RenderType render_type, graphics::Color color, bool ignore_view_zoom = false)
{
    if (ignore_view_zoom)
        screen.setRenderMode(graphics::RenderMode::UI);
    screen.renderRectangle(x, y, width, height, render_type, color);
    if (ignore_view_zoom)
        screen.setRenderMode(graphics::RenderMode::WORLD);
}

inline void drawScaledSprite(graphics::GpuRenderer& screen, const graphics::Sprite& sprite, float x, float y, float width, float height, bool ignore_view_zoom = false, graphics::Color color = graphics::Color::WHITE)
{
    if (ignore_view_zoom)
        screen.setRenderMode(graphics::RenderMode::UI);
    screen.renderSprite(sprite, x, y, width, height, 0.0f, SDL_FLIP_NONE, color);
    if (ignore_view_zoom) 
        screen.setRenderMode(graphics::RenderMode::WORLD);
}

inline void drawRotatedSprite(graphics::GpuRenderer& screen, const graphics::Sprite& sprite, float x, float y, float width, float height, float angle,
    SDL_FlipMode flip_mode, bool ignore_view_zoom = false, graphics::Color color = graphics::Color::WHITE)
{
    if (ignore_view_zoom) 
        screen.setRenderMode(graphics::RenderMode::UI);
    screen.renderSprite(sprite, x, y, width, height, angle, flip_mode, color);
    if (ignore_view_zoom) 
        screen.setRenderMode(graphics::RenderMode::WORLD);
}

inline void printText(graphics::GpuRenderer& screen, const graphics::Text& text, float x, float y, float w, float h, bool ignore_view_zoom = false)
{
    if (ignore_view_zoom) 
        screen.setRenderMode(graphics::RenderMode::UI);
    screen.renderText(text, x, y, w, h);
    if (ignore_view_zoom) 
        screen.setRenderMode(graphics::RenderMode::WORLD);
}

inline void printTextScaled(graphics::GpuRenderer& screen, const graphics::Text& text, float x, float y, float scale_x, float scale_y, bool ignore_view_zoom = false)
{
    if (ignore_view_zoom) 
        screen.setRenderMode(graphics::RenderMode::UI);
    glm::vec2 scaled_size = text.getTextSize({scale_x, scale_y});
    screen.renderText(text, x, y, scaled_size.x, scaled_size.y);
    if (ignore_view_zoom)
        screen.setRenderMode(graphics::RenderMode::WORLD);
}
