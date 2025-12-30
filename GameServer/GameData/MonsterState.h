#pragma once
namespace bugat::GameData
{
	class State;
	class MonsterState
	{
	public:
		static void LinkParams(std::map<int, bugat::GameData::MonsterState*>& mapMonsterState, std::map<int, bugat::GameData::State*>& mapState)
		{
			for (auto& [key, value] : mapMonsterState)
				for (auto& [key2, value2] : value->Params)
					if (auto find = mapState.find(key2); find != mapState.end())
						value2 = find->second;
		}

		static void Load(std::string jsonDir, std::map<int, MonsterState*>&data);
		int ID = 0;
		std::map<int, const State*> Params;
	};

	void from_json(const json& j, MonsterState& dataObj)
	{
		dataObj.ID = j.at("ID").get<int>();
		{
			auto ids = j.at("Params").get<std::vector<int>>();
			for(auto id : ids) dataObj.Params[id] = nullptr;
		}
	}

	void MonsterState::Load(std::string jsonDir, std::map<int, MonsterState*>&data)
	{
		std::ifstream inputFile(jsonDir +"/MonsterState.json");
		if (inputFile.is_open())
		{
			std::stringstream buffer;
			buffer << inputFile.rdbuf();
			json j = json::parse(buffer.str());
			for (const auto& elem : j)
			{
				auto item = elem.get<MonsterState>();
				data.emplace(item.ID, new MonsterState(item));
			}
		}
	}

}
