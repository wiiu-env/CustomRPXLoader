# CustomRPXLoader
This custom loader for `.rpx` files which can be used with any `payload.elf` loader. (For example [https://github.com/wiiu-env/PayloadFromRPX](https://github.com/wiiu-env/PayloadFromRPX) or [JsTypeHax](https://github.com/wiiu-env/JsTypeHax))

## Usage
Place the `payload.elf` in the `sd:/wiiu` folder of your sd card and run a exploit which loads `payload.elf`, this will load the `sd:/wiiu/payload.rpx`into memory and execute it. The maximum size of the `payload.rpx` depends on the size of this loader, but should > 7Mib.

## Building

For building you just need [wut](https://github.com/devkitPro/wut/) installed, then use the `make` command.

## Credits
- Maschell
- orboditilt
- Copy pasted the solution for using wut header in .elf files from [RetroArch](https://github.com/libretro/RetroArch)
- Copy pasted resolving the ElfRelocations from [decaf](https://github.com/decaf-emu/decaf-emu)
- Copy pasted SD mounting code + devoptabs from [libutils](https://github.com/Maschell/libutils)