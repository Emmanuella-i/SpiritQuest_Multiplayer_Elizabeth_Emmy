#include "Player.hpp"
#include "ReceiverCategories.hpp"
#include "Character.hpp"
/*ET mostly for multiplayer programming*/
struct CharacterMover
{
    CharacterMover(float vx, float vy) :velocity(vx, vy)
    {}
    void operator()(Character& character, sf::Time) const
    {
        character.Accelerate(velocity);
    }

    sf::Vector2f velocity;
};

Player::Player() : m_current_mission_status(MissionStatus::kMissionRunning)
{
  //  const bool isGhost = (id == ID::Player1);

    // PLAYER 1(Ghost) Controls (WASD +Space/M)
   // if (isGhost) {
        //Set initial key bindings
        m_key_binding[sf::Keyboard::A] = Action::kMoveLeft;
        m_key_binding[sf::Keyboard::D] = Action::kMoveRight;
        m_key_binding[sf::Keyboard::W] = Action::kMoveUp;
        m_key_binding[sf::Keyboard::S] = Action::kMoveDown;
        m_key_binding[sf::Keyboard::M] = Action::kMissileFire;
        m_key_binding[sf::Keyboard::Space] = Action::kBulletFire;

   // }
  ///  else {
        // ET:PLAYER 2(Reaper) Controls (Arrow Keys +Enter/Shift)
        m_key_binding[sf::Keyboard::Left] = Action::kMoveLeftP2;
        m_key_binding[sf::Keyboard::Right] = Action::kMoveRightP2;
        m_key_binding[sf::Keyboard::Up] = Action::kMoveUpP2;
        m_key_binding[sf::Keyboard::Down] = Action::kMoveDownP2;
        m_key_binding[sf::Keyboard::RShift] = Action::kMissileFireP2;
        m_key_binding[sf::Keyboard::Enter] = Action::kBulletFireP2;

    //}
    // unsigned int 32bit .. actions assigned to correct player category 
   // unsigned int category = (isGhost) ? static_cast<unsigned int> (ReceiverCategories::kPlayer1):static_cast <unsigned int>(ReceiverCategories::kPlayer2);
    //Set initial action bindings
    InitialiseActions();

    
//ET individually sets each command to player 

    m_action_binding[Action::kMoveLeft].category = static_cast<unsigned int>(ReceiverCategories::kPlayer1);
    m_action_binding[Action::kMoveRight].category = static_cast<unsigned int>(ReceiverCategories::kPlayer1);
    m_action_binding[Action::kMoveUp].category = static_cast<unsigned int>(ReceiverCategories::kPlayer1);
    m_action_binding[Action::kMoveDown].category = static_cast<unsigned int>(ReceiverCategories::kPlayer1);
    m_action_binding[Action::kMissileFire].category = static_cast<unsigned int>(ReceiverCategories::kPlayer1);
    m_action_binding[Action::kBulletFire].category = static_cast<unsigned int>(ReceiverCategories::kPlayer1);

    m_action_binding[Action::kMoveLeftP2].category = static_cast<unsigned int>(ReceiverCategories::kPlayer2);
    m_action_binding[Action::kMoveRightP2].category = static_cast<unsigned int>(ReceiverCategories::kPlayer2);
    m_action_binding[Action::kMoveUpP2].category = static_cast<unsigned int>(ReceiverCategories::kPlayer2);
    m_action_binding[Action::kMoveDownP2].category = static_cast<unsigned int>(ReceiverCategories::kPlayer2);
    m_action_binding[Action::kMissileFireP2].category = static_cast<unsigned int>(ReceiverCategories::kPlayer2);
    m_action_binding[Action::kBulletFireP2].category = static_cast<unsigned int>(ReceiverCategories::kPlayer2);


    //Assign all categories to a player's aircraft
    //for (auto& pair : m_action_binding)
   // {
//pair.second.category = static_cast<unsigned int>(ReceiverCategories::kPlayer1);
   // }
}

void Player::HandleEvent(const sf::Event& event, CommandQueue& command_queue)
{
    if (event.type == sf::Event::KeyPressed)
    {
        auto found = m_key_binding.find(event.key.code);
        if (found != m_key_binding.end() && !IsRealTimeAction(found->second))
        {
            command_queue.Push(m_action_binding[found->second]);
        }
    }
}

