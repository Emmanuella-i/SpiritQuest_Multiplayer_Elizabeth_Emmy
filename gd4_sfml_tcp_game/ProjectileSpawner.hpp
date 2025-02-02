//EI
#pragma once
#include "ResourceIdentifiers.hpp"
#include "TextNode.hpp"
#include "Utility.hpp"
#include "ProjectileType.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include "Animation.hpp"

class ProjectileSpawner 
{
public:
	ProjectileSpawner(const TextureHolder& textures);
	

	void IncreaseFireRate();
	void IncreaseFireSpread();
	

	
	void UpdateMovementPattern(sf::Time dt);

	float GetMaxSpeed() const;
	void Fire();
	
	void CreateBullet(SceneNode& node, const TextureHolder& textures) const;
	void CreateProjectile(SceneNode& node, ProjectileType type, float x_float, float y_offset, const TextureHolder& textures) const;
	void PlayLocalSound(CommandQueue& commands, SoundEffect effect);

private:
	void CheckProjectileLaunch(sf::Time dt, CommandQueue& commands);
	bool IsAllied() const;
	void CreatePickup(SceneNode& node, const TextureHolder& textures) const;
	void CheckPickupDrop(CommandQueue& commands);

private:
	sf::Sprite m_sprite;
	float m_distance_travelled;
	int m_directions_index;

	Command m_fire_command;
	Command m_missile_command;
	Command m_drop_pickup_command;

	unsigned int m_fire_rate;
	unsigned int m_spread_level;
	unsigned int m_missile_ammo;

	bool m_is_firing;
	bool m_is_launching_missile;
	sf::Time m_fire_countdown;

	bool m_is_marked_for_removal;
	bool m_show_explosion;
	bool m_spawned_pickup;
	bool m_played_explosion_sound;
};