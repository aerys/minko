package aerys.minko.render.effect.basic
{
	import aerys.minko.render.effect.IEffect;
	import aerys.minko.render.effect.IEffectPass;
	import aerys.minko.render.effect.fog.FogStyle;
	import aerys.minko.render.RenderTarget;
	import aerys.minko.render.ressource.TextureRessource;
	import aerys.minko.render.shader.ParametricShader;
	import aerys.minko.render.shader.node.INode;
	import aerys.minko.render.shader.node.common.DiffuseMapTexture;
	import aerys.minko.render.state.Blending;
	import aerys.minko.render.state.RenderState;
	import aerys.minko.render.state.TriangleCulling;
	import aerys.minko.scene.visitor.data.StyleStack;
	import aerys.minko.scene.visitor.data.TransformData;
	
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
								  local			: TransformData, 
								  world			: Dictionary) : Vector.<IEffectPass>
		{
			return _passes;
		}
		
		override public function fillRenderState(state	: RenderState, 
												 style	: StyleStack, 
												 local	: TransformData, 
												 world	: Dictionary) : Boolean
		{
			super.fillRenderState(state, style, local, world);
			
			state.blending			= style.get(BasicStyle.BLENDING, Blending.ALPHA) as uint;
			state.triangleCulling	= style.get(BasicStyle.TRIANGLE_CULLING, TriangleCulling.BACK) as uint;
			state.priority			= _priority;
			state.renderTarget		= _renderTarget;
			
			return true;
		}
		
		override protected function getOutputPosition(style	: StyleStack,
													  local	: TransformData,
													  world	: Dictionary) : INode
		{
			return vertexClipspacePosition;
		}
		
		override protected function getOutputColor(style	: StyleStack,
												   local	: TransformData,
												   world	: Dictionary) : INode
		{
			var diffuseMap : TextureRessource = style.get(BasicStyle.DIFFUSE_MAP, null) as TextureRessource;
			
			if (diffuseMap)
				return new DiffuseMapTexture();
			
			return interpolate(vertexColor);
		}
		
		override protected function getDataHash(style	: StyleStack,
												local	: TransformData,
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
