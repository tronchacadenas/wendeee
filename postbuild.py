# Source https://github.com/esphome/esphome/blob/dev/esphome/components/esp32/post_build.py.script
# Source https://github.com/usini/esp32_platformio/blob/main/prepare_bin.py
# Source https://www.hackster.io/usini/platformio-esp32-starter-project-with-esp-web-tools-745568

# pylint: disable=E0602
Import("env")  # noqa

import os
import shutil

try:
    import esptool
except ImportError:
    env.Execute("$PYTHONEXE -m pip install esptool")

from SCons.Script import ARGUMENTS

def esp32_create_combined_bin(source, target, env):
    print("--------- Combine Firmware for webusb upload -----------------")
    verbose = bool(int(ARGUMENTS.get("PIOVERBOSE", "0")))
    if verbose:
        print("Generating combined binary for serial flashing")
    app_offset = 0x10000

    new_file_name = env.subst("$BUILD_DIR/${PROGNAME}-factory.bin")
    sections = env.subst(env.get("FLASH_EXTRA_IMAGES"))
    firmware_name = env.subst("$BUILD_DIR/${PROGNAME}.bin")
    chip = env.get("BOARD_MCU")
    flash_size = env.BoardConfig().get("upload.flash_size")
    cmd = [
        "--chip",
        chip,
        "merge_bin",
        "-o",
        new_file_name,
        "--flash_size",
        flash_size,
    ]
    if verbose:
        print("    Offset | File")
    for section in sections:
        sect_adr, sect_file = section.split(" ", 1)
        if verbose:
            print(f" -  {sect_adr} | {sect_file}")
        cmd += [sect_adr, sect_file]

    cmd += [hex(app_offset), firmware_name]

    if verbose:
        print(f" - {hex(app_offset)} | {firmware_name}")
        print()
        print(f"Using esptool.py arguments: {' '.join(cmd)}")
        print()

    esptool.main(cmd)
    print("-------------------------------------------------------------")

def esp32_copy_combined_bin(source, target, env):
    print("--------- Save Firmware for webusb upload -------------------")
    
    # Get source dir (.pio/name_env/)
    source = env.get("PROJECT_BUILD_DIR") + "/" + env.get("PIOENV")
    # Save at destination docs/bins
    destination = os.getcwd() + "/docs/bins" + "/" #+ env.get("PIOENV")
    shutil.copyfile(source + "/firmware-factory.bin", destination + "/firmware-factory.bin") # for webUSB
    shutil.copyfile(source + "/firmware.bin", destination + "/firmware.bin") # for OTA
    print(source)
    print(destination)
    print("-------------------------------------------------------------")

# pylint: disable=E0602
env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", esp32_create_combined_bin)  # noqa
env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", esp32_copy_combined_bin)