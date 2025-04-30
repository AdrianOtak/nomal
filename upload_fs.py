Import("env")
import subprocess

def before_upload(source, target, env):
    print(">>> Automatisches Hochladen von SPIFFS...")
    result = subprocess.run(["pio", "run", "--target", "uploadfs"])
    if result.returncode != 0:
        print(">>> Fehler beim SPIFFS-Upload!")
        env.Exit(1)

env.AddPreAction("upload", before_upload)
