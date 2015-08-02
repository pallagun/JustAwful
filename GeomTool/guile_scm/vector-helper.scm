;; basic vector helper stuff

(define-module (gt-interface vector-helper)
  #:use-module ((rnrs bytevectors)
		#:select (make-bytevector
			  bytevector-length
			  bytevector-ieee-single-native-ref
			  bytevector-ieee-single-native-set!))
  #:export (floatvec-create
	    floatvec-create-bytes
	    floatvec-ref
	    floatvec-set!
	    floatvec-length
	    floatvec->list)
  #:duplicates (check))


(define floatvec-create-bytes
  (lambda (num-bytes)
    (make-bytevector num-bytes)))

(define floatvec-create
  (lambda (num-elements)
    (make-bytevector (* num-elements 4) 0)))

;; someday
;; (define floatvec-range-set!
;;   (lambda (vec idx-start values)
;;     (bytevector-ieee-single-native-ref

(define floatvec-accum
  (lambda (vec idx accum)
    ;; (pk "floatvec-accum" vec idx accum)
    (if (eq? idx 0)
	(cons (floatvec-ref vec 0) accum)
	(floatvec-accum vec
			(- idx 1)
			(cons (floatvec-ref vec idx) accum)))))

(define floatvec->list
  (lambda (vec)
    (floatvec-accum vec
		    (- (floatvec-length vec) 1)
		    '())))

(define floatvec-set!
  (lambda (vec idx value)
    (bytevector-ieee-single-native-set! vec (* idx 4) value)))

(define floatvec-ref
  (lambda (vec idx)
    ;; (pk "floatvec-ref" vec idx)
    (bytevector-ieee-single-native-ref vec (* idx 4))))

(define floatvec-length
  (lambda (vec)
    (/ (bytevector-length vec) 4)))
