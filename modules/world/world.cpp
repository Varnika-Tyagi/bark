// Copyright (c) 2019 fortiss GmbH, Julian Bernhard, Klemens Esterle, Patrick Hart, Tobias Kessler
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.


#include "modules/world/world.hpp"
#include "modules/world/observed_world.hpp"

namespace modules {
namespace world {

World::World(commons::Params* params) :
  commons::BaseType(params),
  map_(),
  agents_(),
  collision_checkers_(),
  world_time_(0.0) {}

World::World(const World& world)  :
         commons::BaseType(world.get_params()),
         map_(world.get_map()),
         agents_(world.get_agents()),
         objects_(world.get_objects()),
         collision_checkers_(world.get_collision_checkers()),
         world_time_(world.get_world_time()) {}

void World::add_agent(const objects::AgentPtr& agent) {
  agents_[agent->agent_id_] = agent;
}

void World::add_object(const objects::ObjectPtr& object) {
  objects_[object->agent_id_] = object;
}

void World::add_collision_checker(const CollisionCheckerPtr& cchecker) {
  collision_checkers_.push_back(cchecker);
}

void World::MoveAgents(float delta_time) {
  WorldPtr current_world_state(this->Clone());
  for (auto agent : agents_) {
      //! clone current world
      ObservedWorld observed_world(*current_world_state,
                                   agent.first);
      agent.second->Move(delta_time, observed_world);
  }
  world_time_ += delta_time;
}


bool World::CheckCollision() const {
  bool colliding = false;
  for(auto checker : collision_checkers_) {
    if (checker->checkCollision(*this)) {
      colliding = true;
      break; // for now, we just exit if one checker yields a collision
    }
  }
  return colliding;
}

void World::UpdateHorizonDrivingCorridors() {
  for (auto agent : agents_) {
    // TODO(@hart): parameter
    // TODO(@hart): check if update is required
    agent.second->UpdateDrivingCorridor(40.0);
  }
}

void World::Step(float delta_time) {
  UpdateHorizonDrivingCorridors();
  MoveAgents(delta_time);
  // TODO(@fortiss): add post world collision check
}

}  // namespace world
}  // namespace modules
