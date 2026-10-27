# Validation

## Automated browser simulation checks

Run from the repository root:

```powershell
node --test Web/tests/core.test.mjs
```

Twenty-six checks pass: generated platform counts; configuration rejection; moving-platform endpoints; movement at 30/60/120 Hz; normalized diagonals; jumping/landing; held-jump prevention; underside collisions; moving-platform carry; pause; damage cooldowns; heal/boost effects; fall/respawn; checkpoint updates; summit victory; survival victory; enemy transitions; complete new-run reset; normalized top-ten records; a complete unboosted climb through every platform and the summit ring; corner-hiding pursuit; three role differences; enemy speed across simulation frequencies; uninterruptible attack recovery; elevated-player safety; and timed pickup combo scoring.

The full-route test initially exposed a moving platform obstructing a normal jump. Movers now sit on outer side routes, and the route is checked end to end. The final test includes walking to the ring after landing on the last platform rather than treating any contact with that platform as victory.

## Native build

The upgraded C++ Debug/x64 solution was compiled and linked using the local dependency setup with zero errors. The legacy toolchain/dependencies still produce warnings (the final recorded logged build reported 174). A hidden-window runtime check then compiled the shaders, loaded models/textures/audio, advanced 120 gameplay frames at 60 Hz and exited normally; the final run also exercised enemy damage (85 health remaining). Its captured frame is in `screenshots/native-gameplay.png`. This check exposed and fixed an unused missing-shader startup crash; compilation alone had not found it. That result does not establish Release/Dist support or a standalone fresh-clone build without local SDKs and libraries.

## Browser checks

The local WebGL edition was inspected in the real browser. Keyboard input successfully moved and jumped from platform 1 to platform 2 after the upgrades; pause/resume worked and no browser console errors or warnings were reported. Pause and phone-width layout were checked, including on-screen jump and camera controls. The scene includes actual 3D geometry, lighting and shadows rather than a screenshot or video standing in for gameplay.

Automated simulation tests are separate from graphics/input tests. The route test has no enemies or pickups so it isolates jump reachability; it does not prove human difficulty or normal completion rates. Mobile hardware, long sessions, perceptual audio balance and exhaustive input combinations need broader manual evaluation.

## Repeat the native runtime check

After restoring the native dependencies and building Debug/x64, run from `game/`:

```powershell
$env:BOUNCE_SMOKE = "$env:TEMP\bounce-smoke.ppm"
..\bin\Debug-windows-x86_64\game\game.exe
Remove-Item Env:BOUNCE_SMOKE
```

The optional check opens a hidden GLFW window. A successful run logs `BOUNCE_SMOKE completed` and saves a screenshot. The native runtime is not included in the public repository; see the setup guide for SDK requirements. The test does not synthesize keyboard input or verify a complete native climb.
