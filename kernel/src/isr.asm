%macro exc_err_stub 1
isr_stub_%+%1:	
  mov dil, %1             ; vector
  pop rsi                 ; error code
	call exception_handler
	iretq
%endmacro

%macro exc_no_err_stub 1
isr_stub_%+%1:	
  mov dil, %1             ; vector
  mov rsi, 0
	call exception_handler
	iretq
%endmacro

extern exception_handler
exc_no_err_stub 0
exc_no_err_stub 1
exc_no_err_stub 2
exc_no_err_stub 3
exc_no_err_stub 4
exc_no_err_stub 5
exc_no_err_stub 6
exc_no_err_stub 7
exc_err_stub    8
exc_no_err_stub 9
exc_err_stub    10
exc_err_stub    11
exc_err_stub    12
exc_err_stub    13
exc_err_stub    14
exc_no_err_stub 15
exc_no_err_stub 16
exc_err_stub    17
exc_no_err_stub 18
exc_no_err_stub 19
exc_no_err_stub 20
exc_no_err_stub 21
exc_no_err_stub 22
exc_no_err_stub 23
exc_no_err_stub 24
exc_no_err_stub 25
exc_no_err_stub 26
exc_no_err_stub 27
exc_no_err_stub 28
exc_no_err_stub 29
exc_err_stub    30
exc_no_err_stub 31

%macro irq_stub 1
isr_stub_%+%1:	
  mov dil, %1       ; vector
	call irq_handler
	iretq
%endmacro

extern irq_handler
%assign i 32
%rep 16
  irq_stub i
  %assign i i + 1
%endrep

global isr_stub_table
isr_stub_table:
%assign i 0
%rep 48
	dq isr_stub_%+i
  %assign i i + 1
%endrep
