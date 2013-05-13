package aerys.minko.scene.controller
{
	import flash.display.BitmapData;
	import flash.utils.Dictionary;
	
	import aerys.minko.ns.minko_math;
	import aerys.minko.render.Viewport;
	import aerys.minko.scene.node.AbstractSceneNode;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.Signal;
	import aerys.minko.type.math.Matrix4x4;
	
	use namespace minko_math;
	
	/**
	 * The TransformController handles the batched update of all the local to world matrices
	 * of a sub-scene. As such, it will only be active on the root node of a sub-scene and will
	 * automatically disable itself on other nodes.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public final class TransformController extends AbstractController
	{
		
		private static const FLAG_NONE                      : uint  = 0;
		private static const FLAG_INIT_LOCAL_TO_WORLD       : uint  = 1;
		private static const FLAG_INIT_WORLD_TO_LOCAL       : uint  = 2;
		private static const FLAG_SYNCHRONIZE_TRANSFORMS    : uint  = 4;
		private static const FLAG_LOCK_TRANSFORMS           : uint  = 8;
		
		private var _target					: ISceneNode;
		
		private var _invalidList			: Boolean;
		
		private var _nodeToId				: Dictionary;
		private var _idToNode				: Vector.<ISceneNode>
		private var _transforms				: Vector.<Matrix4x4>;
		private var _flags	                : Vector.<uint>;
		private var _localToWorldTransforms : Vector.<Matrix4x4>;
		private var _worldToLocalTransforms : Vector.<Matrix4x4>;
		private var _worldToLocalUpToDate	: Vector.<Boolean>;
		private var _numChildren			: Vector.<uint>;
		private var _firstChildId			: Vector.<uint>;
		private var _parentId				: Vector.<int>;
		private var _prevLocalToWorld		: Matrix4x4;
		
		public function TransformController()
		{
			super();
			
			initialize();
		}
		
		private function initialize() : void
		{
			_transforms = new <Matrix4x4>[];
			_flags = new <uint>[];
			_localToWorldTransforms = new <Matrix4x4>[];
			_worldToLocalTransforms = new <Matrix4x4>[];
			_numChildren = new <uint>[];
			_firstChildId = new <uint>[];
			_idToNode = new <ISceneNode>[];
			_parentId = new <int>[];
			_prevLocalToWorld = new Matrix4x4();
			
			targetAdded.add(targetAddedHandler);
			targetRemoved.add(targetRemovedHandler);
		}
		
		private function renderingBeginHandler(scene		: Scene,
											   viewport		: Viewport,
											   destination	: BitmapData,
											   time			: Number) : void
		{
			if (_invalidList)
				updateTransformsList();
			
			if (_transforms.length)
				updateLocalToWorld();
		}
		
		private function updateRootLocalToWorld(nodeId : uint = 0) : void
		{
			var rootLocalToWorld		: Matrix4x4		= _localToWorldTransforms[nodeId];
			var rootTransform			: Matrix4x4		= _transforms[nodeId];
			var root					: ISceneNode	= _idToNode[nodeId];
			var rootFlags           	: uint			= _flags[nodeId];
			
			if (rootTransform._hasChanged || !(rootFlags & FLAG_INIT_LOCAL_TO_WORLD))
			{
				_prevLocalToWorld.copyFrom(rootLocalToWorld);
				
				if (rootFlags & FLAG_LOCK_TRANSFORMS)
					rootLocalToWorld.lock();
				
				rootLocalToWorld.copyFrom(rootTransform);
				
				if (nodeId != 0)
					rootLocalToWorld.append(_localToWorldTransforms[_parentId[nodeId]]);
				
				if (rootFlags & FLAG_LOCK_TRANSFORMS)
					rootLocalToWorld.unlock();
				
				rootTransform._hasChanged = false;
				rootFlags = (rootFlags | FLAG_INIT_LOCAL_TO_WORLD) & ~FLAG_INIT_WORLD_TO_LOCAL;
				
				if (rootFlags & FLAG_SYNCHRONIZE_TRANSFORMS)
				{
					var rootWorldToLocal : Matrix4x4 = _worldToLocalTransforms[nodeId]
						|| (_worldToLocalTransforms[nodeId] = new Matrix4x4());
					
					if (rootFlags & FLAG_LOCK_TRANSFORMS)
						rootWorldToLocal.lock();
					
					rootWorldToLocal.copyFrom(rootLocalToWorld).invert();
					rootFlags |= FLAG_INIT_WORLD_TO_LOCAL;
					
					if (rootFlags & FLAG_LOCK_TRANSFORMS)
						rootWorldToLocal.unlock();
				}
				
				_flags[nodeId] = rootFlags;
				
				var localToWorldTransformChanged : Signal = root.localToWorldTransformChanged;
				
				if (root.localToWorldTransformChanged.enabled && !_prevLocalToWorld.compareTo(rootLocalToWorld))
					root.localToWorldTransformChanged.execute(root, rootLocalToWorld);
			}
		}
		
		private function updateLocalToWorld(nodeId : uint = 0) : void
		{
			var numNodes 				: uint			= _transforms.length;
			var subtreeMax  			: uint			= nodeId;
			
			updateRootLocalToWorld(nodeId);
			
			while (nodeId < numNodes)
			{
				var localToWorld 	: Matrix4x4	= _localToWorldTransforms[nodeId];
				var numChildren		: uint		= _numChildren[nodeId];
				var firstChildId	: uint		= _firstChildId[nodeId];
				var lastChildId		: uint		= firstChildId + numChildren;
				var isDirty			: Boolean	= localToWorld._hasChanged;
				
				localToWorld._hasChanged = false;
				
				if (lastChildId > subtreeMax)
					subtreeMax = lastChildId;
				
				for (var childId : uint = firstChildId; childId < lastChildId; ++childId)
				{
					var childTransform		: Matrix4x4		= _transforms[childId];
					var childLocalToWorld	: Matrix4x4		= _localToWorldTransforms[childId];
					var childFlags          : uint          = _flags[childId];
					var childIsDirty		: Boolean		= isDirty || childTransform._hasChanged
						|| !(childFlags & FLAG_INIT_LOCAL_TO_WORLD);
															
					if (childIsDirty)
					{
						_prevLocalToWorld.copyFrom(childLocalToWorld);
						
						var child	: ISceneNode	= _idToNode[childId];
						
						if (childFlags & FLAG_LOCK_TRANSFORMS)
							childLocalToWorld.lock();
						
						childLocalToWorld
						.copyFrom(childTransform)
							.append(localToWorld);
						
						if (childFlags & FLAG_LOCK_TRANSFORMS)
							childLocalToWorld.unlock();
						
						childTransform._hasChanged = false;
						childFlags = (childFlags | FLAG_INIT_LOCAL_TO_WORLD)
							& ~FLAG_INIT_WORLD_TO_LOCAL;
						
						if (childFlags & FLAG_SYNCHRONIZE_TRANSFORMS)
						{
							var childWorldToLocal : Matrix4x4 = _worldToLocalTransforms[childId]
								|| (_worldToLocalTransforms[childId] = new Matrix4x4());
							
							if (childFlags & FLAG_LOCK_TRANSFORMS)
								childWorldToLocal.lock();
							
							childWorldToLocal.copyFrom(childLocalToWorld).invert();
							childFlags |= FLAG_INIT_WORLD_TO_LOCAL;
							
							if (childFlags & FLAG_LOCK_TRANSFORMS)
								childWorldToLocal.unlock();
						}
						
						_flags[childId] = childFlags;
						
						var localToWorldTransformChanged : Signal = child.localToWorldTransformChanged;
						
						if (localToWorldTransformChanged.enabled && !_prevLocalToWorld.compareTo(childLocalToWorld))
							localToWorldTransformChanged.execute(child, childLocalToWorld);
					}
				}
				
				++nodeId;
			}
		}
		
		private function updateLocalToWorldPath(path : Vector.<uint> = null) : void
		{
			var numNodes				: uint		= path.length;
			var nodeId					: uint		= path[uint(numNodes - 1)];
			var localToWorld 			: Matrix4x4	= _localToWorldTransforms[nodeId];
			
			updateRootLocalToWorld(nodeId);
			
			for (var i : int = numNodes - 2; i >= 0; --i)
			{
				var childId             : uint          = path[i];
				var childTransform		: Matrix4x4		= _transforms[childId];
				var childLocalToWorld	: Matrix4x4		= _localToWorldTransforms[childId];
				var childFlags          : uint          = _flags[childId];
				var child	            : ISceneNode	= _idToNode[childId];
				
				_prevLocalToWorld.copyFrom(childLocalToWorld);
				
				if (childFlags & FLAG_LOCK_TRANSFORMS)
					childLocalToWorld.lock();
				
				childLocalToWorld
					.copyFrom(childTransform)
					.append(localToWorld);
				
				
				if (childFlags & FLAG_LOCK_TRANSFORMS)
					childLocalToWorld.unlock();
				
				childTransform._hasChanged = false;
				childFlags = (childFlags | FLAG_INIT_LOCAL_TO_WORLD)
					& ~FLAG_INIT_WORLD_TO_LOCAL;
				
				if (childFlags & FLAG_SYNCHRONIZE_TRANSFORMS)
				{
					var childWorldToLocal : Matrix4x4 = _worldToLocalTransforms[childId]
						|| (_worldToLocalTransforms[childId] = new Matrix4x4());
					
					if (childFlags & FLAG_LOCK_TRANSFORMS)
						childWorldToLocal.lock();
					
					childWorldToLocal.copyFrom(childLocalToWorld).invert();
					childFlags |= FLAG_INIT_WORLD_TO_LOCAL;
					
					if (childFlags & FLAG_LOCK_TRANSFORMS)
						childWorldToLocal.unlock();
				}
				
				_flags[childId]					= childFlags;
				_worldToLocalUpToDate[childId]	= true;
				
				if (child is Group)
					setDescendantsAsDirty(child as Group, path);
				
				var localToWorldTransformChanged : Signal = child.localToWorldTransformChanged;
				
				if (localToWorldTransformChanged.enabled && !_prevLocalToWorld.compareTo(childLocalToWorld))
					localToWorldTransformChanged.execute(child, childLocalToWorld);
				
				localToWorld = childLocalToWorld;
			}
		}
		
		private function setDescendantsAsDirty(node : Group, ignoreIDs : Vector.<uint> = null) : void
		{
			for (var i : int = 0; i < node.numChildren; ++i)
			{
				var child	: ISceneNode	= node.getChildAt(i);
				var id		: int			= _nodeToId[child];
				
				if (ignoreIDs.indexOf(id) != -1)
					continue;
				
				if (child is Group)
					setDescendantsAsDirty(child as Group, ignoreIDs);
				
				_worldToLocalUpToDate[id] = false;
			}
		}
		
		private function updateAncestorsAndSelfLocalToWorld(nodeId : uint) : void
		{
			var dirtyRoot   : int           = -1;
			var tmpNodeId   : int           = nodeId;
			var path        : Vector.<uint> = new <uint>[];
			var numNodes    : uint          = 0;
			
			while (tmpNodeId >= 0)
			{
				if ((_transforms[tmpNodeId] as Matrix4x4)._hasChanged 
					|| (tmpNodeId != nodeId && _localToWorldTransforms[tmpNodeId]._hasChanged)
					|| !(_flags[nodeId] & FLAG_INIT_LOCAL_TO_WORLD)
					|| !_worldToLocalUpToDate[tmpNodeId])
					dirtyRoot = tmpNodeId;
				
				path[numNodes] = tmpNodeId;
				++numNodes;
				
				tmpNodeId = _parentId[tmpNodeId];
			}
			
			if (dirtyRoot >= 0)
				updateLocalToWorldPath(path);
			//                updateLocalToWorld(dirtyRoot, nodeId);
		}
		
		private function targetAddedHandler(ctrl	: TransformController,
											target	: ISceneNode) : void
		{
			if (_target)
				throw new Error('The TransformController cannot have more than one target.');
			
			_target = target;
			_invalidList = true;
			
			if (target is Group)
			{
				var targetGroup : Group = target as Group;
				
				targetGroup.descendantAdded.add(descendantAddedHandler);
				targetGroup.descendantRemoved.add(descendantRemovedHandler);
			}
			
			if (target is Scene)
			{
				(target as Scene).renderingBegin.add(renderingBeginHandler);
				return;
			}
			
			target.added.add(addedHandler);
		}
		
		private function targetRemovedHandler(ctrl		: TransformController,
											  target	: ISceneNode) : void
		{
			target.added.remove(addedHandler);
			
			if (target is Group)
			{
				var targetGroup : Group = target as Group;
				
				targetGroup.descendantAdded.remove(descendantAddedHandler);
				targetGroup.descendantRemoved.remove(descendantRemovedHandler);
			}
			
			_invalidList = false;
			_target = null;
			
			_nodeToId = null;
			_transforms.length = 0;
			_flags.length = 0;
			_localToWorldTransforms.length = 0;
			_worldToLocalTransforms.length = 0;
			_numChildren.length = 0;
			_firstChildId.length = 0;
			_idToNode.length = 0;
			_parentId.length = 0;
		}
		
		private function addedHandler(target : ISceneNode, ancestor : Group) : void
		{
			// the controller will remove itself from the node when it's not its own root anymore
			// but it will watch for the 'removed' signal to add itself back if the node becomes
			// its own root again
			_target.removed.add(removedHandler);
			_target.removeController(this);
		}
		
		private function removedHandler(target : ISceneNode, ancestor : Group) : void
		{
			if (target.root == target)
			{
				target.removed.remove(removedHandler);
				target.addController(this);
			}
		}
		
		private function descendantAddedHandler(root		: Group,
												descendant	: ISceneNode) : void
		{
			_invalidList = true;
		}
		
		private function descendantRemovedHandler(root			: Group,
												  descendant	: ISceneNode) : void
		{
			_invalidList = true;
		}
		
		private function getNodeId(node : ISceneNode) : uint
		{
			if (_invalidList || !(node in _nodeToId))
				updateTransformsList();
			
			return _nodeToId[node];
		}
		
		private function updateTransformsList() : void
		{
			var root    	                : ISceneNode 			= _target.root;
			var nodes   	                : Vector.<ISceneNode> 	= new <ISceneNode>[root];
			var nodeId  	                : uint 					= 0;
			var oldNodeToId                 : Dictionary            = _nodeToId;
			var oldInitialized              : Vector.<uint>         = _flags;
			var oldLocalToWorldTransforms   : Vector.<Matrix4x4>    = _localToWorldTransforms;
			var oldWorldToLocalTransform    : Vector.<Matrix4x4>    = _worldToLocalTransforms;
			
			_nodeToId = new Dictionary(true);
			_transforms.length = 0;
			_flags = new <uint>[];
			_localToWorldTransforms = new <Matrix4x4>[];
			_worldToLocalTransforms = new <Matrix4x4>[];
			_worldToLocalUpToDate = new Vector.<Boolean>();
			_numChildren.length = 0;
			_firstChildId.length = 0;
			_idToNode.length = 0;
			_parentId.length = 1;
			_parentId[0] = -1;
			
			while (nodes.length)
			{
				var node 	    : ISceneNode 	= nodes.shift();
				var group       : Group 		= node as Group;
				var oldNodeId   : int           = oldNodeToId && node in oldNodeToId
					? oldNodeToId[node]
					: -1;
				
				_nodeToId[node] = nodeId;
				_idToNode[nodeId] = node;
				_transforms[nodeId] = node.transform;
				_worldToLocalUpToDate[nodeId] = false;
				
				if (oldNodeId >= 0 && oldNodeId < oldLocalToWorldTransforms.length)
				{
					_localToWorldTransforms[nodeId] = oldLocalToWorldTransforms[oldNodeId];
					_worldToLocalTransforms[nodeId] = oldWorldToLocalTransform[oldNodeId];
					_flags[nodeId] = oldInitialized[oldNodeId];
				}
				else
				{
					_localToWorldTransforms[nodeId] = new Matrix4x4().lock();
					_worldToLocalTransforms[nodeId] = null;
					_flags[nodeId] = FLAG_NONE;
				}
				
				if (group)
				{
					var numChildren 	: uint = group.numChildren;
					var firstChildId 	: uint = nodeId + nodes.length + 1;
					
					_numChildren[nodeId] = numChildren;
					_firstChildId[nodeId] = firstChildId;
					for (var childId : uint = 0; childId < numChildren; ++childId)
					{
						_parentId[uint(firstChildId + childId)] = nodeId;
						nodes.push(group.getChildAt(childId));
					}
				}
				else
				{
					_numChildren[nodeId] = 0;
					_firstChildId[nodeId] = 0;
				}
				
				++nodeId;
			}
			
			_worldToLocalTransforms.length = _localToWorldTransforms.length;
			_invalidList = false;
		}
		
		public function getLocalToWorldTransform(node 			: ISceneNode,
												 forceUpdate 	: Boolean = false) : Matrix4x4
		{
			var nodeId : uint = getNodeId(node);
			
			if (forceUpdate)
				updateAncestorsAndSelfLocalToWorld(nodeId);
			
			return _localToWorldTransforms[nodeId];
		}
		
		public function getWorldToLocalTransform(node           : ISceneNode,
												 forceUpdate    : Boolean   = false) : Matrix4x4
		{
			var nodeId                  : uint      = getNodeId(node);
			var worldToLocalTransform   : Matrix4x4 = _worldToLocalTransforms[nodeId];
			
			if (!worldToLocalTransform)
			{
				_worldToLocalTransforms[nodeId] = worldToLocalTransform = new Matrix4x4();
				if (!forceUpdate)
				{
					worldToLocalTransform.copyFrom(_localToWorldTransforms[nodeId]).invert();
					_flags[nodeId] |= FLAG_INIT_WORLD_TO_LOCAL;
				}
			}
			
			if (forceUpdate)
				updateAncestorsAndSelfLocalToWorld(nodeId);
			
			var flags : uint = _flags[nodeId];
			
			if (!(flags & FLAG_INIT_WORLD_TO_LOCAL))
			{
				_flags[nodeId] |= FLAG_INIT_WORLD_TO_LOCAL;
				
				if (flags & FLAG_LOCK_TRANSFORMS)
					worldToLocalTransform.lock();
				
				worldToLocalTransform
				.copyFrom(_localToWorldTransforms[nodeId])
					.invert();
				
				if (flags & FLAG_LOCK_TRANSFORMS)
					worldToLocalTransform.unlock();
			}
			
			return worldToLocalTransform;
		}
		
		public function synchronizeTransforms(node : ISceneNode, enabled : Boolean) : void
		{
			var nodeId : uint = getNodeId(node);
			
			_flags[nodeId] = enabled
				? _flags[nodeId] | FLAG_SYNCHRONIZE_TRANSFORMS
				: _flags[nodeId] & ~FLAG_SYNCHRONIZE_TRANSFORMS;
		}
		
		public function lockTransformsBeforeUpdate(node : ISceneNode, enabled : Boolean) : void
		{
			var nodeId : uint = getNodeId(node);
			
			_flags[nodeId] = enabled
				? _flags[nodeId] | FLAG_LOCK_TRANSFORMS
				: _flags[nodeId] & ~FLAG_LOCK_TRANSFORMS;
		}
	}
}