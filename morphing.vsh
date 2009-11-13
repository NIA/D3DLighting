vs_1_1
dcl_position v0
dcl_color v1
dcl_normal v3

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; c0 - c3 is view matrix           ;;
;; c27 - c30 is pos.*rot. matrix    ;;
;; c31 is FINAL_RADIUS              ;;
;; c32 is t                         ;;
;;                                  ;;
;; c111 is 1.0f                     ;;
;;                                  ;;
;; r0 is 1/current radius           ;;
;; r1 is a quotient                 ;;
;; r2 is a vertex after morphing    ;;
;; r3 is 1.0f in non-const mem      ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

def c111, 1.0, 1.0, 1.0, 1.0 ;constant one

dp3 r0, v0, v0
rsq r0, r0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  r1.x = 1+t*((FINAL_RADIUS/r0) -1)   ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

mov r3, c111            ; r3 = 1.0f
mad r1, r0, c31, -r3    ; r1 = ((FINAL_RADIUS/r0) -1)
mad r1, r1, c32, r3     ; r1 = 1+t*((FINAL_RADIUS/r0) -1)

mul r2.xyz, v0.xyz, r1.x
mov r2.w, c111.w

m4x4 r4, r2, c27        ; position and rotation
m4x4 oPos, r4, c0
mov oD0, v1
