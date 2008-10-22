class Runtime;

Cell* function_apply			( Runtime& runtime, Cell* params );
// aref
Cell* function_backquote		( Runtime& runtime, Cell* params );
Cell* function_block			( Runtime& runtime, Cell* params );
Cell* function_car				( Runtime& runtime, Cell* params );
Cell* function_cdr				( Runtime& runtime, Cell* params );
// ceiling
// char=
Cell* function_cons				( Runtime& runtime, Cell* params );
Cell* function_eval				( Runtime& runtime, Cell* params );
Cell* function_defmacro			( Runtime& runtime, Cell* params );
Cell* function_defun			( Runtime& runtime, Cell* params );
// documentation
Cell* function_eq				( Runtime& runtime, Cell* params );
Cell* function_error			( Runtime& runtime, Cell* params );
// expt
// fdefinition
// function
// floor
// gensym
// get-setf-expression
Cell* function_if				( Runtime& runtime, Cell* params );
Cell* function_imagpart			( Runtime& runtime, Cell* params );
// labels
Cell* function_length			( Runtime& runtime, Cell* params );
// multiple-value-bind
Cell* function_nth_value		( Runtime& runtime, Cell* params );
Cell* function_quote			( Runtime& runtime, Cell* params );
Cell* function_realpart			( Runtime& runtime, Cell* params );
Cell* function_return_from		( Runtime& runtime, Cell* params );
// symbol-function
// tagbody
// type-of
// typep
// =
Cell* function_plus				( Runtime& runtime, Cell* params );
Cell* function_minus			( Runtime& runtime, Cell* params );
Cell* function_greater_than		( Runtime& runtime,	Cell* params );
Cell* function_less_than		( Runtime& runtime, Cell* params );

// may not be needed:
Cell* function_load				( Runtime& runtime,	Cell* params );
Cell* function_setf				( Runtime& runtime, Cell* params );
Cell* function_stringp			( Runtime& runtime, Cell* params );