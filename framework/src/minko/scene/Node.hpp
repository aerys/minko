#pragma once

#include "minko/Common.hpp"
#include "minko/type/Signal.hpp"
#include "minko/scene/data/DataBindings.hpp"

class Node :
	public std::enable_shared_from_this<Node>
{
public:
	typedef std::shared_ptr<Node>	ptr;

private:
	static unsigned int									_id;

	std::string 										_name;
	unsigned int										_tags;
	ptr 												_root;
	ptr													_parent;
	std::list<ptr>										_children;
	std::shared_ptr<DataBindings>						_bindings;
	std::list<std::shared_ptr<AbstractController>>		_controllers;

	std::shared_ptr<Signal<ptr, ptr>>					_added;
	std::shared_ptr<Signal<ptr, ptr>>					_removed;
	std::shared_ptr<Signal<ptr, ptr>>					_descendantAdded;
	std::shared_ptr<Signal<ptr, ptr>>					_descendantRemoved;
	std::shared_ptr<Signal<ptr>> 						_tagsChanged;

	std::map<ptr, Signal<ptr, ptr>::cd>					_childToDescendantAddedCd;
	std::map<ptr, Signal<ptr, ptr>::cd>					_childToDescendantRemovedCd;
	std::map<ptr, Signal<ptr, ptr>::cd> 				_childToAddedCd;
	std::map<ptr, Signal<ptr, ptr>::cd> 				_childToRemovedCd;

public:

	static
	ptr
	create()
	{
		return std::shared_ptr<Node>(new Node());
	}

	static
	ptr
	create(const std::string& name)
	{
		ptr node = std::shared_ptr<Node>(new Node());

		node->name(name);

		return node;
	}

	inline
	const std::string&
	name()
	{
		return _name;
	}

	inline
	void
	name(const std::string& name)
	{
		_name = name;
	}

	inline
	unsigned int
	tags()
	{
		return _tags;
	}
	inline
	void
	tags(unsigned int tags)
	{
		if (_tags != tags)
		{
			_tags = tags;
			(*_tagsChanged)(shared_from_this());
		}
	}

	inline
	ptr
	parent()
	{
		return _parent;
	}

	inline
	ptr
	root()
	{
		return _root;
	}

	inline
	std::shared_ptr<DataBindings>
	bindings()
	{
		return _bindings;
	}

	inline
	Signal<ptr, ptr>::ptr
	added()
	{
		return _added;
	}

	inline
	Signal<ptr, ptr>::ptr
	removed()
	{
		return _removed;
	}

	inline
	Signal<ptr, ptr>::ptr
	descendantAdded()
	{
		return _descendantAdded;
	}

	inline
	Signal<ptr, ptr>::ptr
	descendantRemoved()
	{
		return _descendantRemoved;
	}

	inline
	Signal<ptr>::ptr
	tagsChanged()
	{
		return _tagsChanged;
	}

	ptr
	addChild(ptr Node);
	
	ptr
	removeChild(ptr Node);
	
	bool
	contains(ptr Node);

	ptr
	addController(std::shared_ptr<AbstractController> controller);

	ptr
	removeController(std::shared_ptr<AbstractController> controller);

	bool
	hasController(std::shared_ptr<AbstractController> controller);

private:
	Node();

	void
	updateRoot();
};