# Repository Guidelines

## Project Structure & Module Organization
- `Sources/` holds core C++ logic shared across the app.
- `Qt_SRC/` and `Qt_HEAD/` contain Qt-specific implementations and headers.
- `Qt_UI/` stores `.ui` files for Qt Designer; `Qt_QRC/` stores `.qrc` resources.
- `CDIRCalculator/` is a subproject built alongside the main app.
- `assets/` and `docs/` hold images and user-facing documentation.
- `EasyTier/` and `publicserver.json` are runtime resources packaged on Windows.

## Build, Test, and Development Commands
- Configure: `mkdir build && cd build && cmake ..` (requires Qt 6 and CMake >= 3.16).
- Build (Windows release): `cmake --build . --config Release`.
- Install bundle: `cmake --install . --config Release` (installs under `Install/`).
- Output binaries are placed in `build/bin/` by default.

## Coding Style & Naming Conventions
- C++ standard is C++20; keep Qt patterns (signals/slots, `QObject` ownership) consistent.
- Indentation is 4 spaces in existing files; follow the surrounding style.
- UI files go to `Qt_UI/`, resources to `Qt_QRC/`, and headers to `Qt_HEAD/`.
- Prefer descriptive class names (e.g., `MainWindow`, `NetPage`) and keep file names aligned with class names.

## Testing Guidelines
- No automated test targets or `CTest` entries are currently defined in CMake.
- Validate changes by launching the app and exercising affected UI flows.
- If adding tests, introduce a `tests/` directory and wire them with `add_test()`.

## Commit & Pull Request Guidelines
- Recent history uses Conventional Commits with Chinese summaries (e.g., `chore(workflow): 删除旧的CI/CD流水线配置文件`).
- Use `feat:`, `fix:`, `docs:`, `chore:` with an optional scope; keep messages concise.
- PRs should include a clear description, steps to verify, and screenshots for UI changes.

## Configuration & Packaging Notes
- Windows packaging copies `BreezeStyle/`, `EasyTier/`, and `publicserver.json` into the output directory.
- Ensure these assets exist and match expected paths before release builds.
