(define-module (gt-interface functions)
  #:use-module (gt-interface gtlib)
  #:use-module (gt-interface container)
  #:use-module (gt-interface vector-helper)
  #:export (gt-append
	    gt-new-seglist2
	    gt-new-region2
	    gt-new-set2
	    gt-line2
	    gt-arc2
	    gt-union
	    gt-union-accum
	    gt-intersect
	    gt-intersect-accum)
  #:duplicates (check))


(define gt-new-seglist2
  (lambda (name)
    (container-add name "segmentlist2")))

(define gt-new-region2
  (lambda (name)
    (container-add name "region2")))

(define gt-new-set2
  (lambda (name)
    (container-add name "set2")))

(define gt-line2
  (lambda (x1 y1 x2 y2)
    (gt-make-line2 x1 y1 x2 y2)))

(define get-gt-from-thing
  (lambda (thing)
    (cond ((gt? thing) thing)  	                     ;is it already an gt?
 	  ((list? thing) (assoc-ref thing "gt"))            ;maybe its an alist with gt in it
 	  ((not (eq? thing #f)) (container-get thing "gt")) ;maybe its a name of an obj
	  (#t #f))))					     ;ehh, give up

(define gt-union-accum
  (lambda (accum input)
    (let ((accum-gt (get-gt-from-thing accum))
	  (input-gt (get-gt-from-thing input))
	  (temp-gt (gt-make-set2)))
      (cond ((gt-set2? accum)
	     (gt-set2-union accum-gt, input-gt temp-gt)
	     (gt-set2-clear accum-gt)
	     (gt-set2-append accum-gt temp-gt))
	    (else #f)))))

(define gt-intersect-accum
  (lambda (accum input)
    (let ((accum-gt (get-gt-from-thing accum))
	  (input-gt (get-gt-from-thing input))
	  (temp-gt (gt-make-set2)))
      (cond ((gt-set2? accum)
	     (gt-set2-intersect accum-gt, input-gt temp-gt)
	     (gt-set2-clear accum-gt)
	     (gt-set2-append accum-gt temp-gt))
	    (else #f)))))

(define gt-invert
  (lambda (A Result)
    (let ((A-gt (get-gt-from-thing A))
	  (Result-gt (get-gt-from-thing Result)))
      (cond ((gt-segmentlist2? A-gt)
	     (gt-segmentlist2-invert A-gt Result-gt))
	    ((or (gt-region2? A-gt) (gt-set2? A-gt))
	     (gt-set2-invert A-gt Result-gt))
	    (else #f)))))

(define gt-union
  (lambda (A B Result)
    (let ((A-gt (get-gt-from-thing A))
	  (B-gt (get-gt-from-thing B))
	  (Result-gt (get-gt-from-thing Result)))
      (gt-set2-union A-gt B-gt Result-gt))))

(define gt-intersect
  (lambda (A B Result)
    (let ((A-gt (get-gt-from-thing A))
	  (B-gt (get-gt-from-thing B))
	  (Result-gt (get-gt-from-thing Result)))
      (gt-set2-intersect A-gt B-gt Result-gt))))

(define gt-append
  (lambda (accum addition)
    (let ((accum-gt (get-gt-from-thing accum))
 	  (addit-gt (get-gt-from-thing addition)))
      (cond ((gt-segmentlist2? accum-gt)
	     (gt-segmentlist2-append accum-gt addit-gt))
	    ((gt-region2? accum-gt)
	     (gt-region2-append accum-gt addit-gt))
	    ((gt-set2? accum-gt)
	     (gt-set2-append accum-gt addit-gt))
	    (else #f)))))

(define s-test
  (lambda (name)
    (get-vertex-data name)))

