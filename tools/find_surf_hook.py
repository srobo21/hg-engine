#!/usr/bin/env python3
"""
Finds vanilla GetIdxOfFirstPartyMonWithMove in your ROM's arm9 and installs the Inflatable Raft's
A-on-water hook for it (adds one line to the `hooks` file).

Run from the hg-engine root after at least one build (it reads the decompressed base/arm9.bin):
    python3 tools/find_surf_hook.py

How it finds the function: in vanilla HeartGold it sits just before GetFirstAliveMonInParty_CrashIfNone
(0x02054388), and it is the only function there that calls Party_GetCount, Party_GetMonByIndex and
GetMonData five times (one egg check plus four move slots). The script refuses to write anything
unless exactly one function matches.
"""
import re
import struct
import sys

ARM9 = "base/arm9.bin"
HOOKS = "hooks"
LOAD = 0x02000000
SEARCH_START, SEARCH_END = 0x02054200, 0x02054388   # end = GetFirstAliveMonInParty_CrashIfNone
PARTY_GET_COUNT = 0x02074640
PARTY_GET_MON = 0x02074644
GET_MON_DATA = 0x0206E540
HOOK_SYMBOL = "GetIdxOfFirstPartyMonWithMove_Hook"
HOOK_REGISTER = 2   # r0 = party, r1 = move; r2 is free for the hook's jump


def bl_targets(code, start, end):
    """(address, target) for every Thumb BL/BLX pair between start and end."""
    out = []
    a = start
    while a < end - 2:
        hi = struct.unpack_from("<H", code, a - LOAD)[0]
        lo = struct.unpack_from("<H", code, a - LOAD + 2)[0]
        if (hi & 0xF800) == 0xF000 and (lo & 0xE800) == 0xE800:
            off = ((hi & 0x7FF) << 12) | ((lo & 0x7FF) << 1)
            if off & 0x400000:
                off -= 0x800000
            out.append((a, a + 4 + off))
            a += 4
        else:
            a += 2
    return out


def function_starts(code, start, end):
    """Addresses of Thumb `push {..., lr}` instructions (0xB5xx) between start and end."""
    return [a for a in range(start, end, 2) if (struct.unpack_from("<H", code, a - LOAD)[0] & 0xFF00) == 0xB500]


def main():
    hooks = open(HOOKS, encoding="utf-8", newline="").read()
    if re.search(rf"^\s*arm9\s+{HOOK_SYMBOL}\s", hooks, re.M):
        # once installed, the hook overwrites the start of the function in base/arm9.bin, so don't rescan
        print(f"Hook already installed ({HOOK_SYMBOL}). Nothing to do.")
        return

    try:
        code = open(ARM9, "rb").read()
    except FileNotFoundError:
        sys.exit(f"{ARM9} not found. Run this from the hg-engine root after building once.")
    if len(code) < 0xBC000:
        sys.exit(f"{ARM9} still looks compressed. Build once so hg-engine decompresses it, then rerun.")

    starts = function_starts(code, SEARCH_START, SEARCH_END) + [SEARCH_END]
    matches = []
    for fstart, fend in zip(starts, starts[1:]):
        calls = [t for _, t in bl_targets(code, fstart, fend)]
        if calls.count(PARTY_GET_COUNT) >= 1 and calls.count(PARTY_GET_MON) >= 1 and calls.count(GET_MON_DATA) >= 5:
            matches.append(fstart)

    if len(matches) != 1:
        sys.exit(f"Expected exactly one match, found {len(matches)}: {[hex(m) for m in matches]}. Nothing changed.")

    address = matches[0]
    line = f"arm9 {HOOK_SYMBOL} {address:08X} {HOOK_REGISTER}"
    newline = "\r\n" if "\r\n" in hooks else "\n"
    if not hooks.endswith(("\n", "\r\n")):
        hooks += newline
    hooks += f"{newline}# Inflatable Raft: A on water offers the raft (address found by tools/find_surf_hook.py){newline}{line}{newline}"
    open(HOOKS, "w", encoding="utf-8", newline="").write(hooks)
    print(f"Found GetIdxOfFirstPartyMonWithMove at {address:08X}. Added to hooks:\n  {line}\nRebuild to apply.")


if __name__ == "__main__":
    main()
