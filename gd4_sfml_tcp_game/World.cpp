#include "World.hpp"
#include "Pickup.hpp"
#include "Projectile.hpp"
#include "ParticleNode.hpp"
#include "SoundNode.hpp"

World::World(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds)
	:m_target(output_target)
	,m_camera(output_target.getDefaultView())
	,m_textures()
	,m_fonts(font)
	,m_sounds(sounds)
	,m_scenegraph(ReceiverCategories::kNone)
	,m_scene_layers()
	,m_world_bounds(0.f,0.f, m_camera.getSize().x, 3000.f)
	,m_spawn_position(m_camera.getSize().x/2.f, m_world_bounds.height - m_camera.getSize().y/2.f)
	,m_scrollspeed(-50.f)
	,m_player_aircraft(nullptr)
	,m_player2_aircraft(nullptr)
{
	m_scene_texture.create(m_target.getSize().x, m_target.getSize().y);
	LoadTextures();
	BuildScene();
	m_camera.setCenter(m_spawn_position);
}

void World::Update(sf::Time dt)
{
	//Scroll the world
	m_camera.move(0, m_scrollspeed * dt.asSeconds());
	
	m_player_aircraft->SetVelocity(0.f, 0.f);

	DestroyEntitiesOutsideView();
	GuideMissiles();

	//Forward commands to the scenegraph
	while (!m_command_queue.IsEmpty())
	{
		m_scenegraph.OnCommand(m_command_queue.Pop(), dt);
	}
	AdaptPlayerVelocity();

	HandleCollisions();


	HandleDeteriorate();

	m_scenegraph.RemoveWrecks();

	SpawnEnemies();

	m_scenegraph.Update(dt, m_command_queue);
	AdaptPlayerPosition();
}

void World::Draw()
{
	/*if (PostEffect::IsSupported())
	{
		m_scene_texture.clear();
		m_scene_texture.setView(m_camera);
		m_scene_texture.draw(m_scenegraph);
		m_scene_texture.display();
		m_bloom_effect.Apply(m_scene_texture, m_target);
	}
	else
	{*/
		m_target.setView(m_camera);
		m_target.draw(m_scenegraph);
	//}
}

CommandQueue& World::GetCommandQueue()
{
	return m_command_queue;
}

bool World::HasAlivePlayer() const
{
	return !m_player_aircraft->IsMarkedForRemoval();
}

bool World::HasPlayerReachedEnd() const
{
	return !m_world_bounds.contains(m_player_aircraft->getPosition());
}

void World::LoadTextures()
{
	m_textures.Load(TextureID::kGhost, "Media/Textures/Ghost/Ghost(1).png");
	m_textures.Load(TextureID::kReaper, "Media/Textures/Reaper(AnimatedPixelArt)/Preview/Reap.png");
	m_textures.Load(TextureID::kLandscape, "Media/Textures/Clouds 3/1.png");
	m_textures.Load(TextureID::kBullet, "Media/Textures/Bullet.png");
	m_textures.Load(TextureID::kMissile, "Media/Textures/Missile.png");

	m_textures.Load(TextureID::kHealthRefill, "Media/Textures/HealthRefill.png");
	m_textures.Load(TextureID::kMissileRefill, "Media/Textures/MissileRefill.png");
	m_textures.Load(TextureID::kFireSpread, "Media/Textures/FireSpread.png");
	m_textures.Load(TextureID::kFireRate, "Media/Textures/FireRate.png");
	m_textures.Load(TextureID::kFinishLine, "Media/Textures/FinishLine.png");
	m_textures.Load(TextureID::kBottom, "Media/Textures/Pixel-16px Graveyard/parallax/parallax2(exp).png");

	m_textures.Load(TextureID::kPlat1, "Media/Textures/Pixel-16px Graveyard/parallax/plat1.png");
	m_textures.Load(TextureID::kPlat2, "Media/Textures/Pixel-16px Graveyard/parallax/plat2.png");

	m_textures.Load(TextureID::kEntities, "Media/Textures/Reaper(AnimatedPixelArt)/Preview/Reap(1).png");
	m_textures.Load(TextureID::kJungle, "Media/Textures/Clouds 3/1.png");
	m_textures.Load(TextureID::kExplosion, "Media/Textures/Explosion.png");
	m_textures.Load(TextureID::kParticle, "Media/Textures/Particle.png");


}

