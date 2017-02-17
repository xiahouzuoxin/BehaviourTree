
## Features

Just a simple implemention of BehaviourTree framework for game. Reference to `BTexample.cpp` for use guidance.

1. C++11 Implemention. 
2. Json file support by depended on [jsoncpp](https://github.com/open-source-parsers/jsoncpp).
3. `BT_REGISTER_TASK_FUNC` for task regist, define task example:

	```c
	BT_status task_func() {}
	BT_REGISTER_TASK_FUNC(task_func);   // Register task_func to 
	```

## TODO

- [ ] Make task_func support any arguments input
- [ ] Implement more common decorators
- [ ] Make project to be thread safe