package aerys.minko.render.shader {
import aerys.minko.render.shader.Shader3DAsset;
import aerys.minko.type.vertex.format.Vertex3DComponent;
import flash.utils.ByteArray;
/**
* 
*/
public final class DefaultShader3D extends Shader3DAsset { 
private static const VERTEX_SHADER : ByteArray = Shader3DAsset.decode('W8DIwMCwkEGCAQT4mYHEEwYIeMLIAAP8LIwIcTAAAA==')
private static const VERTEX_COMPONENTS : Vector.<Vertex3DComponent> = Vector.<Vertex3DComponent>([Vertex3DComponent.XYZ,Vertex3DComponent.UV,null,null,null,null,null,null]);
private static const FRAGMENT_SHADER : ByteArray = Shader3DAsset.decode('W8DIwMCwkFGDAQT4mYDEExYGCGBlEBCAsPiZQeJMDAgAAA==')
public function DefaultShader3D() {super(VERTEX_SHADER, FRAGMENT_SHADER, VERTEX_COMPONENTS);}
}
}