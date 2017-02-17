#pragma once

/**
 * Implement BehaviourTree by C++11
 */

#ifndef _BEHAVIOUR_TREE_H
#define _BEHAVIOUR_TREE_H

#include <vector>
#include <string>
#include <iterator>
#include <functional>
#include <memory>
#include <random>
#include <algorithm>
#include <assert.h>

#define BT_NonDeterministic
#define BT_BUILD_WITH_JSON    // Need [jsoncpp](https://github.com/open-source-parsers/jsoncpp/releases) for support. (test on 1.8.0)

#define BT_LOG(msg)             printf("%s:%d	%s\n", __FILE__, __LINE__, msg)
#define BT_ASSERT(expr, msg)    assert( (expr) && (msg) )

enum class BT_status {
	SUCCESS,
	FAILURE,
	INVALID,
	RUNNING
};

class BT_task {
public:
	BT_task(std::function<BT_status(void)> task= [] { return BT_status::SUCCESS; }, const std::string& taskname = "Unknow", const BT_status& status = BT_status::SUCCESS)
		: task_func(task), mStatus(status), mName(taskname) {}

	const BT_status checkStatus() {
		return mStatus;
	}

	BT_status addChild(BT_task* child) {
		try {
			mChildren.emplace_back(child);
			return BT_status::SUCCESS;
		} catch (std::exception) {
			return BT_status::FAILURE;
		}
	}

	virtual BT_status run() {
		mStatus = BT_status::RUNNING;
		mStatus = task_func();
		return mStatus;
	};

protected:
	std::string mName;
	BT_status mStatus;
	std::vector<std::shared_ptr<BT_task> > mChildren;
	std::function<BT_status(void)> task_func;
};

class BT_selector : public BT_task {
public:
	BT_selector(std::function<BT_status(void)> task = [] { return BT_status::SUCCESS; }, const std::string& name = "Unknow")
		: BT_task(task, name) {}

	virtual BT_status run() {
#ifdef BT_NonDeterministic
		std::vector<std::shared_ptr<BT_task> > copy_mChildren = mChildren;
		std::random_device rd;
		std::mt19937 generator(rd());
		std::shuffle(copy_mChildren.begin(), copy_mChildren.end(), generator);
		for (auto &curChild : mChildren) {
#else
		for (auto &curChild : mChildren) {
#endif
			BT_status state = curChild->run();
			if (state != BT_status::SUCCESS) {
				return state;
			}
		}
		return BT_status::SUCCESS;
	}
};

class BT_sequence : public BT_task {
public:
	BT_sequence(std::function<BT_status(void)> task = [] { return BT_status::SUCCESS; }, const std::string& name = "Unknow")
		: BT_task(task, name) {}

	virtual BT_status run() {
#ifdef BT_NonDeterministic
		std::vector<std::shared_ptr<BT_task> > copy_mChildren = mChildren;
		std::random_device rd;
		std::mt19937 generator(rd());
		std::shuffle(copy_mChildren.begin(), copy_mChildren.end(), generator);
		for (auto &curChild : mChildren) {
#else
		for (auto &curChild : mChildren) {
#endif
			BT_status state = curChild->run();
			if (state != BT_status::FAILURE) {
				return state;
			}
		}
		return BT_status::FAILURE;
	}
};

// Decorators
class BT_DecoratorLimit : public BT_task {
public:
	BT_DecoratorLimit(const int limit_times, BT_task* task)
		: BT_task(), decorator(task), limit_times(limit_times) {
		run_sofar = 0;
	}

	virtual BT_status run() {
		if (run_sofar >= limit_times) {
			return BT_status::FAILURE;
		} else {
			run_sofar++;
			return decorator->run();
		}
	}
protected:
	std::shared_ptr<BT_task> decorator;
private:
	int run_sofar;
	int limit_times;
};

class BT_DecoratorUtilfail : public BT_task {
public:
	BT_DecoratorUtilfail(BT_task* task)
		: BT_task(), decorator(task) {}

	BT_status run() {
		while(BT_status::SUCCESS == decorator->run()) {}
		return BT_status::SUCCESS;
	}

protected:
	std::shared_ptr<BT_task> decorator;
};

#ifdef BT_BUILD_WITH_JSON
#include <fstream>
#include "json/json.h"

extern std::string BT_getConfigJson(const std::string& path);
extern BT_task* BT_buildTreeWithJsonValue(const Json::Value& json);
extern std::map<std::string, std::function<BT_status(void)> > g_BTree_registered_tasks;

/**
* Following codes for register task to global table @g_BTree_registered_tasks
*/
// User class instead of inline function
class _BT_REGISTER {
public:
	_BT_REGISTER(const std::string& func_str, std::function<BT_status(void)> func) {
		BT_ASSERT(g_BTree_registered_tasks.find(func_str) == g_BTree_registered_tasks.end(), 
			"Register task_func failed because of it exist!");
		g_BTree_registered_tasks.emplace(std::make_pair(func_str, func));
	}
};
class _BT_UNREGISTER {
public:
	_BT_UNREGISTER(const std::string& func_str) {
		std::map<std::string, std::function<BT_status(void)> >::iterator it = g_BTree_registered_tasks.find(func_str);
		BT_ASSERT(it != g_BTree_registered_tasks.end(),
			"Unregister task_func failed because of it not exist!");
		g_BTree_registered_tasks.erase(it);
	}
};

#define BT_REGISTER_TASK_FUNC(func)  \
	static _BT_REGISTER g_register_bttask_##func(#func, func)
#define BT_UNREGISTER_TASK_FUNC(func)  \
	static _BT_UNREGISTER g_unregister_bttask_##func(#func)

// \Deprecated: Replaced by upper #define marocs
inline bool _BT_REGISTER_TASK_FUNC(const std::string& func_str, std::function<BT_status(void)>& func) {
	try {
		if (g_BTree_registered_tasks.find(func_str) != g_BTree_registered_tasks.end()) {
			BT_LOG("Register task_func failed because of it exist!");
			return false;
		}
		
		g_BTree_registered_tasks.emplace(std::make_pair(func_str, func));
		return true;
	}
	catch (std::exception) {
		return false;
	}
}

inline bool _BT_REGISTER_TASK_FUNC(const std::string& func_str) {
	try {
		std::map<std::string, std::function<BT_status(void)> >::iterator it = g_BTree_registered_tasks.find(func_str);
		if ( it == g_BTree_registered_tasks.end() ) {
			BT_LOG("Unregister task_func failed because of it not exist!");
			return false;
		}
		g_BTree_registered_tasks.erase(it);
		return true;
	}
	catch (std::exception) {
		return false;
	}
}

#endif

#endif
