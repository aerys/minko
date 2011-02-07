package aerys.minko.scene.group
{
	import aerys.minko.query.IScene3DQuery;
	import aerys.minko.scene.AbstractScene3D;
	import aerys.minko.scene.IScene3D;

	/**
	 * The Object3DContainer provides a basic support for Object3D grouping. Such
	 * groups can be used to provide local z-sorting or apply a common 3D transform.
	 * An Object3DContainer can contain any object implementing the IChild3D interface,
	 * including Object3D or even Object3DContainer object.
	 *
	 * @author Jean-Marc Le Roux
	 * @see aerys.minko.scene.containers.IContainer3D
	 * @see aerys.minko.scene.Object3D
	 * @see aerys.minko.scene.IChild3D
	 */
	public class Group3D extends AbstractScene3D implements IGroup3D
	{
		private static var _id		: uint				= 0;
		
		//{ region vars
		private var _visiting		: IScene3D			= null;
		
		private var _toRemove		: Vector.<IScene3D>	= new Vector.<IScene3D>();
		private var _toAdd			: Vector.<IScene3D>	= new Vector.<IScene3D>();
		private var _toAddAt		: Vector.<int>		= new Vector.<int>();
		
		private var _children		: Vector.<IScene3D>	= null;
		
		private var _numChildren	: int				= 0;
		//} endregion
		
		//{ region getters/setters
		protected function get children() : Vector.<IScene3D> 	{ return _children; }
		
		protected function set children(value : Vector.<IScene3D>) : void
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
		//} endregion
		
		public function Group3D(...children)
		{
			super();

			initialize(children);
		}
		
		private function initialize(children : Array) : void
		{
			while (children.length == 1 && children[0] is Array)
				children = children[0];
			
			_numChildren = children.length;
			_children = _numChildren ? Vector.<IScene3D>(children)
									 : new Vector.<IScene3D>();
		}
		
		//{ region methods
		public function contains(myChild : IScene3D) : Boolean
		{
			return getChildIndex(myChild) >= 0;
		}
		
		public function getChildIndex(myChild : IScene3D) : int
		{
			for (var i : int = 0; i < _numChildren; i++)
				if (_children[i] === myChild)
					return i;
			
			return -1;
		}
		
		public function getChildByName(myName : String) : IScene3D
		{
			for (var i : int = 0; i < _numChildren; i++)
				if (_children[i].name === myName)
					return _children[i];
			
			return null;
		}
		
		/**
		 * Add a child to the container.
		 *
		 * @param	myChild The child to add.
		 */
		public function addChild(myScene : IScene3D) : IScene3D
		{
			if (!myScene)
				throw new Error();
			
			if (_visiting)
			{
				_toAdd.push(myScene);
				_toAddAt.push(-1);
				
				return myScene;
			}
			
			_children.push(myScene);
			++_numChildren;
			
			//myScene.added(this);
			
			return myScene;
		}
		
		public function addChildAt(myScene : IScene3D, myPosition : uint) : IScene3D
		{
			if (!myScene)
				throw new Error();
			
			var numChildren : int = _children.length;
			
			if (_visiting)
			{
				_toAdd.push(myScene);
				_toAddAt.push(myPosition);
				
				return myScene;
			}
			
			if (myPosition >= numChildren)
				return addChild(myScene);
			
			for (var i : int = numChildren; i > myPosition; --i)
				_children[i] = _children[int(i - 1)];
			_children[myPosition] = myScene;
			
			++_numChildren;
			//myScene.added(this);
			
			return myScene;
		}
		
		/**
		 * Remove a child from the container.
		 *
		 * @param	myChild The child to remove.
		 * @return Whether the child was actually removed or not.
		 */
		public function removeChild(myChild : IScene3D) : IScene3D
		{
			var numChildren : int = _children.length;
			var i : int	= 0;

			while (i < numChildren && _children[i] !== myChild)
				++i;
			
			if (i >= numChildren)
				return null;
			
			return removeChildAt(i);
		}
		
		public function removeChildAt(myPosition : uint) : IScene3D
		{
			var removed 	: IScene3D 	= null;
	
			if (myPosition < _numChildren)
			{
				removed = _children[myPosition];
				
				if (_visiting)
				{
					_toRemove.push(removed);
					
					return removed;
				}
				
				while (myPosition < _numChildren - 1)
					_children[myPosition] = _children[int(++myPosition)];
				_children.length = --_numChildren;
				
				//removed.removed(this);
			}
			
			return removed;
		}
		
		public function removeAllChildren() : uint
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
				
				return _numChildren;
			}
			else
			{
				_children.length = 0;
				_numChildren = 0;
			}
			
			return numChildren;
		}
		
		public function getChildAt(myPosition : uint) : IScene3D
		{
			return myPosition < _numChildren ? _children[myPosition] : null;
		}
		
		public function swapChildren(myChild1	: IScene3D,
									 myChild2	: IScene3D) : Boolean
		{
			var id1	: int 	= getChildIndex(myChild1);
			var id2 : int	= getChildIndex(myChild2);
			
			if (id1 == -1 || id2 == -1)
				return false;
			
			var tmp : IScene3D = _children[id2];
			
			_children[id2] = _children[id1];
			_children[id1] = tmp;
			
			return true;
		}

		/**
		 * Render child nodes.
		 *
		 * @param myGraphics The Graphics3D object that describes the frame being rendered.
		 */
		override public function accept(query : IScene3DQuery) : void
		{
			var numChildren : int = _numChildren;
			var i 			: int = 0;
			
			// lock
			_visiting = this;
			
			for (i = 0; i < numChildren; ++i)
				childVisited(_children[i], query);
			
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
		
		protected function childVisited(child 	: IScene3D,
									  	visitor : IScene3DQuery) : void
		{
			visitor.query(child);
		}
		//} endregion
	}
}