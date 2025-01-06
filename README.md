# vaxRaptor
This is a modified _Raptor: Call Of The Shadows_ based on DOS source code v1.2
with the Apogee Sound System and the DMX wrapper APODMX instead of the proprietary DMX library.

The goals of the project:
* have a portable and up-to-date source code compatible with new platforms and standards
* provide an executable for people to run Raptor with DOS v1.2 GLBs without having
  to do Steam or Windows release shenanigans
* fix some minor bugs and nuisances
* add long-needed features (e.g. quick mode)
* remove obsolete or unneeded stuff (including GLB/save encryption)
* add modding support
* expose some fun stuff
* have a clean commit history allowing potential hackers to undo the more arguable
  or opinionated stuff I do or simply to cherry-pick the goodies

## Build
 Currently supported language/compiler(s): C99 on [Open Watcom V2](https://github.com/open-watcom/open-watcom-v2)

<sub><sup>Older versions of Watcom C will not work now (Watcom C 10.0 was used previously) due to no support for C99</sup></sub>

 Code migration status:
 * SOURCE: all asm and int386 calls removed, fully compatible
 * GFX: rewriting asm to C in progress; cross-compilation with Win64 OWv2 possible

## License
Code is distributed under the GPL Version 2 or newer, see [LICENSE](https://github.com/FyiurAmron/dosraptor/blob/master/LICENSE).

## Thanks
Thanks go to:
* [Scott Host](https://www.mking.com) - original author of Raptor
* [skynettx](https://github.com/skynettx) - handled the original code release as [dosraptor](https://github.com/skynettx/dosraptor)
* [nukeykt](https://github.com/nukeykt) - maintainer of APODMX
* [NY00123](https://github.com/NY00123) - helped with the original DOS code release
