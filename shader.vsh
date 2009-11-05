vs_1_1
dcl_position v0
dcl_color v1
dcl_texcoord v2
dcl_normal v3

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; c0 - c3 is view matrix          ;;
;; c4 - c7 is 1st bone matrix      ;;
;; c8 - c11 is 2nd bone matrix     ;;
;; c12 is directional light vector ;;
;; c13 is directional light color  ;;
;; c14 is diffuse coefficient      ;;
;; c15 is ambient color            ;;
;;                                 ;;
;; c100 is constant 0.0f           ;;
;;                                 ;;
;; r0 is vertex after 1st bone     ;;
;; r1 is vertex after 2nd bone     ;;
;; r2 is transformed vertex        ;;
;; r3 is diffuse coefficient       ;;
;; r4 is light intensity           ;;
;; r5 is cos(theta)                ;;
;; r6 is result color              ;;
;; r7 is (color > 0)?              ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

def c100, 0.0, 0.0, 0.0, 0.0

m4x4 r0, v0, c4
mul r0.xyz, r0.xyz, v2.x
m4x4 r1, v0, c8
mad r1.xyz, r1.xyz, v2.y, r0.xyz

mov r3, c14
dp4 r5, c12, v3         ; r5 = cos(theta)
mul r4, c13, r3.x       ; r4 = I(direct)*coef(diffuse)
mul r4, r4, r5.x        ; r4 *= cos(theta)

sge r7.xyz, r4.xyz, c100.xyz    ; if some color comp. < 0
mul r4.xyz, r4.xyz, r7.xyz      ; => make it == 0

add r4, r4, c15         ; r4 += I(ambient)

m4x4 oPos, r1, c0
mul oD0, v1, r4
;mov oD0, v1
