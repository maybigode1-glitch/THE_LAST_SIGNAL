# Development and test environment

## Free cloud test lane

GitHub Actions on GitHub-hosted Linux runners is used for lightweight repository checks. Keep the repository public and stay within GitHub's current usage and fair-use limits to use the standard public-repository runners without charge.

## What CI currently verifies

- Python syntax/importability of the validation script.
- .uproject JSON syntax and basic metadata, when a project descriptor exists at the repository root.
- Three unit tests for valid, missing-field, and malformed project metadata.

## What CI does not verify

- UnrealBuildTool or Unreal Editor availability.
- Compilation of Unreal C++ modules.
- Loading maps/assets, rendering, audio, input, gameplay, or performance.
- Windows packaging or a playable build.

Do not label these repository checks as an Unreal Engine build.

## Local commands

```bash
python Scripts/validate_project.py
python -m unittest discover -s Tests -v
```

## Future Unreal validation

A genuine Unreal build lane requires an authorized Unreal Engine installation and a runner with compatible OS, memory, storage, and (for editor/rendering tests) GPU support. Do not put Epic credentials or license secrets in repository files. A free generic Linux runner is not a substitute for a Windows Unreal workstation.
