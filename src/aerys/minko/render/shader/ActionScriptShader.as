package aerys.minko.render.shader
{
	import aerys.minko.ns.minko;
	import aerys.minko.render.effect.basic.BasicStyle;
	import aerys.minko.render.effect.skinning.SkinningStyle;
	import aerys.minko.render.renderer.state.RendererState;
	import aerys.minko.render.resource.TextureResource;
	import aerys.minko.render.shader.node.Components;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.fog.Fog;
	import aerys.minko.render.shader.node.leaf.AbstractConstant;
	import aerys.minko.render.shader.node.leaf.Attribute;
	import aerys.minko.render.shader.node.leaf.Constant;
	import aerys.minko.render.shader.node.leaf.Sampler;
	import aerys.minko.render.shader.node.leaf.StyleParameter;
	import aerys.minko.render.shader.node.leaf.TransformParameter;
	import aerys.minko.render.shader.node.leaf.WorldParameter;
	import aerys.minko.render.shader.node.operation.builtin.*;
	import aerys.minko.render.shader.node.operation.manipulation.*;
	import aerys.minko.render.shader.node.operation.math.PlanarReflection;
	import aerys.minko.render.shader.node.operation.math.Product;
	import aerys.minko.render.shader.node.operation.math.Sum;
	import aerys.minko.render.shader.node.skinning.DQSkinnedPosition;
	import aerys.minko.render.shader.node.skinning.MatrixSkinnedPosition;
	import aerys.minko.scene.data.CameraData;
	import aerys.minko.scene.data.LocalData;
	import aerys.minko.scene.data.StyleStack;
	import aerys.minko.type.math.Matrix4x4;
	import aerys.minko.type.math.Vector4;
	import aerys.minko.type.skinning.SkinningMethod;
	import aerys.minko.type.stream.format.VertexComponent;
	
	import flash.geom.Point;
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
	 * Matrix4x4 and SValue. Arguments with types different from SValue will
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
		
		private var _hashToShader	: Object		= new Object();
		
		private var _invalid		: Boolean		= true;
		
		private var _styleStack		: StyleStack	= null;
		private var _local			: LocalData		= null;
		private var _world			: Dictionary	= null;
		
		protected function get diffuseColor() : SValue
		{
			if (styleIsSet(BasicStyle.DIFFUSE))
			{
				var diffuseStyle	: Object 	= getStyleConstant(BasicStyle.DIFFUSE);
				
				if (diffuseStyle is uint || diffuseStyle is Vector4)
					return getStyleParameter(4, BasicStyle.DIFFUSE);
				else if (diffuseStyle is TextureResource)
					return sampleTexture(BasicStyle.DIFFUSE, interpolate(vertexUV));
				else
					throw new Error('Invalid BasicStyle.DIFFUSE value.');
			}
			else
				return float4(interpolate(vertexRGBColor).rgb, 1.);
		}
		
		public function fillRenderState(state	: RendererState, 
										style	: StyleStack, 
										local	: LocalData, 
										world	: Dictionary) : Boolean
		{
			var hash 	: String 		= getDataHash(style, local, world);
			var shader 	: Shader = _hashToShader[hash];
			
			_styleStack = style;
			_local = local;
			_world = world;
			
			if (!shader || _invalid)
			{
				_invalid = false;
				_hashToShader[hash] = shader = Shader.create(getOutputPosition()._node,
															 getOutputColor()._node);
			}
			
			shader.fillRenderState(state, style, local, world);
			
			return true;
		}
		
		/**
		 * The getDataHash method returns a String computed from the style, local and world data.
		 * This value is used as a hash that defines all the values used as conditionnals in the
		 * vertex (getOutputPosition) or frament (getOutputColor) shaders.
		 *  
		 * @param style
		 * @param local
		 * @param world
		 * @return 
		 * 
		 */
		protected function getDataHash(style	: StyleStack, 
									   local	: LocalData, 
									   world	: Dictionary) : String
		{
			return "";
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
			return _styleStack.get(styleId, defaultValue);
		}
		
		/**
		 * Return whether a specific style is set or not. 
		 * @param styleId
		 * @return 
		 * 
		 */
		protected final function styleIsSet(styleId : int) : Boolean
		{
			return _styleStack.isSet(styleId);
		}
		
		/**
		 * Invalidate both the vertex and fragment shader bytecode and force it
		 * to be recompiled next time the shader will be used to draw
		 * triangles.
		 * 
		 * <p>You can use the "invalidate" method to force the shader
		 * compilation anytime a value used in conditionnals or loop
		 * statements changes.</p>
		 * 
		 * <p>If you do not want to have to maniually invalidate the shader
		 * bytecode, you should implement the "getHash" method properly.</p>
		 * 
		 */
		public function invalidate() : void
		{
			_invalid = true;
		}
		
		public function dispose() : void
		{
			for each (var shader : Shader in _hashToShader)
				shader.resource.dispose();
		}
	}
}