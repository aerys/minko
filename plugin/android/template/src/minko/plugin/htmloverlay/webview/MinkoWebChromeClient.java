package minko.plugin.htmloverlay;

import android.webkit.JsResult;
import android.webkit.WebChromeClient;
import android.webkit.WebView;
import android.util.Log;

final class MinkoWebChromeClient extends WebChromeClient
{
    @Override
    public boolean onJsAlert(WebView view, String url, String message, JsResult result)
    {
        Log.i("minko-java", "[MinkoWebChromeClient] onJsAlert: " + message);
        result.confirm();
		
        return true;
    }
}