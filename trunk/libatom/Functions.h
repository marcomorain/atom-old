class Runtime;

Cell* function_setf		( Runtime& runtime, Cell* params );
Cell* function_car		( Runtime& runtime, Cell* params );
Cell* function_cdr		( Runtime& runtime, Cell* params );
Cell* function_stringp	( Runtime& runtime, Cell* params );
Cell* function_if		( Runtime& runtime, Cell* params );
Cell* function_plus		( Runtime& runtime, Cell* params );
Cell* function_load		( Runtime& runtime,	Cell* params );
Cell* function_defun	( Runtime& runtime, Cell* params );