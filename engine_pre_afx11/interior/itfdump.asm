;-----------------------------------------------------------------------------
; Torque Game Engine
; Copyright (C) GarageGames.com, Inc.
;-----------------------------------------------------------------------------

segment .data

storeebp dd 0

srcPoints  dd 0
srcColors  dd 0
srcIndices dd 0
numPoints  dd 0
two55	   dd 0x437F0000
alpha	   dd 0

%ifdef LINUX
; No underscore needed for ELF object files
%define _texGen0 texGen0
%define _texGen1 texGen1
%define _fogCoordinatePointer fogCoordinatePointer
%endif
extern _texGen0
extern _texGen1
extern _fogCoordinatePointer


segment .text

;
; these macros are good for both functions
;

%define in_dst         [ebp+8]
%define in_src_points  [ebp+12]
%define in_src_indices [ebp+16]
%define in_numpoints   [ebp+20]

%define in_srcColors   [ebp+24] ; Valid only for SP

; CodeWarrior sucks :P
%ifdef LINUX
global processTriFan

processTriFan:
%else
global _processTriFan

_processTriFan:
%endif

    ; prologue
    push ebp
    mov ebp, esp
	push eax
	push ebx
	push ecx
	push edi
	push esi

    ; Store the destination and source pointers
    mov     eax, in_src_points
    mov     [srcPoints], eax
    mov     eax, in_src_indices
    mov     [srcIndices], eax
    mov     eax, in_numpoints
    mov     [numPoints], eax

    mov     edi, in_dst

    mov [storeebp], ebp
    xor     ebp, ebp
procPointLp1:
    ; This could be faster
    mov     esi, [srcIndices]
    lea     esi, [esi + ebp*4]
    mov     eax, dword [esi]
    shl     eax, 4                  ; idx *= 16
    mov     esi, [srcPoints]
    lea     esi, [esi + eax]

    mov     eax, [esi + 0]              ; x
    mov     ebx, [esi + 4]              ; y
    mov     ecx, [esi + 8]              ; z
    mov     edx, [esi + 12]             ; f
    mov     [edi + 0],  eax             ; <- x
    mov     [edi + 4],  ebx             ; <- y
    mov     [edi + 8],  ecx             ; <- z
    mov     [edi + 12], edx             ; <- f

    ; tc0.s
    fld     dword [_texGen0 + 0]   ; tg0.s.x   
    fmul    dword [esi + 0]
    fld     dword [_texGen0 + 4]   ; tg0.s.y
    fmul    dword [esi + 4]
    fld     dword [_texGen0 + 8]   ; tg0.s.z
    fmul    dword [esi + 8]
    fld     dword [_texGen0 + 12]  ; tg0.s.w
    faddp   st3, st0
    faddp   st1, st0
    faddp   st1, st0
    fstp    dword [edi + 16]    ; tc0.s

    ; tc0.t
    fld     dword [_texGen0 + 16]   ; tg0.t.x   
    fmul    dword [esi + 0]
    fld     dword [_texGen0 + 20]   ; tg0.t.y
    fmul    dword [esi + 4]
    fld     dword [_texGen0 + 24]   ; tg0.t.z
    fmul    dword [esi + 8]
    fld     dword [_texGen0 + 28]  ; tg0.t.w
    faddp   st3, st0
    faddp   st1, st0
    faddp   st1, st0
    fstp    dword [edi + 20]    ; tc0.t

    ; tc1.s
    fld     dword [_texGen1 + 0]   ; tg1.s.x   
    fmul    dword [esi + 0]
    fld     dword [_texGen1 + 4]   ; tg1.s.y
    fmul    dword [esi + 4]
    fld     dword [_texGen1 + 8]   ; tg1.s.z
    fmul    dword [esi + 8]
    fld     dword [_texGen1 + 12]  ; tg1.s.w
    faddp   st3, st0
    faddp   st1, st0
    faddp   st1, st0
    fstp    dword [edi + 24]    ; tc1.s

    ; tc1.t
    fld     dword [_texGen1 + 16]   ; tg1.t.x   
    fmul    dword [esi + 0]
    fld     dword [_texGen1 + 20]   ; tg1.t.y
    fmul    dword [esi + 4]
    fld     dword [_texGen1 + 24]   ; tg1.t.z
    fmul    dword [esi + 8]
    fld     dword [_texGen1 + 28]   ; tg1.t.w
    faddp   st3, st0
    faddp   st1, st0
    faddp   st1, st0
    fstp    dword [edi + 28]    ; tc1.t

    add     edi, 32

    inc     ebp
    cmp     ebp, [numPoints]
    jl      near procPointLp1

    mov ebp, [storeebp]

    ; epilogue
	pop esi
	pop edi
	pop ecx
	pop ebx
	pop eax
    pop ebp
    ret


