   ;
   ; #defines for NASM
   ;

;//////////////////////////////////////////////////////////////////////////
;           WIN32 macros
;//////////////////////////////////////////////////////////////////////////

%macro extrn 1
  extern _%1
  %define %1 _%1
%endmacro
%macro globl 1
  global _%1
  %define %1 _%1
%endmacro

%macro DATA 0
[section .data align=8 write alloc USE32]
%endmacro
%macro TEXT 0
[section .text align=16 nowrite alloc exec USE32]
%endmacro

;//////////////////////////////////////////////////////////////////////////

%macro align4 0
times ($$-$) & 7 nop    ; pad with NOPs to 8-bytes boundary
%endmacro
%macro align2 0
times ($$-$) & 1 nop
%endmacro

%macro Enter 0
   push ebp
   push edi
   push esi
   push ebx
   push eax
   push ecx
   push edx
%endmacro
%macro Leave 0
   pop edx
   pop ecx
   pop eax
   pop ebx
   pop esi
   pop edi
   pop ebp
   ret
%endmacro

%define OFF_STCK	   32

;//////////////////////////////////////////////////////////////////////////

;	OFFSET des champs dans les structures MESH, BLOB...

%define OFF_VTX       32
%define OFF_NB_VTX    52
%define OFF_POLYS     44
%define OFF_NB_POLYS  56
%define OFF_UV        40
%define OFF_NORMALS   36
%define OFF_CST       106
%define POLY_NO       16
%define POLY_NDC      44
%define OFF_FLAGS     20

%define OFF_K         8
%define OFF_Gn        12
%define OFF_INV_RAD2  172
%define OFF_RAD       140
%define OFF_RAD2      204

%define SIZEOF_POLY   64
%define SIZEOF_CST    44
%define SIZEOF_RAY    96

%define RAY_ORIG    0
%define RAY_DIR     12
%define RAY_Z       40
%define RAY_ZMIN    44
%define RAY_ID      56
%define RAY_X       88
%define RAY_Y       92
%define CST_U2      0
%define CST_V2      4
%define CST_UV      8
%define CST_X0      12
%define CST_Y0      16
%define CST_POP1    20
%define CST_POP2    32

%define CLIPS        16
%define BASE_PTR     84
%define PIX_WIDTH    72
%define PIX_HEIGHT   76
%define PIX_BPS      80
%define Z_SCALE      300
%define Z_OFF        304

%define VB_W    4
%define VB_H    8
%define VB_BPS  12
%define VB_BITS 24
%define VB_CMAP 32

;//////////////////////////////////////////////////////////////////////////
;//////////////////////////////////////////////////////////////////////////
