;; I want to use fold, so I need srfi-1 (apparently)
;; (use-modules ((srfi srfi-1) #:select (fold)))

(define-module (gt-interface quat)
  #:use-module ((srfi srfi-1) #:select (fold))
  #:export (quat-init
	    quat-mult
	    quat-vec-rotate
	    quat-to-rotmatrix)
  #:duplicates (check))
  

(define pi 3.14159265358979323846264338327950288419716939937510)

(define vec-magnitude
  (lambda (vec)
    (sqrt (fold (lambda (x init) (+ (* x x) init))
		0
		vec))))

(define vec-normalize 
  (lambda (vec)
    (let ( (mag (vec-magnitude vec)) )
      (map (lambda (x) (/ x mag)) vec))))

;; (define fold (lambda (list func init)
;; (fold (lambda (element accum)) init vec)
;; (define fold
;;   ((lambda (x) (x x))
;;    (lambda (myself)
;;      (lambda (func init list)
;;        (if (equal? list '())
;; 	   init
;; 	   (let ((first (car list))
;; 	      (rest (cdr list)))
;; 	     ((myself myself) func (func first init) rest)))))))
  

;; (lambda (list func accum)
;;   (func (car list) accum)
;; (lambda (x accum) (+ (* x x) accum))
(define quat-null '(1 0 0 0))
(define quat-init (lambda (theta vec)
		    (let (
			  (c (cos (/ theta 2)))
			  (s (sin (/ theta 2)))
			  (q (vec-normalize vec)))
		      (cons c 
			    (map (lambda (x) (* s x)) q)))))

(define quat-inverse 
  (lambda (quat)
    (cons (car quat)
	  (map (lambda (x) (- x)) (cdr quat)))))

(define quat-mult
  ;; http://en.wikipedia.org/wiki/Quaternion#Hamilton_product
  (lambda (A B)
    (let ((q0 (car A))
	  (q1 (cadr A))
	  (q2 (caddr A))
	  (q3 (cadddr A))
	  (r0 (car B))
	  (r1 (cadr B))
	  (r2 (caddr B))
	  (r3 (cadddr B)))
      (vec-normalize
       (list (- (* q0 r0) (* q1 r1)     (* q2 r2) (* q3 r3))
	     (+ (* q0 r1) (* q1 r0)     (* q2 r3) (* (- q3) r2))
	     (+ (* q0 r2) (* (- q1) r3) (* q2 r0) (* q3 r1))
	     (+ (* q0 r3) (* q1 r2) (* (- q2) r1) (* q3 r0)))))))

;; (let ((a1 (car A))
    ;; 	  (b1 (cadr A))
    ;; 	  (c1 (caddr A))
    ;; 	  (d1 (cadddr A))
    ;; 	  (a2 (car B))
    ;; 	  (b2 (cadr B))
    ;; 	  (c2 (caddr B))
    ;; 	  (d2 (cadddr B)))
    ;;   (pk 'not-working)
    ;;   (list (- (* a1 a2) (* b1 b2)     (* c1 c2) (* d1 d2))
    ;; 	    (+ (* a1 b2) (* b2 a1)     (* c1 d2) (* (- d1) c2))
    ;; 	    (+ (* a1 c2) (* (- b1) d2) (* c1 a2) (* d1 b2))
    ;; 	    (+ (* a1 d2) (* b1 c2) (* (- c1) b2) (* d1 a2))))))

(define quat-vec-rotate
  (lambda (quat vec)
    (let ( (pad (cons 0 vec)) )
      (cdr
       (quat-mult (quat-mult quat pad)
		  (quat-inverse quat))))))

(define quat-to-rotmatrix
  (lambda (quat)
    (list (quat-vec-rotate quat '(1 0 0))
	  (quat-vec-rotate quat '(0 1 0))
	  (quat-vec-rotate quat '(0 0 1)))))