; More suckage
%ifdef LINUX
global processTriFanSP

processTriFanSP:
%else
global _processTriFanSP

_processTriFanSP:
%endif

    ; prologue
    push ebp
    mov ebp, esp
	push eax
	push ebx
	push ecx
	push edi
	push esi

    ; Store the destination and source pointers
    mov     eax, in_src_points
    mov     [srcPoints], eax
    mov     eax, in_src_indices
    mov     [srcIndices], eax
    mov     eax, in_numpoints
    mov     [numPoints], eax
    mov     eax, in_srcColors
    mov     [srcColors], eax
        
    mov     edi, in_dst

    mov [storeebp], ebp
    xor     ebp, ebp
procPointLp2:
    ; This could be faster
    mov     esi, [srcIndices]
    lea     esi, [esi + ebp*4]
    mov     eax, dword [esi]
    shl     eax, 4                  ; idx *= 16
    mov     esi, [srcPoints]
    lea     esi, [esi + eax]

    mov     eax, [esi + 0]              ; x
    mov     ebx, [esi + 4]              ; y
    mov     ecx, [esi + 8]              ; z
    mov     edx, [srcColors]                        ; color
    mov     [edi + 0],  eax             ; <- x
    lea     edx, [edx + ebp*4]                      ; color
    mov     [edi + 4],  ebx             ; <- y
    mov     edx, [edx]                              ; color
    mov     [edi + 8],  ecx             ; <- z
    mov     [edi + 12], edx                         ; color
        
    ; tc0.s
    fld     dword [_texGen0 + 0]   ; tg0.s.x   
    fmul    dword [esi + 0]
    fld     dword [_texGen0 + 4]   ; tg0.s.y
    fmul    dword [esi + 4]
    fld     dword [_texGen0 + 8]   ; tg0.s.z
    fmul    dword [esi + 8]
    fld     dword [_texGen0 + 12]  ; tg0.s.w
    faddp   st3, st0
    faddp   st1, st0
    faddp   st1, st0
    fstp    dword [edi + 16]    ; tc0.s

    ; tc0.t
    fld     dword [_texGen0 + 16]   ; tg0.t.x   
    fmul    dword [esi + 0]
    fld     dword [_texGen0 + 20]   ; tg0.t.y
    fmul    dword [esi + 4]
    fld     dword [_texGen0 + 24]   ; tg0.t.z
    fmul    dword [esi + 8]
    fld     dword [_texGen0 + 28]  ; tg0.t.w
    faddp   st3, st0
    faddp   st1, st0
    faddp   st1, st0
    fstp    dword [edi + 20]    ; tc0.t

    add     edi, 32

    inc     ebp
    cmp     ebp, [numPoints]
    jl      near procPointLp2

    mov ebp, [storeebp]

    ; epilogue
	pop esi
	pop edi
	pop ecx
	pop ebx
	pop eax
    pop ebp
    ret


; More suckage
%ifdef LINUX
global processTriFanVC_TF

processTriFanVC_TF:
%else
global _processTriFanVC_TF

_processTriFanVC_TF:
%endif

    ; prologue
    push ebp
    mov ebp, esp
	push eax
	push ebx
	push ecx
	push edi
	push esi

    ; Store the destination and source pointers
    mov     eax, in_src_points
    mov     [srcPoints], eax
    mov     eax, in_src_indices
    mov     [srcIndices], eax
    mov     eax, in_numpoints
    mov     [numPoints], eax
    mov     eax, in_srcColors
    mov     [srcColors], eax
        
    mov     edi, in_dst

    mov [storeebp], ebp
    xor     ebp, ebp
