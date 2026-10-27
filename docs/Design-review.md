# Design review

The spiral gives players a clear, visible destination while the arena offers a survival alternative. Moving platforms create optional timing challenges. Three enemy roles make threats readable by colour and behaviour, and attack recovery gives players a short escape window. Periodic investigation raises pressure on players who remain on the ground. Checkpoints in the browser reduce the cost of learning the route.

The strongest engineering improvement is consistency: one model-space collision convention, timestep-based enemy travel, current-frame moving-platform displacement, and explicit reset paths. The separate browser rules module makes full-route and AI regression checks practical. Public documentation describes implemented values rather than intended features.

There are deliberate limits. Box colliders approximate imported scenery; enemies do not plan routes around obstacles. Ground enemies cannot follow a player up the spiral, so the tower is primarily a movement challenge. Survival difficulty has regression coverage for one corner scenario but still needs playtesting with several players, spawn seeds and input devices. Additional enemy roles are parameterized variations of a shared state machine rather than wholly separate AI algorithms.

The desktop engine still depends on older third-party code and local SDK libraries; its compiler warnings remain. The browser is easier to run, but has a simpler physics model and procedural assets. Native Debug/x64 and the current browser were tested; broad hardware, Release/Dist builds, long-session memory profiling and accessibility beyond keyboard/touch UI are not established.

Future work should prioritize obstacle-aware enemy navigation, native checkpoint parity, more complete native gameplay automation, a freely redistributable font, and verified item-level source/license records for legacy art/audio. Replacing uncertain legacy assets with original work would make a packaged desktop release easier to distribute. A downloadable native release should follow those checks rather than relying on a successful local build alone.
