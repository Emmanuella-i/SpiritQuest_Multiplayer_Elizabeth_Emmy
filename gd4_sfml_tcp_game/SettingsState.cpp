//ET
#include "SettingsState.hpp"
#include "ResourceHolder.hpp"
#include "Utility.hpp"

SettingsState::SettingsState(StateStack& stack, Context context)
	: State(stack, context)
	, m_gui_container()
{
	m_background_sprite.setTexture(context.textures->Get(TextureID::kSettingsBackground));

	// posistion for player 1 and player 2 on the x axis of screen
	float p1X = 80.f;
	float p2X = 500.f;

	//Build key binding buttons and labels for player 1  on the Left
	AddButtonLabel(Action::kMoveUp, p1X,150.f, "P1 Move Up", context);
	AddButtonLabel(Action::kMoveDown, p1X, 200.f, "P1 Move Down", context);
	AddButtonLabel(Action::kMoveRight, p1X, 250.f, "P1 Move Right", context);
	AddButtonLabel(Action::kMoveLeft, p1X, 300.f, "P1 Move Left", context);
	AddButtonLabel(Action::kBulletFire, p1X, 350.f, "P1 Fire", context);
	AddButtonLabel(Action::kMissileFire, p1X, 400.f, "P1 Missile Fire", context);

	//Build key binding buttons and labels for player 2  on the Right
	AddButtonLabel(Action::kMoveUpP2 , p2X, 150.f, " P2 Move Up", context);
	AddButtonLabel(Action::kMoveDownP2 , p2X, 200.f, "P2 Move Down", context);
	AddButtonLabel(Action::kMoveRightP2 , p2X, 250.f, "P2 Move Right", context);
	AddButtonLabel(Action::kMoveLeftP2 , p2X, 300.f, "P2 Move Left", context);
	AddButtonLabel(Action::kBulletFireP2 , p2X, 350.f, "P2 Fire", context);
	AddButtonLabel(Action::kMissileFireP2 , p2X, 400.f, "P2 Missile Fire", context);

	UpdateLabels();

	auto back_button = std::make_shared<gui::Button>(context);
	back_button->setPosition(80.f, 475.f);
	back_button->SetText("Back");
	back_button->SetCallback(std::bind(&SettingsState::RequestStackPop, this));
	m_gui_container.Pack(back_button);
}

void SettingsState::Draw()
{
	sf::RenderWindow& window = *GetContext().window;
	window.draw(m_background_sprite);
	window.draw(m_gui_container);
}

bool SettingsState::Update(sf::Time dt)
{
	return true;
}

bool SettingsState::HandleEvent(const sf::Event& event)
{
	bool is_key_binding = false;

	//Iterate through all of the key binding buttons to see if they are being presssed, waiting for the user to enter a key
	for (std::size_t action = 0; action < static_cast<int>(Action::kActionCount); ++action)
	{
		if (m_binding_buttons[action]->IsActive())
		{
			is_key_binding = true;
			if (event.type == sf::Event::KeyReleased)
			{
				GetContext().player->AssignKey(static_cast<Action>(action), event.key.code);
				m_binding_buttons[action]->Deactivate();
			}
			break;
		}
	}

	//If pressed button changed key bindings, then update the labels
	if (is_key_binding)
	{
		UpdateLabels();
	}
	else
	{
		m_gui_container.HandleEvent(event);
	}
	return false;
}

void SettingsState::UpdateLabels()
{
	Player& player = *GetContext().player;
	for (std::size_t i = 0; i < static_cast<int>(Action::kActionCount); ++i)
	{
		sf::Keyboard::Key key = player.GetAssignedKey(static_cast<Action>(i));
		m_binding_labels[i]->SetText(Utility::toString(key));
	}
}
//ET:adding an x float to change x position
void SettingsState::AddButtonLabel(Action action, float x, float y, const std::string& text, Context context)
{
	m_binding_buttons[static_cast<int>(action)] = std::make_shared<gui::Button>(context);
	m_binding_buttons[static_cast<int>(action)]->setPosition(x, y);// changed here
	m_binding_buttons[static_cast<int>(action)]->SetText(text);
	m_binding_buttons[static_cast<int>(action)]->SetToggle(true);

	m_binding_labels[static_cast<int>(action)] = std::make_shared<gui::Label>("", *context.fonts);
	m_binding_labels[static_cast<int>(action)]->setPosition(x + 300.f, y + 15.f);// chnaged here

	m_gui_container.Pack(m_binding_buttons[static_cast<int>(action)]);
	m_gui_container.Pack(m_binding_labels[static_cast<int>(action)]);
}