void World::BuildScene()
{
	//Initialize the different layers
	for (std::size_t i = 0; i < static_cast<int>(SceneLayers::kLayerCount); ++i)
	{
		ReceiverCategories category = (i == static_cast<int>(SceneLayers::kLowerAir)) ? ReceiverCategories::kScene : ReceiverCategories::kNone;
		SceneNode::Ptr layer(new SceneNode(category));
		m_scene_layers[i] = layer.get();
		m_scenegraph.AttachChild(std::move(layer));
	}

	//Prepare the background
	sf::Texture& texture = m_textures.Get(TextureID::kJungle);
	sf::IntRect textureRect(m_world_bounds);
	texture.setRepeated(true);

	//Add the background sprite to the world
	std::unique_ptr<SpriteNode> background_sprite(new SpriteNode(texture, textureRect));
	background_sprite->setPosition(m_world_bounds.left, m_world_bounds.top);
	m_scene_layers[static_cast<int>(SceneLayers::kBackground)]->AttachChild(std::move(background_sprite));

	//Add the bottom sprite to the world
	sf::Texture& bottom_texture = m_textures.Get(TextureID::kBottom);
	std::unique_ptr<SpriteNode> bottom_sprite(new SpriteNode(bottom_texture));
	bottom_sprite->setPosition(0.f, 2400.f);
	m_scene_layers[static_cast<int>(SceneLayers::kLowerAir)]->AttachChild(std::move(bottom_sprite));

	//Add Platforms
	sf::Texture& plat1_texture = m_textures.Get(TextureID::kPlat1);
	std::unique_ptr<SpriteNode> plat1_sprite(new SpriteNode(plat1_texture));
	plat1_sprite->setPosition(0.f, 2200.f);
	m_scene_layers[static_cast<int>(SceneLayers::kUpperAir)]->AttachChild(std::move(plat1_sprite));

	sf::Texture& plat2_texture = m_textures.Get(TextureID::kPlat2);
	std::unique_ptr<SpriteNode> plat2_sprite(new SpriteNode(plat2_texture));
	plat2_sprite->setPosition(0.f, 1000.f);
	m_scene_layers[static_cast<int>(SceneLayers::kUpperAir)]->AttachChild(std::move(plat2_sprite));

	//Add the finish line
	sf::Texture& finish_texture = m_textures.Get(TextureID::kFinishLine);
	std::unique_ptr<SpriteNode> finish_sprite(new SpriteNode(finish_texture));
	finish_sprite->setPosition(0.f, -76.f);
	m_scene_layers[static_cast<int>(SceneLayers::kBackground)]->AttachChild(std::move(finish_sprite));

	//Add the player 1's character
	std::unique_ptr<Character> leader(new Character(CharacterType::kGhost, m_textures, m_fonts, m_playerID));
	m_player_aircraft = leader.get();
	m_player_aircraft->setPosition(m_spawn_position);
	m_player_aircraft->SetVelocity(40.f, 40.f);
	m_scene_layers[static_cast<int>(SceneLayers::kUpperAir)]->AttachChild(std::move(leader));

	//ET: adding player 2
	//Add the player 2's chracter
	std::unique_ptr<Character> leader(new Character(CharacterType::kReaper, m_textures, m_fonts, m_playerID));
	m_player2_aircraft = leader.get();
	m_player2_aircraft->setPosition(m_spawn_position);
	m_player2_aircraft->SetVelocity(40.f, 40.f);
	m_scene_layers[static_cast<int>(SceneLayers::kUpperAir)]->AttachChild(std::move(leader));


	//Add the particle nodes to the scene
	std::unique_ptr<ParticleNode> smokeNode(new ParticleNode(ParticleType::kSmoke, m_textures));
	m_scene_layers[static_cast<int>(SceneLayers::kLowerAir)]->AttachChild(std::move(smokeNode));

	std::unique_ptr<ParticleNode> propellantNode(new ParticleNode(ParticleType::kPropellant, m_textures));
	m_scene_layers[static_cast<int>(SceneLayers::kLowerAir)]->AttachChild(std::move(propellantNode));

	// Add sound effect node
	std::unique_ptr<SoundNode> soundNode(new SoundNode(m_sounds));
	m_scenegraph.AttachChild(std::move(soundNode));

	AddEnemies();

	

	/*std::unique_ptr<Aircraft> right_escort(new Aircraft(AircraftType::kRaptor, m_textures, m_fonts));
	right_escort->setPosition(80.f, 50.f);
	m_player_aircraft->AttachChild(std::move(right_escort));*/
}

