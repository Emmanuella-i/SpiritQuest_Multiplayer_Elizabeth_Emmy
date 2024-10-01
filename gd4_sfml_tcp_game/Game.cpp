#include "Game.hpp"
#include "Constants.hpp"
#include "Utility.hpp"

const sf::Time Game::kTimePerFrame = sf::seconds(1.f / 60.f);

Game::Game()
	: m_window(sf::VideoMode(640, 480), "SFML Game"), m_world(m_window)
{
}

void Game::Run()
{
	sf::Clock clock;
	sf::Time time_since_last_update = sf::Time::Zero;
	while (m_window.isOpen())
	{
		time_since_last_update += clock.restart();
		if (time_since_last_update.asSeconds() > TIME_PER_FRAME)
		{
			time_since_last_update -= sf::seconds(TIME_PER_FRAME);
			ProcessEvents();
			Update(sf::seconds(TIME_PER_FRAME));
		}	
		Render();

	}
}

void Game::ProcessEvents()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		switch (event.type)
		{
			case sf::Event::Closed:
			{
				m_window.close();
				break;
			}
		}
	}
}

void Game::Update(sf::Time delta_time)
{
	/*sf::Vector2f movement(0.f, 0.f);
	if (m_is_moving_up)
		movement.y -= 1;
	if (m_is_moving_down)
		movement.y += 1;
	if (m_is_moving_left)
		movement.x -= 1;
	if (m_is_moving_right)
		movement.x += 1;
	m_player.move(Utility::Normalise(movement) * PLAYER_SPEED * delta_time.asSeconds());*/
	m_world.Update(delta_time);

}

void Game::Render()
{
	m_window.clear();
	m_world.Draw();
	m_window.display();
}

//void Game::HandlePlayerInput(sf::Keyboard::Key key, bool is_pressed)
//{
//	if (key == sf::Keyboard::W)
//	{
//		m_is_moving_up = is_pressed;
//	}
//	if (key == sf::Keyboard::S)
//	{
//		m_is_moving_down = is_pressed;
//	}
//	if (key == sf::Keyboard::A)
//	{
//		m_is_moving_left = is_pressed;
//	}
//	if (key == sf::Keyboard::D)
//	{
//		m_is_moving_right = is_pressed;
//	}
//}
