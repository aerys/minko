package aerys.minko.render.shader {
import aerys.minko.render.shader.ShaderAsset;
import aerys.minko.type.vertex.format.VertexComponent;
import flash.utils.ByteArray;
/**
* 
*/
public final class DefaultShader extends ShaderAsset { 
private static const VERTEX_SHADER : ByteArray = ShaderAsset.decode('W8DIwMCwkEGCAQT4mYHEEwYIeMLIAAP8LIwIcTAAAA==')
private static const VERTEX_COMPONENTS : Vector.<VertexComponent> = Vector.<VertexComponent>([VertexComponent.XYZ,VertexComponent.UV,null,null,null,null,null,null]);
private static const FRAGMENT_SHADER : ByteArray = ShaderAsset.decode('W8DIwMCwkFGDAQT4mYDEExYGCGBlEBCAsPiZQeJMDAgAAA==')
public function DefaultShader() {super(VERTEX_SHADER, FRAGMENT_SHADER, VERTEX_COMPONENTS);}
}
}