procPointLp4:
    ; This could be faster
    mov     esi, [srcIndices]
    lea     esi, [esi + ebp*4]
    mov     eax, dword [esi]
    shl     eax, 4                  ; idx *= 16
    mov     esi, [srcPoints]
    lea     esi, [esi + eax]

    ; Fog tex coord
    mov     ebx, [_fogCoordinatePointer]
    shr     eax, 1                  ; idx /= 2
    lea     ebx, [ebx + eax]
    mov     ecx, [ebx + 0];
    mov     edx, [ebx + 4];
    mov     [edi + 16], ecx
    mov     [edi + 20], edx

    mov     eax, [esi + 0]              ; x
    mov     ebx, [esi + 4]              ; y
    mov     ecx, [esi + 8]              ; z
    mov     edx, [srcColors]                        ; color
    mov     [edi + 0],  eax             ; <- x
    lea     edx, [edx + ebp*4]                      ; color
    mov     [edi + 4],  ebx             ; <- y
    mov     edx, [edx]                              ; color
    mov     [edi + 8],  ecx             ; <- z
    mov     [edi + 12], edx                         ; color


    ; tc0.s
    fld     dword [_texGen0 + 0]   ; tg0.s.x   
    fmul    dword [esi + 0]
    fld     dword [_texGen0 + 4]   ; tg0.s.y
    fmul    dword [esi + 4]
    fld     dword [_texGen0 + 8]   ; tg0.s.z
    fmul    dword [esi + 8]
    fld     dword [_texGen0 + 12]  ; tg0.s.w
    faddp   st3, st0
    faddp   st1, st0
    faddp   st1, st0
    fstp    dword [edi + 24]    ; tc0.s

    ; tc0.t
    fld     dword [_texGen0 + 16]   ; tg0.t.x   
    fmul    dword [esi + 0]
    fld     dword [_texGen0 + 20]   ; tg0.t.y
    fmul    dword [esi + 4]
    fld     dword [_texGen0 + 24]   ; tg0.t.z
    fmul    dword [esi + 8]
    fld     dword [_texGen0 + 28]  ; tg0.t.w
    faddp   st3, st0
    faddp   st1, st0
    faddp   st1, st0
    fstp    dword [edi + 28]    ; tc0.t

    add     edi, 32

    inc     ebp
    cmp     ebp, [numPoints]
    jl      near procPointLp4

    mov ebp, [storeebp]

    ; epilogue
	pop esi
	pop edi
	pop ecx
	pop ebx
	pop eax
    pop ebp
    pop ebp
    ret


; More suckagea
%ifdef LINUX
global processTriFanSP_FC

processTriFanSP_FC:
%else
global _processTriFanSP_FC

_processTriFanSP_FC:
%endif

    ; prologue
    push ebp
    mov ebp, esp
	push eax
	push ebx
	push ecx
	push edi
	push esi

    ; Store the destination and source pointers
    mov     eax, in_src_points
    mov     [srcPoints], eax
    mov     eax, in_src_indices
    mov     [srcIndices], eax
    mov     eax, in_numpoints
    mov     [numPoints], eax
    mov     eax, in_srcColors
    mov     [srcColors], eax
        
    mov     edi, in_dst

    mov [storeebp], ebp
    xor     ebp, ebp
procPointLp2_fc:
    ; This could be faster
    mov     esi, [srcIndices]
    lea     esi, [esi + ebp*4]
    mov     eax, dword [esi]
    shl     eax, 4                  ; idx *= 16
    mov     esi, [srcPoints]
    lea     esi, [esi + eax]

    mov     eax, [esi + 0]              ; x
    mov     ebx, [esi + 4]              ; y
    mov     ecx, [esi + 8]              ; z
    mov     edx, [esi + 12]             ; fc
    mov     [edi + 0],  eax             ; <- x
    mov     [edi + 4],  ebx             ; <- y
    mov     [edi + 8],  ecx             ; <- z
    mov     [edi + 24], edx             ; <- fc (lmcoord.x)
    mov     edx, [srcColors]            ; color
    lea     edx, [edx + ebp*4]          ; color
    mov     edx, [edx]                  ; color
    mov     [edi + 12], edx             ; color
        
    ; tc0.s
    fld     dword [_texGen0 + 0]   ; tg0.s.x   
    fmul    dword [esi + 0]
    fld     dword [_texGen0 + 4]   ; tg0.s.y
    fmul    dword [esi + 4]
    fld     dword [_texGen0 + 8]   ; tg0.s.z
    fmul    dword [esi + 8]
    fld     dword [_texGen0 + 12]  ; tg0.s.w
    faddp   st3, st0
    faddp   st1, st0
    faddp   st1, st0
    fstp    dword [edi + 16]    ; tc0.s

    ; tc0.t
    fld     dword [_texGen0 + 16]   ; tg0.t.x   
    fmul    dword [esi + 0]
    fld     dword [_texGen0 + 20]   ; tg0.t.y
    fmul    dword [esi + 4]
    fld     dword [_texGen0 + 24]   ; tg0.t.z
    fmul    dword [esi + 8]
    fld     dword [_texGen0 + 28]  ; tg0.t.w
    faddp   st3, st0
    faddp   st1, st0
    faddp   st1, st0
    fstp    dword [edi + 20]    ; tc0.t

    add     edi, 32

    inc     ebp
    cmp     ebp, [numPoints]
    jl      near procPointLp2_fc

    mov ebp, [storeebp]

    ; epilogue
	pop esi
	pop edi
	pop ecx
	pop ebx
	pop eax
    pop ebp
    ret


