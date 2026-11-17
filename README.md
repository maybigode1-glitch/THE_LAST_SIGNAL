# THE LAST SIGNAL

First-person survival-horror prototype built with Unreal Engine 5.4.

## Current prototype

The repository now contains a minimal Unreal C++ project foundation:

- First-person character and camera.
- WASD movement and mouse look through legacy input mappings.
- E-key interaction via a visibility-channel line trace.
- Blueprintable signal-interactable actor with a Blueprint-native interaction event.
- GameMode that spawns the prototype character.

The initial prototype intentionally has no binary Unreal assets or level yet. Create the first test map and assign meshes in Unreal Editor as described in [Docs/PROTOTYPE_SCOPE.md](Docs/PROTOTYPE_SCOPE.md).

## Open in Unreal

1. Install Unreal Engine 5.4 (or a compatible 5.x version).
2. Open `THE_LAST_SIGNAL.uproject`.
3. Allow Unreal to generate IDE project files and compile the C++ module.
4. Follow the editor setup checklist in [Docs/PROTOTYPE_SCOPE.md](Docs/PROTOTYPE_SCOPE.md).

## Repository checks

```bash
python Scripts/validate_project.py
python -m unittest discover -s Tests -v
```

GitHub Actions runs metadata, project-structure, and Python unit checks on pushes and pull requests.

## Honest test boundary

A green CI run means repository-level checks passed. It does **not** mean the Unreal C++ module was compiled, the editor launched, gameplay was exercised, or a Windows build was produced. See [Docs/TEST_STATUS.md](Docs/TEST_STATUS.md).
