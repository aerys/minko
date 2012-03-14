package aerys.minko.scene.controller.mesh
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.render.DrawCall;
	import aerys.minko.render.RenderingList;
	import aerys.minko.render.effect.Effect;
	import aerys.minko.render.shader.ActionScriptShader;
	import aerys.minko.render.shader.Shader;
	import aerys.minko.render.shader.ShaderSignature;
	import aerys.minko.scene.controller.AbstractController;
	import aerys.minko.scene.controller.ControllerMode;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.scene.node.mesh.Mesh;
	import aerys.minko.scene.node.mesh.geometry.Geometry;
	import aerys.minko.type.Signal;
	import aerys.minko.type.bounding.FrustumCulling;
	import aerys.minko.type.data.DataBindings;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.StreamUsage;
	import aerys.minko.type.stream.format.VertexComponent;
	
	import flash.utils.Dictionary;

	/**
	 * The RenderingController works on meshes to issue all the related
	 * rendering operations and push them in the rendering list.
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public final class RenderingController extends AbstractController
	{
		use namespace minko_scene;
		
		private static var _instance	: RenderingController		= null;
		
		public static function get renderingController() : RenderingController
		{
			return _instance || (_instance = new RenderingController());
		}
		
		private var _effect				: Effect					= null;
		
		private var _signatures			: Vector.<ShaderSignature>	= new <ShaderSignature>[];
		private var _invalidSignatures	: Vector.<ShaderSignature>	= new <ShaderSignature>[];
		private var _hashToInstance		: Object					= new Object();
		
		private var _meshToPasses		: Dictionary				= new Dictionary(true);
		private var _meshToDrawCalls	: Dictionary				= new Dictionary(true);
		
		private var _drawCallCreated	: Signal					= new Signal();
		
		public function get drawCallCreated() : Signal
		{
			return _drawCallCreated;
		}
		
		public function RenderingController(effect 	: Effect	= null)
		{
			super(Mesh, ControllerMode.SIGNAL);
			
			_effect = effect;
			
			initialize();
		}
		
		override public function clone() : AbstractController
		{
			return this;
		}
		
		private function initialize() : void
		{
			targetAdded.add(targetAddedHandler);
			targetRemoved.add(targetRemovedHandler);
		}
		
		private function targetAddedHandler(controller	: RenderingController,
											target		: Mesh) : void
		{
			target.addedToScene.add(meshAddedToSceneHandler);
			target.removedFromScene.add(meshRemovedFromSceneHandler);
			
			if (target.root is Scene)
				meshAddedToSceneHandler(target, target.root as Scene);
		}
		
		private function targetRemovedHandler(controller	: RenderingController,
											  target		: Mesh) : void
		{
			target.addedToScene.remove(meshAddedToSceneHandler);
			target.removedFromScene.remove(meshRemovedFromSceneHandler);
		}
		
		private function meshAddedToSceneHandler(mesh	: Mesh,
												 scene	: Scene) : void
		{
			addDrawCalls(mesh, scene);
			
			mesh.effectChanged.add(meshEffectChangedHandler);
			mesh.visibilityChanged.add(meshVisibilityChangedHandler);
		}
		
		private function meshRemovedFromSceneHandler(mesh	: Mesh,
													 scene	: Scene) : void
		{
			removeDrawCalls(mesh, scene);
			
			mesh.effectChanged.remove(meshEffectChangedHandler);
			mesh.visibilityChanged.remove(meshVisibilityChangedHandler);
		}
		
		private function meshVisibilityChangedHandler(mesh			: Mesh,
													  visibility	: Boolean) : void
		{
			var drawCalls		: Vector.<DrawCall>	= _meshToDrawCalls[mesh];
			var numDrawCalls	: uint				= drawCalls.length;
			
			for (var callId : uint = 0; callId < numDrawCalls; ++callId)
				(drawCalls[callId] as DrawCall).enabled = visibility;
		}
		
		private function meshEffectChangedHandler(mesh		: Mesh,
												  oldEffect	: Effect,
												  newEffect	: Effect) : void
		{
			var scene : Scene = mesh.root as Scene;
			
			removeDrawCalls(mesh, scene);
			addDrawCalls(mesh, scene);
		}
		
		private function meshFrustumCullingChanged(mesh				: Mesh,
												   frustumCulling	: uint) : void
		{
			if (frustumCulling == FrustumCulling.DISABLED)
			{
				
			}
			else
			{
				
			}
		}
		
		private function addDrawCalls(mesh : Mesh, scene : Scene) : void
		{
			var effect		: Effect	= _effect || mesh.effect;
			var numPasses	: uint		= effect.numPasses;
			
			for (var i : uint = 0; i < numPasses; ++i)
				createShader(effect, mesh, i);
			
			scene.renderingList.addDrawCalls(
				_meshToPasses[mesh], _meshToDrawCalls[mesh]
			);
		}
		
		private function removeDrawCalls(mesh : Mesh, scene : Scene) : void
		{
			var passes 		: Vector.<Shader> 	= _meshToPasses[mesh];
			var numShaders	: uint				= passes.length;
			
			for (var i : uint = 0; i < numShaders; ++i)
				unwatchSignature(passes[i].signature, mesh.bindings, scene.bindings);
			
			scene.renderingList.removeDrawCalls(
				_meshToPasses[mesh], _meshToDrawCalls[mesh]
			);
		}
		
		private function createShader(effect 	: Effect,
									  mesh		: Mesh,
									  passId 	: uint) : void
		{
			var meshBindings	: DataBindings			= mesh.bindings;
			var sceneBindings	: DataBindings			= (mesh.root as Scene).bindings;
			var asShader		: ActionScriptShader	= effect.getPass(passId);
			var instance		: Shader 				= asShader.fork(
				meshBindings,
				sceneBindings
			);
			var drawCall		: DrawCall				= instance.program.createDrawCall();
			var drawCalls		: Vector.<DrawCall>		= _meshToDrawCalls[mesh] as Vector.<DrawCall>;
			var passes			: Vector.<Shader>		= _meshToPasses[mesh] as Vector.<Shader>;
			
			configureDrawCall(mesh, drawCall);
			
			if (!drawCalls)
			{
				_meshToDrawCalls[mesh] = new <DrawCall>[drawCall];
				_meshToPasses[mesh] = new <Shader>[instance];
			}
			else
			{
				drawCalls[passId] = drawCall;
				passes[passId] = instance;
			}
			
			watchSignature(instance.signature, meshBindings, sceneBindings);
			
			_drawCallCreated.execute(this, drawCall);
		}
		
		private function configureDrawCall(mesh		: Mesh,
										   drawCall : DrawCall) : void
		{
			var components 	: Vector.<VertexComponent> 	= drawCall.vertexComponents;
			var geom		: Geometry					= mesh.geometry;
			var vstream		: IVertexStream				= geom.getVertexStream(0);
			
			if (components.indexOf(VertexComponent.TANGENT) >= 0
				&& vstream.getStreamByComponent(VertexComponent.TANGENT) == null)
			{
				geom.computeTangentSpace(StreamUsage.STATIC);
			}
			else if (components.indexOf(VertexComponent.NORMAL) >= 0
				&& vstream.getStreamByComponent(VertexComponent.NORMAL) == null)
			{
				geom.computeNormals(StreamUsage.STATIC);
			}
			
			drawCall.setStreams(geom._vertexStreams, geom.indexStream);
			drawCall.setBindings(mesh.bindings, (mesh.root as Scene).bindings);
		}
		
		private function watchSignature(signature 		: ShaderSignature,
										meshBindings	: DataBindings,
										sceneBindings	: DataBindings) : void
		{
			_signatures.push(signature);
			
			var numKeys	: uint	= signature.numKeys;
			
			for (var i : uint = 0; i < numKeys; ++i)
			{
				var key 	: String	= signature.getKey(i);
				var flags	: uint		= signature.getFlags(i);
				
				if (flags & ShaderSignature.SOURCE_MESH)
				{
					meshBindings.getPropertyChangedSignal(key).add(
						meshPropertyChangedHandler
					);
				}
				else
				{
					sceneBindings.getPropertyChangedSignal(key).add(
						scenePropertyChangedHandler
					);
				}
			}
		}
		
		private function unwatchSignature(signature		: ShaderSignature,
										  meshBindings	: DataBindings,
										  sceneBindings	: DataBindings) : void
		{
			var numKeys			: uint	= signature.numKeys;
			var index 			: int 	= _signatures.indexOf(signature);
			var numSignatures	: uint	= _signatures.length - 1;
			
			_signatures[index] = _signatures[numSignatures];
			_signatures.length = numSignatures;
			
			for (var i : uint = 0; i < numKeys; ++i)
			{
				var key 	: String	= signature.getKey(i);
				var flags	: uint		= signature.getFlags(i);
				
				if (flags & ShaderSignature.SOURCE_MESH)
				{
					meshBindings.getPropertyChangedSignal(key).remove(
						meshPropertyChangedHandler
					);
				}
				else
				{
					sceneBindings.getPropertyChangedSignal(key).remove(
						scenePropertyChangedHandler
					);
				}
			}
		}
		
		private function meshPropertyChangedHandler(dataBindings	: DataBindings,
													propertyName	: String,
													oldValue		: Object,
													newValue		: Object) : void
		{
			checkSignatures(propertyName, newValue, ShaderSignature.SOURCE_MESH);
		}
		
		private function scenePropertyChangedHandler(dataBindings	: DataBindings,
													 propertyName	: String,
													 oldValue		: Object,
													 newValue		: Object) : void
		{
			checkSignatures(propertyName, newValue, ShaderSignature.SOURCE_SCENE);
		}
		
		private function checkSignatures(propertyName	: String,
										 newValue		: Object,
										 flags			: uint) : void
		{
			var numSignatures	: uint	= _signatures.length;
			
			for (var i : uint = 0; i < numSignatures; ++i)
			{
				var signature		: ShaderSignature 	= _signatures[i] as ShaderSignature;
				var validSignature	: Boolean 			= signature.checkProperty(
					propertyName,
					newValue,
					flags
				);
				
				if (!validSignature && _invalidSignatures.indexOf(signature) < 0)
					invalidateSignature(signature);
			}
		}
		
		private function invalidateSignature(signature : ShaderSignature) : void
		{
			var fork : Shader	= signature.shader.getForkBySignature(
				signature
			);
			
			for (var meshObject : Object in _meshToPasses)
			{
				var passes		: Vector.<Shader>	= _meshToPasses[meshObject];
				var passId		: int				= passes.indexOf(fork);
				
				if (passId >= 0)
				{
					var mesh		: Mesh				= meshObject as Mesh;
					var list		: RenderingList		= (mesh.root as Scene).renderingList;
					var drawCalls	: Vector.<DrawCall>	= _meshToDrawCalls[meshObject];
					
					list.removeDrawCall(passes[passId], drawCalls[passId]);
					createShader(mesh.effect, mesh, passId);
					list.addDrawCall(passes[passId], drawCalls[passId]);
				}
			}
		}
	}
}