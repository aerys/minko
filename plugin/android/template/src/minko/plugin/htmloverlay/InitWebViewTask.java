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
	EvalJSManager _evalJSManager;
	private int _uniqueId;
	
	public native void webViewInitialized();
	
	public InitWebViewTask(Activity sdlActivity, String url)
	{
		_sdlActivity = sdlActivity;
		_url = url;
		_webView = null;
		_uniqueId = 0;
	}

    @Override
    public void run() 
	{
		// LinearLayout layout = (LinearLayout) _sdlActivity.findViewById(R.id.mainview);
		// ViewGroup view = (ViewGroup)_sdlActivity.getWindow().getDecorView();
        // LinearLayout layout = (LinearLayout)view.getChildAt(0);
		ViewGroup layout = SDLActivity.getLayout();
		layout.setBackgroundColor(Color.RED);
		
		d("MINKOJAVA", "Layout instance = " + layout);
		d("MINKOJAVA", "SDL activity = " + _sdlActivity);
		
		// Create the WebView from SDLActivity context
		_webView = new WebView(SDLActivity.getContext());
		
		d("MINKOJAVA", "WEBVIEW IS NOW INSTANCIATED: " + _webView);
		
		// Enable the JS for the WebView
        _webView.getSettings().setJavaScriptEnabled(true);
        WebChromeClient wcc = new WebChromeClient();
        _webView.setWebChromeClient(wcc);
		
		_webView.setWebViewClient(new MinkoWebViewClient());

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
		
		d("MINKOJAVA", "WEBVIEW HAS NOW LOADED AN URL! (" + _url + ")");
		
		// Add a JavaScript interface
		_webView.addJavascriptInterface(new WebViewJSInterface(), "MinkoNativeInterface");
		
		// Instantiate the EvalJS manager
		_evalJSManager = new EvalJSManager(_sdlActivity, _webView);
		
		webViewInitialized();
    }
	
	public WebView getWebView()
	{
		d("MINKOJAVA", "RETURN WEBVIEW: " + _webView);
		
		return _webView;
	}
	
	public String evalJS(String js)
	{
		_uniqueId++;
		
		d("MINKOJAVA", "[ID = " + _uniqueId + "]Try to evaluate JS: " + js);
		
		return _evalJSManager.evalJS(js, _uniqueId);
	}
}