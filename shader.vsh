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
;;                                 ;;
;; r0 is vertex after 1st bone     ;;
;; r1 is vertex after 2nd bone     ;;
;; r2 is transformed vertex        ;;
;; r3 is diffuse coefficient       ;;
;; r4 is light intensity           ;;
;; r5 is cos(theta)                ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

m4x4 r0, v0, c4
mul r0.xyz, r0.xyz, v2.x
m4x4 r1, v0, c8
mad r1.xyz, r1.xyz, v2.y, r0.xyz

mov r3, c14
mul r4.x, c12.x, r3.x
dp4 r5, c12, v3
mul r4.x, r4.x, r5.x
mul r4, c13, r4.x

m4x4 oPos, r1, c0
mul oD0, v1, r4
;mov oD0, v1
