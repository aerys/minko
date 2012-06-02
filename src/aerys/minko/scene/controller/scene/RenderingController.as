package aerys.minko.scene.controller.scene
{
	import aerys.minko.Minko;
	import aerys.minko.ns.minko_render;
	import aerys.minko.render.DrawCall;
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.Viewport;
	import aerys.minko.render.effect.Effect;
	import aerys.minko.render.resource.Context3DResource;
	import aerys.minko.render.resource.texture.TextureResource;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.ShaderInstance;
	import aerys.minko.render.shader.Signature;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.scene.node.mesh.Mesh;
	import aerys.minko.scene.node.mesh.geometry.Geometry;
	import aerys.minko.scene.node.mesh.geometry.primitive.QuadGeometry;
	import aerys.minko.type.Factory;
	import aerys.minko.type.Sort;
	import aerys.minko.type.data.DataBindings;
	import aerys.minko.type.data.DataProvider;
	import aerys.minko.type.log.DebugLevel;
	
	import flash.display.BitmapData;
	import flash.utils.Dictionary;
	
	/**
	 * The RenderingController works on the scene to issue all the related
	 * rendering operations and push them in the rendering list.
	 * 
	 * It can only be set on a single scene object, and will raise an exception otherwise.
	 * 
	 * @author Jean-Marc Le Roux
	 * @author Romain Gilliotte
	 */
	public final class RenderingController extends AbstractController
	{
		use namespace minko_render;
		
		private static const TMP_MESHES 	: Vector.<ISceneNode> 	= new <ISceneNode>[];
		private static const TMP_NUMBERS	: Vector.<Number>		= new <Number>[];
		
		private var _scene						: Scene			= null;
		
		private var _numTriangles				: uint			= 0;
		
		private var _stashedPropertyChanges		: Dictionary	= null;
		
		private var _passes						: Array			= null;
		private var _passesAreSorted			: Boolean		= false;
		
		private var _drawCallToPassInstance		: Dictionary	= null;
		private var _passInstanceToDrawCalls	: Dictionary	= null;
		
		private var _meshToDrawCalls			: Dictionary	= null;
		private var _drawCallToMeshBindings		: Dictionary	= null;
		
		private var _postProcessingBackBuffer	: RenderTarget	= null;
		private var _postProcessingEffect		: Effect		= null;
		private var _postProcessingScene		: Scene			= null;
		private var _postProcessingProperties	: DataProvider	= new DataProvider();
		
		private var _lastViewportWidth			: Number		= 0;
		private var _lastViewportHeight			: Number		= 0;
		
		/**
		 * Index meshes by their own databinding.
		 *   * this is required to handle meshBindings.properyChange without
		 *		having to check all meshes.
		 * 
		 * Another way would be to put a field 'associatedSceneNode' into
		 * the associated DataBindings object, as there is always a 1-1 relationship
		 * between an ISceneNode and DataBindings instance.
		 * 
		 * However, we don't want to trust AVM to handle circular references properly.
		 */
		private var _meshBindingsToMesh			: Dictionary				= null;
		
		/**
		 * Index meshes by their effect.
		 *  * this is useless when the effect of a mesh is swapped by another one
		 *		- The user executes: mesh.effect = xyz
		 *		- and then: mesh.effectChanged is executed
		 * 
		 *   * it is required when the effect of a mesh is changed
		 *		- a dynamic effect removes a pass
		 *		- and then: mesh.effect.changed is executed
		 */
		private var _effectToMeshes				: Dictionary				= null;
		private var _numStashedPropertyChanges	: int						= 0;
		
		public function get numTriangles() : uint
		{
			return _numTriangles;
		}
		
		public function get postProcessingEffect() : Effect
		{
			return _postProcessingEffect;
		}
		public function set postProcessingEffect(value : Effect) : void
		{
			_postProcessingEffect = value;
			
			initializePostProcessing();
		}
		
		public function get postProcessingProperties() : DataProvider
		{
			return _postProcessingProperties;
		}
		
		public function RenderingController()
		{
			super(Scene);
			
			initialize();
		}
		
		private function initialize() : void
		{
			_passes						= [];
			_passesAreSorted			= true;
			
			_drawCallToPassInstance		= new Dictionary();
			_drawCallToMeshBindings		= new Dictionary();
			_passInstanceToDrawCalls	= new Dictionary();
			_meshToDrawCalls			= new Dictionary();
			_effectToMeshes				= new Dictionary();
			_meshBindingsToMesh			= new Dictionary();
			_stashedPropertyChanges		= new Dictionary();
			
			targetAdded.add(targetAddedHandler);
			targetRemoved.add(targetRemovedHandler);
		}
		
		private function initializePostProcessing() : void
		{
			if (_postProcessingEffect)
			{
				if (!_postProcessingScene)
				{
					_postProcessingScene = new Scene(
						new Mesh(
							new QuadGeometry(),
							null,
							_postProcessingEffect
						)
					);
					
					_postProcessingScene.bindings.addProvider(
						_postProcessingProperties
					);
				}
				else
				{
					var screen : Mesh = Mesh(_postProcessingScene.getChildAt(0));
					
					screen.effect = _postProcessingEffect;
				}
			}
		}
		
		private function getRenderingBackBuffer(backBuffer : RenderTarget) : RenderTarget
		{
			if (_postProcessingEffect)
			{
				var w 		: int 	= 1 << Math.ceil(Math.log(backBuffer.width) * Math.LOG2E);
				var h 		: int 	= 1 << Math.ceil(Math.log(backBuffer.height) * Math.LOG2E);
				var bgcolor : uint 	= backBuffer.backgroundColor;
				
				if (!_postProcessingBackBuffer)
				{
					_postProcessingBackBuffer = new RenderTarget(
						w,
						h,
						new TextureResource(w, h),
						0,
						bgcolor
					);
					
					_postProcessingProperties.setProperty(
						'backBuffer',
						_postProcessingBackBuffer.textureResource
					);
				}
				else if (_postProcessingBackBuffer.width != w
						 || _postProcessingBackBuffer.height != h
						 || _postProcessingBackBuffer.backgroundColor != bgcolor)
				{
					_postProcessingBackBuffer.textureResource.setSize(
						w,
						h
					);
					_postProcessingBackBuffer.backgroundColor = bgcolor;
				}
				
				return _postProcessingBackBuffer;
			}
			
			return backBuffer;
		}
		
		/**
		 * Render current Scene
		 */
		private function render(viewport	: Viewport,
								destination	: BitmapData) : uint
		{
			applyBindingChanges();

			var context			: Context3DResource	= viewport.context3D;
			var backBuffer 		: RenderTarget		= getRenderingBackBuffer(viewport.backBuffer);
			var numPasses		: uint 				= _passes.length;
			var numTriangles	: uint 				= 0;
			
			var sortValues 		: Vector.<Number> 	= TMP_NUMBERS;
			var i				: uint				= 0;
			
			context.enableErrorChecking = (Minko.debugLevel & DebugLevel.CONTEXT) != 0;
			
			// sort passes
			if (!_passesAreSorted && numPasses > 1)
			{
				for (i = 0; i < numPasses; ++i)
					sortValues[i] = -(_passes[i] as ShaderInstance).settings.priority;
				
				Sort.flashSort(sortValues, _passes, numPasses);
				_passesAreSorted = true;
			}
			
			// apply passes
			var previous 		: ShaderInstance 	= null;
			var callTriangles	: uint				= 0;
			var call			: DrawCall			= null;
			var previousCall	: DrawCall			= null;
			
			for (i = 0; i < numPasses; ++i)
			{
				var pass 		: ShaderInstance	= _passes[i];
				var calls 		: Array				= _passInstanceToDrawCalls[pass];
				var numCalls	: uint				= calls.length;
				
				if (!pass.settings.enabled || !pass.generator.enabled)
					continue;
				
				pass.begin.execute(pass, context, backBuffer);
				pass.prepareContext(context, backBuffer, previous);
				previous = pass;
				
				// sort draw calls if necessary
				if (pass.settings.depthSortDrawCalls)
				{
					for (var k : uint = 0; k < numCalls; ++k)
						sortValues[k] = -(calls[k] as DrawCall).depth;
					
					Sort.flashSort(sortValues, calls, numCalls);
				}
				
				for (var j : uint = 0; j < numCalls; ++j)
				{
					call = calls[j];
					callTriangles = call.apply(context, previousCall);
					
					if (callTriangles != 0)
					{
						numTriangles += callTriangles;
						previousCall = call;
					}
				}
				
				pass.end.execute(pass, context, backBuffer);
			}
			
			// force clear is nothing was rendered
			if (numTriangles == 0)
			{
				var color : uint = backBuffer.backgroundColor;
				
				context.clear(
					(color >>> 24) / 255.,
					((color >> 16) & 0xff) / 255.,
					((color >> 8) & 0xff) / 255.,
					(color & 0xff) / 255.
				);
			}
			
			// present
			if (_postProcessingEffect)
				_postProcessingScene.render(viewport, destination);
			else
			{
				if (destination)
					context.drawToBitmapData(destination);
				else
					context.present();
			}
			
			return numTriangles;
		}
		
		/**
		 * Keep a pointer to the scene this RenderingController is used on.
		 * If this controller was already added to a scene, an error is thrown.
		 */
		private function targetAddedHandler(controller	: RenderingController,
											scene		: Scene) : void
		{
			if (_scene != null)
			{
				throw new Error(
					'The SceneRenderingController cannot target more than one Scene.'
				);
			}
			
			_scene = scene;
			_scene.enterFrame.add(sceneEnterFrameHandler);
			_scene.exitFrame.add(sceneExitFrameHandler);
			_scene.descendantAdded.add(descendantAddedHandler);
			_scene.descendantRemoved.add(descendantRemovedHandler);
		}
		
		private function sceneEnterFrameHandler(scene 		: Scene,
												viewport 	: Viewport,
												target 		: BitmapData,
												time		: Number) : void
		{
			var viewportWidth	: Number = viewport.width;
			var viewportHeight	: Number = viewport.height;
			
			_scene.properties.setProperty("time", time);
			
			if (viewportWidth != _lastViewportWidth)
				_scene.properties.setProperty('viewportWidth', viewport.width);
			
			if (viewportHeight != _lastViewportHeight)
				_scene.properties.setProperty('viewportHeight', viewport.height);
			
			_lastViewportWidth	= viewportWidth;
			_lastViewportHeight	= viewportHeight
		}
		
		private function sceneExitFrameHandler(scene 	: Scene,
											   viewport : Viewport,
											   target 	: BitmapData,
											   time		: Number) : void
		{
			if (viewport.ready)
			{
				_numTriangles = render(viewport, target);
				Factory.sweep();
			}
		}
		
		/**
		 * Remove callbacks and reset the whole controller.
		 */
		private function targetRemovedHandler(controller : RenderingController,
											  scene		 : Scene) : void
		{
			_scene.descendantAdded.remove(descendantAddedHandler);
			_scene.descendantRemoved.remove(descendantRemovedHandler);
			_scene = null;
		}
		
		/**
		 * Is called each time something is added to the scene.
		 * This will forward its calls to addMesh.
		 */
		private function descendantAddedHandler(group : Group,
												child : ISceneNode) : void
		{
			if (child is Mesh)
				addMesh(Mesh(child));
			else if (child is Group)
			{
				for each (var mesh : Mesh in Group(child).getDescendantsByType(Mesh, TMP_MESHES))
					addMesh(mesh);
				TMP_MESHES.length = 0;
			}
		}
		
		/**
		 * Is called each time something is removed from the scene.
		 * This will forward its calls to removeMesh.
		 */
		private function descendantRemovedHandler(group : Group,
											 	  child : ISceneNode) : void
		{
			if (child is Mesh)
				removeMesh(Mesh(child));
			else if (child is Group)
			{
				for each (var mesh : Mesh in Group(child).getDescendantsByType(Mesh, TMP_MESHES))
					removeMesh(mesh);
				TMP_MESHES.length = 0;
			}
		}
		
		private function addMesh(mesh : Mesh) : void
		{
			// retrieve references to the data we want to use, to save some function calls
			var meshEffect		: Effect		= mesh.effect;
			var meshBindings	: DataBindings	= mesh.bindings;
			var numPasses		: uint			= meshEffect.numPasses;
			var sceneBindings	: DataBindings	= _scene.bindings;
			
			// iterate on meshEffect passes
			var drawCalls : Vector.<DrawCall>	= new <DrawCall>[];
			
			for (var i : uint = 0; i < numPasses; ++i)
			{
				// fork pass if needed
				var asShader		: Shader			= meshEffect.getPass(i);
				var passInstance	: ShaderInstance	= asShader.fork(meshBindings, sceneBindings);
				
				// create drawcall
				var drawCall		: DrawCall			= new DrawCall();
				
				drawCall.enabled = mesh.visible;
				if (passInstance.program != null)
				{
					drawCall.configure(
						passInstance.program,
						mesh.geometry,
						meshBindings,
						sceneBindings,
						passInstance.settings.depthSortDrawCalls
					);
				}
				drawCalls[i] = drawCall;
				
				// retain the instance, update indexes, watch for invalidation, give to renderingList.
				bind(passInstance, drawCall, meshBindings);
			}
			
			// update indexes
			_meshToDrawCalls[mesh]				= drawCalls;
			_meshBindingsToMesh[meshBindings]	= mesh;
			
			if (!_effectToMeshes[meshEffect])
			{
				_effectToMeshes[meshEffect] = new <Mesh>[];
				meshEffect.passesChanged.add(effectPassesChangedHandler);
			}
			
			_effectToMeshes[meshEffect].push(mesh);
			
			//register to visibility change signal
			mesh.effectChanged.add(meshEffectChangedHandler);
			mesh.visibilityChanged.add(meshVisibilityChangedHandler);
			mesh.frameChanged.add(meshFrameChangedHandler);
			mesh.geometryChanged.add(meshGeometryChangedHandler);
		}
		
		private function removeMesh(mesh : Mesh) : void
		{
			// retrieve references to the data we want to use, to save some function calls
			var meshEffect		: Effect		= mesh.effect;
			var meshBindings	: DataBindings	= mesh.bindings;
			
			// retrieve drawcalls
			var drawCalls		: Vector.<DrawCall>	= _meshToDrawCalls[mesh];
			var numDrawCalls	: uint				= drawCalls.length;
			
			for (var drawCallId : uint = 0; drawCallId < numDrawCalls; ++drawCallId)
			{
				// retrieve drawcall, and shaderInstance
				var drawCall		: DrawCall			= drawCalls[drawCallId];
				var passInstance	: ShaderInstance	= _drawCallToPassInstance[drawCall];
				
				unbind(passInstance, drawCall, meshBindings);
			}
			
			// update indexes
			delete _meshToDrawCalls[mesh];
			delete _meshBindingsToMesh[meshBindings];
			
			var meshesWithSameEffect : Vector.<Mesh> = _effectToMeshes[meshEffect];
			
			meshesWithSameEffect.splice(meshesWithSameEffect.indexOf(mesh), 1);
			
			if (meshesWithSameEffect.length == 0)
			{
				delete _effectToMeshes[meshEffect];
				meshEffect.passesChanged.remove(effectPassesChangedHandler);
			}
			
			//remove to visibility change signal
			mesh.effectChanged.remove(meshEffectChangedHandler);
			mesh.visibilityChanged.remove(meshVisibilityChangedHandler);
			mesh.frameChanged.remove(meshFrameChangedHandler);
			mesh.geometryChanged.remove(meshGeometryChangedHandler);
		}
		
		private function effectPassesChangedHandler(effect : Effect) : void
		{
			for each (var mesh : Mesh in _effectToMeshes[effect])
			{
				var meshBindings	: DataBindings		= mesh.bindings;
				var sceneBindings	: DataBindings		= _scene.bindings;
				var drawCalls		: Vector.<DrawCall>	= _meshToDrawCalls[mesh];
				
				for each (var oldDrawCall : DrawCall in drawCalls)
					unbind(_drawCallToPassInstance[oldDrawCall], oldDrawCall, meshBindings);
				
				drawCalls.length = 0;
				
				var numPasses	: uint	= effect.numPasses;
				
				for (var i : uint = 0; i < numPasses; ++i)
				{
					// fork pass if needed
					var asShader		: Shader			= effect.getPass(i);
					var passInstance	: ShaderInstance	= asShader.fork(meshBindings, sceneBindings);
					
					// create drawcall
					var newDrawCall		: DrawCall			= new DrawCall();
					
					if (passInstance.program != null)
					{
						newDrawCall.configure(
							passInstance.program,
							mesh.geometry,
							meshBindings,
							sceneBindings,
							passInstance.settings.depthSortDrawCalls
						);
					}
					
					drawCalls[i] = newDrawCall;
					
					// retain the instance, update indexes, watch for invalidation, give to renderingList.
					bind(passInstance, newDrawCall, meshBindings);
				}
			}
		}
		
		private function meshEffectChangedHandler(mesh		: Mesh,
												  oldEffect	: Effect,
												  newEffect	: Effect) : void
		{
			if (oldEffect === newEffect)
				return;
			
			var meshBindings	: DataBindings		= mesh.bindings;
			var sceneBindings	: DataBindings		= _scene.bindings;
			var drawCalls		: Vector.<DrawCall>	= _meshToDrawCalls[mesh];
			
			// stripped down 'removeMesh' method, that unscribe everything related
			// to the old effect and the mesh (but keep all that is related only to the mesh
			// or only to the effect)
			var oldNumDrawCalls	: uint					= drawCalls.length;
			for (var oldDrawCallId : uint = 0; oldDrawCallId < oldNumDrawCalls; ++oldDrawCallId)
			{
				var oldDrawCall		: DrawCall			= drawCalls[oldDrawCallId];
				var oldPassInstance	: ShaderInstance	= _drawCallToPassInstance[oldDrawCall];
				unbind(oldPassInstance, oldDrawCall, meshBindings);
			}
			
			drawCalls.length = 0;
			
			var meshesWithSameEffect : Vector.<Mesh> = _effectToMeshes[oldEffect];
			meshesWithSameEffect.splice(meshesWithSameEffect.indexOf(mesh), 1);
			if (meshesWithSameEffect.length == 0)
			{
				delete _effectToMeshes[oldEffect];
				oldEffect.passesChanged.remove(effectPassesChangedHandler);
			}
			
			// stripped down 'addMesh' method, to do the same reversed
			var newNumPasses	: uint			= newEffect.numPasses;
			
			for (var i : uint = 0; i < newNumPasses; ++i)
			{
				var passTemplate	: Shader			= newEffect.getPass(i);
				var passInstance	: ShaderInstance	= passTemplate.fork(meshBindings, sceneBindings);
				var drawCall		: DrawCall			= new DrawCall();
				
				if (passInstance.program != null)
				{
					drawCall.configure(
						passInstance.program,
						mesh.geometry,
						meshBindings,
						sceneBindings,
						passInstance.settings.depthSortDrawCalls
					);
				}
				
				drawCalls[i] = drawCall;
				bind(passInstance, drawCall, meshBindings);
			}
			
			if (!_effectToMeshes[newEffect])
			{
				_effectToMeshes[newEffect] = new Vector.<Mesh>();
				newEffect.passesChanged.add(effectPassesChangedHandler);
			}
			
			_effectToMeshes[newEffect].push(mesh);
		}
		
		private function meshVisibilityChangedHandler(mesh		 : Mesh,
													  visibility : Boolean) : void
		{
			var drawCalls		: Vector.<DrawCall>	= _meshToDrawCalls[mesh];
			var numDrawCalls	: uint				= drawCalls.length;
			
			for (var drawCallId : uint = 0; drawCallId < numDrawCalls; ++drawCallId)
				drawCalls[drawCallId].enabled = visibility;
		}
		
		private function meshFrameChangedHandler(mesh		: Mesh,
												 oldFrame	: uint,
												 newFrame	: uint) : void
		{
			var drawCalls	: Vector.<DrawCall>	= _meshToDrawCalls[mesh];
			var numCalls	: uint				= drawCalls.length;
			var geom		: Geometry			= mesh.geometry;
			
			for (var callId : uint = 0; callId < numCalls; ++callId)
			{
				var drawCall	: DrawCall	= drawCalls[callId];
				
				drawCall.setGeometry(geom, newFrame);
			}
		}
		
		private function meshGeometryChangedHandler(mesh		: Mesh,
													oldGeometry	: Geometry,
													newGeometry	: Geometry) : void
		{
			var drawCalls	: Vector.<DrawCall>	= _meshToDrawCalls[mesh];
			var numCalls	: uint				= drawCalls.length;
			var frame		: uint				= mesh.frame;
			
			for (var callId : uint = 0; callId < numCalls; ++callId)
			{
				var drawCall	: DrawCall	= drawCalls[callId];
				
				drawCall.updateGeometry(newGeometry);
				drawCall.setGeometry(newGeometry, frame);
			}
		}
		
		private function applyBindingChanges() : void
		{
			if (_numStashedPropertyChanges == 0)
				return;
			
			var sceneBindings		: DataBindings		= _scene.bindings;
			var sceneChanges		: Vector.<String>	= _stashedPropertyChanges[sceneBindings];
			var numShaderInstances	: int				= _passes.length;
			
			for (var shaderInstanceId : int = numShaderInstances - 1; shaderInstanceId >= 0; --shaderInstanceId)
			{
				var passInstance			: ShaderInstance	= _passes[shaderInstanceId];
				var passInstanceSignature	: Signature			= passInstance.signature;
				var drawCalls				: Array				= _passInstanceToDrawCalls[passInstance];
				var numDrawCalls			: int				= drawCalls.length;
				
				var needUpdateFromScene		: Boolean 			= sceneChanges != null
					? passInstanceSignature.useProperties(_stashedPropertyChanges[sceneBindings], true) 
					: false;
				
				for (var drawCallId : int = numDrawCalls - 1; drawCallId >= 0; --drawCallId)
				{
					var drawCall			: DrawCall			= drawCalls[drawCallId];
					var meshBindings		: DataBindings		= _drawCallToMeshBindings[drawCall];
					var meshGeometry		: Geometry			= Mesh(_meshBindingsToMesh[meshBindings]).geometry
					var meshChanges			: Vector.<String>	= _stashedPropertyChanges[meshBindings];
					
					var needUpdateFromMesh	: Boolean 			= meshChanges != null ?
						passInstanceSignature.useProperties(meshChanges, false) :
						false;
					
					// This signature is neither affected by the scene or mesh changes
					if (!needUpdateFromScene && !needUpdateFromMesh)
						continue;
					
					// This signature did subscribe to at least one property that changed,
					// we need to check if it is still valid.
					if (passInstanceSignature.isValid(meshBindings, sceneBindings))
						continue;
					
					// our shader is no longer valid, we need to find a new one.
					unbind(passInstance, drawCall, meshBindings);
					
					var replacementInstance : ShaderInstance = passInstance.generator.fork(
						meshBindings,
						sceneBindings
					);
					
					if (replacementInstance.program != null)
						drawCall.configure(
							replacementInstance.program,
							meshGeometry,
							meshBindings,
							sceneBindings,
							replacementInstance.settings.depthSortDrawCalls
						);
					bind(replacementInstance, drawCall, meshBindings);
				}
			}
			
			// reset all changes stashes
			_numStashedPropertyChanges = 0;
			for each (var changes : Object in _stashedPropertyChanges)
				Vector.<String>(changes).length = 0;
		}
		
		private function bind(passInstance	: ShaderInstance,
							  drawCall		: DrawCall,
							  meshBindings	: DataBindings) : void
		{
			var sceneBindings	: DataBindings	= _scene.bindings;
			
			// add callbacks on binding changes
			var signature		: Signature		= passInstance.signature;
			var numKeys			: uint			= signature.numKeys;
			
			for (var i : uint = 0; i < numKeys; ++i)
			{
				var key 	: String	= signature.getKey(i);
				var flags	: uint		= signature.getFlags(i);
				
				if (flags & Signature.SOURCE_MESH)
					meshBindings.addCallback(key, bindingsPropertyChangedHandler);
				else
					sceneBindings.addCallback(key, bindingsPropertyChangedHandler);
			}
			
			// retain the shader
			passInstance.retain();
			
			// update indexes
			_drawCallToPassInstance[drawCall] = passInstance;
			
			if (!_passInstanceToDrawCalls[passInstance])
			{
				_passInstanceToDrawCalls[passInstance] = [];
				_passes.push(passInstance);
				_passesAreSorted = false;
			}
			
			_drawCallToMeshBindings[drawCall] = meshBindings;
			_passInstanceToDrawCalls[passInstance].push(drawCall);
		}
		
		private function unbind(passInstance	: ShaderInstance,
								drawCall		: DrawCall,
								meshBindings	: DataBindings) : void
		{
			var sceneBindings	: DataBindings	= _scene.bindings;
			
			// remove callback on binding changes
			var signature		: Signature		= passInstance.signature;
			var numKeys			: uint			= signature.numKeys;
			
			for (var i : uint = 0; i < numKeys; ++i)
			{
				var key 	: String	= signature.getKey(i);
				var flags	: uint		= signature.getFlags(i);
				
				if (flags & Signature.SOURCE_MESH)
					meshBindings.removeCallback(key, bindingsPropertyChangedHandler);
				else
					sceneBindings.removeCallback(key, bindingsPropertyChangedHandler);
			}
			
			// release the shader
			passInstance.release();
			
			// update indexes
			delete _drawCallToPassInstance[drawCall];
			delete _drawCallToMeshBindings[drawCall];
			
			var drawCalls 		: Array	= _passInstanceToDrawCalls[passInstance];
			var numDrawCalls	: uint	= drawCalls.length - 1;
			
			for (i = drawCalls.indexOf(drawCall); i < numDrawCalls; ++i)
				drawCalls[i] = drawCalls[uint(i + 1)];
			drawCalls.length = numDrawCalls;
			
			if (drawCalls.length == 0)
			{
				delete _passInstanceToDrawCalls[passInstance];
				_passes.splice(_passes.indexOf(passInstance), 1);
			}
		}
		
		private function bindingsPropertyChangedHandler(meshBindings 	: DataBindings,
														propertyName	: String,
														newValue		: Object) : void
		{
			var changes : Vector.<String>	= _stashedPropertyChanges[meshBindings];
			
			if (!changes)
				_stashedPropertyChanges[meshBindings] = changes = new <String>[propertyName];
			else
				changes.push(propertyName);
			
			++_numStashedPropertyChanges;
		}
	}
}