; CodeWarrior still sucks :P
%ifdef LINUX
global processTriFanFC_VB

processTriFanFC_VB:
%else
global _processTriFanFC_VB

_processTriFanFC_VB:
%endif

    ; prologue
    push ebp
    mov ebp, esp
	push eax
	push ebx
	push ecx
	push edi
	push esi

    ; Store the destination and source pointers
    mov     eax, in_src_points
    mov     [srcPoints], eax
    mov     eax, in_src_indices
    mov     [srcIndices], eax
    mov     eax, in_numpoints
    mov     [numPoints], eax

    mov     edi, in_dst

    mov [storeebp], ebp
    xor     ebp, ebp
procPointLp1_fc_vb:
    ; This could be faster
    mov     esi, [srcIndices]
    lea     esi, [esi + ebp*4]
    mov     eax, dword [esi]
    shl     eax, 4                  ; idx *= 16
    mov     esi, [srcPoints]
    lea     esi, [esi + eax]

    mov     eax, [esi + 0]              ; x
    mov     ebx, [esi + 4]              ; y
    mov     ecx, [esi + 8]              ; z
    mov		edx, 0xFFFFFFFF				; c
    mov     [edi + 0],  eax             ; <- x
    mov     [edi + 4],  ebx             ; <- y
    mov     [edi + 8],  ecx             ; <- z
    mov		[edi + 12], edx				; <- c

    fld		dword [esi + 12]
	fld		dword [two55]
	fmulp	st1, st0
	fistp	dword [alpha]
	mov		eax, 255
	sub		eax, [alpha]
	cmp		eax, 0
	jge		near procPointLp1a_fc_vb
	mov		eax, 0
procPointLp1a_fc_vb:
	shl		eax, 24
    mov     [edi + 16], eax             ; <- f

    ; tc0.s
    fld     dword [_texGen0 + 0]   ; tg0.s.x   
    fmul    dword [esi + 0]
    fld     dword [_texGen0 + 4]   ; tg0.s.y
    fmul    dword [esi + 4]
    fld     dword [_texGen0 + 8]   ; tg0.s.z
    fmul    dword [esi + 8]
    fld     dword [_texGen0 + 12]  ; tg0.s.w
    faddp   st3, st0
    faddp   st1, st0
    faddp   st1, st0
    fstp    dword [edi + 28]    ; tc0.s

    ; tc0.t
    fld     dword [_texGen0 + 16]   ; tg0.t.x   
    fmul    dword [esi + 0]
    fld     dword [_texGen0 + 20]   ; tg0.t.y
    fmul    dword [esi + 4]
    fld     dword [_texGen0 + 24]   ; tg0.t.z
    fmul    dword [esi + 8]
    fld     dword [_texGen0 + 28]  ; tg0.t.w
    faddp   st3, st0
    faddp   st1, st0
    faddp   st1, st0
    fstp    dword [edi + 32]    ; tc0.t

    ; tc1.s
    fld     dword [_texGen1 + 0]   ; tg1.s.x   
    fmul    dword [esi + 0]
    fld     dword [_texGen1 + 4]   ; tg1.s.y
    fmul    dword [esi + 4]
    fld     dword [_texGen1 + 8]   ; tg1.s.z
    fmul    dword [esi + 8]
    fld     dword [_texGen1 + 12]  ; tg1.s.w
    faddp   st3, st0
    faddp   st1, st0
    faddp   st1, st0
    fstp    dword [edi + 20]    ; tc1.s

    ; tc1.t
    fld     dword [_texGen1 + 16]   ; tg1.t.x   
    fmul    dword [esi + 0]
    fld     dword [_texGen1 + 20]   ; tg1.t.y
    fmul    dword [esi + 4]
    fld     dword [_texGen1 + 24]   ; tg1.t.z
    fmul    dword [esi + 8]
    fld     dword [_texGen1 + 28]   ; tg1.t.w
    faddp   st3, st0
    faddp   st1, st0
    faddp   st1, st0
    fstp    dword [edi + 24]    ; tc1.t

    add     edi, 36

    inc     ebp
    cmp     ebp, [numPoints]
    jl      near procPointLp1_fc_vb

    mov ebp, [storeebp]

    ; epilogue
	pop esi
	pop edi
	pop ecx
	pop ebx
	pop eax
    pop ebp
    ret


