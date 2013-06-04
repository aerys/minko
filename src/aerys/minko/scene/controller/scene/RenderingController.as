package aerys.minko.scene.controller.scene
{
	import flash.display.BitmapData;
	import flash.utils.Dictionary;
	import flash.utils.getTimer;
	
	import aerys.minko.Minko;
	import aerys.minko.ns.minko_render;
	import aerys.minko.render.DrawCall;
	import aerys.minko.render.Effect;
	import aerys.minko.render.EffectInstance;
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.Viewport;
	import aerys.minko.render.geometry.Geometry;
	import aerys.minko.render.geometry.primitive.QuadGeometry;
	import aerys.minko.render.material.Material;
	import aerys.minko.render.resource.Context3DResource;
	import aerys.minko.render.resource.texture.TextureResource;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.ShaderInstance;
	import aerys.minko.render.shader.ShaderSettings;
	import aerys.minko.render.shader.compiler.ShaderCompilerError;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Mesh;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.type.Sort;
	import aerys.minko.type.binding.DataBindings;
	import aerys.minko.type.binding.DataProvider;
	import aerys.minko.type.binding.Signature;
	import aerys.minko.type.enum.DataProviderUsage;
	import aerys.minko.type.enum.FrustumCulling;
	import aerys.minko.type.log.DebugLevel;
	
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
		
		private var _scene						: Scene;
		
		private var _numTriangles				: uint;
		private var _numEnabledPasses			: uint;
		private var _numDrawCalls				: uint;
		private var _numEnabledDrawCalls		: uint;
		
		private var _stashedPropertyChanges		: Dictionary;
		
		private var _effectInstances			: Vector.<EffectInstance>;
		
		private var _passes						: Array;
		private var _passesAreSorted			: Boolean;
		
		private var _drawCallToPassInstance		: Dictionary;
		private var _passInstanceToDrawCalls	: Dictionary;
		
		private var _meshToDrawCalls			: Dictionary;
		private var _meshToEffectInstance		: Dictionary;
		private var _drawCallToMeshBindings		: Dictionary;
        private var _meshBindingsUsageCount     : Dictionary;
        private var _sceneBindingsUsageCount    : Object;
		
		private var _postProcessingBackBuffer	: RenderTarget;
		private var _postProcessingEffect		: Effect;
		private var _postProcessingScene		: Scene;
		private var _postProcessingProperties	: DataProvider;
		
		private var _lastViewportWidth			: Number;
		private var _lastViewportHeight			: Number;
		
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
		private var _effectInstanceToMeshes		: Dictionary;
		private var _numStashedPropertyChanges	: int;
		
		public function get numPasses() : uint
		{
			var numPasses : uint = _passes.length;
			
			if (_postProcessingEffect)
				numPasses += _postProcessingEffect.numExtraPasses;
			
			return numPasses;
		}
		
		public function get numEnabledPasses() : uint
		{
			return _postProcessingScene
				? _numEnabledPasses + _postProcessingScene.numEnabledPasses
				: _numEnabledPasses;
		}
		
		public function get numDrawCalls() : uint
		{
			return _numDrawCalls;
		}
		
		public function get numEnabledDrawCalls() : uint
		{
			return _numEnabledDrawCalls;
		}
		
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
			_effectInstances			= new <EffectInstance>[];
			
			_passes						= [];
			_passesAreSorted			= true;
			
			_postProcessingProperties 	= new DataProvider(DataProviderUsage.MANAGED);
			
			_drawCallToPassInstance		= new Dictionary();
			_drawCallToMeshBindings		= new Dictionary();
			_passInstanceToDrawCalls	= new Dictionary();
			
			_meshToEffectInstance		= new Dictionary(true);
			_meshToDrawCalls			= new Dictionary(true);
            _meshBindingsUsageCount     = new Dictionary(true);
            _sceneBindingsUsageCount    = {};
			
			_effectInstanceToMeshes		= new Dictionary(true);
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
                    var screenQuad : Mesh = new Mesh(
                        new QuadGeometry(),
                        new Material(_postProcessingEffect)
                    );
                    
                    screenQuad.frustumCulling = FrustumCulling.DISABLED;
					_postProcessingScene = new Scene(screenQuad);
					
					_postProcessingScene.bindings.addProvider(
						_postProcessingProperties
					);
					
					_postProcessingScene.beforePresent.add(postProcessingSceneBeforePresentHandler);
					_postProcessingScene.afterPresent.add(postProcessingSceneAfterPresentHandler);
				}
				else
				{
					var screen : Mesh = Mesh(_postProcessingScene.getChildAt(0));
					
					screen.material.effect = _postProcessingEffect;
				}
			}
		}
		
		private function postProcessingSceneBeforePresentHandler(scene : Scene, viewport : Viewport, destination : BitmapData, time : uint) : void
		{
			_scene.beforePresent.execute(scene, viewport, destination, time);
		}
		
		private function postProcessingSceneAfterPresentHandler(scene : Scene, viewport : Viewport, destination : BitmapData, time : uint) : void
		{
			_scene.afterPresent.execute(scene, viewport, destination, time);
		}
		
		private function getRenderingBackBuffer(backBuffer : RenderTarget) : RenderTarget
		{
			if (_postProcessingEffect)
			{
				var w 		: uint 	= 1 << Math.ceil(Math.log(backBuffer.width) * Math.LOG2E);
				var h 		: uint 	= 1 << Math.ceil(Math.log(backBuffer.height) * Math.LOG2E);
				var bgcolor : uint 	= backBuffer.backgroundColor;
				
				if (!_postProcessingBackBuffer
                    || _postProcessingBackBuffer.width != w
                    || _postProcessingBackBuffer.height != h
                    || _postProcessingBackBuffer.backgroundColor != bgcolor)
				{
                    if (_postProcessingBackBuffer)
                        _postProcessingBackBuffer.textureResource.dispose();
                    
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
				
				return _postProcessingBackBuffer;
			}
			
			return backBuffer;
		}
		
		/**
		 * Render current Scene
		 */
		public function render(viewport		: Viewport,
							   destination	: BitmapData) : uint
		{
			applyBindingChanges();
			
			_numEnabledPasses = 0;
            
			var context			: Context3DResource	= viewport.context3D;
			var backBuffer 		: RenderTarget		= getRenderingBackBuffer(viewport.backBuffer);
			var numPasses		: uint 				= _passes.length;
			var numTriangles	: uint 				= 0;
			
			var sortValues 		: Vector.<Number> 	= TMP_NUMBERS;
			var passId			: uint				= 0;
			
			context.enableErrorChecking = (Minko.debugLevel & DebugLevel.CONTEXT) != 0;
            
			// sort passes
			if (!_passesAreSorted && numPasses > 1)
			{
				for (passId = 0; passId < numPasses; ++passId)
					sortValues[passId] = -(_passes[passId] as ShaderInstance).settings.priority;
				
				Sort.flashSort(sortValues, _passes, numPasses);
				_passesAreSorted = true;
			}
			
			// apply passes
			var previous 		: ShaderInstance	= null;
			var call			: DrawCall			= null;
			var previousCall	: DrawCall			= null;
			var passes			: Array				= _passes.concat();
			
			for (passId = 0; passId < numPasses; ++passId)
			{
				var pass			: ShaderInstance	= passes[passId];
				var calls 			: Array				= _passInstanceToDrawCalls[pass];
				var settings		: ShaderSettings	= pass.settings;
				var renderTarget 	: RenderTarget 		= settings.renderTarget || backBuffer;
				
				settings.setupRenderTarget(
					context,
					backBuffer,
					previous ? previous.settings : null
				);
				
				if (!pass.settings.enabled || !pass.shader.enabled || !calls)
                {
                    previous = pass;
					continue;
                }
				
				++_numEnabledPasses;
				
				var numCalls	: uint	= calls.length;
				
				pass.prepareContext(context, backBuffer, previous);
				pass.begin.execute(pass, context, backBuffer);
				previous = pass;
				
				// sort draw calls if necessary
				if (pass.settings.depthSortDrawCalls && numCalls > 1)
				{
					for (var drawCallId : uint = 0; drawCallId < numCalls; ++drawCallId)
						sortValues[drawCallId] = -(calls[drawCallId] as DrawCall).depth;
					
					Sort.flashSort(sortValues, calls, numCalls);
				}
				
				for (var j : uint = 0; j < numCalls; ++j)
				{
					call = calls[j];
					if (call.enabled)
					{
						numTriangles += call.apply(context, previousCall);
						previousCall = call;
					}
				}
				
				pass.end.execute(pass, context, backBuffer);
			}
			
			// force clear if nothing was rendered
			if (numTriangles == 0)
			{
				var color : uint = backBuffer.backgroundColor;
				_scene.beforeClear.execute(_scene, viewport, destination, getTimer());
				context.clear(
					(color >>> 24) / 255.,
					((color >> 16) & 0xff) / 255.,
					((color >> 8) & 0xff) / 255.,
					(color & 0xff) / 255.
				);
				_scene.afterClear.execute(_scene, viewport, destination, getTimer());
			}
			
			// present
			if (_postProcessingEffect)
				_postProcessingScene.render(viewport, destination);
			else
			{
				if (destination)
					context.drawToBitmapData(destination);
				else
				{
					_scene.beforePresent.execute(_scene, viewport, destination, getTimer());
					context.present();
					_scene.afterPresent.execute(_scene, viewport, destination, getTimer());
				}
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
			_scene.descendantAdded.add(descendantAddedHandler);
		}
		
		private function sceneEnterFrameHandler(scene 		: Scene,
												viewport 	: Viewport,
												target 		: BitmapData,
												time		: Number) : void
		{
			var viewportWidth	: Number 		= viewport.width;
			var viewportHeight	: Number 		= viewport.height;
			var sceneProperties	: DataProvider 	= _scene.properties;
			
			sceneProperties.setProperty('time', time);
			
			if (viewportWidth != _lastViewportWidth)
				sceneProperties.setProperty('viewportWidth', viewportWidth);
			
			if (viewportHeight != _lastViewportHeight)
				sceneProperties.setProperty('viewportHeight', viewportHeight);
			
			_lastViewportWidth	= viewportWidth;
			_lastViewportHeight	= viewportHeight
		}
		
		/**
		 * Remove callbacks and reset the whole controller.
		 */
		private function targetRemovedHandler(controller : RenderingController,
											  scene		 : Scene) : void
		{
			throw new Error();
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
		
		public function addMesh(mesh : Mesh) : void
		{
			var meshBindings : DataBindings = mesh.bindings;
			
			mesh.removed.add(meshRemovedHandler);
			meshBindings.addCallback('effect', meshEffectChangedHandler);
			meshBindings.addCallback('computedVisibility', meshVisibilityChangedHandler);
			meshBindings.addCallback('frame', meshFrameChangedHandler);
			meshBindings.addCallback('geometry', meshGeometryChangedHandler);
            
			// retrieve references to the data we want to use, to save some function calls
			var material	: Material	= mesh.material;
			
			if (material == null)
				return ;
			
			var effect	: Effect	= material.effect;
			
			if (effect == null)
				return ;
			
			createDrawCalls(mesh, effect);
		}
		
		private function meshRemovedHandler(mesh : Mesh, ancestor : Group) : void
		{
			removeMesh(mesh);
		}
		
		public function removeMesh(mesh : Mesh) : void
		{
			var meshBindings : DataBindings = mesh.bindings;
			
			mesh.removed.remove(meshRemovedHandler);
			meshBindings.removeCallback('effect', meshEffectChangedHandler);
			meshBindings.removeCallback('computedVisibility', meshVisibilityChangedHandler);
			meshBindings.removeCallback('frame', meshFrameChangedHandler);
			meshBindings.removeCallback('geometry', meshGeometryChangedHandler);
			
			delete _stashedPropertyChanges[meshBindings];
            
			var material : Material = mesh.material;
			
			if (!material)
                return ;
            
            var effect  : Effect    = material.effect;
            
            if (!effect)
                return ;
            
			deleteDrawCalls(mesh, effect);
		}
		
		private function effectInstancePassesChangedHandler(effectInstance : EffectInstance) : void
		{
			var effect		: Effect		= effectInstance.effect;
			var meshes 		: Vector.<Mesh> = _effectInstanceToMeshes[effectInstance];
			var numMeshes 	: uint 			= meshes.length;
			
			meshes = meshes.concat();
			for (var meshId : uint = 0; meshId < numMeshes; ++meshId)
			{
				var mesh : Mesh = meshes[meshId];
				
				deleteDrawCalls(mesh, effect);
				createDrawCalls(mesh, effect);
			}
		}
		
		private function meshEffectChangedHandler(bindings		: DataBindings,
												  propertyName	: String,
												  oldEffect		: Effect,
												  newEffect		: Effect) : void
		{
			updateDrawCalls(bindings.owner as Mesh, oldEffect, newEffect);
		}
		
		private function updateDrawCalls(mesh 		: Mesh,
										 oldEffect	: Effect,
										 newEffect	: Effect) : void
		{
			if (newEffect == oldEffect)
				return ;
			
			if (oldEffect == null)
			{
				createDrawCalls(mesh, newEffect);
				
				return ;
			}
			
			if (newEffect == null)
			{
				deleteDrawCalls(mesh, oldEffect);
				
				return ;
			}
			
			deleteDrawCalls(mesh, oldEffect);
			createDrawCalls(mesh, newEffect);
		}
		
		private function createDrawCalls(mesh : Mesh, effect : Effect) : void
		{
			var meshBindings	: DataBindings		= mesh.bindings;
			var sceneBindings	: DataBindings		= _scene.bindings;
			var effectInstance	: EffectInstance	= effect.fork(sceneBindings, meshBindings);
			var numPasses		: uint				= effectInstance.numPasses;
			var drawCalls 		: Vector.<DrawCall>	= new <DrawCall>[];
            var passInstance    : ShaderInstance    = null;
            
            // try to fork every shader
            var instances   : Vector.<ShaderInstance>   = new <ShaderInstance>[];
            var fallback    : Boolean                   = false;
            var passId      : int                       = 0;
            
            while (passId < numPasses)
            {
                var pass    : Shader    = effectInstance.getPass(passId);
                
                passInstance = pass.fork(sceneBindings, meshBindings);
                if (!passInstance)
                {
                    if (fallback)
                        throw new Error();
                    
                    // some shader compilation failed: fallback
                    fallback = true;
                    effectInstance = effect.fork(sceneBindings, meshBindings, fallback);
                    
                    // reset the loop
                    numPasses = effectInstance.numPasses;
                    passId = 0;
                    
                    continue ;
                }
                
                instances[passId] = passInstance;
                ++passId
            }
            
            _meshToEffectInstance[mesh] = effectInstance;
            _meshBindingsUsageCount[meshBindings] = {};
            
            bindEffectInstance(effectInstance, meshBindings);
            
            var numInstances : uint = instances.length;
            for (var instanceId : uint = 0; instanceId < numInstances; ++instanceId)
            {
				var drawCall    : DrawCall  = new DrawCall();
                
                passInstance = instances[instanceId];
                
				drawCall.enabled = mesh.computedVisibility;
                
                if (passInstance.program)
                {
    				drawCall.configure(
    					passInstance.program,
    					mesh.geometry,
    					meshBindings,
    					sceneBindings,
    					passInstance.settings.depthSortDrawCalls
    				);
                }
                    
				drawCalls[instanceId] = drawCall;
				++_numDrawCalls;
                if (drawCall.enabled)
                    ++_numEnabledDrawCalls;
                
				// retain the instance, update indexes, watch for invalidation, give to renderingList.
				bindShaderInstance(passInstance, drawCall, meshBindings);
			}
			
			// update indexes
			_meshToDrawCalls[mesh] = drawCalls;
			
			// update effect instances list and mesh to effect instance indexes
			if (!_effectInstanceToMeshes[effectInstance])
			{
				_effectInstanceToMeshes[effectInstance] = new <Mesh>[mesh];
				_effectInstances.push(effectInstance);
				effectInstance.passesChanged.add(effectInstancePassesChangedHandler);
			}
			else
				_effectInstanceToMeshes[effectInstance].push(mesh);
		}
		
		private function deleteDrawCalls(mesh : Mesh, effect : Effect) : void
		{
			var meshBindings	: DataBindings		= mesh.bindings;
			var drawCalls		: Vector.<DrawCall>	= _meshToDrawCalls[mesh];
			
			if (drawCalls)
            {
    			var effectInstance	: EffectInstance	= _meshToEffectInstance[mesh];
    			var numDrawCalls	: uint	            = drawCalls.length;
                
    			unbindEffectInstance(effectInstance, meshBindings);
                
    			for (var drawCallId : uint = 0; drawCallId < numDrawCalls; ++drawCallId)
    			{
    				// retrieve drawcall, and shaderInstance
    				var drawCall		: DrawCall			= drawCalls[drawCallId];
    				var passInstance	: ShaderInstance	= _drawCallToPassInstance[drawCall];
    				
                    if (drawCall.enabled)
                        --_numEnabledDrawCalls;
                    
    				unbindShaderInstance(passInstance, drawCall, meshBindings);
    			}
    			_numDrawCalls -= numDrawCalls;
                delete _meshToEffectInstance[mesh];
                delete _meshBindingsUsageCount[meshBindings];
                delete _meshToDrawCalls[mesh];

				// update effect instances list and mesh to effect instance indexes
                var meshesWithSameEffectInstance : Vector.<Mesh> = _effectInstanceToMeshes[effectInstance];
                
                meshesWithSameEffectInstance.splice(meshesWithSameEffectInstance.indexOf(mesh), 1);
                if (meshesWithSameEffectInstance.length == 0)
                {
                    delete _effectInstanceToMeshes[effectInstance];
                    _effectInstances.splice(_effectInstances.indexOf(effectInstance), 1);
                    effectInstance.passesChanged.remove(effectInstancePassesChangedHandler);
                }
            }
		}
		
		private function meshVisibilityChangedHandler(bindings			: DataBindings,
													  propertyName		: String,
													  oldVisibility		: Boolean,
													  newVisibility		: Boolean) : void
		{
			var mesh			: Mesh				= bindings.owner as Mesh;
			var drawCalls		: Vector.<DrawCall>	= _meshToDrawCalls[mesh];
            
            if (!drawCalls)
                return;
            
			var numDrawCalls	: uint	= drawCalls.length;
			for (var drawCallId : uint = 0; drawCallId < numDrawCalls; ++drawCallId)
				(drawCalls[drawCallId] as DrawCall).enabled = newVisibility;
		}
		
		private function meshFrameChangedHandler(bindings		: DataBindings,
												 propertyName	: String,
												 oldFrame		: uint,
												 newFrame		: uint) : void
		{
			var mesh		: Mesh				= bindings.owner as Mesh;
			var geom		: Geometry			= mesh.geometry;
			var drawCalls	: Vector.<DrawCall>	= _meshToDrawCalls[mesh];

			if (!geom || !drawCalls)
				return ;
			
			var numDrawCalls	: uint				= drawCalls.length;
			
			for (var drawCallId : uint = 0; drawCallId < numDrawCalls; ++drawCallId)
			{
				var drawCall	: DrawCall	= drawCalls[drawCallId];
				
				drawCall.setGeometry(geom, newFrame);
			}
		}
		
		private function meshGeometryChangedHandler(bindings		: DataBindings,
													propertyName	: String,
													oldGeometry		: Geometry,
													newGeometry		: Geometry) : void
		{
			var mesh		: Mesh				= bindings.owner as Mesh;
			var drawCalls	: Vector.<DrawCall>	= _meshToDrawCalls[mesh];
			
			if (drawCalls != null)
			{
				if (!newGeometry)
					deleteDrawCalls(mesh, mesh.material.effect);
				else
				{
					var numDrawCalls	: uint	= drawCalls.length;
					var frame			: uint	= mesh.frame;
					
					for (var drawCallId : uint = 0; drawCallId < numDrawCalls; ++drawCallId)
					{
						var drawCall	: DrawCall	= drawCalls[drawCallId];
						
						drawCall.setGeometry(newGeometry, frame);
					}
				}
			}
		}
		
		private function applyBindingChanges() : void
		{
			if (_numStashedPropertyChanges == 0)
				return;
			
			applyEffectInstancesBindingsChanges();
			applyPassInstancesBindingChanges();
			
			// reset all changes stashes
			_numStashedPropertyChanges = 0;
			for each (var changes : Object in _stashedPropertyChanges)
				Vector.<String>(changes).length = 0;
		}
		
		private function applyEffectInstancesBindingsChanges() : void
		{
			var effectInstances		: Vector.<EffectInstance>	= _effectInstances.concat();
			var numEffectInstances	: uint						= effectInstances.length;
			var sceneBindings		: DataBindings				= _scene.bindings;
			var sceneChanges		: Vector.<String>			= _stashedPropertyChanges[sceneBindings];
			
			for (var effectInstanceId : uint = 0;
				 effectInstanceId < numEffectInstances;
				 ++effectInstanceId)
			{
				var effectInstance 			: EffectInstance	= effectInstances[effectInstanceId];
				var effect					: Effect			= effectInstance.effect;
				var signature				: Signature 		= effectInstance.signature;
				
				var needUpdateFromScene		: Boolean 			= sceneChanges != null
					? signature.useProperties(sceneChanges, true) 
					: false;

				var meshes 		: Vector.<Mesh> = _effectInstanceToMeshes[effectInstance];
				var numMeshes 	: uint 			= meshes.length;
				
				meshes = meshes.concat();
				for (var meshId : uint = 0; meshId < numMeshes; ++meshId)
				{
					var mesh 				: Mesh 				= meshes[meshId];
					var meshBindings		: DataBindings		= mesh.bindings;
					var meshChanges			: Vector.<String>	= _stashedPropertyChanges[meshBindings];
					var needUpdateFromMesh	: Boolean			= meshChanges != null
						? signature.useProperties(meshChanges, false)
						: false;
					
					if ((needUpdateFromScene || needUpdateFromMesh)
						&& !effectInstance.signature.isValid(sceneBindings, meshBindings))
					{
						deleteDrawCalls(mesh, effect);
						createDrawCalls(mesh, effect);
					}
				}
				
			}
		}
		
		private function applyPassInstancesBindingChanges() : void
		{
			var sceneBindings		: DataBindings		= _scene.bindings;
			var sceneChanges		: Vector.<String>	= _stashedPropertyChanges[sceneBindings];
			var passes				: Array				= _passes.concat();
			var numShaderInstances	: uint				= passes.length;
			
			for (var shaderInstanceId : int = numShaderInstances - 1; shaderInstanceId >= 0; --shaderInstanceId)
			{
				var passInstance			: ShaderInstance	= passes[shaderInstanceId];
				var passInstanceSignature	: Signature			= passInstance.signature;
				var drawCalls				: Array				= _passInstanceToDrawCalls[passInstance];
				var numDrawCalls			: uint				= drawCalls.length;
				
				var needUpdateFromScene		: Boolean 			= sceneChanges != null
					? passInstanceSignature.useProperties(_stashedPropertyChanges[sceneBindings], true) 
					: false;
				
				for (var drawCallId : int = numDrawCalls - 1; drawCallId >= 0; --drawCallId)
				{
					var drawCall			: DrawCall			= drawCalls[drawCallId];
					var meshBindings		: DataBindings		= _drawCallToMeshBindings[drawCall];
					var meshGeometry		: Geometry			= (meshBindings.owner as Mesh).geometry
					var meshChanges			: Vector.<String>	= _stashedPropertyChanges[meshBindings];
					
					var needUpdateFromMesh	: Boolean 			= meshChanges != null ?
						passInstanceSignature.useProperties(meshChanges, false) :
						false;
					
					// This signature is neither affected by the scene or mesh changes
					if (!needUpdateFromScene && !needUpdateFromMesh)
						continue;
					
					// This signature did subscribe to at least one property that changed,
					// we need to check if it is still valid.
					if (passInstanceSignature.isValid(sceneBindings, meshBindings))
						continue;
					
					// our shader is no longer valid, we need to find a new one.
					unbindShaderInstance(passInstance, drawCall, meshBindings);
					
					var replacementInstance : ShaderInstance = passInstance.shader.fork(
						sceneBindings,
						meshBindings
					);
					
                    // the new shader failed to compile
                    if (!replacementInstance)
                    {
                        var mesh    : Mesh      = meshBindings.owner as Mesh;
                        var effect  : Effect    = _meshToEffectInstance[mesh].effect;
                        
                        delete _stashedPropertyChanges[meshBindings];
                        
                        deleteDrawCalls(mesh, effect);
                        createDrawCalls(mesh, effect);
                        
                        break;
                    }
                    
					if (replacementInstance.program != null)
						drawCall.configure(
							replacementInstance.program,
							meshGeometry,
							meshBindings,
							sceneBindings,
							replacementInstance.settings.depthSortDrawCalls
						);
					
					bindShaderInstance(replacementInstance, drawCall, meshBindings);
				}
			}
		}
		
		private function bindEffectInstance(effectInstance	: EffectInstance,
											meshBindings	: DataBindings) : void
		{
			watchSignature(effectInstance.signature, meshBindings);
			// FIXME: effectInstance.retain();
		}
		
		private function bindShaderInstance(passInstance		: ShaderInstance,
											drawCall			: DrawCall,
											meshBindings		: DataBindings) : void
		{
			watchSignature(passInstance.signature, meshBindings);

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
		
		private function watchSignature(signature 		: Signature,
										meshBindings	: DataBindings) : void
		{
			var sceneBindings	        : DataBindings	= _scene.bindings;
			var meshBindingsUsageCount  : Object        = _meshBindingsUsageCount[meshBindings];
			var numKeys	    	    	: uint			= signature.numKeys;
			
			for (var keyId : uint = 0; keyId < numKeys; ++keyId)
			{
				var key 	: String	= signature.getKey(keyId);
				var flags	: uint		= signature.getFlags(keyId);
				
				if (flags & Signature.SOURCE_MESH)
				{
					meshBindingsUsageCount[key]++;
					if (!meshBindings.hasCallback(key, bindingsPropertyChangedHandler))
						meshBindings.addCallback(key, bindingsPropertyChangedHandler);
				}
				else if (!sceneBindings.hasCallback(key, bindingsPropertyChangedHandler))
				{
					_sceneBindingsUsageCount[key]++;
					sceneBindings.addCallback(key, bindingsPropertyChangedHandler);
				}
			}
		}
		
		private function unbindEffectInstance(effectInstance	: EffectInstance,
											  meshBindings		: DataBindings) : void
		{
			unwatchSignature(effectInstance.signature, meshBindings);
			// FIXME: effectInstance.release();
		}
		
		private function unbindShaderInstance(passInstance	: ShaderInstance,
											  drawCall		: DrawCall,
											  meshBindings	: DataBindings) : void
		{
			var sceneBindings : DataBindings = _scene.bindings;
			
			drawCall.unsetBindings(meshBindings, sceneBindings);

            if (passInstance)
            {
				unwatchSignature(passInstance.signature, meshBindings);
    			
    			// release the shader
    			passInstance.release();
    			// update indexes
    			delete _drawCallToPassInstance[drawCall];
            }
			
			delete _drawCallToMeshBindings[drawCall];
			
			var drawCalls 		: Array	= _passInstanceToDrawCalls[passInstance];
			var numDrawCalls	: uint	= drawCalls.length - 1;
			
			for (var drawCallId : uint = drawCalls.indexOf(drawCall);
				drawCallId < numDrawCalls;
				++drawCallId)
				drawCalls[drawCallId] = drawCalls[uint(drawCallId + 1)];
			drawCalls.length = numDrawCalls;
			
			if (drawCalls.length == 0)
			{
				delete _passInstanceToDrawCalls[passInstance];
				_passes.splice(_passes.indexOf(passInstance), 1);
			}
		}
		
		private function unwatchSignature(signature 	: Signature,
										  meshBindings	: DataBindings) : void
		{
			var sceneBindings	        : DataBindings	= _scene.bindings;
			var meshBindingsUsageCount  : Object        = _meshBindingsUsageCount[meshBindings];
			var numKeys					: uint			= signature.numKeys;
			
			for (var keyId : uint = 0; keyId < numKeys; ++keyId)
			{
				var key 	: String	= signature.getKey(keyId);
				var flags	: uint		= signature.getFlags(keyId);
				
				if (flags & Signature.SOURCE_MESH)
				{
					meshBindingsUsageCount[key]--;
					if (meshBindingsUsageCount[key] == 0)
					{
						meshBindings.removeCallback(key, bindingsPropertyChangedHandler);
						delete meshBindingsUsageCount[key];
					}
				}
				else
				{
					_sceneBindingsUsageCount[key]--;
					if (_sceneBindingsUsageCount[key] == 0)
					{
						sceneBindings.removeCallback(key, bindingsPropertyChangedHandler);
						delete _sceneBindingsUsageCount[key];
					}
				}
			}
		}
		
		private function bindingsPropertyChangedHandler(bindings 		: DataBindings,
														propertyName	: String,
														oldValue		: Object,
														newValue		: Object) : void
		{
			var changes : Vector.<String>	= _stashedPropertyChanges[bindings];
			
			if (!changes)
				_stashedPropertyChanges[bindings] = changes = new <String>[propertyName];
			else
				changes.push(propertyName);
			
			++_numStashedPropertyChanges;
		}
        
        private function effectPassCompilationFailedHandler(effect  : Effect,
                                                            pass    : Shader,
                                                            error   : ShaderCompilerError) : void
        {
            
        }
	}
}
