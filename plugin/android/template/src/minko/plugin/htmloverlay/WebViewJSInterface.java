package minko.plugin.htmloverlay;

import android.webkit.WebView;
import android.webkit.JavascriptInterface;
import static android.util.Log.*;

public class WebViewJSInterface
{
	@JavascriptInterface
	public void onmessage(String message)
	{
		minkoNativeOnMessage(message);
	}
	
	@JavascriptInterface
	public void onNativeJSResult(String jsResult)
	{
		d("MINKO", "Receive JS result: " + jsResult);
		minkoNativeOnJSResult(jsResult);
	}
	
	public native void minkoNativeOnMessage(String message);
	public native void minkoNativeOnJSResult(String jsResult);
}