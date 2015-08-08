;; container for objects
(define-module (gt-interface container)
  #:use-module ((srfi srfi-1) #:select (fold map))
  #:use-module (gt-interface gtlib)
  #:use-module (gt-interface vector-helper)
  #:export (container-list
	    container-add
	    container-get
	    container-set!
	    container-refresh)
  #:duplicates (check))

;; the list of objects (gt)
(define container-list '())

;; here are the required fields that each object in the list has to have
;;   '(("gt" . (lambda () (gt-make-obj)))
;;     ("color" . (lambda() ('(0 0 0))))
;;     ("visible" . (lambda () (#f)))
;;     ("state" . (lambda () (#f)))
;;     ("gl-buffs" . (renderables list))

;; and here are what the entries in renderables should look like
;;     (( [[int of gl-buffer-referenc]] . [[int of num vertices]]) ([[same type of pair]]) ...)

;; this is easily one of the dumbest functions I've ever written because I can't program
(define container-new-gt-by-type
  (lambda (type)
    (cond ((equal? type "segmentlist2") (gt-make-segmentlist2))
	  ((equal? type "region2") (gt-make-region2))
	  ((equal? type "set2") (gt-make-set2))
	  (else #f))))

(define container-force-add
  (lambda (name type-or-gt)
    ;; gt = the gt smob
    ;; color = list of 3 values (R G B)
    ;; visible = #t or #f
    ;; state = { "new" "refresh" "good" }
   
    (set! container-list
	  (acons name
		 (acons
		  "gt" ( if (gt? type-or-gt)
			    type-or-gt
			    (container-new-gt-by-type type-or-gt))
		  (acons
		   "color" (list 1 1 1)
		   (acons
		    "visible" #t	;maybe not safe?
		     (acons
		      "state" "new"
		      (acons "gl-buffs" '() '())))))
		 container-list))))
;; (define container-names
;;   (lambda ()
;;     (map (lambda (obj-assoc) (car obj-assoc)) container-list)))		 

(define container-add
  ;; add object by name #t on success #f on failure to add
  (lambda (name type)
    (if (not (assoc name container-list))
	(begin
	  (container-force-add name type)
	  #t)
	#f)))

(define container-get
  ;; get object and optionally a property of that object, #f on missing value
  (lambda* (name #:optional property)
    (let ((obj (assoc name container-list)))
      (if (equal? obj #f)
	  #f
	  (let ((obj (cdr obj)))
	    (if (equal? property #f)
		obj
		(cdr (assoc property obj))))))))

(define container-set!
  ;; set object property to value by object name
  (lambda (name property value)
    (let ((obj (assoc name container-list)))
      (if (not obj)
	  obj
	  (begin
	    (assoc-set! obj property value)
	    (assoc-set! obj "state" "refresh")
	    #t)))))

(define container-refresh
  (lambda (name)
    (let ((obj (container-get name)))
      (if (not (equal? obj #f))
	  (assoc-set! obj "state" "refresh")))))


  
    
