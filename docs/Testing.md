# Validation

## Automated browser simulation checks

Run from the repository root:

```powershell
node --test Web/tests/core.test.mjs
```

Twenty checks pass: generated platform counts; configuration rejection; moving-platform endpoints; movement at 30/60/120 Hz; normalized diagonals; jumping/landing; held-jump prevention; underside collisions; moving-platform carry; pause; damage cooldowns; heal/boost effects; fall/respawn; checkpoint updates; summit victory; survival victory; enemy transitions; complete new-run reset; normalized top-ten records; and a complete unboosted climb through every platform and the summit ring.

The full-route test initially exposed a moving platform obstructing a normal jump. Movers now sit on outer side routes, and the route is checked end to end. The final test includes walking to the ring after landing on the last platform rather than treating any contact with that platform as victory.

## Native build

The original C++ Debug/x64 solution was compiled and linked using the local dependency setup. It completed with zero errors and 460 warnings from the older dependency/toolchain mix. That result does not establish Release/Dist support or a standalone fresh-clone build without local SDKs and libraries.

## Browser checks

The local WebGL edition was inspected in the real browser. Keyboard input successfully moved and jumped from platform 1 to platform 2. Pause and phone-width layout were checked, including on-screen jump and camera controls. The scene includes actual 3D geometry, lighting and shadows rather than a screenshot or video standing in for gameplay.

Automated simulation tests are separate from graphics/input tests. The route test has no enemies or pickups so it isolates jump reachability; it does not prove human difficulty or normal completion rates. Mobile hardware, long sessions, perceptual audio balance and exhaustive input combinations need broader manual evaluation.
