package aerys.minko.render.effect.basic
{
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.effect.IEffect;
	import aerys.minko.render.effect.IEffectPass;
	import aerys.minko.render.effect.fog.FogStyle;
	import aerys.minko.render.renderer.state.Blending;
	import aerys.minko.render.renderer.state.CompareMode;
	import aerys.minko.render.renderer.state.RendererState;
	import aerys.minko.render.renderer.state.TriangleCulling;
	import aerys.minko.render.shader.ActionScriptShader;
	import aerys.minko.render.shader.SValue;
	import aerys.minko.render.shader.node.Components;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.scene.visitor.data.CameraData;
	import aerys.minko.scene.visitor.data.LocalData;
	import aerys.minko.scene.visitor.data.StyleStack;
	import aerys.minko.scene.visitor.data.ViewportData;
	import aerys.minko.type.math.ConstVector4;
	
	import flash.utils.Dictionary;
	
	[StyleParameter(name="basic diffuse map",type="texture")]
	
	[StyleParameter(name="fog enabled",type="boolean")]
	[StyleParameter(name="fog color",type="color")]
	[StyleParameter(name="fog start",type="number")]
	[StyleParameter(name="fog distance",type="number")]

	public class BasicEffect extends ActionScriptShader implements IEffect, IEffectPass
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
		
		override public function fillRenderState(state	: RendererState, 
												 style	: StyleStack, 
												 local	: LocalData, 
												 world	: Dictionary) : Boolean
		{
			super.fillRenderState(state, style, local, world);
			
			var blending : uint = style.get(BasicStyle.BLENDING, Blending.NORMAL) as uint;
			
			state.depthTest			= CompareMode.LESS;
			state.blending			= blending;
			state.triangleCulling	= style.get(BasicStyle.TRIANGLE_CULLING, TriangleCulling.BACK) as uint;
			state.priority			= _priority + .5;
			state.rectangle			= null;
			state.renderTarget		= _renderTarget || world[ViewportData].renderTarget;
			
			if (state.blending != Blending.NORMAL)
				state.priority -= .5;
			
			return true;
		}
		
		override protected function getOutputPosition() : SValue
		{
			return vertexClipspacePosition;
		}
		
		override protected function getOutputColor() : SValue
		{
			var diffuse	: SValue		= null;
			
			if (styleIsSet(BasicStyle.DIFFUSE_MAP))
				diffuse = sampleTexture(BasicStyle.DIFFUSE_MAP, interpolate(vertexUV));
			else
				diffuse = combine(interpolate(vertexRGBColor).rgb, 1.);
		
			// fog
			if (getStyleConstant(FogStyle.FOG_ENABLED, false))
			{
				var zFar		: SValue = styleIsSet(FogStyle.DISTANCE)
										  ? getStyleParameter(1, FogStyle.DISTANCE)
										  : getWorldParameter(1, CameraData, CameraData.Z_FAR);
				var fogColor 	: *		= styleIsSet(FogStyle.COLOR)
										  ? getStyleParameter(3, FogStyle.COLOR)
										  : vector3(0., 0., 0.);
				var fogStart	: *		= styleIsSet(FogStyle.START)
										  ? getStyleParameter(1, FogStyle.START)
										  : 0.;
				
				fogColor = getFogColor(fogStart, zFar, fogColor); 
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
			{
				hash += "_fog(";
				hash += "start=" + style.get(FogStyle.START, 0.);
				hash += ",distance=" + style.get(FogStyle.DISTANCE, 0.);
				hash += ",color=" + style.get(FogStyle.COLOR, 0);
				hash += ");"
			}
			
			return hash;
		}
	}
}
