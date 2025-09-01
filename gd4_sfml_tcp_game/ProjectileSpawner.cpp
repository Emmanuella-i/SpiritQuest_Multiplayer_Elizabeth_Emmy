//E.I D00244320, E.T D00245315
#include "ProjectileSpawner.hpp"
#include "Projectile.hpp"
#include "ProjectileType.hpp"
#include "TextureID.hpp"
#include "ResourceHolder.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include "DataTables.hpp"
#include "Projectile.hpp"
#include "PickupType.hpp"
/*#include "Pickup.hpp"
#include "SoundNode.hpp"
#include "SceneNode.hpp"
#include "Utility.hpp"


//E.I Attempted projectile spawner

namespace
{
	const std::vector<ProjectileSpawnerData> Table = InitializeProjectileSpawnerData();
}

ProjectileSpawner::ProjectileSpawner(const TextureHolder& textures)
{
	m_sprite = textures.Get(Table[static_cast<int>(1)].m_texture);
	m_fire_rate = 1;
	m_spread_level = 1;
	m_is_firing = false;
	m_fire_countdown = sf::Time::Zero;
	m_is_marked_for_removal = false;
	m_spawned_pickup = false;
	m_fire_command.category = static_cast<int>(ReceiverCategories::kScene);
	m_fire_command.action = [this, &textures](SceneNode& node, sf::Time dt)
		{
			CreateBullet(node, textures);
		};

	m_drop_pickup_command.category = static_cast<int>(ReceiverCategories::kScene);
	m_drop_pickup_command.action = [this, &textures](SceneNode& node, sf::Time dt)
		{
			CreatePickup(node, textures);
		};
}

void ProjectileSpawner::IncreaseFireRate()
{
	if (m_fire_rate < 5)
	{
		++m_fire_rate;
	}
}

void ProjectileSpawner::IncreaseFireSpread()
{
	if (m_spread_level < 3)
	{
		++m_spread_level;
	}
}

void ProjectileSpawner::UpdateMovementPattern(sf::Time dt)
{
}

float ProjectileSpawner::GetMaxSpeed() const
{
    return 0.0f;
}

void ProjectileSpawner::Fire()
{
	if (Table[static_cast<int>(m_type)].m_fire_interval != sf::Time::Zero)
	{
		m_is_firing = true;
	}
}

void ProjectileSpawner::CreateBullet(SceneNode& node, const TextureHolder& textures) const
{
	ProjectileType type = ProjectileType::kAlliedBullet;
	CreateProjectile(node, type, 0.0f, 0.5f, textures);
}

void ProjectileSpawner::CreateProjectile(SceneNode& node, ProjectileType type, float x_float, float y_offset, const TextureHolder& textures) const
{
	std::unique_ptr<Projectile> projectile(new Projectile(type, textures));
	sf::Vector2f velocity(0, projectile->GetMaxSpeed());

	float sign = IsAllied() ? -1.f : 1.f;
	projectile->setPosition(GetWorldPosition() * sign);
	projectile->SetVelocity(velocity * sign);
	node.AttachChild(std::move(projectile));
}

void ProjectileSpawner::PlayLocalSound(CommandQueue& commands, SoundEffect effect)
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

void ProjectileSpawner::CheckProjectileLaunch(sf::Time dt, CommandQueue& commands)
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
}

bool ProjectileSpawner::IsAllied() const
{
    return false;
}

void ProjectileSpawner::CreatePickup(SceneNode& node, const TextureHolder& textures) const
{
	auto type = static_cast<PickupType>(Utility::RandomInt(static_cast<int>(PickupType::kPickupCount)));
	std::unique_ptr<Pickup> pickup(new Pickup(type, textures));
	pickup->setPosition(GetWorldPosition());
	pickup->SetVelocity(0.f, 0.f);
	node.AttachChild(std::move(pickup));
}

void ProjectileSpawner::CheckPickupDrop(CommandQueue& commands)
{
}
*/