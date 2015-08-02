;; basic shape creating routines

(define-module (gt-interface shape-factory)
  #:use-module (gt-interface gtlib)
  #:export (factory-square
	    factory-make-square
	    factory-circle
	    factory-make-circle
	    factory-rectangle
	    factory-make-rectangle
	    factory-poly
	    factory-make-poly)
  #:duplicates (check))

(define factory-make-square
  (lambda (size)
    (let ((output (gt-make-segmentlist2)))
      (factory-square output size)
      output)))
(define factory-square
  (lambda (seglist size)
    (let ( (neg (* -1 (/ size 2)))
	   (pos (/ size 2)))
      (gt-segmentlist2-clear seglist)
      (gt-segmentlist2-append seglist (gt-make-line2 neg neg pos neg))
      (gt-segmentlist2-append seglist (gt-make-line2 pos neg pos pos))
      (gt-segmentlist2-append seglist (gt-make-line2 pos pos neg pos))
      (gt-segmentlist2-append seglist (gt-make-line2 neg pos neg neg))
      (gt-segmentlist2-updatecache seglist))))

(define factory-make-rectangle
  (lambda (width height)
    (let ((output (gt-make-segmentlist2)))
      (factory-make-rectangle width height)
      output)))
(define factory-rectangle
  (lambda (seglist width height)
    (let ( (nw (* -1 (/ 2 width)))
	   (nh (* -1 (/ 2 height)))
	   (pw (/ 2 width))
	   (ph (/ 2 height)))
      (gt-segmentlist2-clear seglist)
      (gt-segmentlist2-append seglist (gt-make-line2 nw nh pw nh))
      (gt-segmentlist2-append seglist (gt-make-line2 pw nh pw ph))
      (gt-segmentlist2-append seglist (gt-make-line2 pw ph nw ph))
      (gt-segmentlist2-append seglist (gt-make-line2 nw ph nw nh))
      (gt-segmentlist2-updatecache seglist))))

(define factory-make-circle
  (lambda (radius)
    (let ((output (gt-make-segmentlist2)))
      (factory-circle output radius)
      output)))
(define factory-circle
  (lambda (seglist radius)
    (let ((pi 3.14159265358979323846264338327950288419716939937510))
      (gt-segmentlist2-clear seglist)
      (gt-segmentlist2-append seglist (gt-make-arc2 1 0 0 radius 0 pi))
      (gt-segmentlist2-append seglist (gt-make-arc2 1 0 0 radius pi (* 2 pi)))
      (gt-segmentlist2-updatecache seglist))))

(define linearlist-to-pairlist
  ;; given (X1 Y1 X2 Y2 X3 Y3 ....) turn it into ( (X1 Y1) (X2 Y2) (X3 Y3) ...)
  (lambda (input-list)
    (letrec (( func (lambda (lin-list)
		      (if (eq? lin-list '())
			  lin-list
			  (cons (list (car lin-list) (cadr lin-list))
				(func (cddr lin-list)))))))
      (func input-list))))

(define factory-make-poly
  (lambda (vertex-pair-list)
    (let ((output (gt-make-segmentlist2)))
      (factory-poly output vertex-pair-list)
      output)))
(define factory-poly
  (lambda (seglist vertex-pair-list)
    (gt-segmentlist2-clear seglist)
    (let ((add-vert
    	   (lambda (segmentlist start-pair end-pair)
    	     (gt-segmentlist2-append
	      segmentlist
	      (gt-make-line2 (car start-pair)
			      (cadr start-pair)
			      (car end-pair)
			      (cadr end-pair)))))
	  (last-vert (car vertex-pair-list))
    	  (rem-vert (cdr vertex-pair-list)))
      (for-each (lambda (vert-pair)
		  (add-vert seglist last-vert vert-pair)
      		  (set! last-vert vert-pair))
      		rem-vert))
    (gt-segmentlist2-updatecache seglist)))
      
      
