package aerys.minko.scene.visitor.rendering
{
	import aerys.minko.effect.IEffect;
	import aerys.minko.effect.IEffectPass;
	import aerys.minko.effect.StyleStack;
	import aerys.minko.ns.minko;
	import aerys.minko.scene.visitor.ISceneVisitor;
	import aerys.minko.render.IRenderer;
	import aerys.minko.render.state.RenderState;
	import aerys.minko.scene.graph.IScene;
	import aerys.minko.scene.graph.camera.ICamera;
	import aerys.minko.stage.Viewport;
	import aerys.minko.type.stream.IndexStream;
	import aerys.minko.type.stream.VertexStreamList;
	
	import flash.display3D.Context3DTextureFormat;
	import flash.display3D.textures.TextureBase;
	
	/**
	 * A RenderingQuery object is used to traverse a scene graph and provides
	 * rendering methods and properties such as:
	 * <ul>
	 * <li>The list of IEffect3D that will be use when rendering.</li>
	 * <li>The current Style3D object that will parametrize rendering.</li>
	 * <li>The (enabled) ICamera3D object.</li>
	 * <li>The list of the parent nodes that were traversed.</li>
	 * <li>The list of the ancestor nodes that were traversed.</li>
	 * </ul>
	 *  
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class RenderingVisitor implements ISceneVisitor
	{
		use namespace minko;
		
		private var _renderer	: IRenderer			= null;
		
		private var _current	: IScene				= null;
		private var _parent		: IScene				= null;
		private var _parents	: Vector.<IScene>		= new Vector.<IScene>();
		private var _camera		: ICamera				= null;
		private var _styleStack	: StyleStack			= new StyleStack();
		private var _tm			: TransformManager		= new TransformManager();
		private var _fx			: Vector.<IEffect>	= new Vector.<IEffect>();
		private var _numNodes	: uint					= 0;
		
		public function get parent()		: IScene				{ return _parent; }
		public function get camera()		: ICamera				{ return _camera; }
		public function get style()			: StyleStack			{ return _styleStack; }
		public function get transform()		: TransformManager		{ return _tm; }
		public function get viewport()		: Viewport			{ return _renderer.viewport; }
		public function get numTriangles()	: uint					{ return _renderer.numTriangles; }
		public function get drawingTime()	: int					{ return _renderer.drawingTime; }
		public function get frameId()		: uint					{ return _renderer.frameId; }
		public function get effects()		: Vector.<IEffect>	{ return _fx; }
		public function get numNodes()		: uint					{ return _numNodes; }
		
		public function set style(value : StyleStack) : void
		{
			_styleStack = value;
		}
		
		public function set effects(value : Vector.<IEffect>) : void
		{
			_fx = value;
		}
		
		public function RenderingVisitor(renderer : IRenderer)
		{
			_renderer = renderer;
		}
		
		public function query(scene : IScene) : void
		{
			var numParents : int = _parents.length;
			
			_parents[numParents] = _parent;
			_parent = _current;
			_current = scene;
			
			++_numNodes;
			
			var camera : ICamera = scene as ICamera;
			
			if (camera && camera.enabled)
			{
				if (_camera)
					throw new Error();
				
				_camera = camera;
			}
			
			scene.visited(this);
			
			_current = _parent;
			_parent = _parents[numParents];
			_parents.length = numParents;
		}
		
		public function reset() : void
		{
			_camera = null;
			_parents.length = 0;
			_numNodes = 0;
			_renderer.clear();
			_tm.reset();
//			_style.clear();
			
			if (_fx.length != 0)
				throw new Error('Effect3DList stack should be empty.');
		}
		
		public function draw(vertexStreamList 	: VertexStreamList,
							 indexStream 		: IndexStream,
							 offset				: uint	= 0,
							 numTriangles		: uint	= 0) : void
		{
			var numEffects 	: int 					= _fx.length;
			
			if (numEffects == 0)
				throw new Error("Unable to draw without an effect.");
			
			for (var i : int = 0; i < numEffects; ++i)
			{
				var fx			: IEffect					= _fx[i];
				var passes		: Vector.<IEffectPass>	= fx.passes;
				var numPasses 	: int 						= passes.length;
				
				_styleStack.push(fx.style);
				fx.begin(_renderer, _styleStack);
				
				for (var j : int = 0; j < numPasses; ++j)
				{
					var pass : IEffectPass = passes[j];
					
					_renderer.begin();
					if (pass.begin(_renderer, _styleStack))
					{
						var state:RenderState = _renderer.state;
						
						state.setInputStreams(vertexStreamList, indexStream);
						_renderer.drawTriangles(offset, numTriangles);
					}
					
					pass.end(_renderer, _styleStack);
					_renderer.end();
				}
				
				fx.end(_renderer, _styleStack);
				_styleStack.pop();
			}
		}
		
		public function drawList(vertexStreamList 	: VertexStreamList,
								 indexStream 		: IndexStream,
								 offsets			: Vector.<uint>,
								 numTriangles		: Vector.<uint> = null) : void
		{
			var numEffects 	: int 	= _fx.length;
			
			if (numEffects == 0)
				throw new Error("Unable to draw without an effect.");
			
			var length	: int			= offsets.length;
			
			if (length != numTriangles.length)
				throw new Error();
			
			for (var i : int = 0; i < numEffects; ++i)
			{
				var fx			: IEffect					= _fx[i];
				var passes		: Vector.<IEffectPass>	= fx.passes;
				var numPasses 	: int 						= passes.length;
				
				//_style = fx.style.override(_style);
				_styleStack.push(fx.style);
				fx.begin(_renderer, _styleStack);
				
				for (var j : int = 0; j < numPasses; ++j)
				{
					var pass : IEffectPass = passes[j];
					
					_renderer.begin();
					if (pass.begin(_renderer, _styleStack))
					{
						var state	: RenderState 	= _renderer.state;
						
						state.setInputStreams(vertexStreamList, indexStream);
						for (var k : int = 0; k < length; k += 1)
							_renderer.drawTriangles(offsets[k], numTriangles[k]);
					}
					
					_renderer.end();
					pass.end(_renderer, _styleStack);
				}
				
				fx.end(_renderer, _styleStack);
				//_style = fx.style.override();
				_styleStack.pop();
			}
		}
		
		public function createTexture(width 		: uint,
									  height 		: uint,
									  format 		: String	= Context3DTextureFormat.BGRA,
									  renderTarget	: Boolean 	= false) : TextureBase
		{
			return _renderer.createTexture(width, height, format, renderTarget);
		}
	}
}