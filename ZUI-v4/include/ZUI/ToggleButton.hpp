//#pragma once
//
//#include "ZUIFrame.hpp"
//#include "TextButton.hpp"
//
//namespace zui {
//class ToggleButton : public TextButton {
//public:
//	ToggleButton(const sf::Vector2f& size = sf::Vector2f(0, 0));
//
//	~ToggleButton();
//
//	//void setFillColor(const sf::Color& color) = delete;
//	//void setSelectionFillColor(const sf::Color& color) = delete;
//	void setString(const std::string& string) = delete;
//	void clearString() = delete;
//	void alignText(TextAlign textAlign = TextAlign::CENTER) = delete;
//
//	void setToggle(bool toggle_state);
//
//	bool getToggle() const;
//
//	////////////////////////////////////////////////////////////
//	/// \brief Draws the object to the target applying its states
//	/// 
//	/// \param target -> Render target to draw to
//	/// \param states -> Current render states
//	/// 
//	////////////////////////////////////////////////////////////
//	virtual void draw(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const override;
//
//private:
//	bool m_toggle_state;		/// < toggle state of button (True / False)
//};
//}