package aerys.minko.scene.controller.scene
{
	import aerys.minko.Minko;
	import aerys.minko.ns.minko_render;
	import aerys.minko.render.DrawCall;
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.Viewport;
	import aerys.minko.render.effect.Effect;
	import aerys.minko.render.shader.ActionScriptShader;
	import aerys.minko.render.shader.ShaderInstance;
	import aerys.minko.render.shader.Signature;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.node.Group;
	import aerys.minko.scene.node.ISceneNode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.scene.node.mesh.Mesh;
	import aerys.minko.scene.node.mesh.geometry.Geometry;
	import aerys.minko.type.Factory;
	import aerys.minko.type.data.DataBindings;
	import aerys.minko.type.log.DebugLevel;
	
	import flash.display.BitmapData;
	import flash.display3D.Context3D;
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
	public class RenderingController extends AbstractController
	{
		private static const MESHES : Vector.<ISceneNode> = new Vector.<ISceneNode>();
		
		use namespace minko_render;
		
		private var _scene						: Scene						= null;
		
		private var _stashedPropertyChanges		: Dictionary				= null;
		
		private var _passInstances				: Vector.<ShaderInstance>	= null;
		private var _passInstancesIsSorted		: Boolean					= false;
		
		private var _drawCallToPassInstance		: Dictionary				= null;
		private var _passInstanceToDrawCalls	: Dictionary				= null;
		
		private var _meshToDrawCalls			: Dictionary				= null;
		private var _drawCallToMeshBindings		: Dictionary				= null;
		
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
		private var _meshBindingToMesh			: Dictionary;
		
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
		private var _effectToMeshes				: Dictionary;
		
		public function RenderingController()
		{
			super(Scene);
			
			_passInstances				= new Vector.<ShaderInstance>();
			_passInstancesIsSorted		= true;
			
			_drawCallToPassInstance		= new Dictionary();
			_drawCallToMeshBindings		= new Dictionary();
			_passInstanceToDrawCalls	= new Dictionary();
			_meshToDrawCalls			= new Dictionary();
			_effectToMeshes				= new Dictionary();
			_meshBindingToMesh			= new Dictionary();
			_stashedPropertyChanges		= new Dictionary();
			
			targetAdded.add(targetAddedHandler);
			targetRemoved.add(targetRemovedHandler);
		}
		
		/**
		 * Render current Scene
		 */
		public function render(context		: Context3D, 
							   backBuffer	: RenderTarget) : uint
		{
			applyBindingChanges();
			
			var numPasses		: uint = _passInstances.length;
			var numTriangles	: uint = 0;
			
			context.enableErrorChecking = (Minko.debugLevel & DebugLevel.CONTEXT) != 0;
			
			// sort states
			if (!_passInstancesIsSorted && numPasses != 0)
			{
				ShaderInstance.sort(_passInstances, numPasses);
				_passInstancesIsSorted = true;
			}
			
			// apply states
			var previous 		: ShaderInstance 	= null;
			var callTriangles	: uint				= 0;
			var call			: DrawCall			= null;
			var previousCall	: DrawCall			= null;
			
			for (var i : int = 0; i < numPasses; ++i)
			{
				var pass 		: ShaderInstance		= _passInstances[i];
				var calls 		: Vector.<DrawCall> = _passInstanceToDrawCalls[pass];
				var numCalls	: int				= calls.length;
				
				if (!pass.config.enabled)
					continue;
				
				pass.prepareContext(context, backBuffer, previous);
				
				previous = pass;
				
				for (var j : int = 0; j < numCalls; ++j)
				{
					call = calls[j];
					callTriangles = call.apply(context, previousCall);
					
					if (callTriangles != 0)
					{
						numTriangles += callTriangles;
						previousCall = call;
					}
				}
			}
			
			if (numTriangles == 0)
			{
				var color : uint = backBuffer.backgroundColor;
				
				context.clear(
					((color >> 16) & 0xff) / 255.,
					((color >> 8) & 0xff) / 255.,
					(color & 0xff) / 255.,
					((color >> 24) & 0xff) / 255.
				);
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
				throw new Error('RenderingController must be instanciated for every Scene');
			
			_scene = scene;
			_scene.enterFrame.add(sceneEnterFrameHandler);
			_scene.descendantAdded.add(childAddedHandler);
			_scene.descendantRemoved.add(childRemovedHandler);
		}
		
		private function sceneEnterFrameHandler(scene 		: Scene,
												viewport 	: Viewport,
												target 		: BitmapData,
												time		: Number) : void
		{
			var context		: Context3D		= viewport.getContext3D();
			
			if (context)
			{
				render(context, viewport.getBackBuffer());
				
				if (target)
					context.drawToBitmapData(target);
				else
					context.present();
				
				Factory.sweep();
			}
		}
		
		/**
		 * Remove callbacks and reset the whole controller.
		 */
		private function targetRemovedHandler(controller : RenderingController,
											  scene		 : Scene) : void
		{
			_scene.descendantAdded.remove(childAddedHandler);
			_scene.descendantRemoved.remove(childRemovedHandler);
			_scene = null;
		}
		
		/**
		 * Is called each time something is added to the scene.
		 * This will forward its calls to addMesh.
		 */
		private function childAddedHandler(group : Group,
										   child : ISceneNode) : void
		{
			if (child is Mesh)
				addMesh(Mesh(child));
			else if (child is Group)
			{
				for each (var mesh : Mesh in Group(child).getDescendantsByType(Mesh, MESHES))
					addMesh(mesh);
				MESHES.length = 0;
			}
		}
		
		/**
		 * Is called each time something is removed from the scene.
		 * This will forward its calls to removeMesh.
		 */
		private function childRemovedHandler(group : Group,
											 child : ISceneNode) : void
		{
			if (child is Mesh)
				removeMesh(Mesh(child));
			else if (child is Group)
			{
				for each (var mesh : Mesh in Group(child).getDescendantsByType(Mesh, MESHES))
					removeMesh(mesh);
				MESHES.length = 0;
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
			var drawCalls : Vector.<DrawCall>	= new Vector.<DrawCall>(numPasses, true);
			
			for (var i : uint = 0; i < numPasses; ++i)
			{
				// fork pass if needed
				var asShader		: ActionScriptShader	= meshEffect.getPass(i);
				var passInstance	: ShaderInstance		= asShader.fork(meshBindings, sceneBindings);
				
				// create drawcall
				var drawCall		: DrawCall		= new DrawCall();
				drawCall.configure(passInstance.program, mesh.geometry, meshBindings, sceneBindings);
				drawCalls[i] = drawCall;
				
				// retain the instance, update indexes, watch for invalidation, give to renderingList.
				bind(passInstance, drawCall, meshBindings);
			}
			
			// update indexes
			_meshToDrawCalls[mesh]				= drawCalls;
			_meshBindingToMesh[meshBindings]	= mesh;
			
			if (_effectToMeshes[meshEffect] == undefined)
			{
				_effectToMeshes[meshEffect] = new Vector.<Mesh>();
				meshEffect.passesChanged.add(effectPassesChangedHandler);
			}
			
			_effectToMeshes[meshEffect].push(mesh);
			
			//register to visibility change signal
			mesh.visibilityChanged.add(meshVisibilityChangedHandler);
		}
		
		private function removeMesh(mesh : Mesh) : void
		{
			// retrieve references to the data we want to use, to save some function calls
			var meshEffect		: Effect		= mesh.effect;
			var meshBindings	: DataBindings	= mesh.bindings;
			var numPasses		: uint			= meshEffect.numPasses;
			var sceneBindings	: DataBindings	= _scene.bindings;
			
			// retrieve drawcalls
			var drawCalls : Vector.<DrawCall>	= _meshToDrawCalls[mesh];
			
			for (var drawCallId : uint = 0; drawCallId < numPasses; ++drawCallId)
			{
				// retrieve drawcall, and shaderInstance
				var drawCall		: DrawCall			= drawCalls[drawCallId];
				var passInstance	: ShaderInstance	= _drawCallToPassInstance[drawCall];
				
				unbind(passInstance, drawCall, meshBindings);
			}
			
			// update indexes
			delete _meshToDrawCalls[mesh];
			delete _meshBindingToMesh[meshBindings];
			
			var meshesWithSameEffect : Vector.<Mesh> = _effectToMeshes[meshEffect];
			meshesWithSameEffect.splice(meshesWithSameEffect.indexOf(mesh), 1);
			
			if (meshesWithSameEffect.length == 0)
			{
				delete _effectToMeshes[meshEffect];
				meshEffect.passesChanged.remove(effectPassesChangedHandler);
			}
			
			//remove to visibility change signal
			mesh.visibilityChanged.remove(meshVisibilityChangedHandler);
		}
		
		private function meshVisibilityChangedHandler(mesh		 : Mesh,
													  visibility : Boolean) : void
		{
			var drawCalls		: Vector.<DrawCall>	= _meshToDrawCalls[mesh];
			var numDrawCalls	: uint				= drawCalls.length;
			
			for (var drawCallId : uint = 0; drawCallId < numDrawCalls; ++drawCallId)
				drawCalls[drawCallId].enabled = visibility;
		}
		
		private function effectPassesChangedHandler(effect : Effect) : void
		{
			
		}
		
		private function meshEffectChangedHandler(mesh		: Mesh,
												  oldEffect	: Effect,
												  newEffect	: Effect) : void
		{
			
		}
		
		private function applyBindingChanges() : void
		{
			var sceneBindings		: DataBindings		= _scene.bindings;
			var numShaderInstances	: int				= _passInstances.length;
			var sceneChanges		: Vector.<String>	= _stashedPropertyChanges[sceneBindings];
			
			for (var shaderInstanceId : int = numShaderInstances - 1; shaderInstanceId >= 0; --shaderInstanceId)
			{
				var passInstance			: ShaderInstance		= _passInstances[shaderInstanceId];
				var passInstanceSignature	: Signature			= passInstance.signature;
				var drawCalls				: Vector.<DrawCall>	= _passInstanceToDrawCalls[passInstance];
				var numDrawCalls			: int				= drawCalls.length;
				
				var needUpdateFromScene		: Boolean = sceneChanges != null ? 
					passInstanceSignature.useProperties(_stashedPropertyChanges[sceneBindings], true) : false;
				
				for (var drawCallId : int = numDrawCalls - 1; drawCallId >= 0; --drawCallId)
				{
					var drawCall			: DrawCall			= drawCalls[drawCallId];
					var meshBindings		: DataBindings		= _drawCallToMeshBindings[drawCall];
					var meshGeometry		: Geometry			= Mesh(_meshBindingToMesh[meshBindings]).geometry
					var meshChanges			: Vector.<String>	= _stashedPropertyChanges[meshBindings];
					
					var needUpdateFromMesh	: Boolean = meshChanges != null ?
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
					
					var replacementInstance : ShaderInstance = passInstance.generator.fork(meshBindings, sceneBindings);
					drawCall.configure(replacementInstance.program, meshGeometry, meshBindings, sceneBindings);
					bind(replacementInstance, drawCall, meshBindings);
				}
			}
			
			// reset all changes stashes
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
					meshBindings.getPropertyChangedSignal(key).add(bindingsPropertyChangedHandler);
				else
					sceneBindings.getPropertyChangedSignal(key).add(bindingsPropertyChangedHandler);
			}
			
			// retain the shader
			passInstance.retain();
			
			// update indexes
			_drawCallToPassInstance[drawCall] = passInstance;
			
			if (!_passInstanceToDrawCalls[passInstance])
			{
				_passInstanceToDrawCalls[passInstance] = new <DrawCall>[];
				_passInstances.push(passInstance);
				_passInstancesIsSorted = false;
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
					meshBindings.getPropertyChangedSignal(key).remove(bindingsPropertyChangedHandler);
				else
					sceneBindings.getPropertyChangedSignal(key).remove(bindingsPropertyChangedHandler);
			}
			
			// release the shader
			passInstance.release();
			
			// update indexes
			delete _drawCallToPassInstance[drawCall];
			delete _drawCallToMeshBindings[drawCall];
			
			var drawCalls : Vector.<DrawCall> = _passInstanceToDrawCalls[passInstance];
			drawCalls.splice(drawCalls.indexOf(drawCall), 1);
			
			if (drawCalls.length == 0)
			{
				delete _passInstanceToDrawCalls[passInstance];
				_passInstances.splice(_passInstances.indexOf(passInstance), 1);
			}
		}
		
		private function bindingsPropertyChangedHandler(meshBindings 	: DataBindings,
														propertyName	: String,
														newValue		: Object) : void
		{
			if (!_stashedPropertyChanges[meshBindings])
				_stashedPropertyChanges[meshBindings] = new Vector.<String>();
			
			_stashedPropertyChanges[meshBindings].push(propertyName);
		}
		
	}
}
