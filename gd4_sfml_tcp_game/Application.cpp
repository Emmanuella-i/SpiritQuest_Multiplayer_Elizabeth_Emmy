#include "Application.hpp"
#include "GameState.hpp"
#include "TitleState.hpp"
#include "MenuState.hpp"
#include "PauseState.hpp"
#include "SettingsState.hpp"
#include "GameOverState.hpp"

const sf::Time Application::kTimePerFrame = sf::seconds(1.f/60.f);

Application::Application() : m_window(sf::VideoMode(1024, 576), "States", sf::Style::Close)
	, m_stack(State::Context(m_window, m_textures, m_fonts, m_player, m_player2, m_music, m_sound))
{
	m_window.setKeyRepeatEnabled(false);
	m_fonts.Load(Font::kMain, "Media/Fonts/Super-Mario-World.ttf");
	m_textures.Load(TextureID::kTitleScreen, "Media/Textures/SpiritQuestBackground.png");
	m_textures.Load(TextureID::kButtonNormal, "Media/Textures/32x32 Blue Bubble Buttons/Blank Buttons/Blue_Button_01.png");
	m_textures.Load(TextureID::kButtonSelected, "Media/Textures/32x32 Blue Bubble Buttons/Blank Buttons/Blue_Button_02.png");
	m_textures.Load(TextureID::kButtonActivated, "Media/Textures/32x32 Blue Bubble Buttons/Blank Buttons/Blue_Button_03.png");
	m_textures.Load(TextureID::kButtons, "Media/Textures/BlueButtons.png");
	m_textures.Load(TextureID::kSettingsBackground, "Media/Textures/Clouds 3/1.png");//ET
	m_textures.Load(TextureID::kSettingsHeader, "Media/Textures/32x32 Blue Bubble Buttons/Buttons With Text/Text_Settings_Button_021.png");
	RegisterStates();
	m_stack.PushState(StateID::kTitle);
}




void Application::Run()
{
	sf::Clock clock;
	sf::Time time_since_last_update = sf::Time::Zero;
	while (m_window.isOpen())
	{
		time_since_last_update += clock.restart();
		while(time_since_last_update > kTimePerFrame)
		{
			time_since_last_update -= kTimePerFrame;
			ProcessInput();
			Update(kTimePerFrame);

			if (m_stack.IsEmpty())
			{
				m_window.close();
			}
		}
		Render();

	}
}

void Application::ProcessInput()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		m_stack.HandleEvent(event);

		if (event.type == sf::Event::Closed)
		{
			m_window.close();
		}
	}
}

void Application::Update(sf::Time dt)
{
	m_stack.Update(dt);
}

void Application::Render()
{
	m_window.clear();
	m_stack.Draw();
	m_window.display();
}

void Application::RegisterStates()
{
	m_stack.RegisterState<TitleState>(StateID::kTitle);
	m_stack.RegisterState<MenuState>(StateID::kMenu);
	m_stack.RegisterState<GameState>(StateID::kGame);
	m_stack.RegisterState<PauseState>(StateID::kPause);
	m_stack.RegisterState<SettingsState>(StateID::kSettings);
	m_stack.RegisterState<GameOverState>(StateID::kGameOver);
}
