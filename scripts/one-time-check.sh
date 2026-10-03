#!/usr/bin/env bash
# One-time static + optional live smoke check. Does not modify production data.
set -u
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
FAIL=0

echo '== Fireplace Loader one-time checks =='
echo '[1/4] JavaScript syntax checks'
while IFS= read -r -d '' file; do
  if ! node --check "$file" >/dev/null 2>&1; then
    echo "FAIL syntax: $file"; FAIL=1
  fi
done < <(find frontend admin-panel backend/src backend/tests android-app/app/src/main/assets/www/js -type f -name '*.js' -print0)
if [ "$FAIL" -eq 0 ]; then echo 'PASS: JavaScript syntax'; fi

echo '[2/4] Android bundled frontend matches website frontend'
for rel in index.html css/style.css js/api.js js/app.js js/demo-scene.js; do
  if cmp -s "frontend/$rel" "android-app/app/src/main/assets/www/$rel"; then
    echo "PASS: $rel"
  else
    echo "FAIL: $rel differs"; FAIL=1
  fi
done

echo '[3/4] Android permissions and build wrapper'
grep -q 'android.permission.INTERNET' android-app/app/src/main/AndroidManifest.xml && echo 'PASS: INTERNET permission present' || { echo 'FAIL: INTERNET permission missing'; FAIL=1; }
grep -q 'android.permission.ACCESS_NETWORK_STATE' android-app/app/src/main/AndroidManifest.xml && echo 'PASS: ACCESS_NETWORK_STATE permission present' || { echo 'FAIL: network-state permission missing'; FAIL=1; }
if [ -f android-app/gradle/wrapper/gradle-wrapper.jar ] && [ -f android-app/gradlew ]; then
  echo 'PASS: Gradle wrapper files present'
else
  echo 'BLOCKED: Gradle wrapper executable/JAR missing; build with Android Studio or generate wrapper using installed Gradle'
fi

echo '[4/4] Optional live API smoke test'
if [ -n "${API_BASE_URL:-}" ]; then
  URL="${API_BASE_URL%/}/api/health"
  if command -v curl >/dev/null 2>&1 && curl --fail --silent --show-error --max-time 10 "$URL"; then
    echo
    echo "PASS: health endpoint responded at $URL"
  else
    echo "FAIL: could not verify $URL"; FAIL=1
  fi
else
  echo 'SKIP: set API_BASE_URL to your deployed backend origin to test /api/health (without /api suffix)'
fi

if [ "$FAIL" -eq 0 ]; then echo 'Static checks complete; this does not certify production security or app behavior.'; else echo 'One or more checks failed.'; fi
exit "$FAIL"
