# Third-Party Licenses

DLL DAW VideoSync is built on the following open-source components.
This file is provided for attribution and compliance purposes.

---

## Steinberg VST3 SDK

Used under the **GNU General Public License v3** (dual-licensed by Steinberg).

> VST is a trademark of Steinberg Media Technologies GmbH.
> This product uses the VST3 SDK licensed under GPL v3.
> See: https://www.steinberg.net/developers/

---

## iPlug2

**License:** zlib/libpng (permissive)
**Source:** https://github.com/iPlug2/iPlug2

> Copyright (c) Oliver Larkin and contributors.
> This software is provided 'as-is', without any express or implied warranty.
> Permission is granted to anyone to use this software for any purpose,
> including commercial applications, and to alter it and redistribute it freely,
> subject to the following restrictions:
>
> 1. The origin of this software must not be misrepresented.
> 2. Altered source versions must be plainly marked as such.
> 3. This notice may not be removed or altered from any source distribution.

Bundled sub-libraries (all permissive):
- Cockos WDL (zlib-like)
- NanoVG (zlib)
- NanoSVG (zlib)

---

## libVLC

**License:** GNU Lesser General Public License v2.1 (LGPL-2.1)
**Source:** https://www.videolan.org/vlc/

This plugin dynamically links against libVLC at runtime. The LGPL-2.1 license
requires that the libVLC source code be available to end users. The source can
be obtained from: https://code.videolan.org/videolan/vlc

The full LGPL-2.1 text is included as `LGPL-2.1.txt` in this directory.

---

## CLAP (CLever Audio Plugin)

**License:** MIT
**Source:** https://github.com/free-audio/clap

> Copyright (c) 2021 Alexandre Music.
> Permission is hereby granted, free of charge, to any person obtaining a copy
> of this software and associated documentation files, to deal in the Software
> without restriction.

---

## Windows Media Foundation

Proprietary Microsoft API. Used under standard Windows SDK license terms.
No redistribution of Microsoft binaries is required as WMF is part of Windows.
