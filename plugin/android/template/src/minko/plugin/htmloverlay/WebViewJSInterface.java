package minko.plugin.htmloverlay;

import android.webkit.WebView;
import static android.util.Log.*;
import android.webkit.JavascriptInterface;

public class WebViewJSInterface
{
    @JavascriptInterface
    public String toString() 
	{ 
		return "COUCOU from WebViewJSInterface"; 
	}
	
	@JavascriptInterface
	public void onmessage(String message)
	{
		minkoNativeOnMessage(message);
	}
	
	public native void minkoNativeOnMessage(String message);
}