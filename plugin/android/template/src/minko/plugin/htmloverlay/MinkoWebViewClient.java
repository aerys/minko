package minko.plugin.htmloverlay;

import android.webkit.WebView;
import android.webkit.WebViewClient;

public class MinkoWebViewClient extends WebViewClient 
{
	@Override
	public boolean shouldOverrideUrlLoading(WebView view, String url) 
	{
		view.loadUrl(url);
		return true;
	}
}
