
#include <map>
#include <iterator>
#include "BehaviourTree.h"

#ifdef BT_BUILD_WITH_JSON

std::map<std::string, std::function<BT_status(void)> > g_BTree_registered_tasks = {
	// register all functions for all behaviour trees here
	{ "", [] { return BT_status::SUCCESS; } }
};

std::string BT_getConfigJson(const std::string& path) {
	std::string sjson;
	std::ifstream ifs;
	ifs.open(path.c_str());
	while (!ifs.eof()) {
		std::string tmp;
		ifs >> tmp;
		sjson += tmp;
	}
	return sjson;
}

/**
 * \InputParam
 * \json: Json::Value output from json::reader.parse
 * \task_func_tbl: map from json string to function pointer
 * \Return
 * \root pointer
 */
BT_task* BT_buildTreeWithJsonValue(const Json::Value& json)
{
	if (json["type"] == "Selector")	{
		BT_task* root = new BT_selector();
		for (uint32_t i = 0; i < json["children"].size(); i++) {
			root->addChild( BT_buildTreeWithJsonValue(json["children"].operator[](i)) );
		}
		return root;
	} else if (json["type"] == "RandomSelector") {
		BT_task* root = new BT_random_selector();
		for (uint32_t i = 0; i < json["children"].size(); i++) {
			root->addChild(BT_buildTreeWithJsonValue(json["children"].operator[](i)));
		}
		return root;
	} else if (json["type"] == "Sequence") {
		BT_task* rootseq = new BT_sequence();
		for (uint32_t i = 0; i<json["children"].size(); i++) {
			rootseq->addChild( BT_buildTreeWithJsonValue(json["children"].operator[](i)) );
		}
		return rootseq;
	} else if (json["type"] == "Action") {
		// FIXME: Map string name to function names, add more statements when adding more functions

		BT_task* b = new BT_task(
			g_BTree_registered_tasks[json["func"].asString()],
			json["name"].asString(), 
			(json["sim"].asString() == "true") ? BT_status::SUCCESS : BT_status::FAILURE
		);
		return b;
	} else if (json["type"] == "Condition") {
		BT_task* b = new BT_task(
			g_BTree_registered_tasks[json["func"].asString()],
			json["name"].asString(), 
			(json["sim"].asString() == "true") ? BT_status::SUCCESS : BT_status::FAILURE
		);
		return b;
	} else if (json["type"] == "Start") {
		return BT_buildTreeWithJsonValue(json["children"].operator[](0) );
	} else {
		return BT_buildTreeWithJsonValue(json["nodes"].operator[](0) );
	}
}

#endif