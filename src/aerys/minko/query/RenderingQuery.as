package aerys.minko.query
{
	import aerys.minko.effect.IEffect3D;
	import aerys.minko.effect.IEffect3DPass;
	import aerys.minko.ns.minko;
	import aerys.minko.query.renderdata.style.StyleStack3D;
	import aerys.minko.query.renderdata.transform.TransformData;
	import aerys.minko.query.renderdata.transform.TransformManager;
	import aerys.minko.query.renderdata.world.IWorldData;
	import aerys.minko.render.IRenderer3D;
	import aerys.minko.render.ressource.IRessource3D;
	import aerys.minko.render.ressource.TextureRessource;
	import aerys.minko.render.state.RenderState;
	import aerys.minko.scene.interfaces.IObject3D;
	import aerys.minko.scene.interfaces.IScene3D;
	import aerys.minko.scene.interfaces.IStyled3D;
	import aerys.minko.scene.Model3D;
	import aerys.minko.scene.camera.CameraData;
	import aerys.minko.scene.group.EffectGroup3D;
	import aerys.minko.scene.group.IGroup3D;
	import aerys.minko.scene.material.AnimatedMaterial3D;
	import aerys.minko.scene.material.BitmapMaterial3D;
	import aerys.minko.scene.material.ColorMaterial3D;
	import aerys.minko.scene.material.IMaterial3D;
	import aerys.minko.scene.material.MovieClipMaterial3D;
	import aerys.minko.scene.mesh.IMesh3D;
	import aerys.minko.type.stream.IndexStream3D;
	import aerys.minko.type.stream.VertexStream3DList;
	
	import flash.display3D.textures.Texture;
	import flash.utils.Dictionary;
	
	public class RenderingQuery implements IScene3DQuery
	{
		use namespace minko;
		
		protected var _renderer			: IRenderer3D;
		protected var _numNodes			: uint;
		protected var _frameId			: uint;
		
		protected var _styleStack		: StyleStack3D;
		protected var _worldData		: Dictionary;
		protected var _tm				: TransformManager;
		protected var _fx				: Vector.<IEffect3D>;
		
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
		
		public function RenderingQuery(renderer : IRenderer3D)
		{
			_renderer		= renderer;
			
			_styleStack		= new StyleStack3D();
			_worldData		= new Dictionary();
			_tm				= new TransformManager();
			_fx				= new Vector.<IEffect3D>();
			
			_transformData	= new TransformData(_tm);
			_frameId		= 0;
		}
		
		
		public function reset() : void
		{
			_renderer.clear();
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
		
		public function query(scene : IScene3D) : void
		{
			var transformObject : IObject3D		= scene as IObject3D;
			var styleObject		: IStyled3D		= scene as IStyled3D;
			var effectObject	: EffectGroup3D	= scene as EffectGroup3D;
			
			// push transform, style and effect
			pushData(transformObject, styleObject, effectObject);
			
			// Act in accordance with the node type
			if (scene is IMesh3D)
			{
				queryIMesh3D(scene as IMesh3D);
			}
			else if (scene is IMaterial3D)
			{
				queryIMaterial3D(scene as IMaterial3D);
			}
			else if (scene is Model3D)
			{
				queryModel3D(scene as Model3D);
			}
			else if (scene is IGroup3D)
			{
				// the IGroup test is last, because AnimatedMaterial extends Group
				queryIGroup3D(scene as IGroup3D);
			}
			
			// update statistical data
			++_numNodes;
			
			// pop transform, style and effect
			popData(transformObject, styleObject, effectObject);
		}
		
		protected function pushData(transformObject	: IObject3D, 
									styleObject		: IStyled3D, 
									effectObject	: EffectGroup3D) : void
		{
			
			if (transformObject)
				_tm.world.push().multiply(transformObject.transform);
		
			if (styleObject)
				_styleStack.push(styleObject.style);
			
			if (effectObject)
				for each (var effect : IEffect3D in effectObject.effects)
				_fx.push(effect);
		}
		
		protected function popData(transformObject	: IObject3D, 
								   styleObject		: IStyled3D, 
								   effectObject		: EffectGroup3D) : void
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
		
		protected function queryIGroup3D(scene : IGroup3D) : void
		{
			for (var childIndex : uint = 0; true; ++childIndex)
			{
				var child : IScene3D = scene.getChildAt(childIndex);
				if (child == null)
					break;
				
				query(child);
			}
		}
		
		protected function queryIMesh3D(scene : IMesh3D) : void
		{
			for each (var worldObject : IWorldData in _worldData)
				worldObject.invalidate();
			
			// pass "ready to draw" data to the renderer.
			var vertexStreamList 	: VertexStream3DList	= scene.vertexStreamList;
			var indexStream 		: IndexStream3D			= scene.indexStream;
			
			var numEffects 	: int = _fx.length;
			if (numEffects == 0)
				throw new Error("Unable to draw without an effect.");
			
			for (var i : int = 0; i < numEffects; ++i)
			{
				var fx			: IEffect3D					= _fx[i];
				
				_styleStack.push(fx.style);
				fx.prepare(_styleStack, _transformData, _worldData);
				
				var passes		: Vector.<IEffect3DPass>	= fx.passes;
				var numPasses 	: int 						= passes.length;
				
				for (var j : int = 0; j < numPasses; ++j)
				{
					_renderer.begin();
					
					var pass	: IEffect3DPass = passes[j];
					var state	: RenderState = _renderer.state;
					
					if (pass.fillRenderState(state, _styleStack, _transformData, _worldData))
					{
						state.setInputStreams(vertexStreamList, indexStream);
						_renderer.drawTriangles();
					}
					
					_renderer.end();
				}
				
				_styleStack.pop();
			}
		}
		
		protected function queryIMaterial3D(scene : IMaterial3D) : void
		{
			if (scene is AnimatedMaterial3D)
			{
				queryAnimatedMaterial3D(scene as AnimatedMaterial3D);
			}
			else if (scene is BitmapMaterial3D)
			{
				queryBitmapMaterial3D(scene as BitmapMaterial3D);
			}
		}
		
		protected function queryAnimatedMaterial3D(scene : AnimatedMaterial3D) : void
		{
			scene.gotoFrame(_frameId);
			query(scene.getChildAt(_frameId));
		}
		
		protected function queryBitmapMaterial3D(scene : BitmapMaterial3D) : void
		{
			var styleName			: String		= scene.styleProperty;
			var textureRessource	: IRessource3D	= scene.ressource;
			
			_styleStack.set(styleName, textureRessource);
		}
		
		protected function queryModel3D(scene : Model3D) : void
		{
			if (scene.visible)
			{
				queryIMaterial3D(scene.material);
				queryIMesh3D(scene.mesh);
			}
		}
	}
}