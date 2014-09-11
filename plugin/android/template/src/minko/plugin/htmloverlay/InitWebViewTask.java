package minko.plugin.htmloverlay;

import android.app.Activity;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.webkit.WebChromeClient;
import android.webkit.WebView;
import android.graphics.Color;

import static android.util.Log.*;
import org.libsdl.app.*;

public class InitWebViewTask implements Runnable 
{
	Activity _sdlActivity;
	String _url;
	WebView _webView;
	
	public native void webViewInitialized();
	
	public InitWebViewTask(Activity sdlActivity, String url)
	{
		_sdlActivity = sdlActivity;
		_url = url;
		_webView = null;
	}

    @Override
    public void run() 
	{
		// LinearLayout layout = (LinearLayout) _sdlActivity.findViewById(R.id.mainview);
		// ViewGroup view = (ViewGroup)_sdlActivity.getWindow().getDecorView();
        // LinearLayout layout = (LinearLayout)view.getChildAt(0);
		ViewGroup layout = SDLActivity.getLayout();
		layout.setBackgroundColor(Color.RED);
		
		d("MINKO", "Layout instance = " + layout);
		d("MINKO", "SDL activity = " + _sdlActivity);
		
		// Create the WebView from SDLActivity context
		_webView = new WebView(SDLActivity.getContext());
		
		d("MINKO", "WEBVIEW IS NOW INSTANCIATE: " + _webView);
		
		// Enable the JS for the WebView
        _webView.getSettings().setJavaScriptEnabled(true);
        WebChromeClient wcc = new WebChromeClient();
        _webView.setWebChromeClient(wcc);

        // Transparent background
        _webView.setBackgroundColor(0x00000000);
        _webView.setLayerType(WebView.LAYER_TYPE_SOFTWARE, null);

		// Scale to fit the page
        _webView.getSettings().setLoadWithOverviewMode(true);
        _webView.getSettings().setUseWideViewPort(true);

		// Disable zoom
		_webView.getSettings().setSupportZoom(false);
		_webView.getSettings().setBuiltInZoomControls(true);
		_webView.getSettings().setDisplayZoomControls(false);
		
		// Disable zoom when double tap
		//_webView.getSettings().setUseWideViewPort(false);
		
		// Disable scroll bar
		_webView.setVerticalScrollBarEnabled(false);
		_webView.setHorizontalScrollBarEnabled(false);
		
		// Add the WebView
        layout.addView(_webView);
        _webView.loadUrl(_url);
		
		d("MINKO", "WEBVIEW HAS NOW LOAD AN URL!");
		
		// Add a JavaScript interface
		_webView.addJavascriptInterface(new WebViewJSInterface(), "Minko");
		
		webViewInitialized();
    }
	
	public WebView getWebView()
	{
		d("MINKO", "RETURN WEBVIEW: " + _webView);
		return _webView;
	}
}