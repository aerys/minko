package aerys.minko.render.effect.basic
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.effect.IEffect;
	import aerys.minko.render.effect.IEffectPass;
	import aerys.minko.render.effect.fog.FogStyle;
	import aerys.minko.render.renderer.state.Blending;
	import aerys.minko.render.renderer.state.CompareMode;
	import aerys.minko.render.renderer.state.RenderState;
	import aerys.minko.render.renderer.state.TriangleCulling;
	import aerys.minko.render.shader.ParametricShader;
	import aerys.minko.render.shader.node.Components;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.scene.visitor.data.CameraData;
	import aerys.minko.scene.visitor.data.LocalData;
	import aerys.minko.scene.visitor.data.StyleStack;
	import aerys.minko.type.math.ConstVector4;
	
	import flash.utils.Dictionary;
	
	public class BasicEffect extends ParametricShader implements IEffect, IEffectPass
	{
		protected var _priority			: Number;
		protected var _renderTarget		: RenderTarget;
		
		protected var _passes			: Vector.<IEffectPass>	= Vector.<IEffectPass>([this]);
		
		public function BasicEffect(priority		: Number		= 0,
								  	renderTarget	: RenderTarget	= null)
		{
			_priority		= priority;
			_renderTarget	= renderTarget;
		}
		
		public function getPasses(styleStack	: StyleStack, 
								  local			: LocalData, 
								  world			: Dictionary) : Vector.<IEffectPass>
		{
			return _passes;
		}
		
		override public function fillRenderState(state	: RenderState, 
												 style	: StyleStack, 
												 local	: LocalData, 
												 world	: Dictionary) : Boolean
		{
			super.fillRenderState(state, style, local, world);
			
			state.depthTest			= CompareMode.LESS;
			state.blending			= style.get(BasicStyle.BLENDING, Blending.ALPHA) as uint;
			state.triangleCulling	= style.get(BasicStyle.TRIANGLE_CULLING, TriangleCulling.BACK) as uint;
			state.priority			= _priority;
			//state.renderTarget		= _renderTarget;
			
			return true;
		}
		
		override protected function getOutputPosition() : INode
		{
			return vertexClipspacePosition;
		}
		
		override protected function getOutputColor() : INode
		{
			var diffuse	: INode		= null;
			
			if (styleIsSet(BasicStyle.DIFFUSE_MAP))
				diffuse = sampleTexture(BasicStyle.DIFFUSE_MAP, interpolate(vertexUV));
			else
				diffuse = combine(extract(interpolate(vertexColor), Components.RGB), 1.);
		
			// fog
			if (getStyleConstant(FogStyle.FOG_ENABLED, false))
			{
				var zFar		: INode	= styleIsSet(FogStyle.DISTANCE)
										  ? getStyleParameter(1, FogStyle.DISTANCE)
										  : getWorldParameter(1, CameraData, CameraData.Z_FAR);
				var fogColor 	: *		= styleIsSet(FogStyle.COLOR)
										  ? getStyleParameter(3, FogStyle.COLOR)
										  : ConstVector4.ZERO;
				var fogStart	: *		= styleIsSet(FogStyle.START)
										  ? getStyleParameter(1, FogStyle.START)
										  : 0.;
				
				fogColor = getFolorColor(fogStart, zFar, fogColor); 
				diffuse = blend(fogColor, diffuse, Blending.ALPHA);
			}
				
			return diffuse;
		}
		
		override protected function getDataHash(style	: StyleStack,
												local	: LocalData,
												world	: Dictionary) : String
		{
			var hash : String	= "basic";
			
			hash += style.get(BasicStyle.DIFFUSE_MAP, false) ? "_diffuse" : "_color";
			
			if (style.get(FogStyle.FOG_ENABLED, false))
				hash += "_fog";
			
			return hash;
		}
	}
}
