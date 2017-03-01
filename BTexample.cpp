#include <iostream>
#include "BehaviourTree.h"

BT_status calc_distance() {
	printf("calc_distance...\n");
	return BT_status::SUCCESS;
}
BT_REGISTER_TASK_FUNC(calc_distance);

BT_status buy_house() {
	printf("buy_house...\n");
	return BT_status::SUCCESS;
}
BT_REGISTER_TASK_FUNC(buy_house);

BT_status move() {
	printf("move...\n");
	return BT_status::SUCCESS;
}
BT_REGISTER_TASK_FUNC(move);

BT_status check_hungrey() {
	printf("check_hungrey...\n");
	return BT_status::FAILURE;
}
BT_REGISTER_TASK_FUNC(check_hungrey);

BT_status eat() {
	printf("eat...\n");
	return BT_status::SUCCESS;
}
BT_REGISTER_TASK_FUNC(eat);

BT_status work() {
	printf("work...\n");
	return BT_status::SUCCESS;
}
BT_REGISTER_TASK_FUNC(work);

BT_status check_money() {
	printf("checking money...\n");
	return BT_status::FAILURE;
}
BT_REGISTER_TASK_FUNC(check_money);

void print_registered_tasks() {
	for (const auto &p : g_BTree_registered_tasks) {
		std::cout << p.first << '\n';
	}
}
// BT_UNREGISTER_TASK_FUNC(print_registered_tasks);

int BTexample(void) {
	std::cout << "registered functions: " << std::endl;
	print_registered_tasks();

	std::string json_path = "./BehaviourTree/bt.json";
	std::string config_doc = BT_getConfigJson(json_path);

	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(config_doc, root);
	if (!parsingSuccessful)
	{
		std::cout << "Failed to parse configuration\n" << reader.getFormattedErrorMessages();
		return 0;
	}

	std::string encoding = root.get("encoding", "UTF-8").asString();

	root = root["nodes"].operator[](0);
	root = root["children"].operator[](0);
	// Behavior* behavTree = buildTreeWithJsonValue(root);

	std::cout << "created tree\n\n";

	//always reinitialize the behavior tree to point the current child back to the first left child
	//behavTree->onInitialize();

	std::cout << "=========now begin to play============" << std::endl;
	// this is the main method that starts off all the calculations
	while (true)
	{
		BT_task* behavTree = BT_buildTreeWithJsonValue(root);
		behavTree->run();

		system("pause");
	}
	return 0;
}