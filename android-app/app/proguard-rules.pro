# Add project specific ProGuard rules here.
-keepclassmembers class com.fireplace.loader.AppBridge {
    @android.webkit.JavascriptInterface <methods>;
}
-keep class com.fireplace.loader.** { *; }
-dontwarn okhttp3.**
-dontwarn okio.**
