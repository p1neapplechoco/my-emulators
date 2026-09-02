#!/usr/bin/env python3

import argparse
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent

EMULATORS = [
    {
        "id": "chip8",
        "label": "CHIP-8",
        "dir": "CHIP-8",
        "build": ["make", "-C", "CHIP-8", "build"],
        "run": ["./test.exe"],
    },
]


def list_emulators() -> None:
    print("Available emulators:\n")
    for index, emulator in enumerate(EMULATORS, start=1):
        print(f"{index}. {emulator['label']} ({emulator['id']})")
    print("\n0. Exit")


def run_emulator(emulator: dict) -> None:
    emulator_dir = ROOT / emulator["dir"]

    if not emulator_dir.exists():
        print(f"Error: emulator folder not found: {emulator_dir}")
        raise SystemExit(1)

    print(f"Launching {emulator['label']}...\n")

    build_cmd = emulator.get("build")
    if build_cmd:
        print(f"$ {' '.join(build_cmd)}")
        subprocess.run(build_cmd, cwd=ROOT, check=True)

    run_cmd = emulator.get("run")
    if run_cmd:
        print(f"$ {' '.join(run_cmd)}")
        subprocess.run(run_cmd, cwd=emulator_dir, check=True)


def interactive_menu() -> None:
    while True:
        print("\n=== Emulator Launcher ===")
        list_emulators()

        try:
            choice = input("Select an emulator: ").strip()
        except EOFError:
            print()
            return

        if choice in {"", "0", "q", "quit", "exit"}:
            print("Goodbye.")
            return

        try:
            index = int(choice) - 1
        except ValueError:
            print("Please enter a number from the list.")
            continue

        if 0 <= index < len(EMULATORS):
            run_emulator(EMULATORS[index])
            return

        print("Invalid selection. Try again.")


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Launch one of the available emulators."
    )
    parser.add_argument(
        "name", nargs="?", help="Run a specific emulator by id, e.g. chip8"
    )
    parser.add_argument(
        "--list", action="store_true", help="Show all available emulator ids"
    )
    args = parser.parse_args()

    if args.list:
        list_emulators()
        return

    if args.name:
        for emulator in EMULATORS:
            if emulator["id"] == args.name:
                run_emulator(emulator)
                return
        print(f"Unknown emulator '{args.name}'.")
        list_emulators()
        raise SystemExit(1)

    interactive_menu()


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\nInterrupted by user.")
        raise SystemExit(130)
