package aerys.minko.scene.graph.group
{
	import aerys.minko.scene.graph.AbstractScene;
	import aerys.minko.scene.graph.IScene;
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.scene.visitor.rendering.RenderingVisitor;
	
	import flash.utils.Proxy;
	import flash.utils.flash_proxy;

	/**
	 * The Group3D provides a basic support for scene building.
	 * A Group3D can contain any object implementing the IScene3D interface.
	 * 
	 * Group3D objects do not affect their children with any specific behaviour.
	 *
	 * @author Jean-Marc Le Roux
	 */
	public dynamic class Group extends Proxy implements IGroup
	{
		private static var _id		: uint				= 0;
		
		private var _name			: String			= null;
		
		private var _visiting		: IScene			= null;
		
		private var _toRemove		: Vector.<IScene>	= new Vector.<IScene>();
		private var _toAdd			: Vector.<IScene>	= new Vector.<IScene>();
		private var _toAddAt		: Vector.<int>		= new Vector.<int>();
		
		private var _children		: Vector.<IScene>	= null;
		
		private var _numChildren	: int				= 0;
		
		public function get name()	: String	{ return _name; }
		public function set name(v : String) : void { _name = v; }
		
		protected function get rawChildren()	: Vector.<IScene> 	{ return _children; }
		
		protected function set rawChildren(value : Vector.<IScene>) : void
		{
			_children = value;
			_numChildren = _children.length;
		}
		
		/**
		 * The number of children.
		 */
		public function get numChildren() : uint
		{
			return _numChildren;
		}
		
		public function Group(...children)
		{
			super();
			
			_name = AbstractScene.getDefaultSceneName(this);

			initialize(children);
		}
		
		private function initialize(children : Array) : void
		{
			while (children.length == 1 && children[0] is Array)
				children = children[0];
			
			_numChildren = children.length;
			_children = _numChildren ? Vector.<IScene>(children)
									 : new Vector.<IScene>();
		}
		
		public function contains(scene : IScene) : Boolean
		{
			return getChildIndex(scene) >= 0;
		}
		
		public function getChildIndex(child : IScene) : int
		{
			for (var i : int = 0; i < _numChildren; i++)
				if (_children[i] === child)
					return i;
			
			return -1;
		}
		
		public function getChildByName(name : String) : IScene
		{
			for (var i : int = 0; i < _numChildren; i++)
				if (_children[i].name === name)
					return _children[i];
			
			return null;
		}
		
		/**
		 * Add a child to the container.
		 *
		 * @param	scene The child to add.
		 */
		public function addChild(scene : IScene) : IGroup
		{
			if (!scene)
				throw new Error();
			
			if (_visiting)
			{
				_toAdd.push(scene);
				_toAddAt.push(-1);
			}
			else
			{
				_children.push(scene);
				++_numChildren;
			}
			
			//scene.added(this);
			
			return this;
		}
		
		public function addChildAt(scene : IScene, position : uint) : IGroup
		{
			if (!scene)
				throw new Error();
			
			var numChildren : int = _children.length;
			
			if (_visiting)
			{
				_toAdd.push(scene);
				_toAddAt.push(position);
			}
			else
			{
				if (position >= numChildren)
					return addChild(scene);
				
				for (var i : int = numChildren; i > position; --i)
					_children[i] = _children[int(i - 1)];
				_children[position] = scene;
				
				++_numChildren;
			}
			
			return this;
		}
		
		/**
		 * Remove a child from the container.
		 *
		 * @param	myChild The child to remove.
		 * @return Whether the child was actually removed or not.
		 */
		public function removeChild(child : IScene) : IGroup
		{
			var numChildren : int = _children.length;
			var i : int	= 0;

			while (i < numChildren && _children[i] !== child)
				++i;
			
			if (i >= numChildren)
				return null;
			
			return removeChildAt(i);
		}
		
		public function removeChildAt(position : uint) : IGroup
		{
			var removed 	: IScene 	= null;
	
			if (position < _numChildren)
			{
				removed = _children[position];
				
				if (_visiting)
				{
					_toRemove.push(removed);
				}
				else
				{
					while (position < _numChildren - 1)
						_children[position] = _children[int(++position)];
					_children.length = --_numChildren;
				}
				
				//removed.removed(this);
			}
			
			return this;
		}
		
		public function removeAllChildren() : IGroup
		{
			//var i : int = _numChildren - 1;
			
			/*while (i >= 0)
			{
				//_children[i].removed(this);
				_children.length = i;
				--i;
			}*/
			
			var numChildren : int = _numChildren;
			
			if (_visiting)
			{
				while (numChildren)
					_toRemove.push(_children[int(numChildren--)]);
			}
			else
			{
				_children.length = 0;
				_numChildren = 0;
			}
			
			return this;
		}
		
		public function getChildAt(position : uint) : IScene
		{
			return position < _numChildren ? _children[position] : null;
		}
		
		public function swapChildren(child1	: IScene,
									 child2	: IScene) : IGroup
		{
			var id1	: int 	= getChildIndex(child1);
			var id2 : int	= getChildIndex(child2);
			
			if (id1 == -1 || id2 == -1)
				return this;
			
			var tmp : IScene = _children[id2];
			
			_children[id2] = _children[id1];
			_children[id1] = tmp;
			
			return this;
		}
		
		public function getDescendantByName(name : String) : IScene
		{
			var descendant 	: IScene 	= getChildByName(name);
			var numChildren	: int 		= numChildren;
			
			for (var i : int = 0; i < numChildren && !descendant; ++i)
			{
				var childGroup : IGroup = _children[i] as IGroup;
				
				if (childGroup)
					descendant = childGroup.getDescendantByName(name);
			}
			
			return descendant;
		}

		/**
		 * Render child nodes.
		 *
		 * @param myGraphics The Graphics3D object that describes the frame being rendered.
		 */
		public function visited(query : ISceneVisitor) : void
		{
			if (query is RenderingVisitor)
				acceptRenderingQuery(query as RenderingVisitor);
			else
				visitChildren(query);
		}
		
		protected function acceptRenderingQuery(query : RenderingVisitor) : void
		{
			visitChildren(query);
		}
		
		protected function visitChildren(query : ISceneVisitor) : void
		{
			var numChildren : int = _numChildren;
			var i 			: int = 0;
			
			// lock
			_visiting = this;
			
			for (i = 0; i < numChildren; ++i)
			{
				//childVisited(_children[i], query);
				query.query(_children[i]);
			}
			
			// unlock
			_visiting = null;
			
			if (_toRemove.length)
			{
				var numRemoved : int = _toRemove.length;
				
				for (i = 0; i < numRemoved; ++i)
					removeChild(_toRemove[i]);
				
				_toRemove.length = 0;
			}
			
			if (_toAdd.length)
			{
				var numAdded : int = _toAdd.length;
				
				for (i = 0; i < numAdded; ++i)
				{
					var position : int = _toAddAt[i];
					
					if (position == -1)
						addChild(_toAdd[i]);
					else
						addChildAt(_toAdd[i], position);
				}
				
				_toAdd.length = 0;
			}
		}
		
		override flash_proxy function getProperty(name : *) : *
		{
			return parseInt(name) == name ? getChildAt(name) : getChildByName(name);
		}
		
		override flash_proxy function getDescendants(name : *) : *
		{
			return getDescendantByName(name);
		}
		
		override flash_proxy function nextNameIndex(index : int) : int
		{
			return index < numChildren ? index + 1 : 0;
		}
		
		override flash_proxy function nextName(index : int) : String
		{
			return String(index - 1);
		}
		
		override flash_proxy function nextValue(index : int) : *
		{
			return _children[int(index - 1)];
		}
	}
}