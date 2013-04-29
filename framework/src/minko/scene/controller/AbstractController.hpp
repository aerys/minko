#pragma once

#include <memory>
#include <functional>
#include <list>

#include "minko/Minko.hpp"

class AbstractController
{
public:
	typedef std::shared_ptr<AbstractController>	ptr;

private:
	std::list<std::shared_ptr<Node>>					_targets;

	std::shared_ptr<Signal<ptr, std::shared_ptr<Node>>>	_targetAdded;
	std::shared_ptr<Signal<ptr, std::shared_ptr<Node>>>	_targetRemoved;

protected:
	inline
	std::list<std::shared_ptr<Node>>
	targets()
	{
		return _targets;
	}

public:
	AbstractController() :
		_targetAdded(Signal<ptr, std::shared_ptr<Node>>::create()),
		_targetRemoved(Signal<ptr, std::shared_ptr<Node>>::create())
	{

	}

	const unsigned int
	numTargets()
	{
		return _targets.size();
	}

	inline
	Signal<ptr, std::shared_ptr<Node>>::ptr
	targetAdded()
	{
		return _targetAdded;
	}

	inline
	Signal<ptr, std::shared_ptr<Node>>::ptr
	targetRemoved()
	{
		return _targetRemoved;
	}
};
