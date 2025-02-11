from typing import *


# extmod/modtrezorutils/modtrezorutils-meminfo.h
def meminfo(filename: str) -> None:
    """Dumps map of micropython GC arena to a file.
    The JSON file can be decoded by analyze.py
    Only available in the emulator.
     """


# extmod/modtrezorutils/modtrezorutils.c
def consteq(sec: bytes, pub: bytes) -> bool:
    """
    Compares the private information in `sec` with public, user-provided
    information in `pub`.  Runs in constant time, corresponding to a length
    of `pub`.  Can access memory behind valid length of `sec`, caller is
    expected to avoid any invalid memory access.
    """


# extmod/modtrezorutils/modtrezorutils.c
def memcpy(
    dst: bytearray | memoryview,
    dst_ofs: int,
    src: bytes,
    src_ofs: int,
    n: int | None = None,
) -> int:
    """
    Copies at most `n` bytes from `src` at offset `src_ofs` to
    `dst` at offset `dst_ofs`. Returns the number of actually
    copied bytes. If `n` is not specified, tries to copy
    as much as possible.
    """


# extmod/modtrezorutils/modtrezorutils.c
def halt(msg: str | None = None) -> None:
    """
    Halts execution.
    """


# extmod/modtrezorutils/modtrezorutils.c
def reset() -> None:
    """
    Reset system.
    """


# extmod/modtrezorutils/modtrezorutils.c
def firmware_hash(
    challenge: bytes | None = None,
    callback: Callable[[int, int], None] | None = None,
) -> bytes:
    """
    Computes the Blake2s hash of the firmware with an optional challenge as
    the key.
    """


# extmod/modtrezorutils/modtrezorutils.c
def onekey_firmware_hash() -> bytes:
    """
    Computes the sha256 hash of the firmware
    """


# extmod/modtrezorutils/modtrezorutils.c
def firmware_vendor() -> str:
    """
    Returns the firmware vendor string from the vendor header.
    """


# extmod/modtrezorutils/modtrezorutils.c
def reboot_to_bootloader() -> None:
    """
    Reboots to bootloader.
    """


# extmod/modtrezorutils/modtrezorutils.c
def reboot2boardloader() -> None:
    """
    Reboots to boardloader.
    """


# extmod/modtrezorutils/modtrezorutils.c
def boot_version() -> str:
    """
    Returns the bootloader version string.
    """


# extmod/modtrezorutils/modtrezorutils.c
def boot_hash() -> bytes:
    """
    Returns the bootloader hash string.
    """


# extmod/modtrezorutils/modtrezorutils.c
def board_version() -> str:
    """
    Returns the bootloader version string.
    """


# extmod/modtrezorutils/modtrezorutils.c
def board_hash() -> bytes:
    """
    Returns the boardloader hash.
    """


# extmod/modtrezorutils/modtrezorutils.c
def boot_build_id() -> str:
    """
    Returns the bootloader build_id.
    """


# extmod/modtrezorutils/modtrezorutils.c
def usb_data_connected() -> bool:
    """
    Returns whether USB has been enumerated/configured
    (and is not just connected by cable without data pins)
    """
SCM_REVISION: bytes
BUILD_ID: bytes
VERSION_MAJOR: int
VERSION_MINOR: int
VERSION_PATCH: int
MODEL: str
EMULATOR: bool
BITCOIN_ONLY: bool
FIRMWARE_SECTORS_COUNT: int
