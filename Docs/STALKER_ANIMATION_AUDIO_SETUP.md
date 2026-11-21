# Stalker — animation and audio setup

This guide connects the native C++ hooks to project-owned or properly licensed assets. The repository does not include a skeletal mesh, animation clips, montage, or recorded sound files.

## 1. Skeletal mesh and Animation Blueprint

1. Open the child Blueprint of `ASignalStalker`.
2. Assign a Skeletal Mesh with a valid Skeleton to the inherited Mesh component.
3. Create an Animation Blueprint using that same Skeleton. Set its parent class to `USignalStalkerAnimInstance`.
4. Assign the Animation Blueprint to the Mesh component's Anim Class.

The native animation instance updates these variables every animation tick:

| Variable | Meaning |
|---|---|
| `GroundSpeed` | Horizontal movement speed in cm/s |
| `bIsMoving` | True while moving |
| `bIsChasing` | Player currently visible and being pursued |
| `bIsSearching` | Investigating the last known player position |
| `bIsAttacking` | Attack state window is active |
| `bIsDead` | Reserved for the enemy death state |

## 2. Suggested locomotion state machine

Create a state machine called `Locomotion`:

- **Idle:** idle breathing / idle stance.
- **Move:** walk or run Blend Space driven by `GroundSpeed`.
- Optional **Search** state: cautious walk or look-around animation when `bIsSearching` is true.
- Optional **Chase** state: faster run Blend Space when `bIsChasing` is true.

Suggested transitions:
- Idle → Move when `bIsMoving`.
- Move → Idle when `!bIsMoving`.
- Any locomotion state → Search when `bIsSearching`.
- Search → Chase when `bIsChasing`.
- Chase → Move when `!bIsChasing && bIsMoving`.

Use a Slot node in the AnimGraph (for example, `DefaultGroup.DefaultSlot`) so the attack montage can play over the locomotion pose.

## 3. Attack montage

1. Import or create an attack animation using the same Skeleton as the enemy mesh.
2. Create an Animation Montage from that animation.
3. Put the montage in the same slot used by the AnimGraph.
4. Assign it to `AttackMontage` on the enemy Blueprint.

When the AI attacks, C++ plays the montage and raises `bIsAttacking` for the montage duration (or a 0.45-second fallback window when no montage is assigned). Damage is scheduled after a wind-up: 35% of the montage length, clamped to 0.2–0.6 seconds, or 0.3 seconds without a montage. At impact, the code reacquires the player and skips damage if the player is dead, farther than `AttackRange + 75 cm`, or (when an AI controller is available) no longer visible. This gives the player a short dodge window, but the impact is timer-based rather than synchronized to an animation notify. The timing and dodge behavior still require an Unreal runtime playtest.

## 4. Sound assets

Assign these properties on the enemy Blueprint:

| Property | Asset type | Trigger |
|---|---|---|
| `FootstepSound` | Short one-shot | Every configured travel distance while moving |
| `AlertSound` | Short one-shot | First detection / entering chase |
| `AttackSound` | Short one-shot | Each successful attack |
| `SearchSound` | Short one-shot | Entering search mode |
| `AmbientLoopSound` | Looping Sound Wave / Cue / MetaSound | Default idle/patrol ambience |
| `ChaseLoopSound` | Looping Sound Wave / Cue / MetaSound | While chasing; replaces ambient loop |

For the two loop assets, enable looping in the source asset or its Sound Cue / MetaSound graph. Configure attenuation and spatialisation so sounds originate from the enemy and fade with distance. Keep one-shot sounds short and avoid putting long loops in the one-shot properties.

Footsteps are distance-based rather than tied to animation notifies in this first implementation. This means they continue to work with different locomotion clips, but exact foot-to-ground synchronisation should be added later using Animation Notifies.

## 5. Runtime verification checklist

- [ ] Mesh renders with the intended Skeleton.
- [ ] Animation Blueprint parent is `USignalStalkerAnimInstance`.
- [ ] Idle and movement blend correctly using `GroundSpeed`.
- [ ] Chase and search states react to their Boolean variables.
- [ ] Attack montage plays when the enemy attacks.
- [ ] Footsteps play only while moving.
- [ ] Alert and search cues play on state entry, not every AI decision tick.
- [ ] Attack cue plays once per attack.
- [ ] Ambient loop plays during patrol and chase loop replaces it during pursuit.
- [ ] Audio attenuation makes the enemy's location and distance readable.

## Current implementation boundary

The C++ integration and configurable asset slots are in source control. The actual mesh, animation clips, montage, and audio assets must still be imported/created and assigned in Unreal Editor. No animation or audio runtime test has been performed yet.
