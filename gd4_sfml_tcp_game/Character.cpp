#include "Character.hpp"
#include "TextureID.hpp"
#include "ResourceHolder.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include "DataTables.hpp"
#include "Projectile.hpp"
#include "PickupType.hpp"
#include "Pickup.hpp"
#include "SoundNode.hpp"

namespace
{
	const std::vector<CharacterData> Table = InitializeCharacterData();
}

TextureID ToTextureID(CharacterType type)
{
	switch (type)
	{
	case CharacterType::kGhost:
		return TextureID::kGhost;
		break;
	case CharacterType::kReaper:
		return TextureID::kReaper;
		break;
	}
	return TextureID::kGhost;
}

Character::Character(CharacterType type, const TextureHolder& textures, const FontHolder& fonts)  
	: Entity(Table[static_cast<int>(type)].m_hitpoints)
	, m_type(type)
	, m_sprite(textures.Get(Table[static_cast<int>(type)].m_texture), Table[static_cast<int>(type)].m_texture_rect)
	, m_explosion(textures.Get(TextureID::kExplosion))
	, m_spirit_energy_display(nullptr)
	, m_missile_display(nullptr)
	, m_distance_travelled(0.f)
	, m_directions_index(0)
	, m_fire_rate(1)
	, m_spread_level(1)
	, m_is_firing(false)
	, m_is_launching_missile(false)
	, m_fire_countdown(sf::Time::Zero)
	, m_missile_ammo(2)
	, m_is_marked_for_removal(false)
	, m_show_explosion(true)
	, m_spawned_pickup(false)
	, m_played_explosion_sound(false)

{
	m_explosion.SetFrameSize(sf::Vector2i(256, 256));
	m_explosion.SetNumFrames(16);
	m_explosion.SetDuration(sf::seconds(1));
	Utility::CentreOrigin(m_sprite);
	Utility::CentreOrigin(m_explosion);

	m_fire_command.category = static_cast<int>(ReceiverCategories::kScene);
	m_fire_command.action = [this, &textures](SceneNode& node, sf::Time dt)
		{
			CreateBullet(node, textures);
		};

	m_missile_command.category = static_cast<int>(ReceiverCategories::kScene);
	m_missile_command.action = [this, &textures](SceneNode& node, sf::Time dt)
		{
			CreateProjectile(node, ProjectileType::kMissile, 0.f, 0.5f, textures);
		};

	m_drop_pickup_command.category = static_cast<int>(ReceiverCategories::kScene);
	m_drop_pickup_command.action = [this, &textures](SceneNode& node, sf::Time dt)
		{
			CreatePickup(node, textures);
		};

	std::string* spirit_energy = new std::string("");
	std::unique_ptr<TextNode> spirit_energy_display(new TextNode(fonts, *spirit_energy));
	m_spirit_energy_display = spirit_energy_display.get();
	AttachChild(std::move(spirit_energy_display));

	if (Character::GetCategory() == static_cast<int>(ReceiverCategories::kPlayerAircraft))
	{
		std::string* missile_ammo = new std::string("");
		std::unique_ptr<TextNode> missile_display(new TextNode(fonts, *missile_ammo));
		m_missile_display = missile_display.get();
		AttachChild(std::move(missile_display));
	}

	UpdateTexts();
}

unsigned int Character::GetCategory() const
{
	if (IsAllied())
	{
		return static_cast<unsigned int>(ReceiverCategories::kPlayerAircraft);
	}
	return static_cast<unsigned int>(ReceiverCategories::kEnemyAircraft);

}

void Character::IncreaseFireRate()
{
	if (m_fire_rate < 5)
	{
		++m_fire_rate;
	}
}

void Character::IncreaseFireSpread()
{
	if (m_spread_level < 3)
	{
		++m_spread_level;
	}
}

void Character::CollectMissile(unsigned int count)
{
	m_missile_ammo += count;
}

void Character::UpdateTexts()
{
	m_spirit_energy_display->SetString(std::to_string(GetHitPoints()) + "SPIRIT ENERGY");
	m_spirit_energy_display->setPosition(0.f, 50.f);
	m_spirit_energy_display->setRotation(-getRotation());

	if (m_missile_display)
	{
		m_missile_display->setPosition(0.f, 70.f);
		if (m_missile_ammo == 0)
		{
			m_missile_display->SetString("");
		}
		else
		{
			m_missile_display->SetString("M: " + std::to_string(m_missile_ammo));
		}
	}
}

void Character::UpdateMovementPattern(sf::Time dt)
{
	//Enemy AI
	const std::vector<Direction>& directions = Table[static_cast<int>(m_type)].m_directions;
	if (!directions.empty())
	{
		//Move along the current direction, then change direction
		if (m_distance_travelled > directions[m_directions_index].m_distance)
		{
			m_directions_index = (m_directions_index + 1) % directions.size();
			m_distance_travelled = 0.f;
		}

		//Compute velocity
		//Add 90 to move down the screen, 0 is right

		double radians = Utility::ToRadians(directions[m_directions_index].m_angle + 90.f);
		float vx = GetMaxSpeed() * std::cos(radians);
		float vy = GetMaxSpeed() * std::sin(radians);

		SetVelocity(vx, vy);
		m_distance_travelled += GetMaxSpeed() * dt.asSeconds();
	}
}

float Character::GetMaxSpeed() const
{
	return Table[static_cast<int>(m_type)].m_speed;
}

void Character::Fire()
{
	if (Table[static_cast<int>(m_type)].m_fire_interval != sf::Time::Zero)
	{
		m_is_firing = true;
	}
}


