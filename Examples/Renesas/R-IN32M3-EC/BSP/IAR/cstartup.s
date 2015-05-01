
    SECTION  text:CODE:ROOT(2)

    PUBLIC  __iar_program_start

    IMPORT  __vector_table
    IMPORT  ?main


__iar_program_start:
    LDR  r0, =__vector_table
    LDR  r1, =0xE000ED08
    STR  r0, [R1]
    LDR  r0,=?main
    BX   r0

    END
