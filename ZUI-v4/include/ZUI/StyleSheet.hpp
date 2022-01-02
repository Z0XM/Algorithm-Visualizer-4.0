#pragma once
#include <unordered_map>
#include <string>
#include <SFML/Graphics.hpp>
#include <ZUI/ZUIFrame.hpp>

namespace zui{
class StyleSheet {
public:
	
	StyleSheet();

	~StyleSheet();

	//void loadFromFile(const std::string& relativefilePath);
	//void loadFromString(const std::string& styleString);

	enum StyleType {
		FONT_SIZE, FILL_COLOR, OUT_COLOR, OUT_THICKNESS, S_FILL_COLOR, S_OUT_COLOR, S_OUT_THICKNESS, TEXT_FILL_COLOR, TEXT_OUT_COLOR, TEXT_OUT_THICKNESS, BAR_FILL_COLOR, S_BAR_FILL_COLOR, SIZE, FONT
	};
	
	void set(StyleType type, int value);
	void set(StyleType type, const sf::Color& value);
	void set(StyleType type, const sf::Vector2f& value);
	void set(StyleType type, sf::Font& value);

	void inheritFrom(StyleSheet& sheet);

	void applyTo(zui::Entity& entity);
	void applyTo(zui::Entity* entity);

	static std::unordered_map<std::string, StyleSheet> styleGroup;

private:
	union Style {
		int integer;
		sf::Color color = sf::Color(255, 255, 255);
		sf::Vector2f vector;
		sf::Font* font;
	};

	std::unordered_map<StyleType, Style> m_style;
};

template<typename T>
std::unique_ptr<T> createFromStyleSheet(StyleSheet& sheet)
{
	T* ptr = new T();
	sheet.applyTo(ptr);
	return std::unique_ptr<T>(ptr);
}

template<typename T>
std::unique_ptr<T> createFromStyleSheet(const std::string& className)
{
	T* ptr = new T();
	StyleSheet::styleGroup[className].applyTo(ptr);
	return std::unique_ptr<T>(ptr);
}

}