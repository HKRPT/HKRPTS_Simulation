
MEMORY
{
PAGE 0 :
   /* BEGIN is used for the "boot to SARAM" bootloader mode   */

   BEGIN        : origin = 0x000000, length = 0x000002


   RAMLS0_S5    : origin = 0x008000, length = 0x004000
   RESET        : origin = 0x3FFFC0, length = 0x000002

PAGE 1 :

   BOOT_RSVD   : origin = 0x000002, length = 0x000120     /* Part of M0, BOOT rom will use this for stack */
   RAMM0       : origin = 0x000122, length = 0x0002DE     /* Debug will use this for stack */
/*   RAMM1       : origin = 0x000400, length = 0x000400     /* on-chip RAM block M1 */

/*   RAMD0_D1    : origin = 0x00B000, length = 0x001000*/

   RAMGS0_S14  : origin = 0x00C000, length = 0x00F000

   RAMGS15     : origin = 0x01B000, length = 0x001000     /* Only Available on F28379D, F28377D, F28375D devices. Remove line on other devices. */

   CPU2TOCPU1RAM   : origin = 0x03F800, length = 0x000400
   CPU1TOCPU2RAM   : origin = 0x03FC00, length = 0x000400

   CANA_MSG_RAM     : origin = 0x049000, length = 0x000800
   CANB_MSG_RAM     : origin = 0x04B000, length = 0x000800
}


SECTIONS
{
   codestart        : > BEGIN,     PAGE = 0
   .text            : > RAMLS0_S5, PAGE = 0
   .cinit           : > RAMLS0_S5, PAGE = 0
   .pinit           : > RAMLS0_S5, PAGE = 0
   .switch          : > RAMLS0_S5, PAGE = 0
   .reset           : > RESET,     PAGE = 0, TYPE = DSECT /* not used, */

   .stack           : > RAMM0,     PAGE = 1
   .ebss            : > RAMGS0_S14,PAGE = 1
   .econst          : > RAMGS0_S14,  PAGE = 1
   .esysmem         : > RAMGS0_S14,  PAGE = 1

   Filter_RegsFile  : > RAMGS15,   PAGE = 1

   ramgs0           : > RAMGS15,   PAGE = 1
   ramgs1           : > RAMGS15,   PAGE = 1

#ifdef __TI_COMPILER_VERSION__
   #if __TI_COMPILER_VERSION__ >= 15009000
    .TI.ramfunc : {} > RAMLS0_S5,  PAGE = 0
   #else
    ramfuncs    : > RAMLS0_S5      PAGE = 0
   #endif
#endif

   /* The following section definitions are required when using the IPC API Drivers */
    GROUP : > CPU1TOCPU2RAM, PAGE = 1
    {
        PUTBUFFER
        PUTWRITEIDX
        GETREADIDX
    }

    GROUP : > CPU2TOCPU1RAM, PAGE = 1
    {
        GETBUFFER :    TYPE = DSECT
        GETWRITEIDX :  TYPE = DSECT
        PUTREADIDX :   TYPE = DSECT
    }

    /* The following section definition are for SDFM examples */
   Filter1_RegsFile : > RAMGS15,	PAGE = 1, fill = 0x1111
   Filter2_RegsFile : > RAMGS15,	PAGE = 1, fill = 0x2222
   Filter3_RegsFile : > RAMGS15,	PAGE = 1, fill = 0x3333
   Filter4_RegsFile : > RAMGS15,	PAGE = 1, fill = 0x4444
   Difference_RegsFile : >RAMGS15,  PAGE = 1, fill = 0x3333
}

/*
//===========================================================================
// End of file.
//===========================================================================
*/