void World::AdaptPlayerPosition()
{
	//keep the player on the screen
	//sf::FloatRect view_bounds(m_camera.getCenter() - m_camera.getSize() / 2.f, m_camera.getSize());
	//const float border_distance = 40.f;
	sf::Vector2f position1 = m_player_aircraft->getPosition();
	sf::Vector2f position2 = m_player2_aircraft->getPosition();
	//position.x = std::max(position.x, view_bounds.left + border_distance);
	//position.x = std::min(position.x, view_bounds.left + view_bounds.width - border_distance);
	//position.y = std::max(position.y, view_bounds.top + border_distance);
	//position.y = std::min(position.y, view_bounds.top + view_bounds.height -border_distance);
	m_player_aircraft->setPosition(position1);
	m_player2_aircraft->setPosition(position2);

}

//ET: Player 2 updates 

void World::AdaptPlayerVelocity()
{
	sf::Vector2f velocity1 = m_player_aircraft->GetVelocity();
	sf::Vector2f velocity2 = m_player2_aircraft->GetVelocity();
	//If they are moving diagonally divide by sqrt 2
	if (velocity1.x != 0.f && velocity1.y != 0.f)
	{
		m_player_aircraft->SetVelocity(velocity1 / std::sqrt(2.f));
		HandleDeteriorate();
	}
	//Add scrolling velocity
	m_player_aircraft->Accelerate(0.f, 40.f);
	if (velocity2.x != 0.f && velocity2.y != 0.f)
	{
		m_player2_aircraft->SetVelocity(velocity2 / std::sqrt(2.f));
		HandleDeteriorate();
	}
	//Add scrolling velocity
	m_player2_aircraft->Accelerate(0.f, 40.f);
}



void World::SpawnEnemies()
{
	//Spawn an enemy when it is relevant i.e when it is in the Battlefieldboudns
	while (!m_enemy_spawn_points.empty() && m_enemy_spawn_points.back().m_y > GetBattleFieldBounds().top)
	{
		SpawnPoint spawn = m_enemy_spawn_points.back();
		std::unique_ptr<Character> enemy(new Character(spawn.m_type, m_textures, m_fonts));
		enemy->setPosition(spawn.m_x, spawn.m_y);
		enemy->setRotation(180.f);
		m_scene_layers[static_cast<int>(SceneLayers::kUpperAir)]->AttachChild(std::move(enemy));
		m_enemy_spawn_points.pop_back();
	}
}

void World::AddEnemies()
{
	AddEnemy(CharacterType::kReaper, 0.f, 500.f);
	AddEnemy(CharacterType::kReaper, 0.f, 1000.f);
	AddEnemy(CharacterType::kReaper, 100.f, 1100.f);
	AddEnemy(CharacterType::kReaper, -100.f, 1100.f);

	//Sort the enemies according to y-value so that enemies are checked first
	std::sort(m_enemy_spawn_points.begin(), m_enemy_spawn_points.end(), [](SpawnPoint lhs, SpawnPoint rhs)
	{
		return lhs.m_y < rhs.m_y;
	});

}

void World::AddEnemy(CharacterType type, float relx, float rely)
{
	SpawnPoint spawn(type, m_spawn_position.x + relx, m_spawn_position.y - rely);
	m_enemy_spawn_points.emplace_back(spawn);
}


sf::FloatRect World::GetViewBounds() const
{
	return sf::FloatRect(m_camera.getCenter() - m_camera.getSize()/2.f, m_camera.getSize());
}

sf::FloatRect World::GetBattleFieldBounds() const
{
	//Return camera bounds + a small area at the top where enemies spawn
	sf::FloatRect bounds = GetViewBounds();
	bounds.top -= 100.f;
	bounds.height += 100.f;

	return bounds;

}

void World::DestroyEntitiesOutsideView()
{
	Command command;
	command.category = static_cast<int>(ReceiverCategories::kEnemyAircraft) | static_cast<int>(ReceiverCategories::kProjectile) | static_cast<int>(ReceiverCategories::kPlayer1);
	command.action = DerivedAction<Entity>([this](Entity& e, sf::Time dt)
		{
			//Does the object intersect with the battlefield
			if (!GetBattleFieldBounds().intersects(e.GetBoundingRect()))
			{
				e.Destroy();
			}
		});
	m_command_queue.Push(command);
}

