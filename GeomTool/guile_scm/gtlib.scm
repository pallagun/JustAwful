;; module to load the extension

(define-module (gt-interface gtlib)
  ;; #:use-module (gt-interface vector-helper)
  #:export (
	    gt?
	    ;; smobSegment2 stuff
	    gt-segment2?
	    gt-make-line2
	    gt-make-arc2
	    ;; smobSegmentList2 stuff
	    gt-segmentlist2?
	    gt-make-segmentlist2
	    gt-segmentlist2-append
	    gt-segmentlist2-clear
	    gt-segmentlist2-ptbuff
	    gt-segmentlist2-isclosed
	    gt-segmentlist2-iscontinuous
	    gt-segmentlist2-iszeroarea
	    gt-segmentlist2-getspin
	    gt-segmentlist2-gettype
	    gt-segmentlist2-translate
	    gt-segmentlist2-blindexpand
	    gt-segmentlist2-scaleorigin
	    gt-segmentlist2-updatecache
	    gt-segmentlist2-invert
	    gt-segmentlist2-almostequal
	    gt-segmentlist2-numsegments
	    gt-segmentlist2-simplify
	    ;; smobRegion2 stuff
	    gt-region2?
	    gt-make-region2
	    gt-region2-append
	    gt-region2-clear
	    gt-region2-ptbuff
	    gt-region2-unique
	    gt-region2-list
	    gt-region2-numlists
	    ;; smobSet2 stuff
	    gt-set2?
	    gt-make-set2
	    gt-set2-append
	    gt-set2-clear
	    gt-set2-ptbuff
	    gt-set2-region
	    gt-set2-numregions
	    gt-set2-union
	    gt-set2-intersect
	    gt-set2-invert
	    gt-set2-expand
	    )

  #:duplicates (check))


;; load the library
(eval-when (load eval compile) (load-extension "libguile-geomtool" "init_geomTool"))

(define gt?
  (lambda (thing)
    (or (gt-segment2? thing)
	(gt-segmentlist2? thing)
	(gt-region2? thing)
	(gt-set2? thing))))
