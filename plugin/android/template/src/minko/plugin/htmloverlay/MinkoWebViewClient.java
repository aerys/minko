package minko.plugin.htmloverlay;

import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.util.Base64;
import java.io.IOException;
import java.io.InputStream;
import android.webkit.JsResult;

import static android.util.Log.*;

public class MinkoWebViewClient extends WebViewClient 
{
	// Native functions
	public native void webViewPageLoaded();

	@Override
	public boolean shouldOverrideUrlLoading(WebView view, String url) 
	{
		view.loadUrl(url);
		
		return true;
	}
	
	@Override
	public void onReceivedError (WebView view, int errorCode, String description, String failingUrl)
	{
		d("MINKOCHROMECLIENT", "ERROR: " + description);
	}
	
	@Override
    public void onScaleChanged(WebView view, float oldScale, float newScale)
    {
        view.zoomOut();
    }
	
	@Override
    public void onPageFinished(WebView view, String url) 
	{
        super.onPageFinished(view, url);

		d("MINKOJAVA", "TRY TO INJECT JS INTO THE PAGE LOADED ! (url: " + url + ")");
		
        injectScriptFile(view, "script/minko.android.overlay.js");

        // Test if the script was loaded
        //view.loadUrl("javascript:window.Minko.testFunction('COUCOU');");
		
		webViewPageLoaded();
    }

    private void injectScriptFile(WebView view, String scriptFile) 
	{
        InputStream input;
		
        try 
		{
			// Get the content of the script file
            input = view.getContext().getAssets().open(scriptFile);
            byte[] buffer = new byte[input.available()];
            input.read(buffer);
            input.close();

            // String-ify the script byte-array using BASE64 encoding !!!
            String encoded = Base64.encodeToString(buffer, Base64.NO_WRAP);
            view.loadUrl("javascript:(function() {" +
                    "var parent = document.getElementsByTagName('head').item(0);" +
                    "var script = document.createElement('script');" +
                    "script.type = 'text/javascript';" +
                    // Tell the browser to BASE64-decode the string into your script !!!
                    "script.innerHTML = window.atob('" + encoded + "');" +
                    "parent.appendChild(script)" +
                    "})()"
			);
        } 
		catch (IOException e) 
		{
            e.printStackTrace();
        }
    }
}
