
MEMORY
{
PAGE 0 :
   /* BEGIN is used for the "boot to SARAM" bootloader mode   */

   BEGIN            : origin = 0x000000, length = 0x000002
   RAMM0            : origin = 0x000123, length = 0x0002DD
   RAMD0            : origin = 0x00B000, length = 0x000800
   RAMLS            : origin = 0x008000, length = 0x002800
   RAMGSALL0   		: origin = 0x0157F8, length = 0x006800
   RESET            : origin = 0x3FFFC0, length = 0x000002

  /* Flash sectors */
   FLASHA           : origin = 0x080002, length = 0x001FFE	/* on-chip Flash */
   FLASHB           : origin = 0x082000, length = 0x002000	/* on-chip Flash */
   FLASHC           : origin = 0x084000, length = 0x002000	/* on-chip Flash */
   FLASHD           : origin = 0x086000, length = 0x002000	/* on-chip Flash */
   FLASHE           : origin = 0x088000, length = 0x008000	/* on-chip Flash */
   FLASHF           : origin = 0x090000, length = 0x008000	/* on-chip Flash */
   FLASHG           : origin = 0x098000, length = 0x008000	/* on-chip Flash */
   FLASHH           : origin = 0x0A0000, length = 0x008000	/* on-chip Flash */
   FLASHI           : origin = 0x0A8000, length = 0x008000	/* on-chip Flash */
   FLASHJ           : origin = 0x0B0000, length = 0x008000	/* on-chip Flash */
   FLASHK           : origin = 0x0B8000, length = 0x002000	/* on-chip Flash */
   FLASHL           : origin = 0x0BA000, length = 0x002000	/* on-chip Flash */
   FLASHM           : origin = 0x0BC000, length = 0x002000	/* on-chip Flash */
   FLASHN           : origin = 0x0BE000, length = 0x001FF0	/* on-chip Flash */

   //RAMGS10     : origin = 0x016000, length = 0x001000
//   FLASHN_RSVD     : origin = 0x0BFFF0, length = 0x000010    /* Reserve and do not use for code as per the errata advisory "Memory: Prefetching Beyond Valid Memory" */

PAGE 1 :

   BOOT_RSVD       : origin = 0x000002, length = 0x000121     /* Part of M0, BOOT rom will use this for stack */
   RAMM1           : origin = 0x000400, length = 0x0003F8     /* on-chip RAM block M1 */
//   RAMM1_RSVD      : origin = 0x0007F8, length = 0x000008     /* Reserve and do not use for code as per the errata advisory "Memory: Prefetching Beyond Valid Memory" */
   RAMD1           : origin = 0x00B800, length = 0x000800

   RAMLS5      : origin = 0x00A800, length = 0x000800

   RAMGSDMA    : origin = 0x00C000, length = 0x002FF8
   
   RAMGSALL    : origin = 0x00EFF8, length = 0x006800
//0x01A000
//   RAMGS15_RSVD : origin = 0x01BFF8, length = 0x000008    /* Reserve and do not use for code as per the errata advisory "Memory: Prefetching Beyond Valid Memory" */
                                                            /* Only on F28379D, F28377D, F28375D devices. Remove line on other devices. */

   CPU2TOCPU1RAM   : origin = 0x03F800, length = 0x000400
   CPU1TOCPU2RAM   : origin = 0x03FC00, length = 0x000400

   CANA_MSG_RAM     : origin = 0x049000, length = 0x000800
   CANB_MSG_RAM     : origin = 0x04B000, length = 0x000800
}


SECTIONS
{
   codestart        : > BEGIN,     PAGE = 0
   .text            : >> RAMD0 |  RAMLS | RAMGSALL0,   	PAGE = 0
   .cinit           : > RAMM0  |  RAMGSALL0,     		PAGE = 0
   .switch          : > RAMM0,     PAGE = 0
   .reset           : > RESET,     PAGE = 0, TYPE = DSECT /* not used, */
   .stack           : > RAMM1,     PAGE = 1

#if defined(__TI_EABI__)
   .bss             : > RAMLS5,    PAGE = 1
   .bss:output      : > RAMLS3,    PAGE = 0
   .init_array      : > RAMM0,     PAGE = 0
   .const           : > RAMLS5,    PAGE = 1
   .data            : > RAMLS5,    PAGE = 1
   .sysmem          : > RAMLS5,    PAGE = 1
#else
   .pinit           : > RAMM0,     PAGE = 0
   .ebss            : > RAMGSALL,    PAGE = 1
   .econst          : > RAMGSALL,    PAGE = 1
   .esysmem         : > RAMLS5,    PAGE = 1
#endif

   Filter_RegsFile  : > RAMGSDMA,  	PAGE = 1

   w5500_ramgs		: > RAMGSDMA,	ALIGN(8) PAGE = 1
   flash_ramgs		: > RAMGSDMA, 	ALIGN(8) PAGE = 1
   ramgs0           : > RAMGSDMA,	PAGE = 1
   ramgs1           : > RAMGSDMA,  	PAGE = 1

#ifdef __TI_COMPILER_VERSION__
   #if __TI_COMPILER_VERSION__ >= 15009000
    .TI.ramfunc : {} > RAMM0,      PAGE = 0
   #else
    ramfuncs    : > RAMM0      PAGE = 0   
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
   Filter1_RegsFile : > RAMGSALL	, PAGE = 1, fill=0x1111
   Filter2_RegsFile : > RAMGSALL	, PAGE = 1, fill=0x2222
   Filter3_RegsFile : > RAMGSALL	, PAGE = 1, fill=0x3333
   Filter4_RegsFile : > RAMGSALL	, PAGE = 1, fill=0x4444
   Difference_RegsFile : >RAMGSALL	, PAGE = 1, fill=0x3333
}

/*
//===========================================================================
// End of file.
//===========================================================================
*/
