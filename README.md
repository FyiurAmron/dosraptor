# vaxRaptor
This is a modified _Raptor: Call Of The Shadows_ based on DOS source code v1.2
with the Apogee Sound System and the DMX wrapper APODMX instead of the proprietary DMX library.

The goals of the project:
 * have a portable and up-to-date source code compatible with new platforms and standards
 * provide an executable for people to run Raptor with DOS v1.2 GLBs on up-to-date
   platforms without having to do Steam or Windows release shenanigans
 * fix some minor bugs and nuisances
 * add long-needed features (e.g. quick mode)
 * remove obsolete or unneeded stuff (IBNLT GLB/save encryption)
 * add modding support
 * expose some fun stuff (Taiwan mode FTW :)
 * have a clean commit history allowing potential hackers to undo the more arguable
  or opinionated stuff I do or simply to cherry-pick the goodies
  
## Roadmap
 * ✔️ DONE: standardize code style
 * ✔️ DONE: be able to cross-compile with Open Watcom v2 on Win64 host without any warnings
 * ✔️ DONE: streamline/clean up startup procedures
 * ✔️ DONE: migrate asm files to native C
 * clean up extraneous defines, dead code, naming - ongoing...
 * all logging teed to con/file - ongoing...
 * fix known bugs - ongoing...
 * migrate IO to Allegro 4 (or equiv.) abstraction - TODO
 * do a fully native Win64 build - TODO
 * migrate to/support DJGPP/GCC - ?

## Build
 Currently supported language/compiler(s): C99 on [Open Watcom v2](https://github.com/open-watcom/open-watcom-v2)

#### Code migration status:
 * SOURCE: all asm code and int386 calls removed, fully compatible
 * GFX: all asm code removed, int386 etc. still present as key code

Cross-compilation with Win64 OWv2 possible out-of-the-box now for both modules.

## Historic info
 * older versions of Watcom C will not work now (Watcom C 10.0 was used previously)
   due to no support for C99, but generally speaking the code should be still
   backwards-compatible if refactored to not use C99 constructs,
 * TASM could be safely replaced with OpenWatcom v2's WASM
   with `-mf -zcm=tasm` flags and `$(ASM) $(AFLAGS) $^&.asm` build cmd
 
## License
Code is distributed under the GPL Version 2 or newer, see [LICENSE](https://github.com/FyiurAmron/dosraptor/blob/master/LICENSE).

## Thanks
Thanks go to:
 * [Scott Host](https://www.mking.com) a.k.a. Scott Hostynski - original author of Raptor
 * [skynettx](https://github.com/skynettx) - handled the original code release as [dosraptor](https://github.com/skynettx/dosraptor)
 * [nukeykt](https://github.com/nukeykt) - maintainer of APODMX
 * [NY00123](https://github.com/NY00123) - helped with the original DOS code release
