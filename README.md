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
Original DOS version was built using Watcom C 10.0 and TASM 3.1.
Currently, as a stepping stone, [Open Watcom V2](https://github.com/open-watcom/open-watcom-v2) is the suggested compiler.
An effort to drop the requirement for external assembler is underway.

## License
Code is distributed under the GPL Version 2 or newer, see [LICENSE](https://github.com/FyiurAmron/dosraptor/blob/master/LICENSE).

## Thanks
Thanks go to:
* [Scott Host](https://www.mking.com) - original author of the code
* [nukeykt](https://github.com/nukeykt) & [NY00123](https://github.com/NY00123) - helped with the original DOS code release
* [skynettx](https://github.com/skynettx) - handled the original release as [dosraptor](https://github.com/skynettx/dosraptor)