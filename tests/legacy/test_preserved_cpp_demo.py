"""Independent historical demo preservation checks; never flies a UAV."""
from __future__ import annotations
import hashlib
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]
ARCHIVE = ROOT / "examples" / "legacy-mission-state-machine"
ORIGINAL = ARCHIVE / "original"

def blob_sha(data: bytes) -> str:
    return hashlib.sha1(f"blob {len(data)}\0".encode() + data).hexdigest()

class LegacyPreservationTests(unittest.TestCase):
    def test_exact_git_blob_identity(self):
        rows = (ARCHIVE / "SOURCE_SHA1_MANIFEST.txt").read_text(encoding="utf-8").splitlines()
        self.assertEqual(len(rows), 15)
        for row in rows:
            expected, relative_path = row.split("  ", 1)
            with self.subTest(path=relative_path):
                path = ORIGINAL / relative_path
                self.assertTrue(path.is_file())
                self.assertEqual(blob_sha(path.read_bytes()), expected)

    @unittest.skipUnless(shutil.which("g++"), "g++ not installed")
    def test_legacy_console_demo_runs_separately(self):
        sources = sorted((ORIGINAL / "src").glob("*.cpp"))
        self.assertEqual(len(sources), 5)
        with tempfile.TemporaryDirectory() as temp:
            executable = Path(temp) / "mission_demo"
            subprocess.run(
                ["g++", "-std=c++17", "-Wall", "-Wextra", "-Werror",
                 "-I" + str(ORIGINAL / "include"), *map(str, sources),
                 "-o", str(executable)],
                check=True, capture_output=True, text=True
            )
            result = subprocess.run(
                [str(executable)], capture_output=True, text=True, check=True
            )
            self.assertIn("[EVENT] Mission simulation started", result.stdout)
            self.assertIn("[STATE] COMPLETE", result.stdout)
            self.assertNotIn("Failsafe triggered", result.stdout)
            # Historical sample has a late low-battery reading, but the program
            # exits at COMPLETE before reading it; don't misstate what was tested.

if __name__ == "__main__":
    unittest.main()
