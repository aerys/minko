package aerys.minko.render.shader {
import aerys.minko.render.shader.Shader3DAsset;
import flash.utils.ByteArray;
/**
* 
*/
public final class DefaultShader3D extends Shader3DAsset { 
private static const VS : ByteArray = Shader3DAsset.decode('W8DIwMCwkEGCAQT4mYHEEwYIeMLIAAP8LIwIcTAAAA==')
private static const FS : ByteArray = Shader3DAsset.decode('W8DIwMCwEEQAAT8TkHjCwoAAGhBxZpA4E1SMlUFAAAA=')
public function DefaultShader3D() {super(VS, FS);}
}
}