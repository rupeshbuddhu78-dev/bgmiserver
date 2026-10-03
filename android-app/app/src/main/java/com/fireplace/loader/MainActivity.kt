package com.fireplace.loader

import android.annotation.SuppressLint
import android.content.Context
import android.content.pm.PackageManager
import android.location.Location
import android.location.LocationListener
import android.location.LocationManager
import android.net.ConnectivityManager
import android.net.NetworkCapabilities
import android.os.Build
import android.os.Bundle
import android.webkit.*
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import com.fireplace.loader.databinding.ActivityMainBinding
import android.provider.Settings
import java.security.SecureRandom

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding
    private lateinit var webView: WebView
    private var locationManager: LocationManager? = null
    private var lastKnownLocation: Location? = null
    private var locationUpdatesActive = false

    companion object {
        private const val LOCATION_PERMISSION_REQUEST = 1001
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        locationManager = getSystemService(Context.LOCATION_SERVICE) as LocationManager
        setupWebView()
        loadApp()
    }

    @SuppressLint("SetJavaScriptEnabled")
    private fun setupWebView() {
        webView = binding.webView
        val settings = webView.settings

        // Enable JavaScript (required for the app)
        settings.javaScriptEnabled = true
        settings.domStorageEnabled = true
        settings.databaseEnabled = true

        // Security settings
        settings.allowFileAccess = false
        settings.allowContentAccess = false
        settings.builtInZoomControls = false
        settings.displayZoomControls = false
        settings.setSupportZoom(false)

        // Cache
        settings.cacheMode = WebSettings.LOAD_DEFAULT

        // Mixed content - only allow in debug
        if (BuildConfig.DEBUG) {
            settings.mixedContentMode = WebSettings.MIXED_CONTENT_COMPATIBILITY_MODE
        } else {
            settings.mixedContentMode = WebSettings.MIXED_CONTENT_NEVER_ALLOW
        }

        // Add JavaScript bridge for safe native features only
        webView.addJavascriptInterface(AppBridge(this), "AndroidBridge")

        // WebView client for navigation control
        webView.webViewClient = object : WebViewClient() {
            override fun shouldOverrideUrlLoading(
                view: WebView?,
                request: WebResourceRequest?
            ): Boolean {
                val url = request?.url?.toString() ?: return false
                // Only allow local files and trusted API origins
                if (url.startsWith("file:///android_asset/")) return false
                // Block all other navigation
                return true
            }

            override fun onPageFinished(view: WebView?, url: String?) {
                super.onPageFinished(view, url)
            }
        }

        // WebChromeClient for console messages in debug
        if (BuildConfig.DEBUG) {
            webView.webChromeClient = object : WebChromeClient() {
                override fun onConsoleMessage(consoleMessage: ConsoleMessage?): Boolean {
                    consoleMessage?.let {
                        android.util.Log.d("WebView", "${it.message()} -- ${it.sourceId()}:${it.lineNumber()}")
                    }
                    return true
                }
            }
        }
    }

    private fun loadApp() {
        webView.loadUrl("file:///android_asset/www/index.html")
    }

    // --- Location Management ---

    fun hasLocationPermission(): Boolean {
        return ActivityCompat.checkSelfPermission(
            this, android.Manifest.permission.ACCESS_FINE_LOCATION
        ) == PackageManager.PERMISSION_GRANTED
    }

    fun requestLocationPermission() {
        ActivityCompat.requestPermissions(
            this,
            arrayOf(
                android.Manifest.permission.ACCESS_FINE_LOCATION,
                android.Manifest.permission.ACCESS_COARSE_LOCATION
            ),
            LOCATION_PERMISSION_REQUEST
        )
    }

    fun startLocationUpdates() {
        if (!hasLocationPermission()) return
        if (locationUpdatesActive) return

        try {
            locationManager?.requestLocationUpdates(
                LocationManager.GPS_PROVIDER,
                2000L,
                5f,
                locationListener
            )
            locationManager?.requestLocationUpdates(
                LocationManager.NETWORK_PROVIDER,
                2000L,
                5f,
                locationListener
            )
            locationUpdatesActive = true

            // Get last known location immediately
            val gpsLoc = locationManager?.getLastKnownLocation(LocationManager.GPS_PROVIDER)
            val netLoc = locationManager?.getLastKnownLocation(LocationManager.NETWORK_PROVIDER)
            lastKnownLocation = when {
                gpsLoc != null && netLoc != null -> if (gpsLoc.time > netLoc.time) gpsLoc else netLoc
                gpsLoc != null -> gpsLoc
                else -> netLoc
            }
        } catch (e: SecurityException) {
            android.util.Log.e("MainActivity", "Location permission denied", e)
        }
    }

    fun stopLocationUpdates() {
        if (locationUpdatesActive) {
            locationManager?.removeUpdates(locationListener)
            locationUpdatesActive = false
        }
    }

    fun getLastLocation(): Location? = lastKnownLocation

    fun isLocationEnabled(): Boolean {
        return try {
            val gps = locationManager?.isProviderEnabled(LocationManager.GPS_PROVIDER) ?: false
            val net = locationManager?.isProviderEnabled(LocationManager.NETWORK_PROVIDER) ?: false
            gps || net
        } catch (e: Exception) {
            false
        }
    }

    private val locationListener = object : LocationListener {
        override fun onLocationChanged(location: Location) {
            lastKnownLocation = location
            // Notify WebView of location update
            val json = locationToJson(location)
            runOnUiThread {
                webView.evaluateJavascript(
                    "if(typeof onNativeLocationUpdate==='function')onNativeLocationUpdate($json)",
                    null
                )
            }
        }

        override fun onProviderEnabled(provider: String) {}
        override fun onProviderDisabled(provider: String) {}
    }

    private fun locationToJson(loc: Location): String {
        return """{"lat":${loc.latitude},"lng":${loc.longitude},"accuracy":${loc.accuracy},"altitude":${loc.altitude},"speed":${loc.speed},"time":${loc.time},"provider":"${loc.provider}"}"""
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == LOCATION_PERMISSION_REQUEST) {
            val granted = grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_GRANTED
            runOnUiThread {
                webView.evaluateJavascript(
                    "if(typeof onLocationPermissionResult==='function')onLocationPermissionResult($granted)",
                    null
                )
            }
            if (granted) {
                startLocationUpdates()
            }
        }
    }

    private fun isNetworkAvailable(): Boolean {
        val cm = getSystemService(Context.CONNECTIVITY_SERVICE) as ConnectivityManager
        val network = cm.activeNetwork ?: return false
        val capabilities = cm.getNetworkCapabilities(network) ?: return false
        return capabilities.hasCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET)
    }

    override fun onBackPressed() {
        if (webView.canGoBack()) {
            webView.goBack()
        } else {
            super.onBackPressed()
        }
    }

    override fun onDestroy() {
        stopLocationUpdates()
        webView.destroy()
        super.onDestroy()
    }
}

