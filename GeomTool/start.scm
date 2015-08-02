#!/usr/bin/guile -s
!#

(use-modules (gt-interface container)
	     (gt-interface functions)
	     (gt-interface vector-helper)
	     (gt-interface interface)
	     (gt-interface gtlib)
	     (gt-interface shape-factory))

(define setup-basic
  (lambda ()
    (container-add "L" "segmentlist2")
    (container-add "R" "region2")
    (container-add "S" "set2")
    
    (gt-segmentlist2-append (container-get "L" "gt") (gt-make-line2 0 0 1 0))
    (gt-segmentlist2-append (container-get "L" "gt") (gt-make-line2 1 0 1 1))
    (gt-segmentlist2-append (container-get "L" "gt") (gt-make-line2 1 1 0 1))
    (gt-segmentlist2-append (container-get "L" "gt") (gt-make-line2 0 1 0 0))
    
    (gt-region2-append (container-get "R" "gt") (container-get "L" "gt"))
    (gt-segmentlist2-translate (container-get "L" "gt") 2 2)
    (gt-region2-append (container-get "R" "gt") (container-get "L" "gt"))
    
    (container-set! "R" "visible" #t)))


(define setup-L1-B
  (lambda ()
    (container-add "L1" "segmentlist2")
    (factory-square (container-get "L1" "gt") 2)
    (gt-segmentlist2-translate (container-get "L1" "gt") 1 1)))

(define setup-L1-H
  (lambda ()
    (setup-L1-B)
    (gt-segmentlist2-invert (container-get "L1" "gt"))))

(define setup-L2-B
  (lambda ()
    (container-add "L2" "segmentlist2")
    (factory-square (container-get "L2" "gt") 2)
    (gt-segmentlist2-translate (container-get "L2" "gt") 2 2)))

(define setup-L2-H
  (lambda ()
    (setup-L2-B)
    (gt-segmentlist2-invert (container-get "L2" "gt"))))

    
(define setup-union-test-BB
  (lambda ()
    (setup-L1-B)
    (setup-L2-B)
    (container-add "S" "set2")
    (gt-set2-unionSLSL (container-get "L1" "gt") (container-get "L2" "gt") (container-get "S" "gt"))))

(define setup-intersect-test-BB
  (lambda ()
    (setup-L1-B)
    (setup-L2-B)
    (container-add "S" "set2")
    (gt-set2-intersectSLSL (container-get "L1" "gt") (container-get "L2" "gt") (container-get "S" "gt"))))

(define setup-union-test-BH
  (lambda ()
    (setup-L1-B)
    (setup-L2-H)
    (container-add "S" "set2")
    (gt-set2-unionSLSL (container-get "L1" "gt") (container-get "L2" "gt") (container-get "S" "gt"))))

(define setup-union-test-HH
  (lambda ()
    (setup-L1-H)
    (setup-L2-H)
    (container-add "S" "set2")
    (gt-set2-unionSLSL (container-get "L1" "gt") (container-get "L2" "gt") (container-get "S" "gt"))))



;; (setup-intersect-test-BB)

;; (container-set! "S" "visible" #t)
;; (container-set! "L1" "color" '(1 0 0))
;; (container-set! "L2" "color" '(0 1 0))
;; (container-set! "L1" "visible" #t)
;; (container-set! "L2" "visible" #t)

;; (interface-start)
;; (define L (gt-make-segmentlist2))
;; (define R (gt-make-region2))

;; (gt-segmentlist2-append L (gt-make-line2 0 0 1 0))
;; (gt-segmentlist2-append L (gt-make-line2 1 0 1 1))
;; (gt-segmentlist2-append L (gt-make-line2 1 1 0 1))
;; (gt-segmentlist2-append L (gt-make-line2 0 1 0 0))
;; (gt-region2-append R L)
;; (gt-segmentlist2-translate L 0.5 0.5)
;; (gt-region2-append R L)



