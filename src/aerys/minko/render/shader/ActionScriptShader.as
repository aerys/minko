package aerys.minko.render.shader
{
	import aerys.minko.ns.minko;
	import aerys.minko.render.effect.basic.BasicStyle;
	import aerys.minko.render.renderer.RendererState;
	import aerys.minko.render.resource.Texture3DResource;
	import aerys.minko.render.shader.node.leaf.*;
	import aerys.minko.render.shader.node.operation.builtin.*;
	import aerys.minko.render.shader.node.operation.manipulation.*;
	import aerys.minko.scene.data.TransformData;
	import aerys.minko.scene.data.StyleStack;
	import aerys.minko.scene.data.ViewportData;
	import aerys.minko.scene.data.WorldDataList;
	import aerys.minko.type.math.Vector4;
	
	import flash.utils.Dictionary;
	
	/**
	 * <p>Shader objects define vertex and fragment shaders with
	 * ActionScript code.</p>
	 * 
	 * <p>ActionScript shaders make it possible to write dynamic, parametric
	 * and OOP oriented shaders taking benefits of all the features of
	 * ActionScript. They fully integrate with the rest of your ActionScript
	 * code and the application development process. Thus, they allow to
	 * greatly simplify shaders writing and the rendering process as a whole.
	 * </p>
	 * 
	 * <p>
	 * Because ActionScript shaders are just actual ActionScript code, they
	 * can take into account the scene configuration (the number of lights,
	 * the need for hardware skinning, etc...). Using conditionnal statements
	 * and loops, one can write an ActionScript shader that will be able to
	 * handle many usecases by generating a different shader bytecode anytime
	 * the rendering or the scene configuration changes.
	 * </p>
	 * 
	 * <p>
	 * To create your own shaders using ActionScript code, you should extend
	 * the Shader class and override the following methods:</p>
	 * <ul>
	 * <li>getOutputPosition: this method will define the vertex shader</li>
	 * <li>getOutputColor: this method will define the fragment shader</li>
	 * <li>getHash (optionnal): this method should return a hash value
	 * that will be used as a unique key to identify the shader</li>
	 * </ul>
	 * 
	 * <p>
	 * Those methods and other shader dedicated methods heavily rely on
	 * two data types:
	 * </p>
	 * <ul>
	 * <li>Object: arguments are typed has Object to ensure polymorphism. Any
	 * of the following type can be used: int, uint, Number, Point, Vector4,
	 * Matrix3D and SValue. Arguments with types different from SValue will
	 * always be interpreted as static fixed constants.</li>
	 * <li>SValue: return values are types as SValue objects. They represent
	 * hardware memory proxies.</li>
	 * </ul>
	 * 
	 * <p>
	 * ActionScript shaders workflow at runtime is as follow:
	 * </p>
	 * <ul>
	 * <li>The shader hash is retrieved using the "getHash" method</li>
	 * <li>If a shader bytecode has already been compiled for this hash value,
	 * it is set and used directly.</li>
	 * <li>Else, the getOutputPosition and getOutputColor methods are called.
	 * Those methods transparently build and return a "shader graph"
	 * encapsulated in an SValue object.</li>
	 * <li>This shader graph is then optimized, compiled into AGAL
	 * bytecode and saved using the computed hash as a key.</li>
	 * <li>The compiled AGAL bytecode is then uploaded and used to render the
	 * scene.</li>
	 * </ul>
	 * 
	 * <p>
	 * You should always remember both getOutputPosition and getOutputColor
	 * are <b>never</b> executed on the graphics hardware. Instead, they help
	 * to transparently define a "shader program graph" that will be dynamically
	 * compiled at runtime into AGAL bytecode. Thus, you should always keep in
	 * mind that:
	 * </p>
	 * <ul>
	 * <li>Both getOutputPosition and getOutputColor are called only once when
	 * there is no valid AGAL bytecode available (ie. the "invalidate" method
	 * has been called or their is no shader bytecode associated with the
	 * hash retrieve by calling the "getHash" method).</li>
	 * <li>Conditionnals, loops and method calls with a return type different
	 * from SValue are considered as static fixed CPU-side code.</li>
	 * <li>Any value with a type different from SValue will be transparently
	 * turned into a static fixed shader constant.</li>
	 * <li>If you want to use updatable values, you should use parameters.</li>
	 * </ul>
	 * 
	 * @author Jean-Marc Le Roux
	 * 
	 */
	public class ActionScriptShader extends ActionScriptShaderPart
	{
		use namespace minko;
		
		private var _hashToShader		: Object		= new Object();
		
		private var _styleData			: StyleStack	= null;
		private var _transformData		: TransformData	= null;
		private var _worldData			: Dictionary	= null;
		
		private var _lastFrameId		: uint			= 0;
		private var _styleStackVersion	: uint			= 0;
		private var _lastShader			: Shader		= null;
		
		public function fillRenderState(state		: RendererState, 
										style		: StyleStack, 
										transform	: TransformData, 
										world		: Dictionary) : Boolean
		{
			var frameId	: uint		= (world[ViewportData] as ViewportData).frameId;
			var shader 	: Shader	= _lastShader;

			_styleData = style;
			_transformData = transform;
			_worldData = world;
			
			if (frameId != _lastFrameId  || _styleData.version != _styleStackVersion || !_lastShader)
			{
				var hash : String 	= getDataHash(style, transform, world);
			
				shader = _hashToShader[hash];
				
				if (!shader)
				{
					_hashToShader[hash] = shader = Shader.create(getOutputPosition()._node,
												  				 getOutputColor()._node);
				}
				
				_lastFrameId = frameId;
				_styleStackVersion = _styleData.version;
				_lastShader = shader;
			}
			
			shader.fillRenderState(state, style, transform, world);
			
			return true;
		}
		
		/**
		 * The getOutputPosition method implements a vertex shader using ActionScript code. 
		 * @return 
		 * 
		 */
		protected function getOutputPosition() : SValue
		{
			throw new Error();
		}
		
		/**
		 * The getOutputColor method implements a fragment shader using ActionScript code.
		 * @return 
		 * 
		 */
		protected function getOutputColor() : SValue
		{
			throw new Error();
		}
		
		/**
		 * Return a style value that will be passed as a shader constant.
		 * 
		 * @param styleId
		 * @param defaultValue
		 * @return 
		 * 
		 */
		protected final function getStyleConstant(styleId : int, defaultValue : Object = null) : Object
		{
			return _styleData.get(styleId, defaultValue);
		}
		
		/**
		 * Return whether a specific style is set or not. 
		 * @param styleId
		 * @return 
		 * 
		 */
		protected final function styleIsSet(styleId : int) : Boolean
		{
			return _styleData.isSet(styleId);
		}
		
		protected final function getWorldDataList(key : Class) : WorldDataList
		{
			return _worldData[key];
		}
		
		public function dispose() : void
		{
			for each (var shader : Shader in _hashToShader)
				shader.resource.dispose();
		}
	}
}