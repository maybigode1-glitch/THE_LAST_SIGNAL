# THE LAST SIGNAL

Survival-horror project built with Unreal Engine 5.

## Project status

This repository is the development and validation workspace. Automated checks validate repository structure and project metadata; they do **not** claim to compile or run Unreal Engine.

## Quick start

1. Install the supported Unreal Engine 5 version (see [Docs/DEVELOPMENT.md](Docs/DEVELOPMENT.md)).
2. Generate project files using the Unreal Engine project context menu or UnrealBuildTool.
3. Open `THE_LAST_SIGNAL.uproject` in Unreal Editor.
4. Run the validation script:

```bash
python Scripts/validate_project.py
python -m unittest discover -s Tests -v
```

## CI

GitHub Actions runs metadata validation and Python unit tests on pushes and pull requests.

## Honest test boundary

A green CI run means repository-level checks passed. It does not mean the Unreal C++ module was compiled, the editor launched, gameplay was exercised, or a Windows build was produced.
