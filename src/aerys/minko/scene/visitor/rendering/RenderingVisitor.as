package aerys.minko.scene.visitor.rendering
{
	import aerys.minko.effect.IEffect;
	import aerys.minko.effect.IEffectPass;
	import aerys.minko.ns.minko;
	import aerys.minko.render.IRenderer;
	import aerys.minko.render.ressource.IRessource;
	import aerys.minko.render.state.RenderState;
	import aerys.minko.scene.graph.*;
	import aerys.minko.scene.graph.group.*;
	import aerys.minko.scene.graph.mesh.*;
	import aerys.minko.scene.graph.texture.*;
	import aerys.minko.scene.visitor.*;
	import aerys.minko.scene.visitor.data.*;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.VertexStreamList;
	
	import flash.utils.Dictionary;
	
	public class RenderingVisitor implements ISceneVisitor
	{
		use namespace minko;
		
		protected var _renderer			: IRenderer;
		protected var _numNodes			: uint;
		protected var _frameId			: uint;
		
		protected var _styleStack		: StyleStack;
		protected var _worldData		: Dictionary;
		protected var _tm				: TransformManager;
		protected var _fx				: Vector.<IEffect>;
		
		protected var _transformData	: TransformData;
		
		public function get numNodes() : uint
		{
			return _numNodes;
		}
		
		// [RemoveMe]
		// 		pourquoi on wrap cette methode? elle ne sert que dans Viewport qui connait aussi le renderer
		
				public function get drawingTime() : uint
				{
					return _renderer.drawingTime;
				}
				
				public function get numTriangles() : uint
				{
					return _renderer.numTriangles;
				}
		
		// [/RemoveMe]
		
		public function RenderingVisitor(renderer : IRenderer)
		{
			_renderer		= renderer;
			
			_styleStack		= new StyleStack();
			_worldData		= new Dictionary();
			_tm				= new TransformManager();
			_fx				= new Vector.<IEffect>();
			
			_transformData	= new TransformData(_tm);
			_frameId		= 0;
		}
		
		
		public function reset() : void
		{
//			_renderer.clear();
			_worldData		= null;
			_numNodes		= 0;
			++_frameId;
		}
		
		public function updateWorldData(worldData : Dictionary) : void
		{
			_worldData = worldData;
			
			for each (var worldObject : IWorldData in worldData)
				worldObject.setLocalDataProvider(_styleStack, _transformData);
			
			// update our transformManager if there is a camera, or
			// set it to null to render to screenspace otherwise
			var cameraData : CameraData = worldData[CameraData] as CameraData;
			if (cameraData)
			{
				_tm.view		= cameraData.view;
				_tm.projection	= cameraData.projection;
			}
			
			
		}
		
		public function visit(scene : IScene) : void
		{
			var transformObject : ITransformable	= scene as ITransformable;
			var styleObject		: IStyled			= scene as IStyled;
			var effectObject	: EffectGroup		= scene as EffectGroup;
			
			// push transform, style and effect
			pushData(transformObject, styleObject, effectObject);
			
			// Act in accordance with the node type
			if (scene is IMesh)
			{
				queryIMesh(scene as IMesh);
			}
			else if (scene is ITexture)
			{
				queryITexture(scene as ITexture);
			}
			else if (scene is Model)
			{
				queryModel(scene as Model);
			}
			else if (scene is IGroup)
			{
				// the IGroup test is last, because AnimatedMaterial extends Group
				queryIGroup(scene as IGroup);
			}
			
			// update statistical data
			++_numNodes;
			
			// pop transform, style and effect
			popData(transformObject, styleObject, effectObject);
		}
		
		protected function pushData(transformObject	: ITransformable, 
									styleObject		: IStyled, 
									effectObject	: EffectGroup) : void
		{
			
			if (transformObject)
				_tm.world.push().multiply(transformObject.transform);
		
			if (styleObject)
				_styleStack.push(styleObject.style);
			
			if (effectObject)
				for each (var effect : IEffect in effectObject.effects)
				_fx.push(effect);
		}
		
		protected function popData(transformObject	: ITransformable, 
								   styleObject		: IStyled, 
								   effectObject		: EffectGroup) : void
		{
			if (transformObject)
				_tm.world.pop();
			
			if (styleObject)
				_styleStack.pop();
			
			if (effectObject)
			{
				var effectCount : uint = effectObject.effects.length;
				while (effectCount--)
					_fx.pop();
			}
		}
		
		protected function queryIGroup(scene : IGroup) : void
		{
			for (var childIndex : uint = 0; true; ++childIndex)
			{
				var child : IScene = scene.getChildAt(childIndex);
				if (child == null)
					break;
				
				visit(child);
			}
		}
		
		protected function queryIMesh(scene : IMesh) : void
		{
			for each (var worldObject : IWorldData in _worldData)
				worldObject.invalidate();
			
			// pass "ready to draw" data to the renderer.
			var vertexStreamList 	: VertexStreamList	= scene.vertexStreamList;
			var indexStream 		: IndexStream		= scene.indexStream;
			
			var numEffects 	: int = _fx.length;
			if (numEffects == 0)
				throw new Error("Unable to draw without an effect.");
			
			for (var i : int = 0; i < numEffects; ++i)
			{
				var fx			: IEffect					= _fx[i];
				
//				_styleStack.push(fx.style);
				fx.prepare(_styleStack, _transformData, _worldData);
				
				var passes		: Vector.<IEffectPass>		= fx.passes;
				var numPasses 	: int 						= passes.length;
				
				for (var j : int = 0; j < numPasses; ++j)
				{
					_renderer.begin();
					
					var pass	: IEffectPass = passes[j];
					var state	: RenderState = _renderer.state;
					
					if (pass.fillRenderState(state, _styleStack, _transformData, _worldData))
					{
						state.setInputStreams(vertexStreamList, indexStream);
						_renderer.drawTriangles();
					}
					
					_renderer.end();
				}
				
//				_styleStack.pop();
			}
		}
		
		protected function queryITexture(scene : ITexture) : void
		{
			if (scene is AnimatedTexture)
			{
				queryAnimatedTexture(scene as AnimatedTexture);
			}
			else if (scene is BitmapTexture)
			{
				queryBitmapTexture(scene as BitmapTexture);
			}
		}
		
		protected function queryAnimatedTexture(scene : AnimatedTexture) : void
		{
//			scene.gotoFrame(_frameId);
//			query(scene.getChildAt(_frameId));
		}
		
		protected function queryBitmapTexture(scene : BitmapTexture) : void
		{
			var styleName			: String		= scene.styleProperty;
			var textureRessource	: IRessource	= scene.ressource;
			
			_styleStack.set(styleName, textureRessource);
		}
		
		protected function queryModel(scene : Model) : void
		{
			if (scene.visible)
			{
				queryITexture(scene.texture);
				queryIMesh(scene.mesh);
			}
		}
	}
}