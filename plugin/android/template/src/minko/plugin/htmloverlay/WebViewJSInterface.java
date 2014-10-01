package minko.plugin.htmloverlay;

import android.webkit.WebView;
import android.webkit.JavascriptInterface;

import static android.util.Log.*;

public class WebViewJSInterface
{
	public static boolean ResultReady = false;
	public static String Result = "[UNKNOWN]";

	// Native functions
	public native void minkoNativeOnMessage(String message);
	public native void minkoNativeOnJSResult(String jsResult);
	
	@JavascriptInterface
	public void onmessage(String message)
	{
		minkoNativeOnMessage(message);
	}
	
	@JavascriptInterface
	public void onNativeJSResult(String jsResult)
	{
		WebViewJSInterface.Result = jsResult;
		WebViewJSInterface.ResultReady = true;
	}
}