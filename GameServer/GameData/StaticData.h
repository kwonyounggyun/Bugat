#pragma once
#include <map>
#include <list>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
namespace bugat::GameData
{
	struct Vec3
	{
	public:
		float x;
		float y;
		float z;
	};

	void from_json(const json& j, Vec3& dataObj)
	{
		dataObj.x = j.at("x").get<float>();
		dataObj.y = j.at("y").get<float>();
		dataObj.z = j.at("z").get<float>();
	}

	struct Vec2
	{
	public:
		float x;
		float y;
	};

	void from_json(const json& j, Vec2& dataObj)
	{
		dataObj.x = j.at("x").get<float>();
		dataObj.y = j.at("y").get<float>();
	}

}
#include "Event.h"
#include "SpawnData.h"
#include "MonsterState.h"
#include "State.h"
namespace bugat::GameData
{
	class StaticData
	{
	public:
		void Load(std::string jsonDir)
		{
			std::map <int, bugat::GameData::Event*> _Event;
			std::map <int, bugat::GameData::SpawnData*> _SpawnData;
			std::map <int, bugat::GameData::MonsterState*> _MonsterState;
			std::map <int, bugat::GameData::State*> _State;
			Event::Load(jsonDir, _Event);
			SpawnData::Load(jsonDir, _SpawnData);
			MonsterState::Load(jsonDir, _MonsterState);
			State::Load(jsonDir, _State);
			SpawnData::LinkState(_SpawnData, _State);
			MonsterState::LinkParams(_MonsterState, _State);
			Event.insert(_Event.begin(), _Event.end());
			SpawnData.insert(_SpawnData.begin(), _SpawnData.end());
			MonsterState.insert(_MonsterState.begin(), _MonsterState.end());
			State.insert(_State.begin(), _State.end());
		}

		std::map<int, const bugat::GameData::Event*> Event;
		std::map<int, const bugat::GameData::SpawnData*> SpawnData;
		std::map<int, const bugat::GameData::MonsterState*> MonsterState;
		std::map<int, const bugat::GameData::State*> State;
	};

}
