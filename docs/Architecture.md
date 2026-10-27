# Game architecture

## Native C++ edition

`game/src/game.cpp` creates the application and `example_layer` owns the scene. The layer initializes rendering, lighting, audio, a third-person camera, the player, a generated spiral map, pickups and twenty enemies. The engine's Bullet manager advances the physics world and synchronizes game objects. Rendering uses custom OpenGL shaders and retained model/texture loaders.

`MapGenerator::generate_spiral_map` creates twelve platforms around radius eight with 45-degree angular increments and 1.5-unit height steps. Every third layer adds a horizontal mover, and every fourth adds a vertical mover. `MovingPlatform` interpolates between endpoints. `Player` handles camera-relative input, jumping, health and timed jump multipliers. `Enemy` runs Patrol, Investigate, Chase, Attack and Recover states, with scout, hunter and guard roles. `GameManager` manages intro/play/results screens and calculates survival/pickup scores.

These systems extend an existing engine template. The template license and dependency copyright notices are retained; this repository does not claim the underlying third-party engine and libraries were written from scratch.

## Browser adaptation

The browser implementation is in `Web/`. It preserves the recognizable objective and major mechanics while using a separate JavaScript simulation and Three.js renderer. It does not port Bullet or FMOD to JavaScript.

| File | Responsibility |
| --- | --- |
| `config.json` | Spiral dimensions, movement/gravity/jump parameters, enemy count, health, boost duration, checkpoint cadence and survival target |
| `core.js` | World generation, fixed-step movement, collision responses, enemy behaviour, progression, health, pickups, scoring and record normalization |
| `main.js` | Three.js scene, camera, DOM controls, touch input, Web Audio tones and browser persistence |
| `tests/core.test.mjs` | Deterministic simulation and regression checks |
| `vendor/` | Pinned Three.js module files and license |

### Update and collision order

The animation loop uses a 1/60-second simulation accumulator and caps long frame deltas. A step advances moving platforms, carries any grounded player by their platform's displacement, applies buffered jump input and normalized movement, advances gravity, resolves landings, resolves sides/undersides, applies pickups, advances enemies, then evaluates defeat/victory. Pause and hidden-tab handling stop simulation advancement.

Landing tests sweep the sphere's previous feet position across platform tops. The highest eligible top surface wins, preventing a descending player from tunnelling through a thin platform. Sphere-to-box closest-point responses handle sides and undersides. The island ground is a bounded surface; leaving its perimeter allows falling and checkpoint respawn.

The main spiral remains twelve platforms at the original angular spacing, radius and height interval. Moving platforms are placed on outer side routes so they do not obstruct normal jumps from the main route. The player begins on the first numbered platform. The browser jump/gravity values are tuned for those gaps and checked by a test that walks/jumps the entire spiral without boosts, then walks into the summit ring.

### Player experience

Movement is relative to the current orbit camera. Dragging, Q/E or touch buttons rotate it without requiring pointer lock. Jump input has a short buffer and coyote interval. Holding Jump does not trigger repeated jumps after each landing. Pickups either add 20 health up to the maximum or apply a five-second 1.5x jump multiplier.

Every third numbered platform becomes a respawn checkpoint. A fall costs 25 health and resets position/velocity to that checkpoint. Each new run recreates platforms, enemies, pickups, health and timers, avoiding partially reset sessions. Enemy damage uses both an attack interval and a player damage cooldown.

Patrol enemies choose nearby targets. Scouts detect targets at 14 units; other roles at nine. Hunters chase at 3.6 units/second and other roles at 2.5. Guards deal 15 damage instead of ten. Attacks lead to a 0.65-second recovery window. Every twelve seconds a patrolling enemy investigates the player’s current horizontal location for up to eight seconds. Chase persists beyond the initial detection range on the ground. Ground enemies cannot damage players high above them. A fixed seed makes the initial browser world reproducible.

### Goals and records

Reaching the summit ring or surviving the configured five-minute target ends in victory; depleted health ends in defeat. The browser score is ten points per whole second survived, fifty per pickup, one hundred per alternating pickup combo collected within eight seconds, one hundred per highest completed platform index and a five-hundred-point victory bonus.

Results can be saved to a named top-ten table in browser-local storage. Entries are normalized and invalid data is rejected before rendering. Names are inserted with DOM text operations. There is no network leaderboard, account or collection of player data by a server.

### Rendering and sound

The browser's platform meshes, blue marble, red enemies, pine scenery, route guides, labels and portal are generated in code. Shadows, fog and restrained lighting support spatial readability. Three.js handles WebGL rendering; obsolete scene meshes, materials and textures are disposed on restart. Sound effects and an eight-note background pattern use Web Audio oscillator tones triggered after a user gesture, without downloaded recordings or an FMOD runtime.

The browser uses procedural visuals rather than distributing the native model packs, skybox, music or Arial font. It requires a modern browser with WebGL 2; other devices may have different GPU and input behaviour.
