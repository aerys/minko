package aerys.minko.scene.controller
{
	import aerys.minko.ns.minko_scene;
	import aerys.minko.render.DrawCall;
	import aerys.minko.render.RenderingList;
	import aerys.minko.render.effect.Effect;
	import aerys.minko.render.shader.ShaderInstance;
	import aerys.minko.render.shader.ShaderSignature;
	import aerys.minko.scene.node.Scene;
	import aerys.minko.scene.node.mesh.Mesh;
	import aerys.minko.type.data.DataBindings;
	import aerys.minko.type.stream.IVertexStream;
	import aerys.minko.type.stream.StreamUsage;
	import aerys.minko.type.stream.format.VertexComponent;
	
	import flash.utils.Dictionary;

	public class RenderingController extends AbstractController
	{
		use namespace minko_scene;
		
		private static var _instance	: RenderingController		= null;
		
		public static function get renderingController() : RenderingController
		{
			return _instance || (_instance = new RenderingController());
		}
		
		private var _signatures			: Vector.<ShaderSignature>	= new <ShaderSignature>[];
		private var _invalidSignatures	: Vector.<ShaderSignature>	= new <ShaderSignature>[];
		private var _hashToInstance		: Object					= new Object();
		
		private var _meshToPasses		: Dictionary				= new Dictionary(true);
		private var _meshToDrawCalls	: Dictionary				= new Dictionary(true);
		
		public function RenderingController(mode : uint = ControllerMode.SIGNAL)
		{
			super(mode, Mesh);
			
			initialize();
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
			var effect		: Effect	= mesh.effect;
			var numPasses	: uint		= effect.numPasses;
			
			for (var i : uint = 0; i < numPasses; ++i)
				createShaderInstance(effect, mesh, i);
			
			scene.renderingList.addDrawCalls(
				_meshToPasses[mesh], _meshToDrawCalls[mesh]
			);
		}
		
		private function meshRemovedFromSceneHandler(mesh	: Mesh,
													 scene	: Scene) : void
		{
			scene.renderingList.removeDrawCalls(
				_meshToPasses[mesh], _meshToDrawCalls[mesh]
			);
		}
		
		private function effectChangedHandler(mesh		: Mesh,
											  oldEffect	: Effect,
											  newEffect	: Effect) : void
		{
			throw new Error();
		}
		
		private function createShaderInstance(effect 	: Effect,
											  mesh		: Mesh,
											  passId 	: uint) : void
		{
			var meshBindings	: DataBindings				= mesh.bindings;
			var sceneBindings	: DataBindings				= (mesh.root as Scene).bindings;
			var instance		: ShaderInstance 			= effect.getPass(passId).instanciate(
				meshBindings,
				sceneBindings
			);
			var drawCall		: DrawCall					= instance.program.createDrawCall();
			var drawCalls		: Vector.<DrawCall>			= _meshToDrawCalls[mesh] as Vector.<DrawCall>;
			var passes			: Vector.<ShaderInstance>	= _meshToPasses[mesh] as Vector.<ShaderInstance>;
			
			configureDrawCall(mesh, drawCall);
			
			if (!drawCalls)
			{
				_meshToDrawCalls[mesh] = new <DrawCall>[drawCall];
				_meshToPasses[mesh] = new <ShaderInstance>[instance];
			}
			else
			{
				drawCalls[passId] = drawCall;
				passes[passId] = instance;
			}
			
			watchSignature(instance.signature, meshBindings, sceneBindings);
		}
		
		private function configureDrawCall(mesh		: Mesh,
										   drawCall : DrawCall) : void
		{
			var components 	: Vector.<VertexComponent> 	= drawCall.vertexComponents;
			var vstream		: IVertexStream				= mesh.getVertexStream(0);
			
			if (components.indexOf(VertexComponent.TANGENT) >= 0
				&& vstream.getStreamByComponent(VertexComponent.TANGENT) == null)
			{
				mesh.computeTangentSpace(StreamUsage.STATIC);
			}
			else if (components.indexOf(VertexComponent.NORMAL) >= 0
				&& vstream.getStreamByComponent(VertexComponent.NORMAL) == null)
			{
				mesh.computeNormals(StreamUsage.STATIC);
			}
			
			drawCall.setStreams(mesh._vertexStreams, mesh.indexStream);
			drawCall.setBindings(mesh.bindings, (mesh.root as Scene).bindings);
		}
		
		private function watchSignature(signature 		: ShaderSignature,
										meshBindings	: DataBindings,
										sceneBindings	: DataBindings) : void
		{
			if (_signatures.indexOf(signature) < 0)
				_signatures.push(signature);
			
			var numKeys	: uint	= signature.numKeys;
			
			for (var i : uint = 0; i < numKeys; ++i)
			{
				var key 	: String		= signature.getKey(i);
				var flags	: uint			= signature.getFlags(i);
				
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
				var signature		: ShaderSignature = _signatures[i] as ShaderSignature;
				var validSignature	: Boolean = signature.checkProperty(
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
			var instance	: ShaderInstance	= signature.shader.getInstanceBySignature(
				signature
			);
			
			for (var meshObject : Object in _meshToPasses)
			{
				var passes		: Vector.<ShaderInstance>	= _meshToPasses[meshObject];
				var passId		: int						= passes.indexOf(instance);
				
				if (passId >= 0)
				{
					var mesh		: Mesh				= meshObject as Mesh;
					var list		: RenderingList		= (mesh.root as Scene).renderingList;
					var drawCalls	: Vector.<DrawCall>	= _meshToDrawCalls[meshObject];
					
					list.removeDrawCall(passes[passId], drawCalls[passId]);
					createShaderInstance(mesh.effect, mesh, passId);
					list.addDrawCall(passes[passId], drawCalls[passId]);
				}
			}
		}
	}
}