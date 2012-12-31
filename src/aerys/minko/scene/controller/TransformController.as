package aerys.minko.scene.controller
{
	import aerys.minko.ns.minko_math;
	import aerys.minko.render.Viewport;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.math.Matrix4x4;
	
	import flash.display.BitmapData;
	import flash.utils.Dictionary;

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
		private var _target					: ISceneNode;
		
		private var _invalidList			: Boolean;
		
		private var _nodeToId				: Dictionary;
		private var _idToNode				: Vector.<ISceneNode>
		private var _transforms				: Vector.<Matrix4x4>;
		private var _localToWorldTransforms : Vector.<Matrix4x4>;
		private var _numChildren			: Vector.<uint>;
		private var _firstChildId			: Vector.<uint>
		private var _parentId				: Vector.<int>
		
		public function TransformController()
		{
			super();
			
			targetAdded.add(targetAddedHandler);
			targetRemoved.add(targetRemovedHandler);
		}
		
		private function renderingBeginHandler(scene			: Scene,
										  viewport		: Viewport,
										  destination	: BitmapData,
										  time			: Number) : void
		{
			if (_invalidList)
				updateTransformsList();
			
			if (_transforms.length)
			{
				var rootTransform 	: Matrix4x4	= _transforms[0];
				var isDirty			: Boolean	= rootTransform._hasChanged;
				
				if (isDirty)
				{
					_localToWorldTransforms[0].copyFrom(rootTransform);
					rootTransform._hasChanged = false;
				}
				
				updateLocalToWorld();
			}
		}
		
		private function updateLocalToWorld(nodeId : uint = 0) : void
		{
			var numNodes 			: uint 			= _transforms.length;
			var childrenOffset		: uint			= 1;
			var rootLocalToWorld	: Matrix4x4		= _localToWorldTransforms[nodeId];
			var rootTransform		: Matrix4x4		= _transforms[nodeId];
			var root				: ISceneNode	= _idToNode[childId];
			
			if (rootTransform._hasChanged)
			{
				rootLocalToWorld.copyFrom(rootTransform);
				
				if (nodeId != 0)
					rootLocalToWorld.append(_transforms[_parentId[nodeId]]);
				
				root.localToWorldTransformChanged.execute(root, rootLocalToWorld);
			}

			for (; nodeId < numNodes; ++nodeId)
			{
				var localToWorld 	: Matrix4x4	= _localToWorldTransforms[nodeId];
				var numChildren		: uint		= _numChildren[nodeId];
				var isDirty			: Boolean	= localToWorld._hasChanged;
				var firstChildId	: uint		= _firstChildId[nodeId];
				var lastChildId		: uint		= firstChildId + numChildren;
				
				localToWorld._hasChanged = false;
				
				for (var childId : uint = firstChildId; childId < lastChildId; ++childId)
				{
					var childTransform		: Matrix4x4		= _transforms[childId];
					var childLocalToWorld	: Matrix4x4		= _localToWorldTransforms[childId];
					var childIsDirty		: Boolean		= isDirty || childTransform._hasChanged;
					
					if (childIsDirty)
					{
						var child	: ISceneNode	= _idToNode[childId];
						
						childLocalToWorld
							.copyFrom(childTransform)
							.append(localToWorld);
						
						childTransform._hasChanged = false;

						child.localToWorldTransformChanged.execute(child, childLocalToWorld);
					}
				}
			}
		}
		
		private function updateAncestorsAndSelfLocalToWorld(nodeId : int) : void
		{
			var dirtyRoot : int = nodeId;
			
			while (nodeId >= 0)
			{
				if ((_transforms[nodeId] as Matrix4x4)._hasChanged)
					dirtyRoot = nodeId;
				
				nodeId = _parentId[nodeId];
			}
			
			if (dirtyRoot >= 0)
				updateLocalToWorld(dirtyRoot);
		}
		
		private function targetAddedHandler(ctrl	: TransformController,
											target	: ISceneNode) : void
		{
			if (_target)
				throw new Error('The TransformController cannot have more than one target.');
			
			_target = target;
			_invalidList = true;
			
			if (target is Scene)
			{
				(target as Scene).renderingBegin.add(renderingBeginHandler);
				return;
			}
		
			if (target is Group)
			{
				var targetGroup : Group = target as Group;
				
				targetGroup.descendantAdded.add(descendantAddedHandler);
				targetGroup.descendantRemoved.add(descendantRemovedHandler);
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
			_transforms = null;
			_localToWorldTransforms = null;
			_numChildren = null;
			_idToNode = null;
			_parentId = null;
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
		
		private function updateTransformsList() : void
		{
			var root 	: ISceneNode 			= _target.root;
			var nodes 	: Vector.<ISceneNode> 	= new <ISceneNode>[root];
			var nodeId 	: uint 					= 0;
			
			_nodeToId = new Dictionary(true);
			_transforms = new <Matrix4x4>[];
			_localToWorldTransforms = new <Matrix4x4>[];
			_numChildren = new <uint>[];
			_firstChildId = new <uint>[];
			_idToNode = new <ISceneNode>[];
			_parentId = new <int>[-1];

			while (nodes.length)
			{
				var node 	: ISceneNode 	= nodes.shift();
				var group 	: Group 		= node as Group;
				
				_nodeToId[node] = nodeId;
				_idToNode[nodeId] = node;
				_transforms[nodeId] = node.transform;
				_localToWorldTransforms[nodeId] = new Matrix4x4().lock();
				
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
			
			_invalidList = false;
		}
		
		public function getLocalToWorldTransform(node 			: ISceneNode,
												 forceUpdate 	: Boolean = false) : Matrix4x4
		{
			if (_invalidList || _nodeToId[node] == undefined)
				updateTransformsList();
			
			var nodeId : uint = _nodeToId[node];
			
			if (forceUpdate)
				updateAncestorsAndSelfLocalToWorld(nodeId);
			
			return _localToWorldTransforms[nodeId];
		}
	}
}