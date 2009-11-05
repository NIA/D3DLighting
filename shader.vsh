vs_1_1
dcl_position v0
dcl_color v1
dcl_texcoord v2
dcl_normal v3

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; c0 - c3 is view matrix           ;;
;; c4 - c7 is 1st bone matrix       ;;
;; c8 - c11 is 2nd bone matrix      ;;
;; c12 is directional light vector  ;;
;; c13 is directional light color   ;;
;; c14 is diffuse coefficient       ;;
;; c15 is ambient light color       ;;
;; c16 is point light color         ;;
;; c17 is point light position      ;;
;; c18 are attenuation constants    ;;
;;                                  ;;
;; c100 is constant 0.0f            ;;
;;                                  ;;
;; r0  is vertex after 1st bone     ;;
; !r1  is vertex after 2nd bone     ;;
;; r3  is diffuse coefficient       ;;
;; r4  is light intensity           ;;
;; r5  is cos(theta)                ;;
;; r6  is result color              ;;
;; r7  is (color > 0)?              ;;
;; r9  is vertex after 1st bone     ;;
; !r10 is vertex after 2nd bone     ;;
;; r11 is distance vector           ;;
;; r2  is distance                  ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

def c100, 0.0, 0.0, 0.0, 0.0

;;;;;;;;;;;;;;;;;;;;;; Skinning ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; - - - - - - - - - -  position  - - - - - - - - - - - - - -;
m4x4 r0, v0, c4
mul r0.xyz, r0.xyz, v2.x
m4x4 r1, v0, c8
mad r1.xyz, r1.xyz, v2.y, r0.xyz
; - - - - - - - - - -  normals  - - - - - - - - - - - - - - ;
m4x4 r9, v3, c4
mul r9.xyz, r9.xyz, v2.x
m4x4 r10, v3, c8
mad r10.xyz, r10.xyz, v2.y, r9.xyz

;;;;;;;;;;;;;;;;;;;;; Directional ;;;;;;;;;;;;;;;;;;;;;;;;;;;
; - - - - - - - - - - - diffuse - - - - - - - - - - - - - - ;
mov r3, c14             ; r3 = coef(diffuse)
dp3 r5, c12, r10        ; r5 = cos(theta)
mul r4, c13, r3.x       ; r4 = I(direct)*coef(diffuse)
mul r4, r4, r5.x        ; r4 *= cos(theta)

sge r7, r4, c100    ; if some color comp. < 0
mul r6, r4, r7      ; => make it == 0

;;;;;;;;;;;;;;;;;;;;;;;; Point ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; - - - - - - - - - - - diffuse - - - - - - - - - - - - - - ;
add r11, c17, -r1       ; r11 = position(point) - position(vertex)
dp3 r2, r11, r11        ; r2 = distance**2
rsq r7, r2              ; r7 = distance
mul r11, r11, r7.x      ; normalize r11
dp3 r5, r11, r10        ; r5 = cos(theta)
mul r4, c16, r3.x       ; r4 = I(point)*coef(diffuse)
mul r4, r4, r5.x        ; r4 *= cos(theta)
rcp r2.x, r2.x
;mul r4, r4, r2.x

sge r7, r4, c100    ; if some color comp. < 0
mul r4, r4, r7      ; => make it == 0
add r6, r6, r4

;;;;;;;;;;;;;;;;;;;;;;; Ambient ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
add r6, r6, c15         ; r6 += I(ambient)

;;;;;;;;;;;;;;;;;;;;;;;; Results ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
m4x4 oPos, r1, c0
mul oD0, v1, r6
