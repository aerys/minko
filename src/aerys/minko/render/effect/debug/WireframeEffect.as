package aerys.minko.render.effect.debug
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.effect.SinglePassEffect;
	import aerys.minko.render.effect.animation.AnimationShaderPart;
	import aerys.minko.render.effect.animation.AnimationStyle;
	import aerys.minko.render.effect.basic.BasicStyle;
	import aerys.minko.render.renderer.state.Blending;
	import aerys.minko.render.renderer.state.CompareMode;
	import aerys.minko.render.renderer.state.RendererState;
	import aerys.minko.render.renderer.state.TriangleCulling;
	import aerys.minko.render.resource.Texture3DResource;
	import aerys.minko.render.shader.SValue;
	import aerys.minko.render.shader.node.Components;
	import aerys.minko.render.shader.node.operation.builtin.Multiply3x4;
	import aerys.minko.scene.data.StyleStack;
	import aerys.minko.scene.data.TransformData;
	import aerys.minko.type.animation.AnimationMethod;
	import aerys.minko.type.math.Vector4;
	
	import flash.utils.Dictionary;

	
	/*
	 * Implementation of the Single-Pass Wireframe rendering technique from 
	 * J. Andreas Bærentzen, Steen Lund Nielsen, Mikkel Gjøl, and Bent D. Larsen.
	 * 
	 * References:
	 *     - original article: http://cgg-journal.com/2008-2/06/index.html
	 *     - minimole implementation: https://github.com/lidev/minimole-core/tree/master/com/li/minimole/materials
	 */
	public class WireframeEffect extends SinglePassEffect
	{
		private const LINE_THICKNESS_COEFF	: SValue	= float(1000.);
		
		private var _weight					: SValue 	= null;
		
		
		
		/**
		 * Creates a new WireframeEffect object.
		 *
		 * @param wireColor The color of the wires, if null the color of the object itself will be used.
		 * @param surfaceColor The color of the surfaces, if null surfaces will be of a transparent black.
		 */
		public function WireframeEffect(priority		: Number		= 0,
										renderTarget	: RenderTarget	= null)
		{			
			super(priority, renderTarget);
		}
		
		override public function fillRenderState(state		: RendererState, 
												 style		: StyleStack, 
												 transform	: TransformData, 
												 world		: Dictionary) : Boolean
		{
			super.fillRenderState(state, style, transform, world);
			
			state.triangleCulling = TriangleCulling.DISABLED;
			
			var surfaceColor	: Vector4	= getStyleConstant(WireframeStyle.SURFACE_COLOR, new Vector4(0., 0., 0., 0.)) as Vector4;
			
			if (surfaceColor.w < 1.)
			{
				state.depthTest	= CompareMode.ALWAYS;
				state.blending =  style.get(BasicStyle.BLENDING, Blending.ADDITIVE) as uint;
			}
			return true;
		}
		
		override protected function getOutputPosition() : SValue
		{
			var cameraDistance 	: SValue	= cameraPosition.subtract(vertexWorldPosition).length;
			
			var scale : SValue = multiply3x4(float3(1., 0., 0.), copy(localToWorldMatrix)).length;
			
			// original weight is the distance from the vertex to the opposite
			// side of the triangle, due to the abscence of geometry shader, this
			// is done in the WireframeMeshModifer.
			// It is then modified by its distance to the
			// camera and the scale of the mesh.
			_weight = vertexWeight.multiply(LINE_THICKNESS_COEFF)
								  .divide(cameraDistance)
					  			  .multiply(scale);
			
			var animationShaderPart : AnimationShaderPart = new AnimationShaderPart();
			
			var animationMethod		: uint		= getStyleConstant(AnimationStyle.METHOD, AnimationMethod.DISABLED) as uint;
			var maxInfluences		: uint		= getStyleConstant(AnimationStyle.MAX_INFLUENCES, 0) as uint;
			var numBones			: uint		= getStyleConstant(AnimationStyle.NUM_BONES, 0) as uint;
			
			var animationPosition	: SValue	= animationShaderPart.getVertexPosition(animationMethod, maxInfluences, numBones);
			
			return multiply4x4(animationPosition, localToScreenMatrix);
		}
		
		override protected function getOutputColor() : SValue
		{			
			var wireColor		: Vector4	= getStyleConstant(WireframeStyle.WIRE_COLOR, new Vector4(NaN, NaN, NaN)) as Vector4;
			var surfaceColor	: Vector4	= getStyleConstant(WireframeStyle.SURFACE_COLOR, new Vector4(0., 0., 0., 0.)) as Vector4;
			
			var diffuse 		: SValue	= isNaN(wireColor.x) ? null : float4(wireColor);
			
			if (!diffuse)
			{
				if (styleIsSet(BasicStyle.DIFFUSE))
				{
					var diffuseStyle	: Object 	= getStyleConstant(BasicStyle.DIFFUSE);
					
					if (diffuseStyle is uint || diffuseStyle is Vector4)
						diffuse = getStyleParameter(4, BasicStyle.DIFFUSE);
					else if (diffuseStyle is Texture3DResource)
						diffuse = sampleTexture(BasicStyle.DIFFUSE, interpolate(vertexUV));
					else
						throw new Error('Invalid BasicStyle.DIFFUSE value.');
				}
				else
					diffuse = float4(interpolate(vertexRGBColor).rgb, 1.);
			}
			
			if (styleIsSet(BasicStyle.DIFFUSE_MULTIPLIER))
				diffuse.scaleBy(copy(getStyleParameter(4, BasicStyle.DIFFUSE_MULTIPLIER)));
						
			// the interpolated weight is a vector of dimension 3 containing
			// values representing the distance of the fragment to each side
			// of the triangle
			var interpolatedWeight	: SValue	= interpolate(_weight);
			
			// only the shortest distance is used to compute the color of the fragment
			var d 					: SValue 	= min(extract(interpolatedWeight, Components.R),
												      extract(interpolatedWeight, Components.G),
												   	  extract(interpolatedWeight, Components.B));

			// e is strictly negative and closer to 0 the closer the fragment
			// is to a side of the triangle
			var e 					: SValue 	= multiply(-2., d.pow(6));
			
			// l is between 1 (when e = 0, i.e. when the fragment is on a 
			// triangle edge and rapidly approaches 0 when e decreases (when
			// the fragment gets further from an edge)
			var l					: SValue 	= power(2., e);
			
			// the final color of the pixel is l * line_color + (1 - l) * surface_color
			return add(multiply(l, diffuse),
					   multiply(subtract(1., l), surfaceColor));
		}
		
		override protected function getDataHash(style		: StyleStack,
												transform	: TransformData,
												world		: Dictionary) : String
		{
			var hash 			: String	= "wireframe";
			var diffuseStyle 	: Object 	= style.isSet(BasicStyle.DIFFUSE)
				? style.get(BasicStyle.DIFFUSE)
				: null;
			
			var wireColor		: Vector4	= getStyleConstant(WireframeStyle.WIRE_COLOR, new Vector4(NaN, NaN, NaN)) as Vector4;
			var surfaceColor	: Vector4	= getStyleConstant(WireframeStyle.SURFACE_COLOR, new Vector4(0., 0., 0., 0.)) as Vector4;
			
			hash += "_wireColor=" + (isNaN(wireColor.x) ? "diffuse" : wireColor);
			hash += "_surfaceColor" + surfaceColor;
			
			if (diffuseStyle == null)
				hash += '_colorFromVertex';
			else if (diffuseStyle is uint || diffuseStyle is Vector4)
				hash += '_colorFromConstant';
			else if (diffuseStyle is Texture3DResource)
				hash += '_colorFromTexture';
			else
				throw new Error('Invalid BasicStyle.DIFFUSE value');
			
			if (style.isSet(BasicStyle.DIFFUSE_MULTIPLIER))
				hash += "_diffuseMultiplier";
			
			if (style.get(AnimationStyle.METHOD, AnimationMethod.DISABLED) != AnimationMethod.DISABLED)
			{
				hash += "_animation(";
				hash += "method=" + style.get(AnimationStyle.METHOD);
				hash += ",maxInfluences=" + style.get(AnimationStyle.MAX_INFLUENCES, 0);
				hash += ",numBones=" + style.get(AnimationStyle.NUM_BONES, 0);
				hash += ")";
			}
			
			return hash;
		}
	}
}