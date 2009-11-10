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
;; c19 is specular coefficient      ;;
;; c20 is specular constant 'f'     ;;
;; c21 is eye position              ;;
;;                                  ;;
;; c100 is constant 0.0f            ;;
;;                                  ;;
;; r0  is vertex after 1st bone     ;;
; !r1  is vertex after 2nd bone     ;;
; ?r2  is r (for specular)          ;;
; ?r3  is diff/spec coefficient     ;;
;; r4  is light intensity           ;;
; !r5  is cos(theta)                ;;
; !r6  is result color              ;;
;; r7  is temp                      ;;
;; r8  is cos(phi) (specular)       ;;
;; r9  is normal after 1st bone     ;;
; !r10 is normal after 2nd bone     ;;
;; r11 is distance vector           ;;
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
mad r10, r10, v2.y, r9.xyz
dp3 r2, r10, r10        ; r2 = |normal|**2
rsq r7, r2              ; r7 = 1/|normal|
mul r10, r10, r7.x      ; normalize r10

;;;;;;;;;;;;;;;;;;;;; Directional ;;;;;;;;;;;;;;;;;;;;;;;;;;;
dp3 r5, c12, r10        ; r5 = cos(theta)
; - - - - - - - - - - - diffuse - - - - - - - - - - - - - - ;
mov r3, c14             ; r3 = coef(diffuse)
mul r4, c13, r3.x       ; r4 = I(direct)*coef(diffuse)
mul r4, r4, r5.x        ; r4 *= cos(theta)

max r6, r4, c100        ; if some color comp. < 0 => make it == 0
; - - - - - - - - - - - specular - - - - - - - - - - - - - -;
mov r3, c19             ; r3 = coef(specular)
; calculating r:
mul r2, r10, r5.x   ; r2 = (l, n)*n
add r2, r2, r2      ; r2 = 2*(l, n)*n
add r2, r2, -c12    ; r2 = 2*(l, n)*n - l
; calculating normalized v
add r11, c21, -r1       ; r11 = position(eye) - position(vertex)
dp3 r0, r11, r11        ; r0 = distance**2
rsq r7, r0              ; r7 = 1/distance
mul r11, r11, r7.x      ; normalize r11
; calculating cos(phi)**f
dp3 r8, r2, r11         ; r8 = cos(phi)
max r8, r8, c100        ; if cos < 0, let it = 0
mov r7.y, r8.x
mov r7.w, c20.x
lit r8, r7              ; r8.z = cos(phi)**f

mul r4, c13, r3.x       ; r4 = I(direct)*coef(specular)
mul r4, r4, r8.z        ; r4 *= cos(theta)

max r4, r4, c100        ; if some color comp. < 0 => make it == 0
add r6, r6, r4

;;;;;;;;;;;;;;;;;;;;;;;; Point ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
add r11, c17, -r1       ; r11 = position(point) - position(vertex)
dp3 r2, r11, r11        ; r2 = distance**2
rsq r7, r2              ; r7 = 1/distance
mul r11, r11, r7.x      ; normalize r11
dp3 r5, r11, r10        ; r5 = cos(theta)
; - - - - - - - - - - - diffuse - - - - - - - - - - - - - - ;
mov r3, c14             ; r3 = coef(diffuse)
mul r4, c16, r3.x       ; r4 = I(point)*coef(diffuse)
mul r4, r4, r5.x        ; r4 *= cos(theta)
dst r2, r2, r7          ; r2 = (1, d, d**2, 1/d)
dp4 r7.x, r2, c18       ; r7.x = (a + b*d + c*d**2 + e/d)
rcp r7.x, r7.x          ; r7.x = attenuation coef
mul r4, r4, r7.x

max r4, r4, c100        ; if some color comp. < 0 => make it == 0
add r6, r6, r4

;;;;;;;;;;;;;;;;;;;;;;; Ambient ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
add r6, r6, c15         ; r6 += I(ambient)

;;;;;;;;;;;;;;;;;;;;;;;; Results ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
m4x4 oPos, r1, c0
mul oD0, v1, r6
