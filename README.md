# vaxRaptor
This is a modified _Raptor: Call Of The Shadows_ based on DOS source code v1.2
with the Apogee Sound System and the DMX wrapper APODMX instead of the proprietary DMX library.

The goals of the project:
* have a portable and up-to-date source code compatible with new platforms and standards
* provide an executable for people to run Raptor with DOS v1.2 GLBs without having
  to do Steam or Windows release shenanigans
* fix some minor bugs and nuisances
* add long-needed features
* remove obsolete or unneeded stuff
* add modding support
* expose some fun stuff
* have a clean commit history allowing potential hackers to undo the more arguable
  or opinionated stuff I do or simply to cherry-pick the goodies

## Build
 Currently supported compiler(s): [Open Watcom V2](https://github.com/open-watcom/open-watcom-v2)
 
 Code migration status:
 * SOURCE: all asm and int386 calls removed, fully compatible
 
 and the full migration is pending for GFX due to asm usage
 - an effort to rewrite the code to pure C and thus drop the requirement
 for external assembler is ongoing.

## License
Code is distributed under the GPL Version 2 or newer, see [LICENSE](https://github.com/FyiurAmron/dosraptor/blob/master/LICENSE).

## Thanks
Thanks go to:
* [Scott Host](https://www.mking.com) - original author of Raptor
* [skynettx](https://github.com/skynettx) - handled the original code release as [dosraptor](https://github.com/skynettx/dosraptor)
* [nukeykt](https://github.com/nukeykt) - maintainer of APODMX
* [NY00123](https://github.com/NY00123) - helped with the original DOS code release
