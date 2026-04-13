
MEMORY
{
PAGE 0 :  /* Program Memory */
          /* Memory (RAM/FLASH) blocks can be moved to PAGE1 for data allocation */
          /* BEGIN is used for the "boot to Flash" bootloader mode   */

   BEGIN           	: origin = 0x084000, length = 0x000002
   RAMM0           	: origin = 0x000123, length = 0x0002DD
   RAMD0           	: origin = 0x00B000, length = 0x000800

   RAMLS          	: origin = 0x008000, length = 0x002800


   RESET           	: origin = 0x3FFFC0, length = 0x000002

   /* Flash sectors */
   FLASH_APP      	: origin = 0x084002, length = 0x03BFEE	/* on-chip Flash */

PAGE 1 :

   BOOT_RSVD       : origin = 0x000002, length = 0x000121
   RAMM1           : origin = 0x000400, length = 0x0003F8

   RAMD1           : origin = 0x00B800, length = 0x000800

   RAMLS5      : origin = 0x00A800, length = 0x000800

   RAMGS       : origin = 0x00C000, length = 0x00B000

   CPU2TOCPU1RAM   : origin = 0x03F800, length = 0x000400
   CPU1TOCPU2RAM   : origin = 0x03FC00, length = 0x000400
}

SECTIONS
{
   /* Allocate program areas: */
   .cinit              : > 	FLASH_APP      			PAGE = 0, ALIGN(8)
   .text               : >> FLASH_APP      			PAGE = 0, ALIGN(8)
   codestart           : > BEGIN       				PAGE = 0, ALIGN(8)
   /* Allocate uninitalized data sections: */
   .stack              : > RAMM1       				PAGE = 1
   .switch             : > FLASH_APP      			PAGE = 0, ALIGN(8)
   .reset              : > RESET,      				PAGE = 0, TYPE = DSECT /* not used, */

#if defined(__TI_EABI__)
   .init_array         : > FLASHB,       			PAGE = 0,       ALIGN(8)
   .bss                : > RAMLS5,       			PAGE = 1
   .bss:output         : > RAMLS3,       			PAGE = 0
   .bss:cio            : > RAMLS5,       			PAGE = 1
   .data               : > RAMLS5,       			PAGE = 1
   .sysmem             : > RAMLS5,       			PAGE = 1
   /* Initalized sections go in Flash */
   .const              : > FLASHF,       			PAGE = 0,       ALIGN(8)
#else
   .pinit              : > FLASH_APP,       		PAGE = 0,       ALIGN(8)
   .ebss               : >> RAMGS,    				PAGE = 1
   .esysmem            : > RAMLS5,       			PAGE = 1
   .cio                : > RAMLS5,       			PAGE = 1
   /* Initalized sections go in Flash */
   .econst             : >> FLASH_APP      			PAGE = 0, ALIGN(8)
#endif

   Filter_RegsFile     : > RAMGS,	   				PAGE = 1

   SHARERAMGS0		: > RAMGS,						PAGE = 1
   SHARERAMGS1		: > RAMGS,						PAGE = 1
   SHARERAMGS2		: > RAMGS,						PAGE = 1
   ramgs0           : > RAMGS,     					PAGE = 1
   ramgs1           : > RAMGS,     					PAGE = 1
   w5500_ramgs		: > RAMGS,						PAGE = 1
   flash_ramgs		: > RAMGS, 						ALIGN(8) PAGE = 1

#ifdef __TI_COMPILER_VERSION__
    #if __TI_COMPILER_VERSION__ >= 15009000
        #if defined(__TI_EABI__)
            .TI.ramfunc : {} LOAD = FLASHD,
                                 RUN = RAMLS0,
                                 LOAD_START(RamfuncsLoadStart),
                                 LOAD_SIZE(RamfuncsLoadSize),
                                 LOAD_END(RamfuncsLoadEnd),
                                 RUN_START(RamfuncsRunStart),
                                 RUN_SIZE(RamfuncsRunSize),
                                 RUN_END(RamfuncsRunEnd),
                                 PAGE = 0, ALIGN(8)
        #else
            .TI.ramfunc : {} LOAD = FLASH_APP,
                             RUN = RAMLS,
                             LOAD_START(_RamfuncsLoadStart),
                             LOAD_SIZE(_RamfuncsLoadSize),
                             LOAD_END(_RamfuncsLoadEnd),
                             RUN_START(_RamfuncsRunStart),
                             RUN_SIZE(_RamfuncsRunSize),
                             RUN_END(_RamfuncsRunEnd),
                             PAGE = 0, ALIGN(8)
        #endif
    #else
   ramfuncs            : LOAD = FLASHD,
                         RUN = RAMLS0,
                         LOAD_START(_RamfuncsLoadStart),
                         LOAD_SIZE(_RamfuncsLoadSize),
                         LOAD_END(_RamfuncsLoadEnd),
                         RUN_START(_RamfuncsRunStart),
                         RUN_SIZE(_RamfuncsRunSize),
                         RUN_END(_RamfuncsRunEnd),
                         PAGE = 0, ALIGN(8)
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
   Filter1_RegsFile : > RAMGS,	PAGE = 1, fill=0x1111
   Filter2_RegsFile : > RAMGS,	PAGE = 1, fill=0x2222
   Filter3_RegsFile : > RAMGS,	PAGE = 1, fill=0x3333
   Filter4_RegsFile : > RAMGS,	PAGE = 1, fill=0x4444
   Difference_RegsFile : >RAMGS, 	PAGE = 1, fill=0x3333
}

/*
//===========================================================================
// End of file.
//===========================================================================
*/
