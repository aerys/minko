package aerys.minko.render.shader {
	import aerys.minko.render.shader.Shader3DAsset;
	import aerys.minko.type.vertex.format.Vertex3DComponent;
	
	import flash.utils.ByteArray;
	
	/**
	 * 
	 */
	public final class DefaultShader3D extends Shader3DAsset {
		
		private static const VS : ByteArray = Shader3DAsset.decode('W8DIwMCwkEGCAQT4mYHEEwYIeMLIAAP8LIwIcTAAAA==')
		private static const FS : ByteArray = Shader3DAsset.decode('W8DIwMCwEEQAAT8TkHjCwoAAGhBxZpA4E1SMlUFAAAA=')
		
		public function DefaultShader3D() {
			
			var vertexInput:Vector.<Vertex3DComponent> = new Vector.<Vertex3DComponent>(8, true);
			vertexInput[0] = Vertex3DComponent.XYZ;
			vertexInput[1] = Vertex3DComponent.UV;
			
			super(VS, FS, vertexInput);
			
		}
	}
}
