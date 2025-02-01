#include "DataTables.hpp"
#include "CharacterType.hpp"
#include "ProjectileType.hpp"
#include "PickupType.hpp"
#include "Character.hpp"
#include "ParticleType.hpp"
#include "Deteriorate.hpp"

std::vector<CharacterData> InitializeCharacterData()
{
    std::vector<CharacterData> data(static_cast<int>(CharacterType::kCharacterCount));

    data[static_cast<int>(CharacterType::kGhost)].m_hitpoints = 100;
    data[static_cast<int>(CharacterType::kGhost)].m_speed = 200.f;
    data[static_cast<int>(CharacterType::kGhost)].m_fire_interval = sf::seconds(1);
    data[static_cast<int>(CharacterType::kGhost)].m_texture = TextureID::kGhost;
    data[static_cast<int>(CharacterType::kGhost)].m_texture_rect = sf::IntRect(0, 0, 48, 64);
    data[static_cast<int>(CharacterType::kGhost)].m_has_roll_animation = true;

    data[static_cast<int>(CharacterType::kReaper)].m_hitpoints = 20;
    data[static_cast<int>(CharacterType::kReaper)].m_speed = 80.f;
    data[static_cast<int>(CharacterType::kReaper)].m_fire_interval = sf::Time::Zero;
    data[static_cast<int>(CharacterType::kReaper)].m_texture = TextureID::kReaper;
    data[static_cast<int>(CharacterType::kReaper)].m_texture_rect = sf::IntRect(144, 0, 84, 64);
    data[static_cast<int>(CharacterType::kReaper)].m_has_roll_animation = false;

    //AI for Raptor
    data[static_cast<int>(CharacterType::kReaper)].m_directions.emplace_back(Direction(+45.f, 80.f));
    data[static_cast<int>(CharacterType::kReaper)].m_directions.emplace_back(Direction(-45.f, 160.f));
    data[static_cast<int>(CharacterType::kReaper)].m_directions.emplace_back(Direction(+45.f, 80.f));


    return data;
}

std::vector<ProjectileData> InitializeProjectileData()
{
    std::vector<ProjectileData> data(static_cast<int>(ProjectileType::kProjectileCount));
    data[static_cast<int>(ProjectileType::kAlliedBullet)].m_damage = 10;
    data[static_cast<int>(ProjectileType::kAlliedBullet)].m_speed = 300;
    data[static_cast<int>(ProjectileType::kAlliedBullet)].m_texture = TextureID::kEntities;
    data[static_cast<int>(ProjectileType::kAlliedBullet)].m_texture_rect = sf::IntRect(175, 64, 3, 14);

    data[static_cast<int>(ProjectileType::kEnemyBullet)].m_damage = 10;
    data[static_cast<int>(ProjectileType::kEnemyBullet)].m_speed = 300;
    data[static_cast<int>(ProjectileType::kEnemyBullet)].m_texture = TextureID::kEntities;
    data[static_cast<int>(ProjectileType::kEnemyBullet)].m_texture_rect = sf::IntRect(175, 64, 3, 14);


    data[static_cast<int>(ProjectileType::kMissile)].m_damage = 200;
    data[static_cast<int>(ProjectileType::kMissile)].m_speed = 150;
    data[static_cast<int>(ProjectileType::kMissile)].m_texture = TextureID::kEntities;
    data[static_cast<int>(ProjectileType::kMissile)].m_texture_rect = sf::IntRect(160, 64, 15, 32);

    return data;
}

std::vector<PickupData> InitializePickupData()
{
    std::vector<PickupData> data(static_cast<int>(PickupType::kPickupCount));
    data[static_cast<int>(PickupType::kHealthRefill)].m_texture = TextureID::kEntities;
    data[static_cast<int>(PickupType::kHealthRefill)].m_texture_rect = sf::IntRect(0, 64, 40, 40);
    data[static_cast<int>(PickupType::kHealthRefill)].m_action = [](Character& a)
        {
            a.Repair(25);
        };

    data[static_cast<int>(PickupType::kMissileRefill)].m_texture = TextureID::kEntities;
    data[static_cast<int>(PickupType::kMissileRefill)].m_texture_rect = sf::IntRect(40, 64, 40, 40);
    
    data[static_cast<int>(PickupType::kMissileRefill)].m_action = std::bind(&Character::CollectMissile, std::placeholders::_1, 3);

    data[static_cast<int>(PickupType::kFireSpread)].m_texture = TextureID::kEntities;
    data[static_cast<int>(PickupType::kFireSpread)].m_texture_rect = sf::IntRect(80, 64, 40, 40);
    data[static_cast<int>(PickupType::kFireSpread)].m_action = std::bind(&Character::IncreaseFireSpread, std::placeholders::_1);

    data[static_cast<int>(PickupType::kFireRate)].m_texture = TextureID::kEntities;
    data[static_cast<int>(PickupType::kFireRate)].m_texture_rect = sf::IntRect(120, 64, 40, 40);
    data[static_cast<int>(PickupType::kFireRate)].m_action = std::bind(&Character::IncreaseFireRate, std::placeholders::_1);
    
    return data;
}

std::vector<ParticleData> InitializeParticleData()
{
    std::vector<ParticleData> data(static_cast<int>(ParticleType::kParticleCount));

    data[static_cast<int>(ParticleType::kPropellant)].m_color = sf::Color(255, 255, 50);
    data[static_cast<int>(ParticleType::kPropellant)].m_lifetime = sf::seconds(0.5f);

    data[static_cast<int>(ParticleType::kSmoke)].m_color = sf::Color(50, 50, 50);
    data[static_cast<int>(ParticleType::kSmoke)].m_lifetime = sf::seconds(2.5f);

    return data;
}

std::vector<DeteriorateData> InitializeDeteriorateData()
{
    std::vector<DeteriorateData> data;
    
    DeteriorateData deteriorateData;
    deteriorateData.m_damage = 5; 

    
    data.push_back(deteriorateData);

    return data;
}
