//E.I D00244320, E.T d00245315
#pragma once
#include "State.hpp"
#include "Player.hpp"
#include "Container.hpp"
#include "Button.hpp"
#include "Label.hpp"
#include <SFML/Graphics/Sprite.hpp>

#include <array>


class SettingsState : public State
{
public:
	SettingsState(StateStack& stack, Context context);
	virtual void Draw() override;
	virtual bool Update(sf::Time dt) override;
	virtual bool HandleEvent(const sf::Event& event);

private:
	void UpdateLabels();
	// ET: added x variable 
	void AddButtonLabel(Action action, float x, float y, const std::string& text, Context context);

private:
	sf::Sprite m_background_sprite;
	sf::Sprite m_settings_header;// ET: header added for settings 
	gui::Container m_gui_container;
	std::array<gui::Button::Ptr, static_cast<int>(Action::kActionCount)> m_binding_buttons;
	std::array<gui::Label::Ptr, static_cast<int>(Action::kActionCount)> m_binding_labels;
};

