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
    
    import mx.olap.aggregators.MaxAggregator;
    
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
        private static const INIT_NONE                  : uint  = 0;
        private static const INIT_LOCAL_TO_WORLD        : uint  = 1;
        private static const INIT_WORLD_TO_LOCAL        : uint  = 2;
        
        private var _target								: ISceneNode;
        
        private var _invalidList						: Boolean;
        
        private var _nodeToId							: Dictionary;
        private var _idToNode							: Vector.<ISceneNode>
        private var _transforms							: Vector.<Matrix4x4>;
        private var _initialized	                    : Vector.<uint>;
        private var _localToWorldTransforms 			: Vector.<Matrix4x4>;
        private var _worldToLocalTransforms 			: Vector.<Matrix4x4>;
        private var _numChildren						: Vector.<uint>;
        private var _firstChildId						: Vector.<uint>;
        private var _parentId							: Vector.<int>;
        
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
                updateLocalToWorld();
        }
        
        private function updateLocalToWorld(nodeId : uint = 0, subtreeOnly : Boolean = false) : void
        {
            var numNodes 			: uint 			= _transforms.length;
            var childrenOffset		: uint			= 1;
            var rootLocalToWorld	: Matrix4x4		= _localToWorldTransforms[nodeId];
            var rootTransform		: Matrix4x4		= _transforms[nodeId];
            var root				: ISceneNode	= _idToNode[childId];
            var subtreeMax          : uint          = nodeId;
            
            if (rootTransform._hasChanged || _initialized[nodeId] == INIT_NONE)
            {
                rootLocalToWorld.copyFrom(rootTransform);
                
                if (nodeId != 0)
                    rootLocalToWorld.append(_localToWorldTransforms[_parentId[nodeId]]);
                
                rootTransform._hasChanged = false;
                _initialized[nodeId] = INIT_LOCAL_TO_WORLD;
                root.localToWorldTransformChanged.execute(root, rootLocalToWorld);
            }

            if (lastChildId > subtreeMax)
                subtreeMax = lastChildId;
            
            while (nodeId < numNodes)
            {
                var localToWorld 	: Matrix4x4	= _localToWorldTransforms[nodeId];
                var numChildren		: uint		= _numChildren[nodeId];
                var firstChildId	: uint		= _firstChildId[nodeId];
                var lastChildId		: uint		= firstChildId + numChildren;
                var isDirty			: Boolean	= localToWorld._hasChanged;
                
                localToWorld._hasChanged = false;
                
                for (var childId : uint = firstChildId; childId < lastChildId; ++childId)
                {
                    var childTransform		: Matrix4x4		= _transforms[childId];
                    var childLocalToWorld	: Matrix4x4		= _localToWorldTransforms[childId];
                    var childIsDirty		: Boolean		= isDirty || childTransform._hasChanged
                        || !_initialized[childId];
                    
                    if (childIsDirty)
                    {
                        var child	: ISceneNode	= _idToNode[childId];
                        
                        childLocalToWorld
                            .copyFrom(childTransform)
                            .append(localToWorld);
                        
                        childTransform._hasChanged = false;
                        _initialized[childId] = INIT_LOCAL_TO_WORLD;
                        child.localToWorldTransformChanged.execute(child, childLocalToWorld);
                    }
                }
                
                if (subtreeOnly && nodeId && nodeId >= subtreeMax)
                {
                    var parentId : uint = _parentId[nodeId];
                    
                    nodeId = _firstChildId[parentId];
                    while (!_numChildren[nodeId] && nodeId < subtreeMax)
                        ++nodeId;
                    
                    if (nodeId >= subtreeMax)
                        return ;
                    
                    nodeId = _firstChildId[nodeId];
                }
                else
                    ++nodeId;
            }
        }
        
        private function updateAncestorsAndSelfLocalToWorld(nodeId : int) : void
        {
            var dirtyRoot : int = -1;
            
            while (nodeId >= 0)
            {
                if ((_transforms[nodeId] as Matrix4x4)._hasChanged || !_initialized[nodeId])
                    dirtyRoot = nodeId;
                
                nodeId = _parentId[nodeId];
            }
            
            if (dirtyRoot >= 0)
                updateLocalToWorld(dirtyRoot, true);
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
            _initialized = null;
            _localToWorldTransforms = null;
            _worldToLocalTransforms = null;
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
            var root    	                : ISceneNode 			= _target.root;
            var nodes   	                : Vector.<ISceneNode> 	= new <ISceneNode>[root];
            var nodeId  	                : uint 					= 0;
            var oldNodeToId                 : Dictionary            = _nodeToId;
            var oldInitialized              : Vector.<uint>         = _initialized;
            var oldLocalToWorldTransforms   : Vector.<Matrix4x4>    = _localToWorldTransforms;
            var oldWorldToLocalTransform    : Vector.<Matrix4x4>    = _worldToLocalTransforms;
            
            _nodeToId = new Dictionary(true);
            _transforms = new <Matrix4x4>[];
            _initialized = new <uint>[];
            _localToWorldTransforms = new <Matrix4x4>[];
            _worldToLocalTransforms = new <Matrix4x4>[];
            _numChildren = new <uint>[];
            _firstChildId = new <uint>[];
            _idToNode = new <ISceneNode>[];
            _parentId = new <int>[-1];
            
            while (nodes.length)
            {
                var node 	: ISceneNode 	= nodes.shift();
                var group   : Group 		= node as Group;
                
                _nodeToId[node] = nodeId;
                _idToNode[nodeId] = node;
                _transforms[nodeId] = node.transform;
                
                if (oldNodeToId && node in oldNodeToId)
                {
                    var oldNodeId   : uint  = oldNodeToId[node];
                    
                    _localToWorldTransforms[nodeId] = oldLocalToWorldTransforms[oldNodeId];
                    _worldToLocalTransforms[nodeId] = oldWorldToLocalTransform[oldNodeId];
                    _initialized[nodeId] = oldInitialized[oldNodeId];
                }
                else
                {
                    _localToWorldTransforms[nodeId] = new Matrix4x4().lock();
                    _worldToLocalTransforms[nodeId] = null;
                    _initialized[nodeId] = INIT_NONE;
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
            if (_invalidList || _nodeToId[node] == undefined)
                updateTransformsList();
            
            var nodeId : uint = _nodeToId[node];
            
            if (forceUpdate)
                updateAncestorsAndSelfLocalToWorld(nodeId);
            
            return _localToWorldTransforms[nodeId];
        }
        
        public function getWorldToLocalTransform(node           : ISceneNode,
                                                 forceUpdate    : Boolean   = false) : Matrix4x4
        {
            if (_invalidList || _nodeToId[node] == undefined)
                updateTransformsList();
            
            var nodeId                  : uint      = _nodeToId[node];
            var worldToLocalTransform   : Matrix4x4 = _worldToLocalTransforms[nodeId];
            
            if (!worldToLocalTransform)
            {
                _worldToLocalTransforms[nodeId] = worldToLocalTransform = new Matrix4x4();
                if (!forceUpdate)
                {
                    worldToLocalTransform.copyFrom(_localToWorldTransforms[nodeId]).invert();
                    _initialized[nodeId] |= INIT_WORLD_TO_LOCAL;
                }
            }
            
            if (forceUpdate)
                updateAncestorsAndSelfLocalToWorld(nodeId);
            
            if (!(_initialized[nodeId] & INIT_WORLD_TO_LOCAL))
            {
                _initialized[nodeId] |= INIT_WORLD_TO_LOCAL;
                
                worldToLocalTransform
                    .copyFrom(_localToWorldTransforms[nodeId])
                    .invert();
            }
            
            return worldToLocalTransform;
        }
    }
}