; More suckagea
%ifdef LINUX
global processTriFanSP_FC_VB

processTriFanSP_FC_VB:
%else
global _processTriFanSP_FC_VB

_processTriFanSP_FC_VB:
%endif

    ; prologue
    push ebp
    mov ebp, esp
	push eax
	push ebx
	push ecx
	push edi
	push esi

    ; Store the destination and source pointers
    mov     eax, in_src_points
    mov     [srcPoints], eax
    mov     eax, in_src_indices
    mov     [srcIndices], eax
    mov     eax, in_numpoints
    mov     [numPoints], eax
    mov     eax, in_srcColors
    mov     [srcColors], eax
        
    mov     edi, in_dst

    mov [storeebp], ebp
    xor     ebp, ebp
procPointLp2_fc_vb:
    ; This could be faster
    mov     esi, [srcIndices]
    lea     esi, [esi + ebp*4]
    mov     eax, dword [esi]
    shl     eax, 4                  ; idx *= 16
    mov     esi, [srcPoints]
    lea     esi, [esi + eax]

    mov     eax, [esi + 0]              ; x
    mov     ebx, [esi + 4]              ; y
    mov     ecx, [esi + 8]              ; z
    mov     [edi + 0],  eax             ; <- x
    mov     [edi + 4],  ebx             ; <- y
    mov     [edi + 8],  ecx             ; <- z

	fld		dword [esi + 12]
	fld		dword [two55]
	fmulp	st1, st0
	fistp	dword [alpha]
	mov		eax, 255
	sub		eax, [alpha]
	cmp		eax, 0
	jge		near procPointLp2a_fc_vb
	mov		eax, 0
procPointLp2a_fc_vb:
	shl		eax, 24
    mov     [edi + 16], eax             ; <- fc

    mov     edx, [srcColors]            ; color
    lea     edx, [edx + ebp*4]          ; color
    mov     edx, [edx]                  ; color
	mov		eax, edx
	mov		ebx, 0x00FF00FF
	and		edx, ebx
	not		ebx
	rol		edx, 16
	and		eax, ebx
	or		edx, eax
    mov     [edi + 12], edx             ; color
        
    ; tc0.s
    fld     dword [_texGen0 + 0]   ; tg0.s.x   
    fmul    dword [esi + 0]
    fld     dword [_texGen0 + 4]   ; tg0.s.y
    fmul    dword [esi + 4]
    fld     dword [_texGen0 + 8]   ; tg0.s.z
    fmul    dword [esi + 8]
    fld     dword [_texGen0 + 12]  ; tg0.s.w
    faddp   st3, st0
    faddp   st1, st0
    faddp   st1, st0
    fstp    dword [edi + 20]    ; tc0.s

    ; tc0.t
    fld     dword [_texGen0 + 16]   ; tg0.t.x   
    fmul    dword [esi + 0]
    fld     dword [_texGen0 + 20]   ; tg0.t.y
    fmul    dword [esi + 4]
    fld     dword [_texGen0 + 24]   ; tg0.t.z
    fmul    dword [esi + 8]
    fld     dword [_texGen0 + 28]  ; tg0.t.w
    faddp   st3, st0
    faddp   st1, st0
    faddp   st1, st0
    fstp    dword [edi + 24]    ; tc0.t

    add     edi, 28

    inc     ebp
    cmp     ebp, [numPoints]
    jl      near procPointLp2_fc_vb

    mov ebp, [storeebp]

    ; epilogue
	pop esi
	pop edi
	pop ecx
	pop ebx
	pop eax
    pop ebp
    ret
