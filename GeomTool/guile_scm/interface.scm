;; the part for rendering

(define-module (gt-interface interface)
  #:use-module (gl)
  #:use-module (glu)
  #:use-module (glut)	     
  #:use-module (gl contrib packed-struct)
  #:use-module ((srfi srfi-1) #:select (map fold for-each))
  #:use-module (gt-interface vector-helper)
  #:use-module (gt-interface view-helper)
  #:use-module (gt-interface container)
  #:use-module (gt-interface gtlib)

  #:export (interface-start
	    interface-update))
  ;; #:duplicates (check))


(define window #f)
(define window-name "interface")

(define interface-size (cons 640 480))
(define interface-requires-redraw #f)

(define interface-camera-position '(0 0 -10))
(define interface-mouse-start-pan '(0 0))
(define interface-mouse-stop-pan '(0 1))
(define interface-mouse-start-move '(2 0))
(define interface-mouse-stop-move '(2 1))
(define interface-mouse-scroll-up '(3 0))
(define interface-mouse-scroll-down '(4 0))
(define interface-mouse-scroll-left '(5 0))
(define interface-mouse-screll-right '(6 0))
(define interface-mouse-coord-last '(0 0))
(define interface-mouse-state 0)
(define interface-mouse-state-pan 1)
(define interface-mouse-state-move 2)
(define interface-do-object-refresh #f)

(define vertex-data->gl-buff
  (lambda (vertex-data)
    (let ((gl-buff 0))
      ;; (pk "in vertex-data->gl-buff with" vertex-data)
      ;; (pk "input geom" (floatvec->list vertex-data))
      (set! gl-buff (gl-generate-buffer)) ;make a new buffer
      ;; (pk "gl-buff is now" gl-buff)
      (gl-bind-buffer (version-1-5 array-buffer) gl-buff) ;start using that new buffer
      (set-gl-buffer-data (version-1-5 array-buffer)	  ;set the data in that new buffer
      			  vertex-data
      			  (version-1-5 stream-draw))
      (gl-bind-buffer (version-1-5 array-buffer) 0) ;stop using that new buffer
      gl-buff)))					  ;and finally, spit out the new buff value if you have one
  
(define (interface-camera-rotate del-x del-y)
  ;; (pk 'camrotate del-x del-y)
  (view-help-rotate-x (* del-x .01))
  (view-help-rotate-y (* del-y .01))
  (post-redisplay window))

(define (interface-camera-move delta)
  ;; (pk 'cammove delta)
  (view-help-move delta)
  (post-redisplay window))

(define interface-draw-container
  (lambda ()
    ;; (pk "container" container-list)
    (map (lambda (obj-assoc)
	   (let ((obj (cdr obj-assoc)))
	     (if (equal? (assoc-ref obj "state") "good")
		 (for-each
		  (lambda (gl-buff)
		    (interface-draw-gl-buff-line-strip (assoc-ref obj "color")
						       (car gl-buff)
						       (cdr gl-buff)))
		  (assoc-ref obj "gl-buffs")))))			   
	   container-list)))
  
(define interface-draw-gl-buff-line-strip
  (lambda (color gl-buff num-vertices)
    (pk "draw gl buff, color, buff, size" color gl-buff num-vertices)
    (apply gl-color color)
    (gl-bind-buffer (version-1-5 array-buffer) gl-buff)
    ;; (update-gl-buffer-data (version-1-5 array-buffer) interface-object-float)
    
    (gl-enable-client-state (enable-cap vertex-array))
    ;; (gl-enable-client-state (enable-cap color-array))
    (set-gl-vertex-array (vertex-pointer-type float)
    			 #f
    			 #:stride 12 ;; packed-struct-size color-vertex)
    			 #:offset 0) ;; (packed-struct-offset color-vertex x))
    ;; (set-gl-color-array (color-pointer-type float)
    ;; 			#f
    ;; 			#:stride (packed-struct-size color-vertex)
    ;; 			#:offset (packed-struct-offset color-vertex r))
    (gl-draw-arrays (begin-mode line-strip) 0 num-vertices)
    ;; (gl-draw-arrays (begin-mode quads) 0
    ;; 		    (packed-array-length *vertices* color-vertex))
    ;; (gl-disable-client-state (enable-cap color-array))
    (gl-disable-client-state (enable-cap vertex-array))
    (gl-bind-buffer (version-1-5 array-buffer) 0)))

(define (interface-draw-axis scale)
  ;; Could disable lighting and depth test.
  ;; (format #t "draw axis[~d]\n" scale)
  (gl-begin (begin-mode lines)
	    (gl-color 1 0 0)
	    (gl-vertex 0 0 0)
	    (gl-vertex scale 0 0)

	    (gl-color 0 1 0)
	    (gl-vertex 0 0 0)
	    (gl-vertex 0 scale 0)
	    
	    (gl-color 0 0 1)
	    (gl-vertex 0 0 0)
	    (gl-vertex 0 0 scale)))

(define interface-draw
  (lambda ()
    (pk 'interface-draw)

    ;; this stuff should probably be in a draw-setup thing
    (gl-clear 
     (clear-buffer-mask color-buffer depth-buffer))
    (set-gl-matrix-mode 
     (matrix-mode modelview))
    (gl-load-matrix view-help-view-matrix #:transpose #f)

    (interface-draw-axis 10)
    (interface-draw-container)

    (swap-buffers)))

(define interface-create-gl-buffer-list
  (lambda (vert-list)
    ;; (pk "folding on vert-list" vert-list)
    (fold (lambda (vertdata accum)
    	    ;; (pk "create-gl-buffer-list")
    	    (cons
	     (cons (vertex-data->gl-buff vertdata) (/ (floatvec-length vertdata) 3))
	     accum))
	  '()
    	  vert-list)))

(define interface-idle
  (lambda ()
    (if interface-do-object-refresh
	(begin
	  ;; (pk "I should update some objects")
	  ;; (for-each (lambda (x) (pk "list entry")) container-list)
	  (for-each (lambda (obj)
		      (pk "working on " (car obj))
		      (let* ((obj (cdr obj))
	  		     (gt (assoc-ref obj "gt"))
			     (get-ptbuff (lambda (gt)
					   (cond ((gt-segmentlist2? gt) (gt-segmentlist2-ptbuff gt))
						 ((gt-region2? gt) (gt-region2-ptbuff gt))
						 ((gt-set2? gt) (gt-set2-ptbuff gt))
						 (else '())))))
	  		;; (pk "update obj:" gt)     
	  		;; (pk "former gl-buff" (assoc-ref obj "gl-buffs"))
	  		;; wipe out the old buffers
			(if (list? (assoc-ref obj "gl-buffs"))
			    (for-each (lambda (buff) (gl-delete-buffer (car buff))) (assoc-ref obj "gl-buffs")))
	  		;; make some new ones
			(assoc-set! obj
				    "gl-buffs"
				    (if (assoc-ref obj "visible")
					(interface-create-gl-buffer-list (get-ptbuff gt))
					'()))
	  		;; (pk "obj:" obj)
	  		(assoc-set! obj "state" "good")
			;; (pk "state good done")
			))
	  	    container-list)
	  (set! interface-do-object-refresh #f)
	  (post-redisplay window)))
    (if interface-requires-redraw
	    (begin
	      (display "update interface flag found\n")
	      (set! interface-requires-redraw #f)
	      (post-redisplay window)))))
(define (interface-resize x y)
  (format #t "resizing: ~d x ~d\n" x y)
  (gl-viewport 0 0 x y)
  (set-gl-matrix-mode (matrix-mode projection))
  (gl-load-identity)
  (glu-perspective 60 (/ x y) 0.1 1000))

(define (interface-keyboard key x y)
  ;; (pk 'keyboard key x y)
  (cond ((equal? key 39) (interface-camera-move '(0 0 -1))) ;down
	((equal? key 44) (interface-camera-move '(0 -1 0))) ;forward
	((equal? key 46) (interface-camera-move '(0 0 1))) ;up
	((equal? key 97) (interface-camera-move '(1 0 0))) ;left
	((equal? key 111) (interface-camera-move '(0 1 0))) ;back
	((equal? key 101) (interface-camera-move '(-1 0 0))) ;right
	(else (pk 'unk-keyboard key x y))))
(define (interface-special key x y)
  (pk 'special key x y))
(define (interface-set-mouse-state state)
  (set! interface-mouse-state state))
(define (interface-mouse button state x y)
  (set! interface-mouse-coord-last (list x y))
  (cond ((equal? (list button state) interface-mouse-start-pan)
	 (interface-set-mouse-state interface-mouse-state-pan))
	((equal? (list button state) interface-mouse-start-move)
	 (interface-set-mouse-state interface-mouse-state-move))
	(( or (equal? (list button state) interface-mouse-stop-pan)
	      (equal? (list button state) interface-mouse-stop-move))
	 (interface-set-mouse-state 0))
	
	(else (pk 'mousebutt button state x y))
	))
(define (interface-motion x y)
  (let ( (del-x (- x (car interface-mouse-coord-last)))
	 (del-y (- y (cadr interface-mouse-coord-last))))
    (pk 'intmotion del-x del-y)
    
    (if (= interface-mouse-state interface-mouse-state-pan)
	(interface-camera-rotate del-y del-x))
    (if (= interface-mouse-state interface-mouse-state-move)
	(interface-camera-move '(del-y del-x 0))))
  
  (set! interface-mouse-coord-last (list x y)))

(define interface-make-window
  (lambda ()
    (initialize-glut '("interface") #:window-size interface-size)
    
    (set! window (make-window window-name))

    (set-idle-callback interface-idle)
    (set-display-callback interface-draw)
    (set-reshape-callback interface-resize)
    (set-keyboard-callback interface-keyboard)
    (set-special-callback interface-special)
    (set-mouse-callback interface-mouse)
    (set-motion-callback interface-motion)

    (set-gl-clear-color 0 0 0 1)
    (set-gl-clear-depth 1)

    (set-gl-matrix-mode (matrix-mode modelview))
    
    (gl-enable (enable-cap depth-test))

    ;; (interface-setup-object-float-buffer)

    (view-help-move interface-camera-position)

    (display "\nentering glut main loop\n")
    (glut-main-loop)))

;; functions that you should be able to call

(define interface-start
  (lambda ()
    (call-with-new-thread interface-make-window)
    (interface-update)))

(define interface-update
  (lambda ()
    (interface-update-objects-by-name
     (fold (lambda (object accum)
	     (cons (car object) accum))
	   '()
	   container-list))))
	    

(define interface-update-objects-by-name
  (lambda (name-list)
    (pk "interface-updae obj called with" name-list)
    (let ((names (if (equal? (length name-list) 0)
		     (map (lambda (obj-assoc) (car obj-assoc)) container-list)
		     name-list)))
      (map (lambda (name) (container-set! name "state" "refresh")) names)
      (set! interface-do-object-refresh #t))))
