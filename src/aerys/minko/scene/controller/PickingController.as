package aerys.minko.scene.controller
{
	import flash.display.BitmapData;
	import flash.events.IEventDispatcher;
	import flash.events.MouseEvent;
	import flash.ui.Mouse;
	import flash.ui.MouseCursor;
	import flash.utils.Dictionary;
	import flash.utils.getQualifiedClassName;
	
	import aerys.minko.render.Effect;
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.Viewport;
	import aerys.minko.render.geometry.stream.format.VertexComponent;
	import aerys.minko.render.resource.Context3DResource;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.picking.PickingShader;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Mesh;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.scene.node.camera.AbstractCamera;
	import aerys.minko.type.Signal;
	import aerys.minko.type.binding.DataBindings;
	import aerys.minko.type.binding.DataProvider;
	import aerys.minko.type.enum.DataProviderUsage;
	import aerys.minko.type.enum.PickingTechnique;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Ray;
	
	public class PickingController extends EnterFrameController
	{
		private static var _pickingId				: uint			= 0;
		
		private static const PICKING_MAP			: BitmapData	= new BitmapData(1, 1, true, 0);
		private static const SHADER					: Shader		= new PickingShader();
		
		private static const EFFECT_USE_COUNTER		: Dictionary	= new Dictionary(true);
		
		private static const EVENT_NONE				: uint			= 0;
		private static const EVENT_CLICK			: uint 			= 1 << 0;
		private static const EVENT_DOUBLE_CLICK		: uint 			= 1 << 1;
		private static const EVENT_MOUSE_DOWN		: uint 			= 1 << 2;
		private static const EVENT_MOUSE_UP			: uint 			= 1 << 3;
		private static const EVENT_MOUSE_MOVE		: uint 			= 1 << 4;
		private static const EVENT_MOUSE_OVER		: uint 			= 1 << 5;
		private static const EVENT_MOUSE_OUT		: uint 			= 1 << 6;
		private static const EVENT_MOUSE_WHEEL		: uint 			= 1 << 7;
		private static const EVENT_ROLL_OVER		: uint 			= 1 << 8;
		private static const EVENT_ROLL_OUT			: uint 			= 1 << 9;
		private static const EVENT_RIGHT_CLICK		: uint 			= 1 << 10;
		private static const EVENT_RIGHT_DOWN		: uint 			= 1 << 11;
		private static const EVENT_RIGHT_UP			: uint 			= 1 << 12;
		private static const EVENT_MIDDLE_CLICK		: uint 			= 1 << 13;
		private static const EVENT_MIDDLE_DOWN		: uint 			= 1 << 14;
		private static const EVENT_MIDDLE_UP		: uint 			= 1 << 15;
		
		private static var _disableAntiAliasing		: Boolean		= false;
		private static var _previousAntiAliasing	: int			= 0;
		
        private var _technique          	: uint;
		private var _pickingRate			: Number;
		
		private var _dispatchers			: Dictionary;
        
		private var _lastPickingTime		: uint;
		private var _useHandCursor			: Boolean;
		
		private var _toDispatch				: uint;
		private var _sceneData				: DataProvider;
		private var _meshData				: Dictionary;
		private var _pickingIdToMesh		: Array;
		
		private var _mouseX					: Number;
		private var _mouseY					: Number;
		private var _mouseWheelDelta		: Number;
		private var _oldCursor				: String;
		
		private var _currentMouseOver		: Mesh;
		private var _lastMouseOver			: Mesh;
		
		private var _mouseClick				: Signal;
		private var _mouseDoubleClick		: Signal;
		private var _mouseDown				: Signal;
		private var _mouseMove				: Signal;
		private var _mouseUp				: Signal;
		private var _mouseWheel				: Signal;
		private var _mouseRollOver			: Signal;
		private var _mouseRollOut			: Signal;
		private var _mouseRightClick		: Signal;
		private var _mouseRightDown			: Signal;
		private var _mouseRightUp			: Signal;
		private var _mouseMiddleClick		: Signal;
		private var _mouseMiddleDown		: Signal;
		private var _mouseMiddleUp			: Signal;
		
		private var _tag					: uint;
		
		private var _pixelPickingIncrement	: uint;
		
		public static function get disableAntiAliasing():Boolean
		{
			return _disableAntiAliasing;
		}

		public static function set disableAntiAliasing(value:Boolean):void
		{
			_disableAntiAliasing = value;
		}

        public function get pickingRate() : Number
        {
            return _pickingRate;
        }
        public function set pickingRate(value : Number) : void
        {
            _pickingRate = value;
        }
        
		public function get useHandCursor() : Boolean
		{
			return _useHandCursor;
		}
		public function set useHandCursor(value : Boolean) : void
		{
			_useHandCursor = value;
		}
		
		public function get mouseClick() : Signal
		{
			return _mouseClick;
		}
		
		public function get mouseDoubleClick() : Signal
		{
			return _mouseDoubleClick;
		}
		
		public function get mouseDown() : Signal
		{
			return _mouseDown;
		}
		
		public function get mouseMove() : Signal
		{
			return _mouseMove;
		}
		
		public function get mouseRightClick() : Signal
		{
			return _mouseRightClick;
		}
		
		public function get mouseRightDown() : Signal
		{
			return _mouseRightDown;
		}
		
		public function get mouseRightUp() : Signal
		{
			return _mouseRightUp;
		}
		
		public function get mouseMiddleClick() : Signal
		{
			return _mouseMiddleClick;
		}
		
		public function get mouseMiddleDown() : Signal
		{
			return _mouseMiddleDown;
		}
		
		public function get mouseMiddleUp() : Signal
		{
			return _mouseMiddleUp;
		}
		
		public function get mouseUp() : Signal
		{
			return _mouseUp;
		}
		
		public function get mouseWheel() : Signal
		{
			return _mouseWheel;
		}
		
		public function get mouseRollOver() : Signal
		{
			return _mouseRollOver;
		}
		
		public function get mouseRollOut() : Signal
		{
			return _mouseRollOut;
		}
		
		public function PickingController(pickingTechnique  	: uint      = 1,
                                          pickingRate       	: Number    = 15.,
										  tag					: uint		= 1, 
										  pixelPickingIncrement	: uint		= 1)
		{
            super();
            
            _technique				= pickingTechnique;
			_pickingRate			= pickingRate;
			_tag					= tag;
			_pixelPickingIncrement	= pixelPickingIncrement;
            
			initialize();
		}
		
		private function initialize() : void
		{
			_dispatchers = new Dictionary(true);
			
			_pickingIdToMesh = [];
			_sceneData = new DataProvider({pickingProjection : new Matrix4x4()});
			_meshData = new Dictionary(true);
			
			_mouseClick			= new Signal('PickingController.mouseClick');
			_mouseDoubleClick 	= new Signal('PickingController.mouseDoubleClick');
			_mouseDown 			= new Signal('PickingController.mouseDown');
			_mouseMove 			= new Signal('PickingController.mouseMove');
			_mouseUp 			= new Signal('PickingController.mouseUp');
			_mouseWheel 		= new Signal('PickingController.mouseWheel');
			_mouseRollOver 		= new Signal('PickingController.mouseRollOver');
			_mouseRollOut 		= new Signal('PickingController.mouseRollOut');
			_mouseRightClick 	= new Signal('PickingController.mouseRightClick');
			_mouseRightDown 	= new Signal('PickingController.mouseRightDown');
			_mouseRightUp 		= new Signal('PickingController.mouseRightUp');
			_mouseMiddleClick 	= new Signal('PickingController.mouseMiddleClick');
			_mouseMiddleDown 	= new Signal('PickingController.mouseMiddleDown');
			_mouseMiddleUp 		= new Signal('PickingController.mouseMiddleUp');
			
			if (!SHADER.begin.hasCallback(cleanPickingMap))
			{
				SHADER.begin.add(cleanPickingMap);
				SHADER.end.add(updatePickingMap);
				SHADER.enabled = false;
			}
			SHADER.end.add(pickingShaderEndHandler);
		}
		
		private function addPickingToEffect(effect : Effect) : void
		{
			if (!effect)
				return ;
			
			if (!effect.hasExtraPass(SHADER))
				effect.addExtraPass(SHADER);
			
			EFFECT_USE_COUNTER[effect]++;
		}
		
		private function removePickingFromEffect(effect : Effect) : void
		{
			if (!effect)
				return ;
			
			EFFECT_USE_COUNTER[effect]--;
			
			if (EFFECT_USE_COUNTER[effect] == 0)
				effect.removeExtraPass(SHADER);
		}
		
		private function effectChangedHandler(bindings	: DataBindings,
											  property	: String,
											  oldEffect	: Effect,
											  newEffect	: Effect) : void
		{
			removePickingFromEffect(oldEffect);
			addPickingToEffect(newEffect);
		}
		
		override protected function sceneEnterFrameHandler(scene		: Scene,
														   viewport		: Viewport,
														   destination	: BitmapData,
														   time			: Number) : void
		{
			if (!_dispatchers[viewport])
				bindDefaultInputs(viewport);
			
			_previousAntiAliasing = viewport.antiAliasing;
			
			// toggle picking pass
			if (time - _lastPickingTime > 1000. / _pickingRate && _toDispatch != EVENT_NONE)
			{
                // raycast
                if (_technique & PickingTechnique.RAYCASTING)
                {
                    var ray					: Ray       = null;
                    var hit					: Boolean   = false;
                    var minDistance			: Number	= Number.POSITIVE_INFINITY;				
					
					var currentMouseOver	: Mesh		= _currentMouseOver;
					
                    for (var targetId : uint = 0; targetId < numTargets && !hit; ++targetId)
                    {
                        var target  : ISceneNode    = getTarget(targetId);
                        var scene   : Scene         = target.root as Scene;
                        
                        if (scene && scene.activeCamera)
                        {
                            if (!ray)
                            {
                                var camera  : AbstractCamera    = scene.activeCamera;
                                
                                ray ||= camera.unproject(_mouseX, _mouseY);
                            }
                            
                            if (target is Mesh)
                            {
								var mesh : Mesh = target as Mesh;
								var distance : Number = mesh.cast(ray, Number.POSITIVE_INFINITY, _tag);
                                if (distance > 0.)
                                {
									if (!(_technique & PickingTechnique.PIXEL_PICKING) && distance < minDistance)
									{
										minDistance = distance;
										_lastMouseOver = currentMouseOver;
										_currentMouseOver = mesh;
									}
                                    hit = true;
                                }
                            }
                            else if (target is Group)
                            {
                                var group   : Group                 = target as Group;
                                var hits    : Vector.<ISceneNode>   = group.cast(ray, Number.POSITIVE_INFINITY, _tag);
                                
                                if (hits.length > 0)
                                {
									if (!(_technique & PickingTechnique.PIXEL_PICKING))
									{
										_lastMouseOver = currentMouseOver;
										if (Mesh(hits[0]).cast(ray, Number.POSITIVE_INFINITY, _tag) < minDistance)
										{
											minDistance = distance
											_currentMouseOver = hits[0] as Mesh;
										}
									}
                                    hit = true;
                                }
                            }
                        }
                    }
                    
                    if (hit
                        && (_technique & PickingTechnique.RAYCASTING_GEOMETRY) != 0
                        && _currentMouseOver.geometry.cast(ray, _currentMouseOver.getWorldToLocalTransform()) < 0)
                    {
                        _currentMouseOver = null;
                        hit = false;
                    }
                    
                    if (!hit)
                    {
                        _lastMouseOver = _currentMouseOver;
                        _currentMouseOver = null;
                        updateMouseCursor();
                        executeSignals();
                        
                        return;
                    }
                }
                
                // pixel picking
                if (_technique & PickingTechnique.PIXEL_PICKING)
                {
    				// update picking projection to get the picked pixel in (0, 0)
    				var projection : Matrix4x4 = _sceneData.pickingProjection;
    				
    				projection.lock();
					scene.activeCamera.getProjection(projection);
    				
    				var rawData : Vector.<Number> = projection.getRawData();
    				
    				rawData[8] = -_mouseX / viewport.width * 2.;
    				rawData[9] = _mouseY / viewport.height * 2.;
    				
    				projection.setRawData(rawData);
    				projection.unlock();
    				
    				SHADER.enabled = true;
    				_lastPickingTime = time;
                }
                else
                {
                    updateMouseCursor();
                    executeSignals();
                }
			}
		}
		
		private static function cleanPickingMap(shader		: Shader,
												context		: Context3DResource,
												backBuffer	: RenderTarget) : void
		{
			if (_disableAntiAliasing)
				context.configureBackBuffer(backBuffer.width, backBuffer.height, 0, true);
			
			context.clear(0, 0, 0, 0);
		}
		
		private static function updatePickingMap(shader		: Shader,
												 context	: Context3DResource,
												 backBuffer	: RenderTarget) : void
		{
			var color	: uint	= backBuffer.backgroundColor;
			
			context.drawToBitmapData(PICKING_MAP);
			context.clear(
				(color >>> 24) / 255.,
				((color >> 16) & 0xff) / 255.,
				((color >> 8) & 0xff) / 255.,
				(color & 0xff) / 255.
			);
			
			if (_disableAntiAliasing)
			{
				context.configureBackBuffer(backBuffer.width, backBuffer.height, _previousAntiAliasing, true);
				context.clear(0, 0, 0, 0);
			}
			
			SHADER.enabled = false;
		}
		
		private function pickingShaderEndHandler(shader		: Shader,
												 context	: Context3DResource,
												 backBuffer	: RenderTarget) : void
		{
			updateMouseOverElement();
			updateMouseCursor();
			executeSignals();
		}
		
		private function updateMouseOverElement() : void
		{
			var pixelColor : uint = PICKING_MAP.getPixel32(0, 0);
			
			_lastMouseOver = _currentMouseOver;
			if ((pixelColor >>> 24) == 0xFF)
			{
				var mesh	: Mesh	= _pickingIdToMesh[pixelColor & 0xFFFFFF];
				
				_currentMouseOver = mesh && (mesh.tag & _tag) != 0 ? mesh : null;
			}
			else
				_currentMouseOver = null; // wrong antialiasing color or nothing got picked
		}
		
		private function updateMouseCursor() : void
		{
			if (_currentMouseOver != null)
			{
				if (_useHandCursor && _oldCursor == null)
				{
					_oldCursor = Mouse.cursor;
					Mouse.cursor = MouseCursor.HAND;
				}
			}
			else if (_oldCursor)
			{
				Mouse.cursor = _oldCursor;
				_oldCursor = null;
			}
		}
		
		private function executeSignals() : void
		{
			if ((_toDispatch & EVENT_MOUSE_OUT) && _lastMouseOver != null
				&& _currentMouseOver != _lastMouseOver)
				_mouseRollOut.execute(this, _lastMouseOver, _mouseX, _mouseY);
			
			if ((_toDispatch & EVENT_MOUSE_OVER) && _currentMouseOver != null
				&& _currentMouseOver != _lastMouseOver)
				_mouseRollOver.execute(this, _currentMouseOver, _mouseX, _mouseY);
			
			if (_toDispatch & EVENT_MOUSE_UP)
				_mouseUp.execute(this, _currentMouseOver, _mouseX, _mouseY);
			
			if (_toDispatch & EVENT_MOUSE_DOWN)
				_mouseDown.execute(this, _currentMouseOver, _mouseX, _mouseY);
			
			if (_toDispatch & EVENT_CLICK)
				_mouseClick.execute(this, _currentMouseOver, _mouseX, _mouseY);
			
			if (_toDispatch & EVENT_DOUBLE_CLICK)
				_mouseDoubleClick.execute(this, _currentMouseOver, _mouseX, _mouseY);
			
			if (_toDispatch & EVENT_MOUSE_WHEEL)
				_mouseWheel.execute(this, _currentMouseOver, _mouseX, _mouseY, _mouseWheelDelta);
			
			if (_toDispatch & EVENT_MOUSE_MOVE)
				_mouseMove.execute(this, _currentMouseOver, _mouseX, _mouseY);
			
			if (_toDispatch & EVENT_RIGHT_CLICK)
				_mouseRightClick.execute(this, _currentMouseOver, _mouseX, _mouseY);
			
			if (_toDispatch & EVENT_RIGHT_DOWN)
				_mouseRightDown.execute(this, _currentMouseOver, _mouseX, _mouseY);
			
			if (_toDispatch & EVENT_RIGHT_UP)
				_mouseRightUp.execute(this, _currentMouseOver, _mouseX, _mouseY);
			
			if (_toDispatch & EVENT_MIDDLE_CLICK)
				_mouseMiddleClick.execute(this, _currentMouseOver, _mouseX, _mouseY);
			
			if (_toDispatch & EVENT_MIDDLE_DOWN)
				_mouseMiddleDown.execute(this, _currentMouseOver, _mouseX, _mouseY);
			
			if (_toDispatch & EVENT_MIDDLE_UP)
				_mouseMiddleUp.execute(this, _currentMouseOver, _mouseX, _mouseY);
			
			_toDispatch = EVENT_NONE;
		}
		
		public function bindDefaultInputs(dispatcher : IEventDispatcher) : void
		{
			_dispatchers[dispatcher] = true;
			
			dispatcher.addEventListener(MouseEvent.MOUSE_DOWN, mouseDownHandler);
			dispatcher.addEventListener(MouseEvent.MOUSE_UP, mouseUpHandler);
			dispatcher.addEventListener(MouseEvent.CLICK, clickHandler);
			dispatcher.addEventListener(MouseEvent.DOUBLE_CLICK, doubleClickHandler);
			dispatcher.addEventListener(MouseEvent.MOUSE_MOVE, mouseMoveHandler);
			dispatcher.addEventListener(MouseEvent.MOUSE_WHEEL, mouseWheelHandler);
			
			if (MouseEvent.RIGHT_CLICK)
			{
				dispatcher.addEventListener(MouseEvent.RIGHT_CLICK, mouseRightClickHandler);
				dispatcher.addEventListener(MouseEvent.RIGHT_MOUSE_DOWN, mouseRightDownHandler);
				dispatcher.addEventListener(MouseEvent.RIGHT_MOUSE_UP, mouseRightUpHandler);
			}
			
			if (MouseEvent.MIDDLE_CLICK)
			{
				dispatcher.addEventListener(MouseEvent.MIDDLE_CLICK, mouseMiddleClickHandler);
				dispatcher.addEventListener(MouseEvent.MIDDLE_MOUSE_DOWN, mouseMiddleDownHandler);
				dispatcher.addEventListener(MouseEvent.MIDDLE_MOUSE_UP, mouseMiddleUpHandler);
			}
		}
		
		public function unbindDefaultInputs(dispatcher : IEventDispatcher) : void
		{
			delete _dispatchers[dispatcher];
			
			dispatcher.removeEventListener(MouseEvent.MOUSE_DOWN, mouseDownHandler);
			dispatcher.removeEventListener(MouseEvent.MOUSE_UP, mouseUpHandler);
			dispatcher.removeEventListener(MouseEvent.CLICK, clickHandler);
			dispatcher.removeEventListener(MouseEvent.DOUBLE_CLICK,	doubleClickHandler);
			dispatcher.removeEventListener(MouseEvent.MOUSE_MOVE, mouseMoveHandler);
			dispatcher.removeEventListener(MouseEvent.MOUSE_WHEEL, mouseWheelHandler);
			
			if (MouseEvent.RIGHT_CLICK)
			{
				dispatcher.removeEventListener(MouseEvent.RIGHT_CLICK, mouseRightClickHandler);
				dispatcher.removeEventListener(MouseEvent.RIGHT_MOUSE_DOWN, mouseRightDownHandler);
				dispatcher.removeEventListener(MouseEvent.RIGHT_MOUSE_UP, mouseRightUpHandler);
			}
			
			if (MouseEvent.MIDDLE_CLICK)
			{
				dispatcher.removeEventListener(MouseEvent.MIDDLE_CLICK, mouseMiddleClickHandler);
				dispatcher.removeEventListener(MouseEvent.MIDDLE_MOUSE_DOWN, mouseMiddleDownHandler);
				dispatcher.removeEventListener(MouseEvent.MIDDLE_MOUSE_UP, mouseMiddleUpHandler);
			}
		}
		
		override protected function targetAddedToScene(target : ISceneNode, scene : Scene) : void
		{
			super.targetAddedToScene(target, scene);
			
			if (!scene.bindings.propertyExists('pickingProjection'))
				scene.bindings.addProvider(_sceneData);
			
			if (!addSceneNode(target))
				throw new Error('Invalid target type: ' + getQualifiedClassName(target));
		}
		
		override protected function targetRemovedFromScene(target 	: ISceneNode,
														   scene 	: Scene) : void
		{
			super.targetRemovedFromScene(target, scene);
			
			removeSceneNode(target);
		}
		
		private function addSceneNode(node : ISceneNode, watchDescendants : Boolean = true) : Boolean
		{
			if (node is Mesh)
				addMesh(node as Mesh);
			else if (node is Group)
				addGroup(node as Group, watchDescendants);
			else			
				return false;
			
			return true;
		}
		
		private function removeSceneNode(node : ISceneNode, unwatchDescendants : Boolean = true) : void
		{
			if (node is Mesh)
				removeMesh(node as Mesh);
			else if (node is Group)
				removeGroup(node as Group, unwatchDescendants);
		}
		
		private function addMesh(mesh : Mesh) : void
		{
			_pickingId += _pixelPickingIncrement;
			_pickingIdToMesh[_pickingId] = mesh;
			
			var meshData : DataProvider = new DataProvider(
				{pickingId : _pickingId},
				'pickingData',
				DataProviderUsage.EXCLUSIVE
			);
			
			_meshData[mesh] = meshData;
			mesh.bindings.addProvider(meshData);
			mesh.bindings.addCallback('effect', effectChangedHandler);
			
			if (mesh.material && mesh.material.effect && mesh.geometry.getVertexStream(0).format.hasComponent(VertexComponent.XYZ))
				addPickingToEffect(mesh.material.effect);
		}
		
		private function removeMesh(mesh : Mesh) : void
		{
			if (mesh.material && mesh.material.effect)
				removePickingFromEffect(mesh.material.effect);
			
			mesh.bindings.removeCallback('effect', effectChangedHandler);
			mesh.bindings.removeProvider(_meshData[mesh]);
			
			_pickingIdToMesh[_pickingIdToMesh.indexOf(mesh)] = null;
			delete _meshData[mesh];
		}
		
		private function addGroup(group : Group, watchDescendants : Boolean = true) : void
		{
			var meshes 		: Vector.<ISceneNode> 	= group.getDescendantsByType(Mesh);
			var numMeshes 	: uint 					= meshes.length;
			
			if (watchDescendants)
			{
				group.descendantAdded.add(groupDescendantAddedHandler);
				group.descendantRemoved.add(groupDescendantRemovedHandler);
			}
			
			for (var meshId : uint = 0; meshId < numMeshes; ++meshId)
				addMesh(meshes[meshId] as Mesh);
		}
		
		private function removeGroup(group : Group, unwatchDescendants : Boolean = true) : void
		{
			var meshes 		: Vector.<ISceneNode> 	= group.getDescendantsByType(Mesh);
			var numMeshes 	: uint 					= meshes.length;
			
			if (unwatchDescendants)
			{
				group.descendantAdded.remove(groupDescendantAddedHandler);
				group.descendantRemoved.remove(groupDescendantRemovedHandler);
			}
			
			for (var meshId : uint = 0; meshId < numMeshes; ++meshId)
				removeMesh(meshes[meshId] as Mesh);
		}
		
		private function groupDescendantAddedHandler(group : Group, descendant : ISceneNode) : void
		{
			addSceneNode(descendant, false);
		}
		
		private function groupDescendantRemovedHandler(group : Group, descendant : ISceneNode) : void
		{
			removeSceneNode(descendant, false);
		}
		
		private function mouseUpHandler(e : MouseEvent) : void
		{
			if (_mouseUp.numCallbacks == 0)
				return ;
			
			_toDispatch |= EVENT_MOUSE_UP;
			_mouseX = e.localX;
			_mouseY = e.localY;
		}
		
		private function mouseDownHandler(e : MouseEvent) : void
		{
			if (_mouseDown.numCallbacks == 0)
				return ;
			
			_toDispatch |= EVENT_MOUSE_DOWN;	
			_mouseX = e.localX;
			_mouseY = e.localY;
		}
		
		private function clickHandler(e : MouseEvent) : void
		{
			if (_mouseClick.numCallbacks == 0)
				return ;
			
			_toDispatch |= EVENT_CLICK;
			_mouseX = e.localX;
			_mouseY = e.localY;
		}
		
		private function doubleClickHandler(e : MouseEvent) : void
		{
			if (_mouseDoubleClick.numCallbacks == 0)
				return ;
			
			_toDispatch |= EVENT_DOUBLE_CLICK;
			_mouseX = e.localX;
			_mouseY = e.localY;
		}
		
		private function mouseMoveHandler(e : MouseEvent) : void
		{
			if (_mouseMove.numCallbacks != 0 || _mouseRollOver.numCallbacks != 0
				|| _mouseRollOut.numCallbacks != 0)
			{
				_toDispatch |= EVENT_MOUSE_MOVE | EVENT_MOUSE_OVER | EVENT_MOUSE_OUT;
				_mouseX = e.localX;
				_mouseY = e.localY;
			}
			else if (_useHandCursor)
			{
				_toDispatch |= EVENT_MOUSE_MOVE;
				_mouseX = e.localX;
				_mouseY = e.localY;
			}
		}
		
		private function mouseWheelHandler(e : MouseEvent) : void
		{
			if (_mouseWheel.numCallbacks == 0)
				return ;
			
			_toDispatch |= EVENT_MOUSE_WHEEL;
			_mouseWheelDelta = e.delta;
			_mouseX = e.localX;
			_mouseY = e.localY;
		}
		
		private function mouseRightClickHandler(e : MouseEvent) : void
		{
			if (_mouseRightClick.numCallbacks == 0)
				return ;
			
			_toDispatch |= EVENT_RIGHT_CLICK;
			_mouseX = e.localX;
			_mouseY = e.localY;
		}
		
		private function mouseRightDownHandler(e : MouseEvent) : void
		{
			if (_mouseRightDown.numCallbacks == 0)
				return ;
			
			_toDispatch |= EVENT_RIGHT_DOWN;
			_mouseX = e.localX;
			_mouseY = e.localY;
		}
		
		private function mouseRightUpHandler(e : MouseEvent) : void
		{
			if (_mouseRightUp.numCallbacks == 0)
				return ;
			
			_toDispatch |= EVENT_RIGHT_UP;
			_mouseX = e.localX;
			_mouseY = e.localY;
		}
		
		private function mouseMiddleClickHandler(e : MouseEvent) : void
		{
			if (_mouseMiddleClick.numCallbacks == 0)
				return ;
			
			_toDispatch |= EVENT_MIDDLE_CLICK;
			_mouseX = e.localX;
			_mouseY = e.localY;
		}
		
		private function mouseMiddleDownHandler(e : MouseEvent) : void
		{
			if (_mouseMiddleDown.numCallbacks == 0)
				return ;
			
			_toDispatch |= EVENT_MIDDLE_DOWN;
			_mouseX = e.localX;
			_mouseY = e.localY;
		}
		
		private function mouseMiddleUpHandler(e : MouseEvent) : void
		{
			if (_mouseMiddleUp.numCallbacks == 0)
				return ;
			
			_toDispatch |= EVENT_MIDDLE_UP;
			_mouseX = e.localX;
			_mouseY = e.localY;
		}
	}
}