void World::GuideMissiles()
{
	//Target the closest enemy in the world
	Command enemyCollector;
	enemyCollector.category = static_cast<int>(ReceiverCategories::kEnemyAircraft);
	enemyCollector.action = DerivedAction<Character>([this](Character& enemy, sf::Time)
		{
			if (!enemy.IsDestroyed())
			{
				m_active_enemies.emplace_back(&enemy);
			}
		});

	Command missileGuider;
	missileGuider.category = static_cast<int>(ReceiverCategories::kAlliedProjectile);
	missileGuider.action = DerivedAction<Projectile>([this](Projectile& missile, sf::Time dt)
		{
			if (!missile.IsGuided())
			{
				return;
			}

			float min_distance = std::numeric_limits<float>::max();
			Character* closest_enemy = nullptr;

			for (Character* enemy : m_active_enemies)
			{
				float enemy_distance = Distance(missile, *enemy);
				if (enemy_distance < min_distance)
				{
					closest_enemy = enemy;
					min_distance = enemy_distance;
				}
			}

			if (closest_enemy)
			{
				missile.GuideTowards(closest_enemy->GetWorldPosition());
			}
		});

	m_command_queue.Push(enemyCollector);
	m_command_queue.Push(missileGuider);
	m_active_enemies.clear();
}

bool MatchesCategories(SceneNode::Pair& colliders, ReceiverCategories type1, ReceiverCategories type2)
{
	unsigned int category1 = colliders.first->GetCategory();
	unsigned int category2 = colliders.second->GetCategory();

	if (static_cast<int>(type1) & category1 && static_cast<int>(type2) & category2)
	{
		return true;
	}
	else if (static_cast<int>(type1) & category2 && static_cast<int>(type2) & category1)
	{ 
		std::swap(colliders.first, colliders.second);
	}
	else
	{
		return false;
	}
}


void World::HandleCollisions()
{
	std::set<SceneNode::Pair> collision_pairs;
	m_scenegraph.CheckSceneCollision(m_scenegraph, collision_pairs);
	for (SceneNode::Pair pair : collision_pairs)
	{
		if (MatchesCategories(pair, ReceiverCategories::kPlayer1, ReceiverCategories::kEnemyAircraft))
		{
			auto& player = static_cast<Character&>(*pair.first);
			auto& enemy = static_cast<Character&>(*pair.second);
			//Collision response
			player.Damage(enemy.GetHitPoints());
			enemy.Destroy();
		}

		else if (MatchesCategories(pair, ReceiverCategories::kPlayer1, ReceiverCategories::kPickup))
		{
			auto& player = static_cast<Character&>(*pair.first);
			auto& pickup = static_cast<Pickup&>(*pair.second);
			//Collision response
			pickup.Apply(player);
			pickup.Destroy();
			player.PlayLocalSound(m_command_queue, SoundEffect::kCollectPickup);
		}
		else if (MatchesCategories(pair, ReceiverCategories::kPlayer1, ReceiverCategories::kEnemyProjectile) || MatchesCategories(pair, ReceiverCategories::kEnemyAircraft, ReceiverCategories::kAlliedProjectile))
		{
			auto& aircraft = static_cast<Character&>(*pair.first);
			auto& projectile = static_cast<Projectile&>(*pair.second);
			//Collision response
			aircraft.Damage(projectile.GetDamage());
			projectile.Destroy();
		}
	}
}

void World::HandleDeteriorate()
{

	Command deter;
	deter.category = static_cast<int>(ReceiverCategories::kPlayer1);
	deter.action = DerivedAction<Entity>([this](Entity& e, sf::Time dt)
		{
				
			float timer = 0.0f; // Timer to track elapsed time
			const float interval = 2.0f; // 

					for (int i = 0; i < 100; ++i)
					{
						

						timer += dt.asSeconds();

						
						if (timer >= interval) {
							e.Damage(5);

							
							timer = 0.0f;
						}
					}

		});
	m_command_queue.Push(deter);

}

void World::UpdateSounds()
{
	// Set listener's position to player position
	m_sounds.SetListenerPosition(m_player_aircraft->GetWorldPosition());

	// Remove unused sounds
	m_sounds.RemoveStoppedSounds();
}
