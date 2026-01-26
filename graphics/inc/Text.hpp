#pragma once

#include <string>
#include "Font.hpp"

class Renderer;
struct SDL_Texture;
class Surface;

class  Text
{
public:
	Text(const Font* font, const Renderer& renderer, const std::string& text, SDL_Color color = SDL_Color{0,0,0,255});
	~Text();

	Text(const Text& other) = delete;
	Text(Text&& other) noexcept = delete;
	Text& operator=(const Text& other) = delete;
	Text& operator=(Text&& other) noexcept = delete;

	//Getters
	SDL_Texture* getTexture() const;

	//Setters
	void setFont(const Font* font);
	void setColor(SDL_Color color);
	void setText(const std::string& text);

	void updateText(const Renderer& renderer);
private:
	void loadTexture(const Renderer& renderer, const Surface& surface);
	void generateTextTexture(const Renderer& renderer);

	std::string text;
	SDL_Texture* texture;
	const Font* font;
	SDL_Color color;

	bool is_dirty = false;
};