/**
 * JavaScript bridge exposing only safe native features.
 * No privileged methods, no file system access, no process manipulation.
 */
class AppBridge(private val context: Context) {

    @JavascriptInterface
    fun getApiUrl(): String {
        return BuildConfig.API_BASE_URL
    }

    @JavascriptInterface
    fun getDeviceId(): String {
        val prefs = context.getSharedPreferences("app_prefs", Context.MODE_PRIVATE)
        var deviceId = prefs.getString("device_id", null)
        if (deviceId == null) {
            val random = SecureRandom()
            val bytes = ByteArray(16)
            random.nextBytes(bytes)
            deviceId = "dev_" + bytes.joinToString("") { "%02x".format(it) }
            prefs.edit().putString("device_id", deviceId).apply()
        }
        return deviceId
    }

    @JavascriptInterface
    fun getAppVersion(): String {
        return try {
            context.packageManager.getPackageInfo(context.packageName, 0).versionName ?: "unknown"
        } catch (e: Exception) {
            "unknown"
        }
    }

    @JavascriptInterface
    fun isNetworkAvailable(): Boolean {
        val cm = context.getSystemService(Context.CONNECTIVITY_SERVICE) as ConnectivityManager
        val network = cm.activeNetwork ?: return false
        val capabilities = cm.getNetworkCapabilities(network) ?: return false
        return capabilities.hasCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET)
    }

    // --- Location Bridge Methods ---

    @JavascriptInterface
    fun hasLocationPermission(): Boolean {
        val activity = context as? MainActivity ?: return false
        return activity.hasLocationPermission()
    }

    @JavascriptInterface
    fun requestLocationPermission() {
        val activity = context as? MainActivity ?: return
        activity.runOnUiThread {
            activity.requestLocationPermission()
        }
    }

    @JavascriptInterface
    fun isLocationEnabled(): Boolean {
        val activity = context as? MainActivity ?: return false
        return activity.isLocationEnabled()
    }

    @JavascriptInterface
    fun startLocationUpdates() {
        val activity = context as? MainActivity ?: return
        activity.runOnUiThread {
            activity.startLocationUpdates()
        }
    }

    @JavascriptInterface
    fun stopLocationUpdates() {
        val activity = context as? MainActivity ?: return
        activity.runOnUiThread {
            activity.stopLocationUpdates()
        }
    }

    @JavascriptInterface
    fun getLastLocation(): String {
        val activity = context as? MainActivity ?: return """{"error":"no_activity"}"""
        val loc = activity.getLastLocation() ?: return """{"error":"no_location"}"""
        return """{"lat":${loc.latitude},"lng":${loc.longitude},"accuracy":${loc.accuracy},"altitude":${loc.altitude},"speed":${loc.speed},"time":${loc.time},"provider":"${loc.provider}"}"""
    }
}