void Character::LaunchMissile()
{
	if (m_missile_ammo > 0)
	{
		m_is_launching_missile = true;
		--m_missile_ammo;
	}
}

void Character::CreateBullet(SceneNode& node, const TextureHolder& textures) const
{
	ProjectileType type = IsAllied() ? ProjectileType::kAlliedBullet : ProjectileType::kEnemyBullet;
	switch (m_spread_level)
	{
	case 1:
		CreateProjectile(node, type, 0.0f, 0.5f, textures);
		break;
	case 2:
		CreateProjectile(node, type, -0.5f, 0.5f, textures);
		CreateProjectile(node, type, 0.5f, 0.5f, textures);
		break;
	case 3:
		CreateProjectile(node, type, 0.0f, 0.5f, textures);
		CreateProjectile(node, type, -0.5f, 0.5f, textures);
		CreateProjectile(node, type, 0.5f, 0.5f, textures);
		break;
	}
	
}

void Character::CreateProjectile(SceneNode& node, ProjectileType type, float x_offset, float y_offset, const TextureHolder& textures) const
{
	std::unique_ptr<Projectile> projectile(new Projectile(type, textures));
	sf::Vector2f offset(x_offset * m_sprite.getGlobalBounds().width, y_offset * m_sprite.getGlobalBounds().height);
	sf::Vector2f velocity(0, projectile->GetMaxSpeed());

	float sign = IsAllied() ? -1.f : 1.f;
	projectile->setPosition(GetWorldPosition() + offset * sign);
	projectile->SetVelocity(velocity* sign);
	node.AttachChild(std::move(projectile));
}

sf::FloatRect Character::GetBoundingRect() const
{
	return GetWorldTransform().transformRect(m_sprite.getGlobalBounds());
}

bool Character::IsMarkedForRemoval() const
{
	return IsDestroyed() && (m_explosion.IsFinished() || !m_show_explosion);
}

void Character::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (IsDestroyed() && m_show_explosion)
	{
		target.draw(m_explosion, states);
	}
	else
	{
		target.draw(m_sprite, states);
	}
}

void Character::UpdateCurrent(sf::Time dt, CommandQueue& commands)
{
	if (IsDestroyed())
	{
		CheckPickupDrop(commands);
		m_explosion.Update(dt);
		// Play explosion sound only once
		if (!m_played_explosion_sound)
		{
			SoundEffect soundEffect = (Utility::RandomInt(2) == 0) ? SoundEffect::kExplosion1 : SoundEffect::kExplosion2;
			PlayLocalSound(commands, soundEffect);

			m_played_explosion_sound = true;
		}
		return;
	}

	Entity::UpdateCurrent(dt, commands);
	UpdateTexts();
	UpdateMovementPattern(dt);

	//UpdateRollAnimation();

	//Check if bullets or misiles are fired
	CheckProjectileLaunch(dt, commands);
}

void Character::CheckProjectileLaunch(sf::Time dt, CommandQueue& commands)
{
	if (!IsAllied())
	{
		Fire();
	}

	if (m_is_firing && m_fire_countdown <= sf::Time::Zero)
	{
		PlayLocalSound(commands, IsAllied() ? SoundEffect::kEnemyGunfire : SoundEffect::kAlliedGunfire);
		commands.Push(m_fire_command);
		m_fire_countdown += Table[static_cast<int>(m_type)].m_fire_interval / (m_fire_rate + 1.f);
		m_is_firing = false;
	}
	else if (m_fire_countdown > sf::Time::Zero)
	{
		//Wait, can't fire
		m_fire_countdown -= dt;
		m_is_firing = false;
	}

	//Missile launch
	if (m_is_launching_missile)
	{
		PlayLocalSound(commands, SoundEffect::kLaunchMissile);
		commands.Push(m_missile_command);
		m_is_launching_missile = false;
	}
}

bool Character::IsAllied() const
{
	return m_type == CharacterType::kGhost;
}

void Character::CreatePickup(SceneNode& node, const TextureHolder& textures) const
{
	auto type = static_cast<PickupType>(Utility::RandomInt(static_cast<int>(PickupType::kPickupCount)));
	std::unique_ptr<Pickup> pickup(new Pickup(type, textures));
	pickup->setPosition(GetWorldPosition());
	pickup->SetVelocity(0.f, 0.f);
	node.AttachChild(std::move(pickup));
}

void Character::CheckPickupDrop(CommandQueue& commands)
{
	//TODO Get rid of the magic number 3 here 
	if (!IsAllied() && Utility::RandomInt(3) == 0 && !m_spawned_pickup)
	{
		commands.Push(m_drop_pickup_command);
	}
	m_spawned_pickup = true;
}

void Character::UpdateRollAnimation()
{
	if (Table[static_cast<int>(m_type)].m_has_roll_animation)
	{
		sf::IntRect textureRect = Table[static_cast<int>(m_type)].m_texture_rect;

		//Roll left: Texture rect is offset once
		if (GetVelocity().x < 0.f)
		{
			textureRect.left += textureRect.width;
		}
		else if (GetVelocity().x > 0.f)
		{
			textureRect.left += 2 * textureRect.width;
		}
		m_sprite.setTextureRect(textureRect);

	}
}

void Character::PlayLocalSound(CommandQueue& commands, SoundEffect effect)
{
	sf::Vector2f world_position = GetWorldPosition();

	Command command;
	command.category = static_cast<int>(ReceiverCategories::kSoundEffect);
	command.action = DerivedAction<SoundNode>(
		[effect, world_position](SoundNode& node, sf::Time)
		{
			node.PlaySound(effect, world_position);
		});

	commands.Push(command);
}