void Player::HandleRealTimeInput(CommandQueue& command_queue)
{
    
    //Check if any of the key bindings are pressed
    for (auto pair : m_key_binding)
    {
        if (sf::Keyboard::isKeyPressed(pair.first) && IsRealTimeAction(pair.second))
        {
            command_queue.Push(m_action_binding[pair.second]);
        }
    }
}

void Player::AssignKey(Action action, sf::Keyboard::Key key)
{
    //Remove keys that are currently bound to the action
    for (auto itr = m_key_binding.begin(); itr != m_key_binding.end();)
    {
        if (itr->second == action)
        {
            m_key_binding.erase(itr++);
        }
        else
        {
            ++itr;
        }
    }
    m_key_binding[key] = action;
}

sf::Keyboard::Key Player::GetAssignedKey(Action action) const
{
    for (auto pair : m_key_binding)
    {
        if (pair.second == action)
        {
            return pair.first;
        }
    }
    return sf::Keyboard::Unknown;
}

void Player::SetMissionStatus(MissionStatus status)
{
    m_current_mission_status = status;
}

MissionStatus Player::GetMissionStatus() const
{
    return m_current_mission_status;
}

void Player::InitialiseActions()
{
    const float kPlayerSpeed = 200.f;
    //ET:as before assigning actions for player 1
   // bool isGhost = (m_id == ID::Player1);
    //P1 Ghost 
    //if (isGhost){
    m_action_binding[Action::kMoveRight].category = static_cast<unsigned int>(ReceiverCategories::kPlayer2);
    m_action_binding[Action::kMoveRight].category = static_cast<unsigned int>(ReceiverCategories::kPlayer1);

    m_action_binding[Action::kMoveLeft].action = DerivedAction<Character>(CharacterMover(-kPlayerSpeed, 0.f));
    m_action_binding[Action::kMoveRight].action = DerivedAction<Character>(CharacterMover(kPlayerSpeed, 0.f));
    m_action_binding[Action::kMoveUp].action = DerivedAction<Character>(CharacterMover(0.f, -kPlayerSpeed));
    m_action_binding[Action::kMoveDown].action = DerivedAction<Character>(CharacterMover(0.f, kPlayerSpeed));
        m_action_binding[Action::kBulletFire].action = DerivedAction<Character>([](Character& a, sf::Time dt)
            {
                a.Fire();
            }
        );

        m_action_binding[Action::kMissileFire].action = DerivedAction<Character>([](Character& a, sf::Time dt)
            {
                a.LaunchMissile();
            }
        );
   // }
    //ET:Player2 Reaper
   // else {
        m_action_binding[Action::kMoveLeftP2].action = DerivedAction<Character>(CharacterMover(-kPlayerSpeed, 0.f));
        m_action_binding[Action::kMoveRightP2].action = DerivedAction<Character>(CharacterMover(kPlayerSpeed, 0.f));
        m_action_binding[Action::kMoveUpP2].action = DerivedAction<Character>(CharacterMover(0.f, -kPlayerSpeed));
        m_action_binding[Action::kMoveDownP2].action = DerivedAction<Character>(CharacterMover(0.f, kPlayerSpeed));
        m_action_binding[Action::kBulletFireP2].action = DerivedAction<Character>([](Character& a, sf::Time dt)
            {
                a.Fire();
            }
        );

        m_action_binding[Action::kMissileFireP2].action = DerivedAction<Character>([](Character& a, sf::Time dt)
            {
                a.LaunchMissile();
            }
        );

    }
//}

bool Player::IsRealTimeAction(Action action)
{
    switch (action)
    {
    case Action::kMoveLeft:
    case Action::kMoveRight:
    case Action::kMoveDown:
    case Action::kMoveUp:
    case Action::kBulletFire:
        //ET changes for player 2 below
    case Action::kMoveLeftP2:
    case Action::kMoveRightP2:
    case Action::kMoveDownP2:
    case Action::kMoveUpP2:
    case Action::kBulletFireP2:

        return true;
    default:
        return false;
    }
}
