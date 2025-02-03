//E.I D00244320, E.T d00245315
#pragma once
#include "State.hpp"
#include "World.hpp"

class GameState : public State
{
public:
	GameState(StateStack& stack, Context context);
	virtual void Draw() override;
	virtual bool Update(sf::Time dt) override;
	virtual bool HandleEvent(const sf::Event& event) override;

private:
	World m_world;
	Player& m_player;
	Player& m_player2;
};

