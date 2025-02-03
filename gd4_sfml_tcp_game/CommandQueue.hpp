//E.I D00244320, E.T d00245315
#pragma once
#include "Command.hpp"
#include <queue>

class CommandQueue
{
public:
	void Push(const Command& command);
	Command Pop();
	bool IsEmpty() const;

private:
	std::queue<Command> m_queue;
};

