;; things to help with the view matrix, rotation and translation

(define-module (gt-interface view-helper)
  #:use-module (gt-interface quat)
  #:use-module (gt-interface vector-helper)
  #:export (view-help-view-matrix
	    view-help-move
	    view-help-rotate-x
	    view-help-rotate-y
	    view-help-rotate-z)
  #:duplicates (check))

;; given two lists, return a list of each list's elements func-d together
;; TODO: I'm actually pretty sure "map" already does this
(define combine
  ((lambda (x) (x x))
   (lambda (myself)
     (lambda (A B func)
       ;; (pk 'iter A B)
       (if ( or (equal? A '()) (equal? B '()))
	   '()
	   (let ((my-A (car A))
		 (my-B (car B))
		 (rest-A (cdr A))
		 (rest-B (cdr B)))
	     (cons (func my-A my-B) ((myself myself) rest-A rest-B func))))))))
	     

;; (define-packed-struct 3-vec-float (x float) (y float) (z float))
;; (define-packed-struct 4-vec-float (x float) (y float) (z float) (t float))
;; (pack 4-by-4 0 4-vec-float 0 1 2 3)
;; scheme@(guile-user)> 
;; scheme@(guile-user)> (unpack-each/serial 4-by-4 4-vec-float (lambda (n x y z t) (pk 'farts n x y z t)))
;; (define *vertices* (make-packed-array color-vertex 0))
;; (define *particles* (make-packed-array particle 0))
;; (pack *vertices* base color-vertex
;;              x- y- z
;;              r g b)


;; (define-packed-struct 3-vec-float (x float) (y float) (z float))
;; (define-packed-struct 4-vec-float (x float) (y float) (z float) (t float))
;; (define view-help-view-matrix (make-packed-array 4-vec-float 4))
(define view-help-view-matrix (floatvec-create 16)) ; 4 by 4
(define view-help-position '(0 0 0))
(define view-help-quat (quat-init 0 '(0 0 1)))

;; (define view-help-quat-debug
;;   (lambda ()
;;     (quat-to-rotmatrix view-help-quat)))

(define view-help-move
  (lambda (delta)
    (set! view-help-position (combine view-help-position delta +))
    (view-help-update-view-matrix)))

;; (define view-help-rotate
;;   (lambda (vec)
;;     (quat-vec-rotate view-help-quat vec)))

(define view-help-rotate
  (lambda (radians axis)
    (set! view-help-quat
	  (quat-mult (quat-init radians axis)
		      view-help-quat))
    (view-help-update-view-matrix)))

(define view-help-rotate-x
  (lambda (radians)
    (view-help-rotate radians '(1 0 0))))

(define view-help-rotate-y
  (lambda (radians)
    (view-help-rotate radians '(0 1 0))))

(define view-help-rotate-z
  (lambda (radians)
    (view-help-rotate radians '(0 0 1))))

(define view-help-update-view-matrix
  (lambda ()
    (let ( (x-vec (quat-vec-rotate view-help-quat '(1 0 0)))
	   (y-vec (quat-vec-rotate view-help-quat '(0 1 0)))
	   (z-vec (quat-vec-rotate view-help-quat '(0 0 1)))
	   (pos-x (car view-help-position))
	   (pos-y (cadr view-help-position))
	   (pos-z (caddr view-help-position)))
      (floatvec-set! view-help-view-matrix  0 (car x-vec))
      (floatvec-set! view-help-view-matrix  1 (cadr x-vec))
      (floatvec-set! view-help-view-matrix  2 (caddr x-vec))
      (floatvec-set! view-help-view-matrix  3 0)
      (floatvec-set! view-help-view-matrix  4 (car y-vec))
      (floatvec-set! view-help-view-matrix  5 (cadr y-vec))
      (floatvec-set! view-help-view-matrix  6 (caddr y-vec))
      (floatvec-set! view-help-view-matrix  7 0)
      (floatvec-set! view-help-view-matrix  8 (car z-vec))
      (floatvec-set! view-help-view-matrix  9 (cadr z-vec))
      (floatvec-set! view-help-view-matrix 10 (caddr z-vec))
      (floatvec-set! view-help-view-matrix 11 0)
      (floatvec-set! view-help-view-matrix 12 pos-x)
      (floatvec-set! view-help-view-matrix 13 pos-y)
      (floatvec-set! view-help-view-matrix 14 pos-z)
      (floatvec-set! view-help-view-matrix 15 1)
      )))
      
      
      ;; (pack view-help-view-matrix 0 4-vec-float (car x-vec) (cadr x-vec) (caddr x-vec) 0)
      ;; (pack view-help-view-matrix 1 4-vec-float (car y-vec) (cadr y-vec) (caddr y-vec) 0)
      ;; (pack view-help-view-matrix 2 4-vec-float (car z-vec) (cadr z-vec) (caddr z-vec) 0)
      ;; (pack view-help-view-matrix 3 4-vec-float pos-x pos-y pos-z 1))
      
      ;; (view-help-debug)
      ;; (pk 'quat-mag (vec-magnitude view-help-quat)



;; (define view-help-debug
;;   (lambda ()
;;     (unpack-each/serial view-help-view-matrix 4-vec-float (lambda (n x y z t) (pk n x y z t)))))

;; (define view-helper-rotate-x
;;   (lambda (radians)
;;     (set! view-help-quat
;; 	  (quat-rotate (quat-init 
